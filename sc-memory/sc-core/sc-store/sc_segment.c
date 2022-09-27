/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_segment.h"

#include "sc_element.h"
#include "../sc_memory_private.h"

#include "sc-base/sc_allocator.h"
#include "sc-base/sc_atomic.h"
#include "sc-base/sc_assert_utils.h"

sc_segment * sc_segment_new(sc_addr_seg num)
{
  sc_segment * segment = sc_mem_new(sc_segment, 1);

  // initialize empty count for sections
  sc_uint16 count = SC_SEGMENT_ELEMENTS_COUNT / SC_CONCURRENCY_LEVEL;
  sc_uint16 i, c = count * SC_CONCURRENCY_LEVEL;
  for (i = 0; i < SC_CONCURRENCY_LEVEL; ++i)
  {
    sc_segment_section * section = &(segment->sections[i]);
    section->empty_offset = i;
    section->empty_count = ((c + i) <= SC_SEGMENT_ELEMENTS_COUNT) ? count + 1 : count;
  }

  if (num == 0)
  {
    segment->sections[0].empty_offset += SC_CONCURRENCY_LEVEL;
    --segment->sections[0].empty_count;
  }

  segment->num = num;

  return segment;
}

void sc_segment_loaded(sc_segment * seg)
{
  sc_uint32 i;
  seg->elements_count = 0;

  for (i = 0; i < SC_CONCURRENCY_LEVEL; ++i)
  {
    sc_segment_section * section = &seg->sections[i];
    sc_uint32 idx = i;

    section->empty_count = 0;
    while (idx < SC_SEGMENT_ELEMENTS_COUNT)
    {
      if (seg->elements[idx].flags.type == 0)
      {
        ++section->empty_count;
        section->empty_offset = (sc_int32)idx;
      }
      else
        ++seg->elements_count;
      idx += SC_CONCURRENCY_LEVEL;
    }
  }

  // initialize references to 1
  for (i = 0; i < SC_SEGMENT_ELEMENTS_COUNT; ++i)
  {
    if (seg->elements[i].flags.type != 0)
      seg->meta[i].ref_count = 1;
  }
}

void sc_segment_free(sc_segment * segment)
{
  sc_assert(segment != null_ptr);

  sc_mem_free(segment);
}

void sc_segment_erase_element(sc_segment * seg, sc_uint16 offset)
{
  sc_assert(sc_atomic_pointer_get((void **)&seg->sections[offset % SC_CONCURRENCY_LEVEL].thread_lock) != null_ptr);
  sc_atomic_int_dec_and_test(&seg->elements_count);

  sc_assert(seg != null_ptr);
  sc_assert(offset < SC_SEGMENT_ELEMENTS_COUNT);
  sc_mem_set(&seg->elements[offset], 0, sizeof(sc_element));

  sc_segment_section * section = &(seg->sections[offset % SC_CONCURRENCY_LEVEL]);
  sc_atomic_int_inc(&section->empty_count);
  sc_atomic_int_set(&section->empty_offset, offset);

  sc_assert(offset != 0 || seg->num != 0);
}

sc_uint32 sc_segment_get_elements_count(sc_segment * seg)
{
  sc_assert(seg != null_ptr);

  return sc_atomic_int_get(&seg->elements_count);
}

sc_bool sc_segment_has_empty_slot(sc_segment * segment)
{
  sc_assert(segment != null_ptr);
  if (segment->num == 0)
    return sc_atomic_int_get(&segment->elements_count) < SC_SEGMENT_ELEMENTS_COUNT - 1;

  return sc_atomic_int_get(&segment->elements_count) < SC_SEGMENT_ELEMENTS_COUNT;
}

void sc_segment_collect_elements_stat(sc_segment * seg, sc_stat * stat)
{
  sc_int32 i;
  for (i = 0; i < SC_CONCURRENCY_LEVEL; ++i)
  {
    sc_segment_section * section = &seg->sections[i];
    sc_segment_section_lock(section);
    sc_int32 j = i;
    while (j < SC_SEGMENT_ELEMENTS_COUNT)
    {
      sc_type type = seg->elements[j].flags.type;
      if (type & sc_type_node)
        stat->node_count++;
      else if (type & sc_type_link)
        stat->link_count++;
      else if (type & sc_type_arc_mask)
        stat->arc_count++;
      else
      {
        // do not use empty count, because it can be changed
        if (type == 0)
          stat->empty_count++;
      }

      j += SC_CONCURRENCY_LEVEL;
    }
    sc_segment_section_unlock(section);
  }
}

sc_element_meta * sc_segment_get_meta(sc_segment * seg, sc_addr_offset offset)
{
  sc_assert(seg != null_ptr);
  sc_assert(seg->sections[offset % SC_CONCURRENCY_LEVEL].thread_lock == sc_thread());
  sc_assert(offset < SC_SEGMENT_ELEMENTS_COUNT);

  return &seg->meta[offset];
}

// ---------------------------
sc_element * sc_segment_lock_empty_element(sc_memory_context const * ctx, sc_segment * seg, sc_addr_offset * offset)
{
  sc_uint16 max_attempts = 1;
  while (sc_segment_has_empty_slot(seg) == SC_TRUE)
  {
    sc_uint32 i;
    for (i = 0; i < SC_CONCURRENCY_LEVEL; ++i)
    {
      sc_uint32 sec_id = (ctx->id + i) % SC_CONCURRENCY_LEVEL;
      sc_segment_section * section = &seg->sections[sec_id];

      if (sc_atomic_int_get(&section->empty_count) == 0)
        continue;

      sc_bool locked = sc_segment_section_lock_try(section, 1);

      if (locked == SC_TRUE)
      {
        sc_int32 idx = (sc_int32)section->empty_offset;

        if (section->empty_count > 0)
        {
          sc_atomic_int_inc(&seg->elements_count);
          sc_atomic_int_add(&section->empty_count, -1);

          // trying to find empty element in section
          sc_assert(idx >= 0 && idx < SC_SEGMENT_ELEMENTS_COUNT);
          sc_assert(seg->num + idx > 0);  // not empty addr

          // need to find new empty element
          sc_int32 j = idx + SC_CONCURRENCY_LEVEL;
          while (j < SC_SEGMENT_ELEMENTS_COUNT)
          {
            if (seg->elements[j].flags.type == 0)
            {
              sc_atomic_int_set(&section->empty_offset, j);
              sc_assert(seg->num + j > 0);
              goto result;
            }
            j += SC_CONCURRENCY_LEVEL;
          }
          j = idx - SC_CONCURRENCY_LEVEL;
          sc_int left = (i == 0 && seg->num == 0) ? SC_CONCURRENCY_LEVEL : idx;
          while (j >= left)
          {
            if (seg->elements[j].flags.type == 0)
            {
              sc_atomic_int_set(&section->empty_offset, j);
              sc_assert(seg->num + j > 0);
              goto result;
            }

            j -= SC_CONCURRENCY_LEVEL;
          }
        }

        sc_segment_section_unlock(section);
        continue;

      result:
      {
        sc_assert(sc_atomic_int_get(&section->empty_count) >= 0);
        *offset = idx;
        return &seg->elements[*offset];
      }
      }
    }

    if (max_attempts < SC_CONCURRENCY_LEVEL)
      ++max_attempts;
    else
      return null_ptr;
  }

  return null_ptr;
}

sc_element * sc_segment_lock_element(sc_segment * seg, sc_addr_offset offset)
{
  sc_assert(offset < SC_SEGMENT_ELEMENTS_COUNT && seg != null_ptr);
  sc_segment_section * section = &seg->sections[offset % SC_CONCURRENCY_LEVEL];
  sc_segment_section_lock(section);
  return &seg->elements[offset];
}

sc_element * sc_segment_lock_element_try(sc_segment * seg, sc_addr_offset offset, sc_uint16 max_attempts)
{
  sc_assert(offset < SC_SEGMENT_ELEMENTS_COUNT && seg != null_ptr);
  sc_segment_section * section = &seg->sections[offset % SC_CONCURRENCY_LEVEL];

  if (sc_segment_section_lock_try(section, max_attempts) == SC_TRUE)
    return &seg->elements[offset];

  return null_ptr;
}

void sc_segment_unlock_element(sc_segment * seg, sc_addr_offset offset)
{
  sc_assert(offset < SC_SEGMENT_ELEMENTS_COUNT && seg != null_ptr);
  sc_segment_section * section = &seg->sections[offset % SC_CONCURRENCY_LEVEL];
  sc_segment_section_unlock(section);
}

void sc_segment_section_lock(sc_segment_section * section)
{
  sc_pointer thread = sc_thread();
lock:
{
  while (sc_atomic_int_compare_and_exchange(&section->internal_lock, 0, 1) == SC_FALSE)
    ;
}

  if (sc_atomic_pointer_get((void **)&section->thread_lock) != null_ptr &&
      sc_atomic_pointer_get((void **)&section->thread_lock) != thread)
  {
    sc_atomic_int_set(&section->internal_lock, 0);
    goto lock;
  }

  sc_atomic_pointer_set((void **)&section->thread_lock, thread);
  sc_atomic_int_inc(&section->lock_count);

  sc_atomic_int_set(&section->internal_lock, 0);
}

sc_bool sc_segment_section_lock_try(sc_segment_section * section, sc_uint16 max_attempts)
{
  sc_pointer thread = sc_thread();

  sc_assert(section != null_ptr);
  sc_uint16 attempts = 0;

lock:
{
  while (sc_atomic_int_compare_and_exchange(&section->internal_lock, 0, 1) == SC_FALSE)
  {
    if (max_attempts < attempts++)
      return SC_FALSE;
  }
}

  if (sc_atomic_pointer_get((void **)&section->thread_lock) != null_ptr &&
      sc_atomic_pointer_get((void **)&section->thread_lock) != thread)
  {
    sc_atomic_int_set(&section->internal_lock, 0);
    if (++attempts >= max_attempts)
      return SC_FALSE;
    goto lock;
  }

  sc_atomic_pointer_set((void **)&section->thread_lock, thread);
  sc_atomic_int_inc(&section->lock_count);

  sc_atomic_int_set(&section->internal_lock, 0);

  return SC_TRUE;
}

void sc_segment_section_unlock(sc_segment_section * section)
{
  sc_assert(section != null_ptr);

  while (sc_atomic_int_compare_and_exchange(&section->internal_lock, 0, 1) == SC_FALSE)
    ;

  sc_assert(sc_atomic_pointer_get((void **)&section->thread_lock) == sc_thread());

  if (sc_atomic_int_dec_and_test(&section->lock_count) == SC_TRUE)
    sc_atomic_pointer_set((void **)&section->thread_lock, 0);

  sc_atomic_int_set(&section->internal_lock, 0);
}

void sc_segment_lock(sc_segment * seg)
{
  sc_uint32 i;
  for (i = 0; i < SC_CONCURRENCY_LEVEL; ++i)
    sc_segment_section_lock(&seg->sections[i]);
}

void sc_segment_unlock(sc_segment * seg)
{
  sc_uint32 i;
  for (i = 0; i < SC_CONCURRENCY_LEVEL; ++i)
    sc_segment_section_unlock(&seg->sections[i]);
}

#include "sc_segment.h"

#include "sc_element.h"
#include "sc_storage.h"

sc_segment* sc_segment_new(sc_addr_seg num)
{
#if USE_SEGMENT_EMPTY_SLOT_BUFFER
 guint idx;
#endif

  sc_segment *segment = g_new0(sc_segment, 1);
  //  segment->type = type;

#if USE_SEGMENT_EMPTY_SLOT_BUFFER
  for (idx = (num == 0) ? 1 : 0; idx < SEGMENT_EMPTY_BUFFER_SIZE; idx++)
    segment->empty_slot_buff[idx] = idx;
  segment->empty_slot_buff_head = SEGMENT_EMPTY_BUFFER_SIZE;
  segment->have_empty_slots = TRUE;
#else
  segment->empty_slot = (num == 0) ? 1 : 0;
#endif
  segment->num = num;

  return segment;
}

void sc_segment_free(sc_segment *segment)
{
  g_assert( segment != 0);
 
  g_free(segment); 
}

gboolean sc_segment_have_empty_slot(sc_segment *segment)
{
  g_assert( segment != (sc_segment*)0 );
#if USE_SEGMENT_EMPTY_SLOT_BUFFER
  return segment->have_empty_slots;
#else
  return segment->empty_slot != SEGMENT_SIZE;
#endif
}

gboolean sc_segment_append_element(sc_segment *segment,
				   sc_element *element,
				   guint16 *id)
{
  guint slot = SEGMENT_SIZE;
  g_assert( segment != 0 );
  g_assert( element != 0 );
  
#if USE_SEGMENT_EMPTY_SLOT_BUFFER
  if (segment->empty_slot_buff_head > 0)
    slot = segment->empty_slot_buff[--segment->empty_slot_buff_head];
  else
  {
    if (segment->have_empty_slots == TRUE)
    {
      sc_segment_update_empty_slot(segment);
      if (segment->empty_slot_buff_head > 0)

	slot = segment->empty_slot_buff[--segment->empty_slot_buff_head];
    }
    else 
      return FALSE;
  }
#else
  slot = segment->empty_slot;
#endif

  if (slot >= SEGMENT_SIZE)
    return FALSE;

  segment->elements[slot] = *element;
  *id = slot;

#if USE_SEGMENT_EMPTY_SLOT
  g_print("\n%u", segment->empty_slot_buff_head);
  if (segment->empty_slot_buff_head == 0)
#endif
  sc_segment_update_empty_slot(segment);

  return TRUE;
}

sc_element* sc_segment_get_element(sc_segment *seg, guint id)
{
  g_assert(id < SEGMENT_SIZE && seg != 0);
  return &(seg->elements[id]);
}

void sc_segment_remove_element(sc_segment *segment,
			       guint el_id)
{
  g_assert( segment != (sc_segment*)0 );
  g_assert( el_id < SEGMENT_SIZE );
  segment->elements[el_id].type = 0;

#if USE_SEGMENT_EMPTY_SLOT_BUFFER
  if (segment->empty_slot_buff_head < SEGMENT_EMPTY_BUFFER_SIZE)
    segment->empty_slot_buff[segment->empty_slot_buff_head++] = el_id;
  segment->have_empty_slots = TRUE;
#else
  segment->empty_slot = el_id;
#endif 
}

void sc_segment_update_empty_slot(sc_segment *segment)
{
#if USE_SEGMENT_EMPTY_SLOT_BUFFER
  sc_segment_update_empty_slot_buffer(segment);
#else
  sc_segment_update_empty_slot_value(segment);
#endif
}

#if USE_SEGMENT_EMPTY_SLOT_BUFFER
void sc_segment_update_empty_slot_buffer(sc_segment *segment)
{
  guint idx = segment->empty_slot_buff[0];
  guint v = 0;

  // forward search
  while ((idx < SEGMENT_SIZE) && (segment->empty_slot_buff_head < SEGMENT_EMPTY_BUFFER_SIZE))
  {
    if (segment->elements[idx].type == 0)
      segment->empty_slot_buff[segment->empty_slot_buff_head++] = idx;
    idx++;
  }
  
  if (idx > 0)
  {
    // backward search
    v = (segment->num == 0) ? 1 : G_MAXUINT);
    while ((idx != v) && (segment->empty_slot_buff_head < SEGMENT_EMPTY_BUFFER_SIZE))
    {
	if (segment->elements[idx].type == 0)
	  segment->empty_slot_buff[segment->empty_slot_buff_head++] = idx;
	idx--;
    }
  }

  if (segment->empty_slot_buff_head == 0)
    segment->have_empty_slots = FALSE;
}

#else

void sc_segment_update_empty_slot_value(sc_segment *segment)
{
  guint idx;
  guint v;

#if BOUND_EMPTY_SLOT_SEARCH
  guint len;
#endif

  g_assert( segment != 0);
  //segment->empty_slot = SEGMENT_SIZE;
  // search forward
#if BOUND_EMPTY_SLOT_SEARCH
  len = 0;
  for (idx = segment->empty_slot + 1; (idx < SEGMENT_SIZE) && (len < SEGMENT_EMPTY_SEARCH_LEN); ++idx, ++len)
#else
  for (idx = segment->empty_slot + 1; idx < SEGMENT_SIZE; ++idx)
#endif
    if (segment->elements[idx].type == 0)
    {
      segment->empty_slot = idx;
      return;
    }

  // search back
  if (segment->empty_slot == (segment->num == 0 ? 1 : 0))
  {
    segment->empty_slot = SEGMENT_SIZE;
    return;
  }

  v = (segment->num == 0) ? 1 : G_MAXUINT;
#if BOUND_EMPTY_SLOT_SEARCH
  len = 0;
  for (idx = segment->empty_slot - 1; (idx != v) && (len < SEGMENT_EMPTY_SEARCH_LEN); --idx, ++len)
#else
  for (idx = segment->empty_slot - 1; idx != v; --idx)
#endif
    if (segment->elements[idx].type == 0)
    {
      segment->empty_slot = idx;
      return;
    }

  segment->empty_slot = SEGMENT_SIZE;
}

#endif // USE_SEGMENT_EMPTY_SLOT_BUFFER

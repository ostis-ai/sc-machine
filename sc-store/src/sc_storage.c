#include "sc_storage.h"

#include "sc_defines.h"
#include "sc_segment.h"
#include "sc_element.h"
#include "sc_fs_storage.h"
#include <memory.h>

GPtrArray *segments = 0;
guint seg_id = 0;
guint seg_queue[SEGS_QUEUE_SIZE];
gint seg_queue_heap = -1;
guint time_stamp = 1;

GStaticMutex seg_queue_mutex = G_STATIC_MUTEX_INIT;

// ----------------------------------- SEGMENTS QUEUE --------------------------
gboolean _sc_storage_get_segment_from_queue(sc_addr_seg *seg)
{
  gboolean res = FALSE;
  sc_segment *segment = 0;

  g_static_mutex_lock(&seg_queue_mutex);

  if (seg_queue_heap > -1)
  {
    *seg = seg_queue[seg_queue_heap];
    segment = sc_storage_get_segment(*seg, TRUE);
    g_assert( segment != 0 );
 
    if (sc_segment_have_empty_slot(segment) == TRUE)
      res = TRUE;
    else
      seg_queue_heap--;
  }

  g_static_mutex_unlock(&seg_queue_mutex);
  
  return res;
}

void _sc_storage_append_segment_to_queue(sc_addr_seg seg)
{
  g_static_mutex_lock( &seg_queue_mutex );
  if (seg_queue_heap < (SEGS_QUEUE_SIZE - 1))
    seg_queue[++seg_queue_heap] = seg;

  g_static_mutex_unlock( &seg_queue_mutex );
}

void _sc_storage_update_segment_queue_impl(gpointer data,
					    gpointer user_data)
{
  //! TODO: make synhronization on case, when segment removed, or unloaded
  sc_segment *segment;
  sc_addr_seg idx = 0;
  while ( (seg_queue_heap <= SEGS_QUEUE_SIZE) && (idx < segments->len) )
  {
    segment = sc_storage_get_segment(idx, TRUE);
    g_assert( segment != (sc_segment*)0 );
    if (sc_segment_have_empty_slot(segment) == TRUE)
      _sc_storage_append_segment_to_queue(idx);
  }
}

// -----------------------------------------------------------------------------


gboolean sc_storage_initialize(const gchar *path)
{
  g_assert( segments == (GPtrArray*)0 );

  segments = g_ptr_array_new();

  sc_fs_storage_initialize(path);
  sc_fs_storage_read_from_path(segments);

  time_stamp = 1;

  /*  seg_queue_update_pool = g_thread_pool_new(_sc_storage_update_segment_queue_impl,
					    (gpointer)0,
					    SEG_QUEUE_UPDATE_THREADS_MAX,
					    FALSE,
					    (GError**)0);
  */
  return TRUE;
}

void sc_storage_shutdown()
{
  guint idx = 0;
  g_assert( segments != (GPtrArray*)0 );

  /*g_thread_pool_free( seg_queue_update_pool,
		      TRUE, FALSE);
  seg_queue_update_pool = (GThreadPool*)0;
  */
 // sc_fs_storage_write_to_path(segments);
  sc_fs_storage_shutdown();

  for (idx = 0; idx < segments->len; idx++)
    g_free(g_ptr_array_index(segments, idx));

  g_ptr_array_free(segments, TRUE);
  segments = (GPtrArray*)0;
}

sc_segment* sc_storage_get_segment(sc_addr_seg seg, gboolean force_load)
{
  //! TODO: Make support of force loading
  g_assert( seg >= 0 && seg < segments->len );
  return (sc_segment*)g_ptr_array_index(segments, seg);
}

sc_element* sc_storage_get_element(sc_addr addr, gboolean force_load)
{
  sc_segment *segment = 0;
  sc_element *res = 0;

  if (addr.seg > segments->len) return (sc_element*)0;
  
  segment = (sc_segment*)g_ptr_array_index(segments, addr.seg);
  
  if (segment == 0)
  {
    if (force_load)
    {
      //! TODO: make force load
    }else
      return (sc_element*)0;
  }else
  {
    res = sc_segment_get_element(segment, addr.offset);
  }

  return res;//sc_segment_get_element(segment, uri.id);
}

gboolean sc_storage_is_element(sc_addr addr)
{
  sc_element *el = sc_storage_get_element(addr, TRUE);
  if (el == 0) return FALSE;
  if (el->type == 0) return FALSE;
  if (el->delete_time_stamp > 0) return FALSE;

  return TRUE;
}

void sc_storage_update_segment_queue()
{
  
}

sc_element* sc_storage_append_el_into_segments(sc_element *element, sc_addr *addr)
{
  sc_segment *segment = 0;

  g_assert( addr != 0 );

  if (_sc_storage_get_segment_from_queue(&addr->seg) == TRUE)
  {
    segment = sc_storage_get_segment(addr->seg, TRUE);
    return sc_segment_append_element(segment, element, &addr->offset);
  }

  // if element still not added, then create new segment and append element into it
  segment = sc_segment_new(segments->len);
  addr->seg = segments->len;
  g_ptr_array_add(segments, (gpointer)segment);

  _sc_storage_append_segment_to_queue(addr->seg);

  return sc_segment_append_element(segment, element, &addr->offset);
}

sc_addr sc_storage_element_new(sc_type type)
{
  sc_element el;
  sc_addr addr;

  memset(&el, 0, sizeof(el));
  el.type = type;
  el.create_time_stamp = time_stamp;

  g_assert( sc_storage_append_el_into_segments(&el, &addr) != 0);
  return addr;
}

void sc_storage_element_free(sc_addr addr)
{
  sc_type type;
  sc_segment *segment = 0;
  sc_element *el, *el2;
  sc_addr _addr;
  guint addr_int;

  GSList *remove_list = 0;

  el = el2 = 0;

  g_assert( addr.seg < segments->len );
  g_assert( addr.offset < SEGMENT_SIZE );

  remove_list = g_slist_append(remove_list, GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(addr)));

  while (remove_list != 0)
  {
    // get sc-addr for removing
    addr_int = GPOINTER_TO_UINT(remove_list->data);
    _addr.seg = SC_ADDR_LOCAL_SEG_FROM_INT(addr_int);
    _addr.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT(addr_int);

    // go to next sc-addr in list
    remove_list = g_slist_delete_link(remove_list, remove_list);

    el = sc_storage_get_element(_addr, TRUE);
    g_assert(el != 0 && el->type != 0);

    el->delete_time_stamp = time_stamp;

    // Iterate all connectors for deleted element and append them into remove_list
    _addr = el->first_out_arc;
    while (SC_ADDR_IS_NOT_EMPTY(_addr))
    {
      el2 = sc_storage_get_element(_addr, TRUE);

      // do not append elements, that have delete_time_stamp != 0
      if (el2->delete_time_stamp == 0)
	remove_list = g_slist_append(remove_list, GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_addr)));

      _addr = el2->arc.next_out_arc;
    }

    _addr = el->first_in_arc;
    while (SC_ADDR_IS_NOT_EMPTY(_addr))
    {
      el2 = sc_storage_get_element(_addr, TRUE);

      // do not append elements, that have delete_time_stamp != 0
      if (el2->delete_time_stamp == 0)
	remove_list = g_slist_append(remove_list, GUINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(_addr)));

      _addr = el2->arc.next_in_arc;
    }

    // clean temp addr
    SC_ADDR_MAKE_EMPTY(_addr);    
  }
  
}

sc_addr sc_storage_node_new(sc_type type )
{
  sc_element el;
  sc_addr addr;

  g_assert( !(sc_type_arc & type) );
  memset(&el, 0, sizeof(el));

  el.type = sc_type_node | type;

  g_assert( sc_storage_append_el_into_segments(&el, &addr) != 0 );
  return addr;
}

sc_addr sc_storage_arc_new(sc_type type,
			   sc_addr beg,
			   sc_addr end)
{
  sc_addr addr;
  sc_element el, *beg_el, *end_el, *tmp_el;

  memset(&el, 0, sizeof(el));
  g_assert( !(sc_type_node & type) );
  el.type = sc_type_arc | type;

  el.arc.begin = beg;
  el.arc.end = end;

  // get new element
  tmp_el  =  sc_storage_append_el_into_segments(&el, &addr);
  
  g_assert(tmp_el != 0);

  // get begin and end elements
  beg_el = sc_storage_get_element(beg, TRUE);
  end_el = sc_storage_get_element(end, TRUE);

  // check values
  g_assert(beg_el != 0 && end_el != 0 && beg_el->type != 0 && end_el->type != 0);

  // set next output arc for our created arc
  tmp_el->arc.next_out_arc = beg_el->first_out_arc;
  tmp_el->arc.next_in_arc = end_el->first_in_arc;

  // set our arc as first output/input at begin/end elements
  beg_el->first_out_arc = addr;
  end_el->first_in_arc = addr;
}

void sc_storage_get_elements_stat(sc_elements_stat *stat)
{
  guint s_idx, e_idx;
  sc_segment *segment;
  sc_type type;
  guint32 delete_stamp;
  g_assert( stat != (sc_elements_stat*)0 );

  memset(stat, 0, sizeof(sc_elements_stat));

  //! TODO: add loading of segment

  // iterate all elements and calculate statistics
  for (s_idx = 0; s_idx < segments->len; s_idx++)
  {
    segment = (sc_segment*)g_ptr_array_index(segments, s_idx);
    g_assert( segment != (sc_segment*)0 );
    for (e_idx = 0; e_idx < SEGMENT_SIZE; e_idx++)
    {
      type = segment->elements[e_idx].type;
      delete_stamp = segment->elements[e_idx].delete_time_stamp;
      if (type == 0)
	stat->empty_count++;
      else
	if (type & sc_type_node)
	{
	  stat->node_count++;
	  if (delete_stamp > 0)
	    stat->node_deleted++;
	}
	else
	  if (type & sc_type_arc)
	  {
	    stat->arc_count++;
	    if (delete_stamp > 0)
	      stat->arc_deleted++;
	  }
    }
  }
}
			   

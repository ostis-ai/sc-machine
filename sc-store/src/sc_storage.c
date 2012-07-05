#include "sc_storage.h"

#include "sc_defines.h"
#include "sc_segment.h"
#include "sc_element.h"
#include "sc_fs_storage.h"
#include <memory.h>

GPtrArray *segments = 0;
guint seg_id = 0;
guint seg_queue[SEGS_QUEUE_SIZE];
guint seg_queue_heap = 0;
guint time_stamp = 1;

GStaticMutex seg_queue_mutex = G_STATIC_MUTEX_INIT;

// ----------------------------------- SEGMENTS QUEUE --------------------------
guint _sc_storage_get_segment_from_queue()
{
  guint res;

  g_static_mutex_lock(&seg_queue_mutex);

  res = segments->len - 1;
  if (seg_queue_heap != 0)
    res = seg_queue[--seg_queue_heap];

  g_static_mutex_unlock(&seg_queue_mutex);
  
  return res;
}

void _sc_storage_append_segment_to_queue(guint id)
{
  g_static_mutex_lock( &seg_queue_mutex );
  if (seg_queue_heap <= SEGS_QUEUE_SIZE)
    seg_queue[seg_queue_heap++] = id;
  g_static_mutex_unlock( &seg_queue_mutex );
}

void _sc_storage_update_segment_queue_impl(gpointer data,
					    gpointer user_data)
{
  //! TODO: make synhronization on case, when segment removed, or unloaded
  sc_segment *segment;
  guint idx = 0;
  while ( (seg_queue_heap <= SEGS_QUEUE_SIZE) && (idx < segments->len) )
  {
    segment = sc_storage_get_segment(idx);
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

sc_segment* sc_storage_get_segment(guint id)
{
  g_assert( id >= 0 && id < segments->len );
  return (sc_segment*)g_ptr_array_index(segments, id);
}

sc_element* sc_storage_get_element(sc_addr addr, gboolean force_load)
{
  sc_segment *segment = 0;
  sc_element *res = 0;

  if (addr.seg > segments->len) return (sc_element*)0;
  
  // note that segment id starts with 1
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
    res = sc_segment_get_element(segment, addr.id);
  }

  return res;//sc_segment_get_element(segment, uri.id);
}

gboolean sc_storage_is_element(sc_addr addr)
{
  sc_element *el = sc_storage_get_element(addr, TRUE);
  if (el == 0) return FALSE;

  if (el->type == 0) return FALSE;

  return TRUE;
}

void sc_storage_update_segment_queue()
{
  
}

sc_addr sc_storage_append_el_into_segments(sc_element *element)
{
  sc_segment *segment = 0;
  sc_addr addr;
  if (segments->len > 0)
  {
    //if (seg_queue_heap == 0)
    //  uri.seg = segments->len - 1;
    //else
    //  uri.seg = seg_queue[--seg_queue_heap];
    addr.seg = _sc_storage_get_segment_from_queue();
    segment = sc_storage_get_segment(addr.seg);
    if (sc_segment_append_element(segment, element, &addr.id) == TRUE)
      return addr;
  }

  // if element still not added, then create new segment and append element into it
  segment = sc_segment_new(element->type & sc_type_node ? sc_type_node : sc_type_arc);
  addr.seg = segments->len;
  g_ptr_array_add(segments, (gpointer)segment);
  g_assert( sc_segment_append_element(segment, element, &addr.id) );

  return addr;
}

sc_addr sc_storage_element_new(sc_type type)
{
  sc_element el;
  el.type = type;
#if USE_PARALLEL_SEARCH
  el.create_time_stamp = time_stamp;
  el.delete_time_stamp = 0;
#endif

  return sc_storage_append_el_into_segments(&el);
}

void sc_storage_element_free(sc_addr addr)
{
  sc_type type;
  sc_segment *segment = 0;
  sc_element *el = 0;

  g_assert( addr.seg < segments->len );
  g_assert( addr.id < SEGMENT_SIZE );

  el = sc_storage_get_element(addr, TRUE);

  g_assert(el != 0 && el->type != 0);

#if USE_PARALLEL_SEARCH
  el->delete_time_stamp = time_stamp;
#endif
  
  //_sc_storage_append_segment_to_queue(el_uri.seg);
  //if (seg_queue_heap <= SEGS_QUEUE_SIZE)  
  //  seg_queue[seg_queue_heap++] = el_uri.seg;

  // just mark element, that it's dead

  //  sc_segment_remove_element(sc_storage_get_segment(el_uri.seg), el_uri.id);
}

sc_addr sc_storage_node_new(sc_type type )
{
  sc_element el;

  g_assert( !(sc_type_arc & type) );

  el.type = sc_type_node | type;

  return sc_storage_append_el_into_segments(&el);
}

sc_addr sc_storage_arc_new(sc_type type,
			   sc_addr beg,
			   sc_addr end)
{
  sc_addr addr;
  sc_element el, *beg_el, *end_el, *tmp_el;

  g_assert( !(sc_type_node & type) );
  el.type = sc_type_arc | type;

  el.incident.begin = beg;
  el.incident.end = end;

  // get ne element uri
  addr =  sc_storage_append_el_into_segments(&el);  

  // get begin and end elements
  beg_el = sc_storage_get_element(beg, TRUE);
  end_el = sc_storage_get_element(end, TRUE);

  // check values
  g_assert(beg_el != 0 && end_el != 0 && beg_el->type != 0 && end_el->type != 0);

  // set next output arc for our created arc
  el.incident.next_out_arc = beg_el->first_out_arc;
  el.incident.next_in_arc = end_el->first_in_arc;

  // set our arc as first output/input at begin/end elements
  beg_el->first_out_arc = addr;
  end_el->first_in_arc = addr;
}

void sc_storage_get_elements_stat(sc_elements_stat *stat)
{
  guint s_idx, e_idx;
  sc_segment *segment;
  sc_type type;
  g_assert( stat != (sc_elements_stat*)0 );

  memset(stat, 0, sizeof(sc_elements_stat));

  // iterate all elements and calculate statistics
  for (s_idx = 0; s_idx < segments->len; s_idx++)
  {
    segment = (sc_segment*)g_ptr_array_index(segments, s_idx);
    g_assert( segment != (sc_segment*)0 );
    for (e_idx = 0; e_idx < SEGMENT_SIZE; e_idx++)
    {
      type = segment->elements[e_idx].type;
      if (type == 0)
	stat->empty_count++;
      else
	if (type & sc_type_node)
	  stat->node_count++;
	else
	  if (type & sc_type_arc)
	    stat->arc_count++;
    }
  }
}
			   

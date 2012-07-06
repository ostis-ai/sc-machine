#include <stdio.h>
#include "sc_store.h"
#include "sc_segment.h"

#define nodes_append_count 10000000
#define nodes_remove_count 10000000
#define arcs_append_count  10000000
#define arcs_remove_count  1

const char* repo_path = "repo";
GTimer *timer = 0;

void print_storage_statistics()
{
  sc_elements_stat stat;
  
  //  printf("Get elements statistic\n");
  g_timer_reset(timer);
  sc_storage_get_elements_stat(&stat);
  g_timer_stop(timer);
  //  printf("Timer: %f\n", g_timer_elapsed(timer, 0));
  printf("--- Storage statistics: ---\n \tNodes: %u\n\tArcs: %u\n\tEmpty: %u\n---\n", 
	 stat.node_count, stat.arc_count, stat.empty_count);
}

sc_addr get_random_addr(sc_type type)
{
  sc_addr addr;
  sc_segment *segment = 0 ;

  addr.seg = g_random_int() % segments->len;
  addr.offset = 0;

  segment = g_ptr_array_index(segments, addr.seg);

  //if (segment->type & type)
  //{
  addr.offset = g_random_int() % SEGMENT_SIZE;
    //}

  return addr;
}

sc_element* get_random_element(sc_type type)
{
  sc_element *el = 0;
  sc_addr id, id2;
  sc_segment *segment = 0 ;

  id.seg = g_random_int() % segments->len;

  segment = g_ptr_array_index(segments, id.seg);

  //if (segment->type & type)
  //{
    //    uri.seg = segment->id;
    id.offset = g_random_int() % SEGMENT_SIZE;
    return sc_storage_get_element(id, FALSE);
  //}
  
  //  return (sc_element*)0;
}

void test1()
{
  guint idx = 0;
  sc_element *el1, *el2;
  sc_segment *segment = 0;
  sc_addr id, id2;
 
  sc_storage_initialize("repo");

  printf("Element size: %d\n", sizeof(sc_element));

  timer = g_timer_new();
  print_storage_statistics();
  printf("--- Node creation ---\n");
  g_timer_start(timer);
  for (idx = 0; idx < nodes_append_count; idx++)
  {
    id = sc_storage_element_new(sc_type_node);
    //g_printf("uri: %d\t%d\n", uri.seg, uri.id);
  }
  g_timer_stop(timer);
  printf("Time: %f s\n", g_timer_elapsed(timer, 0));
  printf("Nodes per second: %f\n", (float)nodes_append_count / g_timer_elapsed(timer, 0));
  printf("Segments count: %d\n", (guint)segments->len);
  print_storage_statistics();

  g_timer_reset(timer);
  printf("--- Node segmentation ---\n");
  g_timer_start(timer);
  for (idx = 0; idx < nodes_remove_count; idx++)
  {
    if (idx % 10 < 7)
    {
      id = get_random_addr(sc_type_node);
      if (sc_storage_is_element(id))
	sc_storage_element_free(id);
    }else
      sc_storage_node_new(0);
  }

  g_timer_stop(timer);
  printf("Time: %f s\n", g_timer_elapsed(timer, 0));
  printf("Elements per second: %f\n", (float)nodes_remove_count / g_timer_elapsed(timer, 0));
  printf("Segments count: %d\n", (guint)segments->len);
  print_storage_statistics();
  //  g_mem_profile();

  g_timer_reset(timer);
  printf("--- Arcs creation ---\n");
  g_timer_start(timer);
  for (idx = 0; idx < arcs_append_count; idx++)
  {
    id = get_random_addr(0);
    id2 = get_random_addr(0);
    //el1 = get_random_element(sc_type_node | sc_type_arc);
    //el2 = get_random_element(sc_type_node | sc_type_arc);
    //if (el1 == (sc_element*)0 || el2 == (sc_element*)0) continue;
    //sc_element_get_uri(el1, &uri);
    //sc_element_get_uri(el2, &uri2);

    //if (!sc_storage_get_element(uri2, TRUE) || !sc_storage_get_element(uri, TRUE)) continue;
    //if (sc_storage_get_element(uri2, TRUE)->type == 0 || sc_storage_get_element(uri, TRUE)->type == 0) continue;
    if (!sc_storage_is_element(id) || !sc_storage_is_element(id2)) continue;

    sc_storage_arc_new(0, id, id2);//, uri, uri2);
  }

  g_timer_stop(timer);
  printf("Timer: %fs\n", g_timer_elapsed(timer, 0));
  printf("Arcs per second: %f\n", (float)arcs_append_count / g_timer_elapsed(timer, 0));
  printf("Segments count: %d\n", (guint)segments->len);
  
  print_storage_statistics();

  sc_storage_shutdown();

  g_timer_destroy(timer);
}

int main(int argc, char *argv[])
{
  guint item = 1;

  while (item != 0)
  {
    printf("Commands:\n"
	   "0 - exit\n"
	   "1 - test allocation\n"
	   "\nCommand: ");
    scanf("%d", &item);

    printf("\n----- Test %d -----\n", item);

    switch(item)
    {
    case 1:
      test1();
      break;
    };

    printf("\n----- Finished -----\n");
  }

  return 0;
}

#include <stdio.h>
extern "C"
{
#include "sc_store.h"
#include "sc_segment.h"
}
#include <vector>


#define nodes_append_count 10000000
#define nodes_remove_count 10000000
#define arcs_append_count  10000000
#define arcs_remove_count  0

const char* repo_path = "repo";
GTimer *timer = 0;

std::vector<sc_addr> segment_node_del;
std::vector<sc_addr> arc_creation_vector;

void print_storage_statistics()
{
  sc_elements_stat stat;
  
  sc_storage_get_elements_stat(&stat);
 
  printf("--- Storage statistics: ---\n \tNodes: %u (%u deleted)\n\tArcs: %u (%u deleted)\n\tEmpty: %u\n---\n", 
	 stat.node_count, stat.node_deleted, stat.arc_count, stat.arc_deleted, stat.empty_count);
}

sc_addr get_random_addr(sc_type type)
{
  sc_addr addr;
  sc_segment *segment = 0 ;

  addr.seg = g_random_int() % segments->len;
  addr.offset = 0;

  segment = (sc_segment*)g_ptr_array_index(segments, addr.seg);
  addr.offset = g_random_int() % SEGMENT_SIZE;

  return addr;
}

sc_element* get_random_element(sc_type type)
{
  sc_element *el = 0;
  sc_addr id, id2;
  sc_segment *segment = 0 ;

  id.seg = g_random_int() % segments->len;

  segment = (sc_segment*)g_ptr_array_index(segments, id.seg);

  id.offset = g_random_int() % SEGMENT_SIZE;
  return sc_storage_get_element(id, FALSE);
}

bool is_sc_addr_in_segment_node_vector(sc_addr addr)
{
  guint32 n = segment_node_del.size();
  for (guint32 i = 0; i < n; i++)
  {
    if (SC_ADDR_IS_EQUAL(addr, segment_node_del[i]))
	return true;
  }

  return false;
}

void test1()
{
  guint idx = 0;
  sc_element *el1, *el2;
  sc_segment *segment = 0;
  sc_addr id, id2;
  guint32 count = 0;
 
  printf("Element size: %d bytes\n", sizeof(sc_element));
  printf("Segment size: %d elements\n", SEGMENT_SIZE);

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
  count = 0;
  
  printf("Prepare test...\n");
  for (idx = 1; idx < nodes_remove_count + 1; idx++)
  {
    if (idx % 10 < 5)
    {
      id.seg = idx / SEGMENT_SIZE;
      id.offset = idx % SEGMENT_SIZE;
      /*do
      {
	id = get_random_addr(sc_type_node);
      }
      while(!sc_storage_is_element(id) && is_sc_addr_in_segment_node_vector(id));*/
      g_assert(sc_storage_is_element(id));
      segment_node_del.push_back(id);
    }
  }

  printf("Run test...\n");
  g_timer_start(timer);

  guint32 n = segment_node_del.size();
  for (guint32 i = 0; i < n; ++i)
    sc_storage_element_free(segment_node_del[i]);

  n = nodes_remove_count - n;
  for (guint32 i = 0; i < n; i++)
    sc_storage_node_new(0);

  g_timer_stop(timer);

  segment_node_del.clear();

  printf("Time: %f s\n", g_timer_elapsed(timer, 0));
  printf("Elements per second: %f\n", (float)nodes_remove_count / g_timer_elapsed(timer, 0));
  printf("Segments count: %d\n", (guint)segments->len);
  printf("Element free calls: %u\n", segment_node_del.size());
  print_storage_statistics();
  

  g_timer_reset(timer);
  printf("--- Arcs creation ---\n");
  count = 0;

  printf("Prepare...\n");
  for (idx = 0; idx < arcs_append_count; idx++)
  {
    do
    {
      id = get_random_addr(0);
    }while (!sc_storage_is_element(id));

    do
    {
      id2 = get_random_addr(0);
    }while (!sc_storage_is_element(id2));

    arc_creation_vector.push_back(id);
    arc_creation_vector.push_back(id2);
  }

  printf("Run...\n");
  g_timer_start(timer);
  n = arc_creation_vector.size() / 2;
  for (guint32 i = 0; i < n; ++i)
  {
    sc_storage_arc_new(0, arc_creation_vector[i], arc_creation_vector[i + n]);
  }

  g_timer_stop(timer);
  printf("Timer: %fs\n", g_timer_elapsed(timer, 0));
  printf("Arcs per second: %f\n", (float)n / g_timer_elapsed(timer, 0));
  printf("Segments count: %d\n", (guint)segments->len);
  
  print_storage_statistics();

  g_timer_destroy(timer);
}

void test2()
{
  guint passed = 0;
  guint idx = 0;
  guint32 packed;
  guint32 test_count = 10000000;
  sc_addr addr, addr2;

  printf("Test sc-addr packing\n");
  passed = 0;

  timer = g_timer_new();

  g_timer_reset(timer);
  g_timer_start(timer);

  for (idx = 0; idx < test_count; idx++)
  {
    // make random addr
    addr.seg = g_random_int() % SC_ADDR_SEG_MAX;
    addr.offset = g_random_int() % SC_ADDR_OFFSET_MAX;

    // pack
    packed = SC_ADDR_LOCAL_TO_INT(addr);

    // unpack
    addr2.seg = SC_ADDR_LOCAL_SEG_FROM_INT(packed);
    addr2.offset = SC_ADDR_LOCAL_OFFSET_FROM_INT(packed);

    if (SC_ADDR_IS_NOT_EQUAL(addr, addr2))
    {
      printf("Error!\n");
      printf("Source seg=%d, offset=%d\n", addr.seg, addr.offset);
      printf("Packed=%d\n", packed);
      printf("Unpacked seg=%d, offset=%d", addr2.seg, addr2.offset);
    }else
      passed++;
  }

  g_timer_stop(timer);

  printf("Passed %d of %d tests\n", passed, idx);
  printf("Pack/Unpack operations per second: %f", test_count / g_timer_elapsed(timer, 0));

  g_timer_destroy(timer);
}

void test3()
{
  sc_addr node[10], arc[10][10];
  guint32 i, j;

  printf("Create 10 nodes and 100 arcs, that connect nodes each other\n");
  for (i = 0; i < 10; i++)
    node[i] = sc_storage_element_new(sc_type_node);

  for (i = 0; i < 10; i++)
    for (j = 0; j < 10; j++)
      arc[i][j] = sc_storage_arc_new(0, node[i], node[j]);
  
  printf("Segments count: %d\n", (guint)segments->len);
  print_storage_statistics();

  printf("Delete 5 nodes\n");
  for (i = 0; i < 5; i++)
    sc_storage_element_free(node[i]);

  printf("Segments count: %d\n", (guint)segments->len);
  print_storage_statistics();
}

int main(int argc, char *argv[])
{
  guint item = 1;

  sc_storage_initialize("repo");

  while (item != 0)
  {
    printf("Commands:\n"
	   "0 - exit\n"
	   "1 - test allocation\n"
	   "2 - test sc-addr utilities\n"
	   "3 - test arc deletion\n"
	   "\nCommand: ");
    scanf("%d", &item);

    printf("\n----- Test %d -----\n", item);

    switch(item)
    {
    case 1:
      test1();
      break;

    case 2:
      test2();
      break;
    case 3:
      test3();
      break;
    };

    printf("\n----- Finished -----\n");
  }

  sc_storage_shutdown();

  return 0;
}

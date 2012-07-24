#include <stdio.h>
#include <glib.h>

#include <libsc/libsc.h>
#include <libsc/segment_utils.h>
#include <libsc/sc_segment.h>

#define NODE_ALLOC_COUNT   5000000
#define NODE_SEGMENT_COUNT 5000000
#define ARC_ALLOC_COUNT    5000000

GTimer *timer = 0;
sc_session *s = 0;
sc_segment *seg = 0;

std::vector<sc_addr> segment_node_del;
std::vector<sc_addr> nodes_vector;
std::vector<sc_addr> arcs_vector;

double profile_func( void(*testFunc)(void) )
{
  double res = 0;

  g_timer_reset(timer);
  g_timer_start(timer);

  (*testFunc)();

  g_timer_stop(timer);

  res = g_timer_elapsed(timer, 0);

  printf("Elapsed time: %f s\n", res);

  return res;
}

void _allocate_nodes()
{
  sc_addr addr;

  for (guint32 i = 0; i < NODE_ALLOC_COUNT; ++i)
  {
    addr = s->create_el(seg, SC_NODE);
  }
}

void testNodeAllocation()
{
  printf("Test node allocation\n");
  printf("Number of nodes: %d\n", NODE_ALLOC_COUNT);

  double res = profile_func(&_allocate_nodes);

  printf("Nodes per second: %f\n", (float)(NODE_ALLOC_COUNT) / res);
}

void _segment_nodes()
{
  guint32 n = segment_node_del.size();
  for (guint32 i = 0; i < n; ++i)
  {
    s->erase_el(segment_node_del[i]);
  }

  n = NODE_SEGMENT_COUNT - n;
  for (guint32 i = 0; i < n; ++i)
    s->create_el(seg, SC_NODE);
}

void testNodeSegmentation()
{
  printf("Test node segmentation\n");
  printf("Number of process nodes: %d\n", NODE_SEGMENT_COUNT);

  sc_segment::iterator *it = seg->create_iterator_on_elements();
  guint32 deleted = 0;
  guint32 iterated = 0;
  while (!it->is_over())
  {
    sc_addr_ll addr_ll = it->next();
    // skip links
    if (!seg->is_link(addr_ll))
    {
      sc_addr addr = seg->get_element_addr(addr_ll);
      if (iterated % 10 < 5)
      {
	segment_node_del.push_back(addr);
	deleted++;
      }

      iterated++;
    }
  }

  printf("%d of %d nodes will be deleted\n", deleted, iterated);

  double res = profile_func(&_segment_nodes);

  printf("Nodes per second: %f\n", (float)(NODE_SEGMENT_COUNT) / res);
}

void _allocate_arcs()
{
  guint32 n = arcs_vector.size() / 2;
  for (guint32 i = 0; i < n; ++i)
  {
    sc_addr addr = s->create_el(seg, SC_ARC);
    s->set_beg(addr, arcs_vector[i]);
    s->set_end(addr, arcs_vector[i + n]);
  }
}

void testArcsCreation()
{
  printf("Test arcs creation\n");
  printf("Number of arcs: %d\n", NODE_SEGMENT_COUNT);

  sc_segment::iterator *it = seg->create_iterator_on_elements();
  while (!it->is_over())
  {
    sc_addr_ll addr_ll = it->next();
    // skip links
    if (!seg->is_link(addr_ll))
    {
      sc_addr addr = seg->get_element_addr(addr_ll);
      nodes_vector.push_back(addr);
    }
  }

  guint32 n = nodes_vector.size();
  for (guint32 i = 0; i < ARC_ALLOC_COUNT; ++i)
  {
    arcs_vector.push_back(nodes_vector[g_random_int() % n]);
    arcs_vector.push_back(nodes_vector[g_random_int() % n]);
  }

  double res = profile_func(&_allocate_arcs);

  printf("Arcs per second: %f\n", (float)(ARC_ALLOC_COUNT) / res);
}

int main(int argc, char** argv)
{
  timer = g_timer_new();

  // initialize sc-memory
  printf("Initialize sc-core...\n");
  libsc_init();
  s = libsc_login();
  printf("Create temporary segment...\n");
  seg = create_unique_segment(s, "/tmp/test");

  printf("---\n");
  testNodeAllocation();
  printf("---\n");
  testNodeSegmentation();
  printf("---\n");
  testArcsCreation();

  printf("\nPress any key...\n");
  getchar();
  
  printf("Shutdown sc-core...\n");
  libsc_deinit();

  g_timer_destroy(timer);
  timer = 0;

  return 0;
}

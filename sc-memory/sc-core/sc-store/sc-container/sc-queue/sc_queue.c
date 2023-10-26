/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_queue.h"

#include "../../sc-base/sc_allocator.h"

void sc_queue_init(sc_queue ** queue)
{
  *queue = sc_mem_new(sc_queue, 1);
  (*queue)->begin = null_ptr;
  (*queue)->end = null_ptr;
}

void sc_queue_destroy(sc_queue * queue)
{
  while (queue->begin)
    sc_queue_pop(queue);
  sc_mem_free(queue);
}

void sc_queue_push(sc_queue * queue, void * data)
{
  sc_queue_node * new_node = sc_mem_new(sc_queue_node, 1);
  new_node->data = data;
  new_node->next = null_ptr;

  if (!queue->begin || !queue->end)
  {
    queue->begin = new_node;
    queue->end = new_node;
  }
  else
  {
    queue->end->next = new_node;
    queue->end = new_node;
  }
}

void * sc_queue_pop(sc_queue * queue)
{
  void * data = null_ptr;
  if (queue->begin == null_ptr)
    return data;

  sc_queue_node * temp = queue->begin;
  queue->begin = queue->begin->next;
  data = temp->data;
  sc_mem_free(temp);

  if (!queue->begin)
    queue->end = null_ptr;

  return data;
}

void * sc_queue_front(sc_queue * queue)
{
  if (queue->begin)
    return queue->begin->data;

  return null_ptr;
}

sc_bool sc_queue_empty(sc_queue * queue)
{
  return queue == null_ptr || queue->begin == null_ptr;
}

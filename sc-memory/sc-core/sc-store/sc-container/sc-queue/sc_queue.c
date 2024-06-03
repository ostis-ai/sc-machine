/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_queue.h"

#include "../../sc-base/sc_allocator.h"

#define INITIAL_CAPACITY 4

void sc_queue_init(sc_queue * queue)
{
  queue->data = sc_mem_new(void *, INITIAL_CAPACITY);
  queue->front = 0;
  queue->back = -1;
  queue->size = 0;
  queue->capacity = INITIAL_CAPACITY;
}

void sc_queue_destroy(sc_queue * queue)
{
  sc_mem_free(queue->data);
  queue->front = 0;
  queue->back = -1;
  queue->size = 0;
  queue->capacity = 0;
}

void sc_queue_resize(sc_queue * queue)
{
  sc_int32 new_capacity = queue->capacity * 2;
  void ** new_data = sc_mem_realloc(queue->data, new_capacity, new_capacity * sizeof(void *));
  queue->data = new_data;
  queue->capacity = new_capacity;

  if (queue->front > queue->back)
  {
    for (int i = 0; i < queue->front; ++i)
      queue->data[i + queue->capacity / 2] = queue->data[i];

    queue->back += queue->capacity / 2;
  }
}

void sc_queue_push(sc_queue * queue, void * data)
{
  if (queue->size == queue->capacity)
    sc_queue_resize(queue);

  queue->back = (queue->back + 1) % queue->capacity;
  queue->data[queue->back] = data;
  queue->size++;
}

void * sc_queue_pop(sc_queue * queue)
{
  if (queue->size == 0)
    return null_ptr;

  void * data = queue->data[queue->front];
  queue->front = (queue->front + 1) % queue->capacity;
  queue->size--;
  return data;
}

void * sc_queue_front(sc_queue * queue)
{
  if (queue->size == 0)
    return null_ptr;

  return queue->data[queue->front];
}

sc_bool sc_queue_empty(sc_queue * queue)
{
  return queue->size == 0;
}

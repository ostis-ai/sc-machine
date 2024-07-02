/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_queue.h"

#include "../../sc-base/sc_allocator.h"

#define INITIAL_CAPACITY 4
#define RESIZE_INCREMENT 12

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
  sc_int32 const new_capacity = queue->capacity + RESIZE_INCREMENT;
  void ** new_data = sc_mem_new(void *, new_capacity);

  if (queue->front <= queue->back)
    sc_mem_cpy(new_data, &queue->data[queue->front], queue->size * sizeof(void *));
  else
  {
    sc_int32 const front_part_size = queue->capacity - queue->front;
    sc_mem_cpy(new_data, &queue->data[queue->front], front_part_size * sizeof(void *));
    sc_mem_cpy(&new_data[front_part_size], queue->data, (queue->back + 1) * sizeof(void *));
  }

  sc_mem_free(queue->data);
  queue->data = new_data;
  queue->capacity = new_capacity;
  queue->front = 0;
  queue->back = queue->size - 1;
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

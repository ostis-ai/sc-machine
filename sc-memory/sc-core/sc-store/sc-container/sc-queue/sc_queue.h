/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_queue_h_
#define _sc_queue_h_

#include "../../sc_types.h"

#include "../sc_struct_node.h"
#include "../sc-iterator/sc_container_iterator.h"

typedef struct sc_queue_node
{
  void * data;
  struct sc_queue_node * next;
} sc_queue_node;

typedef struct
{
  sc_queue_node * begin;
  sc_queue_node * end;
} sc_queue;

// Initialize the queue
void sc_queue_init(sc_queue ** queue);

// Destroy the queue
void sc_queue_destroy(sc_queue * queue);

// Insert an element at the back of the queue
void sc_queue_push(sc_queue * queue, void * data);

// Remove the element from the front of the queue
void * sc_queue_pop(sc_queue * queue);

// Return the front element
void * sc_queue_front(sc_queue * queue);

// Check if the queue is empty
sc_bool sc_queue_empty(sc_queue * queue);

#endif

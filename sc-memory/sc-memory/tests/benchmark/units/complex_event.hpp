/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "memory_test.hpp"
#include "sc_event_queue.h"

#include "sc-core/sc_event_subscription.h"
#include "sc_event_private.h"

#include "sc-store/sc_storage.h"
#include "sc-store/sc_storage_private.h"
#include "sc_memory_private.h"
#include "sc-core/sc_memory.h"

#include "sc-core/sc-base/sc_allocator.h"
#include "sc-store/sc-container/sc_struct_node.h"
#include "sc-core/sc_event_subscription.h"


class ComplexEventTest : public TestMemory
{
public:
  void Run()
  {
  ScAddr SubscriptionAddr = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr SubscriptionAddr2 = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr EventType = m_ctx->CreateNode(ScType::NodeConst);
  sc_list * eventsList1 = null_ptr;
  sc_list * eventsList2 = null_ptr;
    sc_list_init(&eventsList1);
    sc_list_init(&eventsList2);


  sc_event_subscription event1 =  sc_complex_event_subscription_new(*m_ctx, SubscriptionAddr, EventType, NULL, NULL, NULL, SC_FALSE, *eventsList1); 
  sc_event_subscription event2 = sc_complex_event_subscription_new(*m_ctx, SubscriptionAddr2, EventType, NULL, NULL, NULL, SC_FALSE, *eventsList2);  

  // Создаем сложное событие, которое сработает только когда оба простых события сработают
  sc_event_subscription complexEvent = sc_complex_event_subscription_new(*m_ctx, SubscriptionAddr, EventType, NULL, NULL, NULL, SC_TRUE, NULL); 


  sc_event_subscription_add_one_complex_event(*complexEvent, *event1);
  sc_event_subscription_add_one_complex_event(*complexEvent, *event2);


  ScAddr Connector = m_ctx->CreateNode(ScType::EdgeAccessConstPosPerm);
  ScAddr OtherAddr = m_ctx->CreateNode(ScType::NodeConst);

  sc_result complexEventTriggered = start_check_condition_to_activate_complex_event(*complexEvent, *m_ctx, SubscriptionAddr, EventType,
                                                                                    Connector, sc_type_unknown, OtherAddr, NULL, NULL, event1);
  

  // Эмитируем первое событие - сложное событие не должно сработать
  sc_event_emit(*m_ctx, SubscriptionAddr, EventType,  Connector, sc_type_unknown, OtherAddr, NULL, event1);
  EXPECT_FALSE(complexEventTriggered == SC_RESULT_OK);

  // Эмитируем второе событие - теперь сложное событие должно сработать
  sc_event_emit(*m_ctx, SubscriptionAddr2, EventType,  Connector, sc_type_unknown, OtherAddr, NULL, event2);
  EXPECT_TRUE(complexEventTriggered == SC_RESULT_OK);

  // Уничтожаем события
  sc_event_subscription_destroy(complexEvent);
  sc_event_subscription_destroy(event2);
  sc_event_subscription_destroy(event1);
  }
};


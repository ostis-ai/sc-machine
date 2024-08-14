# **C++ Agent context API**

!!! warning
    This documentation is correct for only versions of sc-machine that >= 0.10.0.
---

This API provides additional sc-memory methods that simplify working with it and provide additional methods to work with sc-events and agents.

## **ScAgentContext**

The `ScAgentContext` class is derived from `ScMemoryContext` class and provides functionality for managing agent-specific operations, including event subscriptions, actions, and structures within the sc-machine.

!!! note
    All API methods are thread-safe.

!!! note
    To include this API provide `#include <sc-memory/sc_agent_context.hpp>` in your hpp source.

!!! warning
    Objects of `ScAgentContext` class are movable, but not copyable.

### **CreateElementaryEventSubscription**

This method can be useful when you want create sc-event subscription, but don't want to use agents for this. `CreateElementaryEventSubscription` creates sc-event subscription with specified subscription (listen) sc-element and on-event callback. To learn more about sc-event see [**C++ Events API**](events.md), to learn more about sc-event subscriptions see [**C++ Event subscriptions API**](event_subscriptions.md).

```cpp
...
// Create or use existing sc-agent context.
ScAgentContext context;

// Create sc-event subscription (listen) sc-element.
ScAddr const & nodeAddr = context.CreateNode(ScType::NodeConst);
// Choose sc-event type to subscribe for.
using MyEventType = ScEventAddOutputArc<ScType::EdgeAccessConstPosPerm>;
// Create sc-event subscription for created sc-node (listen sc-element) 
// and provide on-event callback.
auto eventSubscription = context.CreateElementaryEventSubscription<MyEventType>(
  nodeAddr,
  [](MyEventType const & event) -> void
  {
    // Handle sc-event.
  });

// Initiate sc-event. Create sc-arc from `nodeAddr`.
ScAddr const & otherNodeAddr = context.CreateNode(ScType:::NodeConst);
context.CreateEdge(ScType::EdgeAccessConstPosPerm, nodeAddr, otherNodeAddr);
// After that sc-arc will be created, sc-event will occur 
// and specified on-event callback will be called.
...
```

!!! note
    You can provide empty on-event callback `{}`.

!!! warning
    You should provide valid subscription sc-element. Otherwise, exception will be thrown.

If you don't know in advance what sc-event you need to subscribe for, you can use override version of this method.

```cpp
...
// Create sc-event subscription (listen) sc-element.
ScAddr const & nodeAddr = context.CreateNode(ScType::NodeConst);
// Choose sc-event type to subscribe or find it.
ScAddr const & eventClassAddr = ScKeynodes::sc_event_add_output_arc;
// Create sc-event subscription for created sc-node (listen sc-element) 
// and provide on-event callback.
auto eventSubscription = context.CreateElementaryEventSubscription(
  eventClassAddr,
  nodeAddr,
  [](ScElementaryEvent const & event) -> void
  {
    // Handle sc-event.
  });

// Initiate sc-event. Create sc-arc from `nodeAddr`.
ScAddr const & otherNodeAddr = context.CreateNode(ScType:::NodeConst);
context.CreateEdge(ScType::EdgeAccessConstPosPerm, nodeAddr, otherNodeAddr);
// After that sc-arc will be created, sc-event will occur 
// and specified on-event callback will be called.
...
```

!!! warning
    A sc-event class must be valid and must belong to `sc_event` class.

### **CreateEventWaiter**

You can create waiter for some sc-event. It is useful when your agent should wait other agent.

```cpp
...
// Find sc-event subscription (listen) sc-element.
ScAddr const & nodeAddr = context.HelperFindBySystemIdtf("my_node");
// Choose sc-event type to subscribe for.
using MyEventType = ScEventAddOutputArc<ScType::EdgeAccessConstPosPerm>;
// Create sc-event waiter for created sc-node (listen sc-element).
auto eventWaiter = context.CreateEventWaiter<MyEventType>(
  nodeAddr,
  []() -> void
  {
    // Here you can provide some code, that will be called
    // when you will `Wait` method.
    // For example, it will be code, that initiate some sc-event.
    // By default, this callback is empty.
  });

// After creation, call method `Wait` and specify time while you 
// will wait sc-event for specified subscription sc-element
eventWaiter->Wait(200); // milliseconds
// By default, this wait time equals to 5000 milliseconds.
// You will wait until sc-event occurs or until specified time expires.

...
// Some other code should initiate sc-event (create sc-arc from `nodeAddr`).
ScAddr const & otherNodeAddr = context.CreateNode(ScType:::NodeConst);
context.CreateEdge(ScType::EdgeAccessConstPosPerm, nodeAddr, otherNodeAddr);
// After that sc-arc will be created and waiter will be resolved.
...
```

!!! warning
    You should provide valid subscription sc-element. Otherwise, exception will be thrown.

Just like for the method of creating sc-event subscription, if you don't know sc-event class in advance, you can create waiters dynamically.

```cpp
...
// Find sc-event subscription (listen) sc-element.
ScAddr const & nodeAddr = context.HelperFindBySystemIdtf("my_node");
// Choose sc-event type to subscribe or find it.
ScAddr const & eventClassAddr = ScKeynodes::sc_event_add_output_arc;
// Create sc-event waiter for created sc-node (listen sc-element)   
// and provide on-event callback.
auto eventWaiter = context.CreateEventWaiter(
  eventClassAddr,
  nodeAddr);
...
```

!!! warning
    A sc-event class must be valid and must belong to `sc_event` class.

### **CreateEventWaiterWithCondition**

In addition to the waiting time, you can also specify check that will be called when sc-event to which we have subscribed occurs.

```cpp
...
// Find sc-event subscription (listen) sc-element.
ScAddr const & nodeAddr = context.HelperFindBySystemIdtf("my_node");
// Choose sc-event type to subscribe for.
using MyEventType = ScEventAddOutputArc<ScType::EdgeAccessConstPosPerm>;
// Create sc-event waiter for created sc-node (listen sc-element).
auto eventWaiter = context.CreateEventWaiterWithCondition<MyEventType>(
  nodeAddr,
  []() -> void
  {
    // Here you can also provide some code, that will be called
    // when you will `Wait` method.
    // For example, it will be code, that initiate some sc-event.
    // By default, this callback is empty.
  },
  [](MyEventType const & event) -> sc_bool
  {
    // Here you can specify check for sc-event that will occur.
    // This check should return bool value.
    // By default, this check is empty `{}`.
  });

eventWaiter->Wait(200); // milliseconds
// You will wait until sc-event occurs or until specified time expires.

...
// Some other code should initiate sc-event (create sc-arc from `nodeAddr`).
ScAddr const & otherNodeAddr = context.CreateNode(ScType:::NodeConst);
context.CreateEdge(ScType::EdgeAccessConstPosPerm, nodeAddr, otherNodeAddr);
// After that sc-arc will be created and waiter will be resolved.
...
```

Waiters with condition are very useful, when you want to call some agent and wait until this agent will finish work.

```cpp
...
// Create action and specify class for it.
ScAddr const & actionAddr = context.CreateNode(ScType::NodeConst);
context.CreateEdge(
    ScType::EdgeAccessConstPosPerm, MyKeynodes::my_action_class, actionAddr);
// Create sc-event waiter for created action (listen sc-element).
// You should wait while sc-arc in action from `action_finished_successfully`
// will created.
auto eventWaiter = context.CreateEventWaiterWithCondition<
  ScEventAddInputArc<ScType::EdgeAccessConstPosPerm>>(
  actionAddr,
  [&]() -> void
  {
    // Initiate sc-event here.
    context.CreateEdge(
        ScType::EdgeAccessConstPosPerm, 
        ScKeynodes::action_initiated, 
        actionAddr);
  },
  [](ScEventAddInputArc<
        ScType::EdgeAccessConstPosPerm> const & event) -> sc_bool
  {
    // Check sc-event here.
    return context.HelperCheckEdge(
        ScKeynodes::action_finished_successfully, 
        event.GetSubscriptionElement(), 
        ScType::EdgeAccessConstPosPerm);
  });

eventWaiter->Wait(200); // milliseconds
// You will wait until sc-event occurs or until specified time expires.
...
```

You can also create waiters with conditions dynamically, providing sc-event class from knowledge base.

---

### **SubscribeAgent**

`ScModule` provides API to register agents statically (see [**C++ Modules API**](modules.md)). `ScAgentContext` also provides methods to register agents. It can be useful if you want to register agents dynamically.

To subscribe agent class to sc-event use `SubscribeAgent`.

```cpp
...
// For agent classes derived from `ScAgent` class write so.
context.SubscribeAgent<MyAgent>(mySubscriptionElementAddr);

// For agent classes derived from `ScActionAgent` class write so.
context.SubscribeAgent<MyActionAgent>();
...
```

### **UnsubscribeAgent**

To unsubscribe agent class to sc-event use `UnsubscribeAgent`.

```cpp
...
// For agent classes derived from `ScAgent` class write so.
context.UnsubscribeAgent<MyAgent>(mySubscriptionElementAddr);

// For agent classes derived from `ScActionAgent` class write so.
context.UnsubscribeAgent<MyActionAgent>();
...
```

!!! note
    If you registered some agent, you should unregister it after. Otherwise, when you will stop the sc-machine, warnings about it will be gotten.

!!! warning
    You must provide valid subscription sc-elements.

### **LoadAndSubscribeAgent**

Also, you can subscribe agent according to specification provided in knowledge base. Instead of subscription sc-elements, you should provide agent implementations.

```cpp
...
context.LoadAndSubscribeAgent<MyAgent>(agentImplementationAddr);
...
```

!!! note
    If some specification elements will be not found, then exceptions will be thrown. Sure, that specification is full, and that it is loaded to knowledge base. This method doesn't load specification to knowledge base for specified agent class.

This method finds specification of abstract agent for specified agent implementation and uses this specification to register specified agent.

### **DestroyAndUnsubscribeAgent**

```cpp
...
context.DestroyAndUnsubscribeAgent<MyAgent>(agentImplementationAddr);
...
```

This method finds specification of abstract agent for specified agent implementation and uses this specification to unregister specified agent.

!!! note
    If some specification elements will be not found, then exceptions will be thrown. Sure, that specification is full, and that it is loaded to knowledge base. This method doesn't load specification to knowledge base for specified agent class.

!!! warning
    You must provide valid agent implementations.

---

### **CreateAction**

All agents interpreter actions. We provide API to work with them. Use `CreateAction` to create object of `ScAction` class. To learn more about actions see [**C++ Action API**](actions.md).

```cpp
// Find action class and create action.
...
ScAction action = context.CreateAction(actionClassAddr);
...
```

### **UseAction**

```cpp
// Or find action and use it.
...
ScAction action = context.UseAction(actionAddr);
...
```

### **CreateSet**

We provide API to easy work with sets and structures in knowledge base.

```cpp
// Create new set.
...
ScSet set = context.CreateSet();
...
```

### **UseSet**

```cpp
// Or find set and use it.
...
ScSet set = context.UseSet(setAddr);
...
```

### **CreateStructure**

```cpp
// Create new structure.
...
ScStructure structure = context.CreateStructure();
...
```

### **UseStructure**


```cpp
// Or find set and use it.
...
ScSet set = context.UseStructure(structureAddr);
...
```

--- 

## **Frequently Asked Questions**

<!-- no toc -->
- [How do I add my method to a `ScAgentContext`?](#how-do-i-add-my-method-to-a-scagentcontext)

### **How do I add my method to a `ScAgentContext`?**

You can create a class that will inherit `ScAgentContext` class. If you want to use it in your agent, do this:

```cpp
class MyContext : public ScAgentContext
{
public:
  // New methods.
};

class MyAgent : public ScAgent<MyEventType, MyContext>
{
public:
  // Agent methods.
};
```

You will be able to use context of class, which you specify, in the agent's methods.

# **C++ Agent context API**

!!! warning
    This documentation is correct for only versions of sc-machine that >= 0.10.0.
---

This API provides additional sc-memory methods that simplify working with it and provide additional methods to work with sc-events and agents.

## **ScAgentContext**

The `ScAgentContext` class is inherited from `ScMemoryContext` class and provides functionality for managing agent-specific operations, including event subscriptions, actions, and structures within the sc-machine.

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
using MyEventType = ScEventAfterGenerateOutgoingArc<ScType::EdgeAccessConstPosPerm>;
// Create sc-event subscription for generated sc-node (listen sc-element) 
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
// After that sc-arc will be generated, sc-event will occur 
// and specified on-event callback will be called.
...
```

!!! note
    You can provide empty on-event callback `{}` in `CreateElementaryEventSubscription`.

!!! warning
    You should provide valid subscription sc-element. Otherwise, exception will be thrown.

If you don't know in advance what sc-event you need to subscribe for, you can use override version of this method.

```cpp
...
// Create sc-event subscription (listen) sc-element.
ScAddr const & nodeAddr = context.CreateNode(ScType::NodeConst);
// Choose sc-event type to subscribe or find it.
ScAddr const & eventClassAddr = GetSomeEventType(); // User-specified method.
// Create sc-event subscription for generated sc-node (listen sc-element) 
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
// After that sc-arc will be generated, sc-event will occur 
// and specified on-event callback will be called.
...
```

!!! warning
    A sc-event class must be valid and must belong to `sc_event` class.
  
!!! note 
    All sc-event classes provided by sc-machine always belongs to `sc_event` class.

### **CreateEventWaiter**

You can create waiter for some sc-event. It is useful when your agent should wait other agent.

```cpp
...
// Find sc-event subscription (listen) sc-element.
ScAddr const & nodeAddr = context.HelperFindBySystemIdtf("my_node");
// Choose sc-event type to subscribe for.
using MyEventType = ScEventAfterGenerateOutgoingArc<ScType::EdgeAccessConstPosPerm>;
// Create sc-event waiter for generated sc-node (listen sc-element).
auto eventWaiter = context.CreateEventWaiter<MyEventType>(
  nodeAddr,
  []() -> void
  {
    // Here you can provide some code, that will be called
    // when you will call `Wait` method.
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
// After that sc-arc will be generated and waiter will be resolved.
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
ScAddr const & eventClassAddr = GetSomeEventType(); // User-specified method.
// Create sc-event waiter for generated sc-node (listen sc-element)   
// and provide on-event callback.
auto eventWaiter = context.CreateEventWaiter(
  eventClassAddr,
  nodeAddr);
...
```

!!! warning
    A sc-event class must be valid and must belong to `sc_event` class.

### **CreateConditionWaiter**

In addition to the waiting time, you can also specify check that will be called when sc-event to which we have subscribed occurs.

```cpp
...
// Find sc-event subscription (listen) sc-element.
ScAddr const & nodeAddr = context.HelperFindBySystemIdtf("my_node");
// Choose sc-event type to subscribe for.
using MyEventType = ScEventAfterGenerateOutgoingArc<ScType::EdgeAccessConstPosPerm>;
// Create sc-event waiter for generated sc-node (listen sc-element).
auto eventWaiter = context.CreateConditionWaiter<MyEventType>(
  nodeAddr,
  []() -> void
  {
    // Here you can also provide some code, that will be called
    // when you will call `Wait` method.
    // For example, it will be code, that initiate some sc-event.
    // By default, this callback is empty.
  },
  [](MyEventType const & event) -> bool
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
// After that sc-arc will be generated and waiter will be resolved.
...
```

Waiters with condition are very useful, when you want to call some agent and wait until this agent will finish work.

```cpp
...
// Create action and specify class for it.
ScAddr const & actionAddr = context.CreateNode(ScType::NodeConst);
context.CreateEdge(
    ScType::EdgeAccessConstPosPerm, MyKeynodes::my_action_class, actionAddr);
// Create sc-event waiter for generated action (listen sc-element).
// You should wait while sc-arc in action from `action_finished_successfully`
// will generated.
auto eventWaiter = context.CreateConditionWaiter<
  ScEventAfterGenerateIncomingArc<ScType::EdgeAccessConstPosPerm>>(
  actionAddr,
  [&]() -> void
  {
    // Initiate sc-event here.
    context.CreateEdge(
        ScType::EdgeAccessConstPosPerm, 
        ScKeynodes::action_initiated, 
        actionAddr);
  },
  [](ScEventAfterGenerateIncomingArc<
        ScType::EdgeAccessConstPosPerm> const & event) -> bool
  {
    // Check sc-event here.
    return event.GetOtherElement()
      == ScKeynodes::action_finished_successfully;
  });

eventWaiter->Wait(200); // milliseconds
// You will wait until sc-event occurs or until specified time expires.
...
```

You can also create waiters with conditions dynamically, providing sc-event class from knowledge base.

---

### **SubscribeAgent**

`ScModule` provides API to subscribe agents to sc-events statically (see [**C++ Modules API**](modules.md)). `ScAgentContext` also provides methods to subscribe agents to sc-events. It can be useful if you want to subscribe agents to sc-events dynamically.

To subscribe agent class to sc-event use `SubscribeAgent`.

```cpp
...
// For agent classes inherited from `ScAgent` class write so.
context.SubscribeAgent<MyAgent>(mySubscriptionElementAddr);

// For agent classes inherited from `ScActionInitiatedAgent` class write so.
context.SubscribeAgent<MyActionAgent>();
...
```

### **UnsubscribeAgent**

To unsubscribe agent class to sc-event use `UnsubscribeAgent`.

```cpp
...
// For agent classes inherited from `ScAgent` class write so.
context.UnsubscribeAgent<MyAgent>(mySubscriptionElementAddr);

// For agent classes inherited from `ScActionInitiatedAgent` class write so.
context.UnsubscribeAgent<MyActionAgent>();
...
```

!!! note
    If you subscribed some agent to sc-event, you should unsubscribe it from this sc-event after. Otherwise, when you will stop the sc-machine, warnings about it will be gotten.

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

This method finds specification of abstract agent for specified agent implementation and uses this specification to subscribe specified agent to sc-event.

### **DestroyAndUnsubscribeAgent**

```cpp
...
context.DestroyAndUnsubscribeAgent<MyAgent>(agentImplementationAddr);
...
```

This method finds specification of abstract agent for specified agent implementation and uses this specification to unsubscribe specified agent from sc-event.

!!! note
    If some specification elements will be not found, then exceptions will be thrown. Sure, that specification is full, and that it is loaded to knowledge base. This method doesn't load specification to knowledge base for specified agent class.

!!! warning
    You must provide valid agent implementations.

---

### **CreateAction**

All agents perform actions. We provide API to work with them. Use `CreateAction` to create object of `ScAction` class. To learn more about actions see [**C++ Action API**](actions.md).

```cpp
// Find action class and create action.
...
ScAction action = context.CreateAction(actionClassAddr);
...
```

### **ConvertToAction**

```cpp
// Or find action and convert it to object of `ScAction` class.
...
ScAction action = context.ConvertToAction(actionAddr);
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

### **ConvertToSet**

```cpp
// Or find set and convert it to object of `ScSet` class.
...
ScSet set = context.ConvertToSet(setAddr);
...
```

### **CreateStructure**

```cpp
// Create new structure.
...
ScStructure structure = context.CreateStructure();
...
```

### **ConvertToStructure**


```cpp
// Or find structure and convert it to object of `ScSet` class.
...
ScStructure structure = context.ConvertToStructure(structureAddr);
...
```

--- 

## **Frequently Asked Questions**

<!-- no toc -->
- [How do I add my method to a `ScAgentContext`?](#how-do-i-add-my-method-to-a-scagentcontext)
- [How can I wait for an event indefinitely?](#how-can-i-wait-for-an-event-indefinitely)
- [What if I need to initiate agent on events from more then one different node?](#what-if-i-need-to-initiate-agent-on-events-from-more-then-one-different-node)
- [When ScMultiset will be implemented?](#when-scmultiset-will-be-implemented)
- [What do I need to initiate agent on `ScEventGeneratingIncomingArc` and `ScEventGeneratingOutgoingArc` for one node?](#what-do-i-need-to-initiate-agent-on-sceventgeneratingincomingarc-and-sceventgeneratingoutgoingarc-for-one-node)

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

### **How can I wait for an event indefinitely?**

By default, you can wait up to 5 seconds for an event to appear. You can wait for another time convenient for you. But we recommend waiting as long as you need, no longer. You don't need to create waiters for several minutes or hours when the agent performs much faster, in milliseconds.

### **What if I need to initiate agent on events from more then one different node?**

You can subscribe an agent to many different sc-elements, but for the same type of sc-event. You can use `SubscribeAgent` and `UnsubscribeAgent` to do this.

```cpp
...
context.SubscribeAgent<MyAgent>(nodeAddr1, nodeAddr2);
// Here, you can provide array of subscriptions elements only 
// for agent classes inherited from `ScAgent` class.
```

But remember, that you have to unsubscribe the agent while the system is shutting down.

```cpp
...
context.UnsubscribeAgent<MyAgent>(nodeAddr1, nodeAddr2);
```

### **When ScMultiset will be implemented?**

Now, `ScSet` and `ScStructure` are implemented only. `ScSet` stores unique sc-elements. `ScMultiset` will be implemented soon.

### **What do I need to initiate agent on `ScEventGeneratingIncomingArc` and `ScEventGeneratingOutgoingArc` for one node?**

You can subscribe an agent to sc-event of generating sc-connector -- `ScEventGeneratingConnector`. Then the agent will be triggered on incoming and outgoing sc-arcs.

### **What's the difference between `ScSet` and `ScStructure`?**

`ScSet` is a class that represents sc-set. It is an entity that links one or more entities into a whole. `ScStructure` is a class represents sc-structure. It is a set of sc-elements, which includes sc-connections or sc-structures linking these sc-elements. Removal of one of sc-elements of the sc-structure may lead to violation of the integrity of this sc-structure. `ScSet` and `ScStructure`  do not differ functionally. `ScStructure` is a stricter representation of sc-set.

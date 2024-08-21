# **C++ Waiters API**

!!! warning
    This documentation is correct for only versions of sc-machine that >= 0.10.0.
--- 

This API provides functionality to implement waiters on C++.

## **ScWaiter** and **ScConditionWaiter**

This type of objects is used to wait until some event emits. It is usually used, when one of an `ScAgent` wants to wait 
result of another one. There are next kind of `ScWaiter` objects:

* `ScWaiter` locks run flow until simple event emits. You can see the list of these events in the [**C++ Events API**](events.md).
* `ScConditionWaiter` locks run flow until simple event emits and specified conditional check returns `SC_TRUE`. In other
  words, this works like an `ScWaiter`, but returns to run flow if special condition function returns `SC_TRUE`. Condition
  function receives 3 parameters (see [**C++ Events API**](events.md) for more details about them).

There are some examples of usage for specified `ScWaiter` objects:

* Wait input sc-connector into sc-element with `nodeAddr`:

```cpp
auto waiter = context.CreateEventWaiter<
  ScEventGenerateIncomingArc<ScType::EdgeAccessConstPosPerm>>(nodeAddr);
waiter.Wait();
```

* Wait input sc-connector into sc-element with `nodeAddr`, with condition:

```cpp
auto const CheckCallback 
  = [](ScEventGenerateIncomingArc<ScType::EdgeAccessConstPosPerm> const & event)
{
  // Check condition here.
  // Return SC_TRUE or SC_FALSE depending on condition.
  return SC_FALSE;
};

auto waiter = context.CreateConditionWaiter<
  ScEventGenerateIncomingArc<
    ScType::EdgeAccessConstPosPerm>>(nodeAddr, CheckCallback);
// Provide wait time value.
waiter.Wait(10000); // milliseconds.
// By default, wait time value is 5000 milliseconds.
```

All constructors of these classes are private. You should [**C++ Agent context API**](agent_context.md) to create waiters.

--- 

## **Frequently Asked Questions**

<!-- no toc -->
- [Why can't I call the constructor of a waiter for sc-event?](#why-cant-i-call-the-constructor-of-a-waiter-for-sc-event)
- [Is it possible to wait for sc-event through the knowledge base?](#is-it-possible-to-wait-for-sc-event-through-the-knowledge-base)

### **Why can't I call the constructor of a waiter for sc-event?**

First of all, it's not safe. We need more checks on input arguments because there are more of them. Secondly, it is correct from the OOP point of view. Constructors should not throw exceptions. Third, it is correct from the point of view of the architecture we use in the sc-machine. The `ScAgentContext` is a facade over all possible objects used by agents.

### **Is it possible to wait for sc-event through the knowledge base**

You can make an agent that will do that.

# **C++ Waiters API**

!!! note
    This is correct for only versions of sc-machine that >= 0.10.0.
--- 

This API provides functionality to implement waiters on C++.

!!! note
    To include this API provide `#include <sc-memory/sc_waiter.hpp>` in your hpp source.

## **ScWaiter** and **ScConditionWaiter**

This type of objects is used to wait until some event emits until the maximum waiting time expires. It is usually used, when one of an `ScAgent` wants to wait result of another one. There are next kind of `ScWaiter` objects:

* `ScWaiter` locks run flow until simple event emits. You can see the list of these events in the [**C++ Events API**](events.md).
* `ScConditionWaiter` locks run flow until simple event emits and specified conditional check returns `true`. In other
  words, this works like an `ScWaiter`, but returns to run flow if special condition function returns `true`. Condition
  function receives 3 parameters (see [**C++ Events API**](events.md) for more details about them).

## **ScWaiter**

To generate object of this class, you should call `GenerateEventWaiter` method from object of `ScAgentContext` class. You can't use constructors of `ScWaiter` class, because they are private.

```cpp
...
// Create of find some sc-node and generate waiter to wait until 
// a sc-connector of the specified type is generated from or to this sc-node.
auto waiter = context.GenerateEventWaiter<
  ScEventAfterGenerateConnector<ScType::EdgeAccessConstPosPerm>>(
    nodeAddr,
    []() -> void
    {
      // Provide logic here, that should be called at exactly the same time 
      // when waiter will start to wait for sc-event to occur.
    });
...
```

We recommend to use `auto` instead of full type of waiter. In provided example waiter has `std::shared_ptr<ScEventWaiter<ScEventAfterGenerateConnector<ScType::EdgeAccessConstPosPerm>>>` type.

### **Wait**

Call this method from object of `ScWaiter` class to wait for sc-event to occur.

```cpp
bool const isWaited = waiter.Wait(200); // milliseconds.
// Here `200` is maximum waiting time for the specified event to occur.
// The event can happen faster than 200 milliseconds, then the result 
// will be obtained earlier too.
```

By default, this method has default wait time the equals to 5000 milliseconds.

There is also a version of this method with three arguments. You can provide function that will be called after successful completion of waiting for the event to occur, and function that will be called after unsuccessful completion of waiting for the event to occur.

```cpp
bool const isWaited = waiter.Wait(
  200, 
  []() -> void
  {
    // Handle if waiter waited for sc-event to occur.
  },
  []() -> void
  {
    // Handle if waiter didn't wait for sc-event to occur.
  });
```

### **SetOnWaitStartDelegate**

Use this method, when you want to change or provide function that should be called at exactly the same time when waiter will start to wait for sc-event to occur.

```cpp
waiter.SetOnWaitStartDelegate([]() -> void
{
  // Provide logic of this function here.
});
```

### **Resolve**

You can tell a waiter to stop waiting.

```cpp
waiter.Resolve();
```

Then the `Wait` method will stop waiting for the specified event to occur.

## **ScConditionWaiter**

This class represents waiter for condition to occur. To generate object of this class, you should call `GenerateConditionWaiter` method from object of `ScAgentContext` class. You can't also use constructors of `ScConditionWaiter` class, because they are private.

```cpp
...
// Create of find some sc-node and generate waiter to wait until 
// a sc-connector of the specified type is generated from or to this sc-node.
auto waiter = context.ScConditionWaiter<
  ScEventAfterGenerateConnector<ScType::EdgeAccessConstPosPerm>>(
    nodeAddr,
    []() -> void
    {
      // Provide logic here, that should be called at exactly the same time 
      // when waiter will start to wait for sc-event to occur.
    },
    [](ScEventAfterGenerateConnector<ScType::EdgeAccessConstPosPerm>> 
        const & event) -> bool
    {
      // Check here, that occurred sc-event is sc-event that this waiter 
      // has been waiting for.
    };
...
```

`ScConditionWaiter` class inherits `ScWaiter` class. You can use all methods of `ScWaiter` class for objects of `ScConditionWaiter` class.

## **Examples of using waiters**

There are some examples of usage for specified `ScWaiter` objects:

* Wait input sc-connector into sc-element with `nodeAddr`:

```cpp
auto waiter = context.GenerateEventWaiter<
  ScEventAfterGenerateIncomingArc<ScType::EdgeAccessConstPosPerm>>(nodeAddr);
waiter.Wait();
```

* Wait input sc-connector into sc-element with `nodeAddr`, with condition:

```cpp
auto const CheckCallback 
  = [](ScEventAfterGenerateIncomingArc<
    ScType::EdgeAccessConstPosPerm> const & event)
{
  // Check condition here.
  // Return true or false depending on condition.
  return false;
};

auto waiter = context.GenerateConditionWaiter<
  ScEventAfterGenerateIncomingArc<
    ScType::EdgeAccessConstPosPerm>>(nodeAddr, CheckCallback);
// Provide wait time value.
waiter.Wait(10000); // milliseconds.
```

!!! warning
    All constructors of these classes are private. You should [**C++ Agent context API**](agent_context.md) to generate waiters.

--- 

## **Frequently Asked Questions**

<!-- no toc -->
- [Why can't I call the constructor of a waiter for sc-event?](#why-cant-i-call-the-constructor-of-a-waiter-for-sc-event)
- [Is it possible to represent a waiter in a knowledge base to wait for sc-event to occur?](#is-it-possible-to-represent-a-waiter-in-a-knowledge-base-to-wait-for-sc-event-to-occur)

### **Why can't I call the constructor of a waiter for sc-event?**

First of all, it's not safe. We need more checks on input arguments because there are more of them. Secondly, it is correct from the OOP point of view. Constructors should not throw exceptions. Third, it is correct from the point of view of the architecture we use in the sc-machine. The `ScAgentContext` is a facade over all possible objects used by agents.

### **Is it possible to represent a waiter in a knowledge base to wait for sc-event to occur?**

Yes, you can make an agent that will do that.

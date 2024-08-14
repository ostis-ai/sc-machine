# **C++ Agents API**

!!! warning
    This documentation is correct for only versions of sc-machine that >= 0.10.0.
--- 

This API provides functionality to implement sc-agents on C++.

## **ScWait** and **ScWaitCondition**

This type of objects is used to wait until some event emits. It is usually used, when one of an `ScAgent` wants to wait 
result of another one. There are next kind of `ScWait` objects:

* `ScWait`- lock run flow until simple event emits. You can see the list of these events in the Class properties table (
  ScEvent property);
* `ScWaitCondition` - lock run flow until simple event emits and specified conditional check returns `SC_TRUE`. In other
  words, this works like an `ScWait`, but returns to run flow if special condition function returns `SC_TRUE`. Condition
  function receives 3 parameters (see [**C++ Events API**](events.md) for more details about them).

There are some examples of usage for specified `ScWait` objects:

* Wait input sc-connector into sc-element with `nodeAddr`:

```cpp
auto waiter = context.CreateEventWaiter<
  ScEventAddInputArc<ScType::EdgeAccessConstPosPerm>>(nodeAddr);
waiter.Wait();
```

* Wait input sc-connector into sc-element with `nodeAddr`, with condition:

```cpp
auto const CheckCallback = [](ScEventAddInputArc<ScType::EdgeAccessConstPosPerm> const & event)
{
  // Check condition here.
  // Return SC_TRUE or SC_FALSE depending on condition.
  return SC_FALSE;
};

auto waiter = context.CreateEventWaiterWithCondition<
  ScEventAddInputArc<ScType::EdgeAccessConstPosPerm>>(nodeAddr, CheckCallback);
// Provide wait time value.
waiter.Wait(10000); // milliseconds.
// By default, wait time value is 5000 milliseconds.
```

All constructors of these classes are private. You should [**C++ Agent context API**] to create waiters.

--- 

## **Frequently Asked Questions**

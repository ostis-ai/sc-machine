# **ScEventSubscription API**

!!! warning
    This documentation is correct for only versions of sc-machine that >= 0.10.0.
--- 

This API provides functionality to subscribe to sc-events on C++.

## **ScEventSubscription**

This class allows you to subscribe to any sc-events. There are C++ classes of subscription that correspond to specified event types:



Each event constructor takes 3 parameters:

* `context` - `ScMemoryContext` that will be used to work with event;
* `addr` - `ScAddr` of sc-element that need to be listened for a specified event;
* `func` - delegate to a callback function, that will be called on each event emit
  (`bool func(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr)`).
  Description of parameters for this function you can see in table below (for each event type).

!!! note
    Callback function will be called in another thread!

The table of description (parameters of callback function named on pictures, if there are no parameter name on picture,
then it's would have an empty value):


# **ScEventSubscription API**

!!! warning
    This documentation is correct for only versions of sc-machine that >= 0.10.0.
--- 

This API provides functionality to subscribe to sc-events on C++.

## **ScEventSubscription**

This class allows you to subscribe to any sc-events. There are types of subscriptions that correspond to specified sc-event types:

* `ScElementaryEventSubscription` is base class for all sc-event subscription, it can be used to catch all sc-events for specified sc-element.
* `ScEventSubscriptionAddOutputArc`, subscribes callback to be emitted each time, when output (outgoing) sc-arc (from specified sc-element) is created;
* `ScEventSubscriptionAddInputArc`, subscribes callback to be emitted each time, when input (ingoing) sc-arc (to specified sc-element) is created;
* `ScEventSubscriptionAddEdge`, subscribes callback to be emitted each time, when sc-edge (from or to specified sc-element) is created;
* `ScEventSubscriptionRemoveOutputArc`, subscribes callback to be emitted each time, when output (outgoing) sc-arc (from specified sc-element) is removing;
* `ScEventSubscriptionRemoveInputArc`, subscribes callback to be emitted each time, when input (ingoing) sc-arc (to specified sc-element) is removing;
* `ScEventSubscriptionRemoveEdge`, subscribes callback to be emitted each time, when sc-edge (from or to specified sc-element) is removing;
* `ScEventSubscriptionRemoveElement`, subscribes callback to be emitted each time, when specified sc-element is removing;
* `ScEventSubscriptionChangeLinkContent`, subscribes callback to be emitted each time, when content of specified sc-link is changing.

The following classes correspond to them:

* `class ScElementaryEventSubscription`;
* `template <ScType const & arcType> class ScEventSubscriptionAddOutputArc<arcType>`;
* `template <ScType const & arcType> class ScEventSubscriptionAddInputArc<arcType>`;
* `template <ScType const & edgeType> class ScEventSubscriptionAddEdge<edgeType>`;
* `template <ScType const & arcType> class ScEventSubscriptionRemoveOutputArc<arcType>`;
* `template <ScType const & arcType> class ScEventSubscriptionRemoveInputArc<arcType>`;
* `template <ScType const & edgeType> class ScEventSubscriptionRemoveEdge<edgeType>`;
* `class ScEventSubscriptionRemoveElement`;
* `class ScEventSubscriptionChangeLinkContent`;

Each sc-event subscription constructor, besides `ScElementaryEventSubscription` constructor, takes 3 parameters:

* `context` - `ScMemoryContext` that will be used to work with sc-event;
* `subscriptionElementAddr` - sc-address of sc-element that need to be listened for a specified sc-event;
* `delegateFunc` - delegate to a callback function, that will be called on each event emit (`void delegateFunc(TScEvent const &)`), where `TScEvent` is corresponfing sc-event class.

!!! note
    Callback function will be called in another thread!

!!! warning
    Subscription sc-element must be valid sc-element.

Constructor of `ScElementaryEventSubscription` class takes 4 parameters:

* `context`, is `ScMemoryContext` that will be used to work with sc-event;
* `eventClassAddr`, is sc-address of sc-event class;
* `subscriptionElementAddr`, is sc-address of sc-element that need to be listened for a specified sc-event;
* `delegateFunc`, is delegate to a callback function, that will be called on each event emit (`void delegateFunc(ScElementaryEvent const &)`).

All sc-event classes are in core sc-keynodes:

* `ScKeynodes::sc_event_add_output_arc`;
* `ScKeynodes::sc_event_add_input_arc`;
* `ScKeynodes::sc_event_add_edge`;
* `ScKeynodes::sc_event_remove_output_arc`;
* `ScKeynodes::sc_event_remove_input_arc`;
* `ScKeynodes::sc_event_remove_edge`;
* `ScKeynodes::sc_event_remove_element`;
* `ScKeynodes::sc_event_change_link_content`.

Use them as `eventClassAddr` for `ScElementaryEventSubscription`.

The table of description (parameters of callback function named on pictures, if there are no parameter name on picture,
then it's would have an empty value):

<table width="95%">
  <tr>
    <th>Class</th>
    <th>Description</th>
  </tr>

  <tr>
    <td><strong>ScElementaryEventSubscription</strong></td>
    <td>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
...
ScElementaryEventSubscription subscription(
  context,
  eventClassAddr,
  elementAddr, 
  [](ScElementaryEvent const & event) -> void
{
  // Handle sc-event.
});
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventAddOutputArc</strong></td>
    <td>
      <scg src="../images/events/sc_event_output_arc.gwf"></scg>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
...
ScEventSubscriptionAddOutputArc<ScType::EdgeAccessConstPosPerm> subscription(
  context, 
  elementAddr, 
  [](ScEventAddOutputArc<ScType::EdgeAccessConstPosPerm> const & event) -> void
{
  // Handle sc-event.
});
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventAddInputArc</strong></td>
    <td>
      <scg src="../images/events/sc_event_input_arc.gwf"></scg>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
...
ScEventSubscriptionAddInputArc<ScType::EdgeAccessConstPosPerm> subscription(
  context, 
  elementAddr, 
  [](ScEventAddInputArc<ScType::EdgeAccessConstPosPerm> const & event) -> void
{
  // Handle sc-event.
});
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventAddEdge</strong></td>
    <td>
      <scg src="../images/events/sc_event_edge.gwf"></scg>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
...
ScEventSubscriptionAddEdge<ScType::EdgeUCommonConst> subscription(
  context, 
  elementAddr, 
  [](ScEventAddEdge<ScType::EdgeUCommonConst> const & event) -> void
{
  // Handle sc-event.
});
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventRemoveOutputArc</strong></td>
    <td>
      <scg src="../images/events/sc_event_output_arc.gwf"></scg>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
...
ScEventSubscriptionRemoveOutputArc<ScType::EdgeAccessConstPosPerm> subscription(
  context, 
  elementAddr, 
  [](ScEventRemoveOutputArc<ScType::EdgeAccessConstPosPerm> const & event) -> void
{
  // Handle sc-event.
});
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventRemoveInputArc</strong></td>
    <td>
      <scg src="../images/events/sc_event_input_arc.gwf"></scg>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
...
ScEventSubscriptionRemovInputArc<ScType::EdgeAccessConstPosPerm> subscription(
  context, 
  elementAddr, 
  [](ScEventRemoveInputArc<ScType::EdgeAccessConstPosPerm> const & event) -> void
{
  // Handle sc-event.
});
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventRemoveEdge</strong></td>
    <td>
      <scg src="../images/events/sc_event_edge.gwf"></scg>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
...
ScEventSubscriptionRemoveEdge<ScType::EdgeUCommonConst> subscription(
  context, 
  elementAddr, 
  [](ScEventRemoveEdge<ScType::EdgeUCommonConst> const & event) -> void
{
  // Handle sc-event.
});
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventRemoveElement</strong></td>
    <td>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
...
ScEventSubscriptionRemoveElement subscription(
  context, 
  elementAddr, 
  [](ScEventRemoveElement const & event) -> void
{
  // Handle sc-event.
});
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventChangeLinkContent</strong></td>
    <td>
      <scg src="../images/events/sc_event_content.gwf"></scg>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
...
ScEventSubscriptionChangeLinkContent subscription(
  context, 
  linkAddr, 
  [](ScEventChangeLinkContent const & event) -> void
{
  // Handle sc-event.
});
...
      </code></pre>
    </td>
  </tr>
</table>

--- 

## **Frequently Asked Questions**

# **ScEventSubscription API**

!!! note
    This is correct for only versions of sc-machine that >= 0.10.0.
--- 

This API provides functionality to subscribe to sc-events on C++.

## **ScEventSubscription**

`ScElementaryEventSubscription` is base class for all sc-event subscription, it can be used to catch all sc-events for specified sc-element.

Each sc-event subscription constructor, besides `ScElementaryEventSubscription` constructor, takes 3 parameters:

* `context` is an object of `ScMemoryContext` that will be used to work with sc-event;
* `subscriptionElementAddr` is an object of `ScAddr` of sc-element that need to be listened for a specified sc-event;
* `delegateFunc` is a delegate to a callback function, that will be called on each event emit (`void delegateFunc(TScEvent const &)`), where `TScEvent` is corresponding sc-event class.

!!! note
    Callback function will be called in another thread!

!!! warning
    Subscription sc-element must be valid sc-element.

Constructor of `ScElementaryEventSubscription` class takes 4 parameters:

* `context` is an object of `ScMemoryContext` that will be used to work with sc-event;
* `eventClassAddr` is an object of `ScAddr` of sc-event class;
* `subscriptionElementAddr` is an object of `ScAddr` of sc-element that need to be listened for a specified sc-event;
* `delegateFunc` is a delegate to a callback function, that will be called on each event emit (`void delegateFunc(ScElementaryEvent const &)`).

All these constructors are private, you can't call these. We provide more safe API to create subscription. Use [**C++ Agent Context API**](agent_context.md) to create sc-event subscriptions.

All sc-event classes are in core keynodes:

* `ScKeynodes::sc_event_after_generate_connector`;
* `ScKeynodes::sc_event_after_generate_outgoing_arc`;
* `ScKeynodes::sc_event_after_generate_incoming_arc`;
* `ScKeynodes::sc_event_after_generate_edge`;
* `ScKeynodes::sc_event_before_erase_connector`;
* `ScKeynodes::sc_event_before_erase_outgoing_arc`;
* `ScKeynodes::sc_event_before_erase_incoming_arc`;
* `ScKeynodes::sc_event_before_erase_edge`;
* `ScKeynodes::sc_event_before_erase_element`;
* `ScKeynodes::sc_event_before_change_link_content`.

Use them as `eventClassAddr` for `CreateElementaryEventSubscription`.

The table of description (parameters of callback function named on pictures, if there are no parameter name on picture,
then it's would have an empty value):

!!! note
    Here `context` is pointer to object of `ScAgentContext` class.

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
auto subscription = context->CreateElementaryEventSubscription(
  eventClassAddr,
  subscriptionElementAddr, 
  [](ScElementaryEvent const & event) -> void
{
  // Handle sc-event.
});
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventAfterGenerateConnector</strong></td>
    <td>
      <scg src="../images/events/sc_event_connector.gwf"></scg>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
...
auto subscription = context->CreateElementaryEventSubscription<
  ScEventAfterGenerateConnector<ScType::EdgeAccessConstPosPerm>>(
  subscriptionElementAddr, 
  [](ScEventAfterGenerateConnector<ScType::EdgeAccessConstPosPerm> const & event) -> void
{
  // Handle sc-event.
});
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventAfterGenerateOutgoingArc</strong></td>
    <td>
      <scg src="../images/events/sc_event_outgoing_arc.gwf"></scg>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
...
auto subscription = context->CreateElementaryEventSubscription<
  ScEventAfterGenerateOutgoingArc<ScType::EdgeAccessConstPosPerm>>(
  subscriptionElementAddr, 
  [](ScEventAfterGenerateOutgoingArc<ScType::EdgeAccessConstPosPerm> const & event) -> void
{
  // Handle sc-event.
});
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventAfterGenerateIncomingArc</strong></td>
    <td>
      <scg src="../images/events/sc_event_incoming_arc.gwf"></scg>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
...
auto subscription = context->CreateElementaryEventSubscription<
  ScEventAfterGenerateIncomingArc<ScType::EdgeAccessConstPosPerm>>(
  subscriptionElementAddr, 
  [](ScEventAfterGenerateIncomingArc<ScType::EdgeAccessConstPosPerm> const & event) -> void
{
  // Handle sc-event.
});
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventAfterGenerateEdge</strong></td>
    <td>
      <scg src="../images/events/sc_event_edge.gwf"></scg>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
...
auto subscription = context->CreateElementaryEventSubscription<
  ScEventAfterGenerateEdge<ScType::EdgeUCommonConst>>(
  subscriptionElementAddr, 
  [](ScEventAfterGenerateEdge<ScType::EdgeUCommonConst> const & event) -> void
{
  // Handle sc-event.
});
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventBeforeEraseConnector</strong></td>
    <td>
      <scg src="../images/events/sc_event_connector.gwf"></scg>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
...
auto subscription = context->CreateElementaryEventSubscription<
  ScEventBeforeEraseConnector<ScType::EdgeAccessConstPosPerm>>(
  subscriptionElementAddr, 
  [](ScEventBeforeEraseConnector<ScType::EdgeAccessConstPosPerm> const & event) -> void
{
  // Handle sc-event.
});
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventBeforeEraseOutgoingArc</strong></td>
    <td>
      <scg src="../images/events/sc_event_outgoing_arc.gwf"></scg>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
...
auto subscription = context->CreateElementaryEventSubscription<
  ScEventBeforeEraseOutgoingArc<ScType::EdgeAccessConstPosPerm>>(
  subscriptionElementAddr, 
  [](ScEventBeforeEraseOutgoingArc<ScType::EdgeAccessConstPosPerm> const & event) -> void
{
  // Handle sc-event.
});
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventBeforeEraseIncomingArc</strong></td>
    <td>
      <scg src="../images/events/sc_event_incoming_arc.gwf"></scg>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
...
auto subscription = context->CreateElementaryEventSubscription<
  ScEventBeforeEraseIncomingArc<ScType::EdgeAccessConstPosPerm>>(
  subscriptionElementAddr, 
  [](ScEventBeforeEraseIncomingArc<ScType::EdgeAccessConstPosPerm> const & event) -> void
{
  // Handle sc-event.
});
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventBeforeEraseEdge</strong></td>
    <td>
      <scg src="../images/events/sc_event_edge.gwf"></scg>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
...
auto subscription = context->CreateElementaryEventSubscription<
  ScEventBeforeEraseEdge<ScType::EdgeUCommonConst>>(
  subscriptionElementAddr, 
  [](ScEventBeforeEraseEdge<ScType::EdgeUCommonConst> const & event) -> void
{
  // Handle sc-event.
});
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventBeforeEraseElement</strong></td>
    <td>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
...
auto subscription = context->CreateElementaryEventSubscription<
  ScEventBeforeEraseElement>(
  subscriptionElementAddr, 
  [](ScEventBeforeEraseElement const & event) -> void
{
  // Handle sc-event.
});
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventBeforeChangeLinkContent</strong></td>
    <td>
      <scg src="../images/events/sc_event_content.gwf"></scg>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
...
auto subscription = context->CreateElementaryEventSubscription<
  ScEventBeforeChangeLinkContent>(
  subscriptionElementAddr, 
  [](ScEventBeforeChangeLinkContent const & event) -> void
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

<!-- no toc -->
- [Whether a function is considered an agent if this function is subscribed to a sc-event and which is called after that sc-event occurs?](#whether-a-function-is-considered-an-agent-if-this-function-is-subscribed-to-a-sc-event-and-which-is-called-after-that-sc-event-occurs)
- [Why can't you call the constructor of a subscription class to sc-event?](#why-cant-i-call-the-constructor-of-a-subscription-class-to-sc-event)
- [Are `ScEventAfterGenerateIncomingArc` or `ScEventAfterGenerateOutgoingArc` events trigger when `ScEventAfterGenerateEdge` event is triggered?](#are-sceventaftergenerateincomingarc-or-sceventaftergenerateoutgoingarc-events-trigger-when-sceventaftergenerateedge-event-is-triggered)

### **Whether a function is considered an agent if this function is subscribed to a sc-event and which is called after that sc-event occurs?**

No, such functions are not agents. Agents have a strict specification. See [C++ Agents API](agents.md).

### **Why can't I call the constructor of a subscription class to sc-event?**

First of all, it's not safe. We need more checks on input arguments because there are more of them. Secondly, it is correct from the OOP point of view. Constructors should not throw exceptions. Third, it is correct from the point of view of the architecture we use in the sc-machine. The `ScAgentContext` is a facade over all possible objects used by agents.

### **Are `ScEventAfterGenerateIncomingArc` or `ScEventAfterGenerateOutgoingArc` events trigger when `ScEventAfterGenerateEdge` event is triggered?**

No, the `ScEventAfterGenerateEdge` event only occurs when sc-edges are generated.

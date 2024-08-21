# **ScEventSubscription API**

!!! warning
    This documentation is correct for only versions of sc-machine that >= 0.10.0.
--- 

This API provides functionality to subscribe to sc-events on C++.

## **ScEventSubscription**

`ScElementaryEventSubscription` is base class for all sc-event subscription, it can be used to catch all sc-events for specified sc-element.

Each sc-event subscription constructor, besides `ScElementaryEventSubscription` constructor, takes 3 parameters:

* `context` - `ScMemoryContext` that will be used to work with sc-event;
* `subscriptionElementAddr` - sc-address of sc-element that need to be listened for a specified sc-event;
* `delegateFunc` - delegate to a callback function, that will be called on each event emit (`void delegateFunc(TScEvent const &)`), where `TScEvent` is corresponding sc-event class.

!!! note
    Callback function will be called in another thread!

!!! warning
    Subscription sc-element must be valid sc-element.

Constructor of `ScElementaryEventSubscription` class takes 4 parameters:

* `context`, is `ScMemoryContext` that will be used to work with sc-event;
* `eventClassAddr`, is sc-address of sc-event class;
* `subscriptionElementAddr`, is sc-address of sc-element that need to be listened for a specified sc-event;
* `delegateFunc`, is delegate to a callback function, that will be called on each event emit (`void delegateFunc(ScElementaryEvent const &)`).

All these constructors are private, you can't call these. We provide more safe API to create subscription. Use [**C++ Agent Context API**](agent_context.md) to create sc-event subscriptions.

All sc-event classes are in core keynodes:

* `ScKeynodes::sc_event_generate_outgoing_arc`;
* `ScKeynodes::sc_event_generate_incoming_arc`;
* `ScKeynodes::sc_event_generate_edge`;
* `ScKeynodes::sc_event_erase_outgoing_arc`;
* `ScKeynodes::sc_event_erase_incoming_arc`;
* `ScKeynodes::sc_event_erase_edge`;
* `ScKeynodes::sc_event_erase_element`;
* `ScKeynodes::sc_event_change_link_content`.

Use them as `eventClassAddr` for `CreateElementaryEventSubscription`.

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
    <td><strong>ScEventGenerateOutgoingArc</strong></td>
    <td>
      <scg src="../images/events/sc_event_outgoing_arc.gwf"></scg>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
...
auto subscription = context->CreateElementaryEventSubscription<
  ScEventGenerateOutgoingArc<ScType::EdgeAccessConstPosPerm>>(
  subscriptionElementAddr, 
  [](ScEventGenerateOutgoingArc<ScType::EdgeAccessConstPosPerm> const & event) -> void
{
  // Handle sc-event.
});
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventGenerateIncomingArc</strong></td>
    <td>
      <scg src="../images/events/sc_event_incoming_arc.gwf"></scg>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
...
auto subscription = context->CreateElementaryEventSubscription<
  ScEventGenerateIncomingArc<ScType::EdgeAccessConstPosPerm>>(
  subscriptionElementAddr, 
  [](ScEventGenerateIncomingArc<ScType::EdgeAccessConstPosPerm> const & event) -> void
{
  // Handle sc-event.
});
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventGenerateEdge</strong></td>
    <td>
      <scg src="../images/events/sc_event_edge.gwf"></scg>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
...
auto subscription = context->CreateElementaryEventSubscription<
  ScEventGenerateEdge<ScType::EdgeUCommonConst>>(
  subscriptionElementAddr, 
  [](ScEventGenerateEdge<ScType::EdgeUCommonConst> const & event) -> void
{
  // Handle sc-event.
});
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventEraseOutgoingArc</strong></td>
    <td>
      <scg src="../images/events/sc_event_outgoing_arc.gwf"></scg>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
...
auto subscription = context->CreateElementaryEventSubscription<
  ScEventEraseOutgoingArc<ScType::EdgeAccessConstPosPerm>>(
  subscriptionElementAddr, 
  [](ScEventEraseOutgoingArc<ScType::EdgeAccessConstPosPerm> const & event) -> void
{
  // Handle sc-event.
});
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventEraseIncomingArc</strong></td>
    <td>
      <scg src="../images/events/sc_event_incoming_arc.gwf"></scg>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
...
auto subscription = context->CreateElementaryEventSubscription<
  ScEventEraseIncomingArc<ScType::EdgeAccessConstPosPerm>>(
  subscriptionElementAddr, 
  [](ScEventEraseIncomingArc<ScType::EdgeAccessConstPosPerm> const & event) -> void
{
  // Handle sc-event.
});
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventEraseEdge</strong></td>
    <td>
      <scg src="../images/events/sc_event_edge.gwf"></scg>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
...
auto subscription = context->CreateElementaryEventSubscription<
  ScEventEraseEdge<ScType::EdgeUCommonConst>>(
  subscriptionElementAddr, 
  [](ScEventEraseEdge<ScType::EdgeUCommonConst> const & event) -> void
{
  // Handle sc-event.
});
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventEraseElement</strong></td>
    <td>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
...
auto subscription = context->CreateElementaryEventSubscription<
  ScEventEraseElement>(
  subscriptionElementAddr, 
  [](ScEventEraseElement const & event) -> void
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
auto subscription = context->CreateElementaryEventSubscription<
  ScEventChangeLinkContent>(
  subscriptionElementAddr, 
  [](ScEventChangeLinkContent const & event) -> void
{
  // Handle sc-event.
});
...
      </code></pre>
    </td>
  </tr>
</table>

!!! note
    Here `context` is pointer to object of `ScAgentContext` class.

--- 

## **Frequently Asked Questions**

<!-- no toc -->
- [Whether a function is considered an agent if this function is subscribed to a sc-event and which is called after that sc-event occurs?](#whether-a-function-is-considered-an-agent-if-this-function-is-subscribed-to-a-sc-event-and-which-is-called-after-that-sc-event-occurs)
- [Why can't you call the constructor of a subscription class to sc-event?](#why-cant-i-call-the-constructor-of-a-subscription-class-to-sc-event)

### **Whether a function is considered an agent if this function is subscribed to a sc-event and which is called after that sc-event occurs?**

No, such functions are not agents. Agents have a strict specification. See [C++ Agents API](agents.md).

### **Why can't I call the constructor of a subscription class to sc-event?**

First of all, it's not safe. We need more checks on input arguments because there are more of them. Secondly, it is correct from the OOP point of view. Constructors should not throw exceptions. Third, it is correct from the point of view of the architecture we use in the sc-machine. The `ScAgentContext` is a facade over all possible objects used by agents.

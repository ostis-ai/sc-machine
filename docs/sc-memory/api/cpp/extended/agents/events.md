# **C++ Events API**

!!! note
    This is correct for only versions of sc-machine that >= 0.10.0.
--- 

The **C++ Events API** is designed to handle event-driven programming within the sc-machine. This section details types of possible events and how to respond to triggered events.

!!! note
    To include this API provide `#include <sc-memory/sc_event.hpp>` in your hpp source.

## **What is event-driven model?**

The sc-machine uses event-driven model to manage processing sc-constructions. The sc-memory stores SC-code constructions, which are graph structures, then any kind of events, occurring in sc-memory, is related to changes in these graph constructions.

These are methods that generate events: 

- `GenerateConnector`, 
- `EraseElement`,
- `SetLinkContent`.

They publish events to an event queue without needing to know which consumers will receive them. These components filter and distribute events to appropriate consumers. They manage the flow of events and ensure that they reach the correct destinations. Event consumers are the components that listen for and process events. Event consumers can be modules, agents or something else. See [**FAQ**](#frequently-asked-questions) to find out why it works that way.

Within the OSTIS Technology, events are considered only situations in which relationships have changed or new relationships have been generated, or link content have been changed, or some sc-element have been erased.

## **ScEvent**

The sc-machine provides functionality for subscribing to the following elementary types of sc-events:

* `ScElementaryEvent` is base class for all sc-events, it can be used to handle all sc-events for specified sc-element;
* `ScEventAfterGenerateConnector`, emits each time, when sc-connector from or to specified sc-element is generated;
* `ScEventAfterGenerateOutgoingArc`, emits each time, when outgoing sc-arc from specified sc-element is generated;
* `ScEventAfterGenerateIncomingArc`, emits each time, when incoming sc-arc to specified sc-element is generated;
* `ScEventAfterGenerateEdge`, emits each time, when sc-edge from or to specified sc-element is generated;
* `ScEventBeforeEraseConnector`, emits each time, when sc-connector from or to specified sc-element is erasing;
* `ScEventBeforeEraseOutgoingArc`, emits each time, when outgoing sc-arc from specified sc-element is erasing;
* `ScEventBeforeEraseIncomingArc`, emits each time, when incoming sc-arc to specified sc-element is erasing;
* `ScEventBeforeEraseEdge`, emits each time, when sc-edge from or to specified sc-element is erasing;
* `ScEventBeforeEraseElement`, emits, when specified sc-element is erasing;
* `ScEventBeforeChangeLinkContent`, emits each time, when content of specified sc-link is changing.

The following classes correspond to them:

* `class ScElementaryEvent`;
* `template <ScType const & connectorType> class ScEventAfterGenerateConnector`;
* `template <ScType const & arcType> class ScEventAfterGenerateOutgoingArc`;
* `template <ScType const & arcType> class ScEventAfterGenerateIncomingArc`;
* `template <ScType const & edgeType> class ScEventAfterGenerateEdge`;
* `template <ScType const & connectorType> class ScEventBeforeEraseConnector`;
* `template <ScType const & arcType> class ScEventBeforeEraseOutgoingArc`;
* `template <ScType const & arcType> class ScEventBeforeEraseIncomingArc`;
* `template <ScType const & edgeType> class ScEventBeforeEraseEdge`;
* `class ScEventBeforeEraseElement`;
* `class ScEventBeforeChangeLinkContent`.

!!! note
    All these sc-events classes are inherited from `ScElementaryEvent` class. `ScElementaryEvent` class is inherited from `ScEvent` class that is an abstract class.

!!! note
    Types of sc-connectors are specified for sc-events of adding (erasing) sc-arc (sc-edge). 

!!! warning
    You can't generate objects of these classes. All constructors of these classes are private.

!!! warning
    All objects of sc-event classes are not copyable and not movable.

### **ScElementaryEvent**

This class provides three common methods for all sc-events: `GetUser`, `GetSubscriptionElement` and `GetTriple`.

#### **GetUser**

Use this method to get sc-address of who initiated the current sc-event.

```cpp
...
ScAddr const userAddr = event.GetUser();
...
```

#### **GetSubscriptionElement**

Each event occurs within a semantic neighbourhood of some sc-element. This sc-element is called sc-event subscription sc-element. In other words, it is an element that is permanently listening for the occurrence of certain sc-events within its neighbourhood. To get it, use this method.

```cpp
...
ScAddr const subscriptionElement = event.GetSubscriptionElement();
...
```

#### **GetTriple**

To get information about sc-elements in initiated sc-event, you can use this method. It returns tuple of three ScAddr. For more information see the table below.

<table width="95%">
  <tr>
    <th>Class</th>
    <th>Description</th>
  </tr>

  <tr>
  <td><strong>ScEventAfterGenerateConnector</strong></td>
  <td>
      <scg src="../images/events/sc_event_connector.gwf"></scg>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
...
auto const [subscriptionElementAddr, connectorAddr, otherAddr] 
  = event.GetTriple();
// `subscriptionElementAddr` is sc-address of source or 
// target sc-element (listen it in sc-event).
// `connectorAddr` is sc-address of generated sc-connector 
// to or from `subscriptionElementAddr`.
// `otherAddr` is sc-address of source or target sc-element 
// of `arcAddr`.
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
auto const [subscriptionElementAddr, arcAddr, otherAddr] 
  = event.GetTriple();
// `subscriptionElementAddr` is sc-address of source sc-element 
// (listen it in sc-event).
// `arcAddr` is sc-address of generated outgoing sc-arc 
// from `subscriptionElementAddr`.
// `otherAddr` is sc-address of target sc-element of `arcAddr`.
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
auto const [subscriptionElementAddr, arcAddr, otherAddr] 
  = event.GetTriple();
// `subscriptionElementAddr` is sc-address of target sc-element 
// (listen it in sc-event).
// `arcAddr` is sc-address of generated incoming sc-arc 
// to `subscriptionElementAddr`.
// `otherAddr` is sc-address of source sc-element of `arcAddr`.
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
auto const [subscriptionElementAddr, edgeAddr, otherAddr] 
  = event.GetTriple();
// `subscriptionElementAddr` is sc-address of source 
// or target sc-element (listen it in sc-event).
// `edgeAddr` is sc-address of generated sc-edge 
// to or from `subscriptionElementAddr`.
// `otherAddr` is sc-address of source or target sc-element 
// of `edgeAddr`.
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
auto const [subscriptionElementAddr, connectorAddr, otherAddr] 
  = event.GetTriple();
// `subscriptionElementAddr` is sc-address of source or
// target sc-element (listen it in sc-event).
// `connectorAddr` is sc-address of erasable sc-connector 
// to or from `subscriptionElementAddr`.
// `otherAddr` is sc-address of source or target sc-element 
// of `arcAddr`.
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
auto const [subscriptionElementAddr, arcAddr, otherAddr] 
  = event.GetTriple();
// `subscriptionElementAddr` is sc-address of source sc-element 
// (listen it in sc-event).
// `arcAddr` is sc-address of erasable outgoing sc-arc 
// from `subscriptionElementAddr`.
// `otherAddr` is sc-address of target sc-element of `arcAddr`.
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
auto const [subscriptionElementAddr, arcAddr, otherAddr] 
  = event.GetTriple();
// `subscriptionElementAddr` is sc-address of target sc-element 
// (listen it in sc-event).
// `arcAddr` is sc-address of erasable incoming sc-arc 
// to `subscriptionElementAddr`.
// `otherAddr` is sc-address of source sc-element of `arcAddr`.
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
auto const [subscriptionElementAddr, edgeAddr, otherAddr] 
  = event.GetTriple();
// `subscriptionElementAddr` is sc-address of source or 
// target sc-element 
// (listen it in sc-event).
// `edgeAddr` is sc-address of erasable sc-edge 
// to or from `subscriptionElementAddr`.
// `otherAddr` is sc-address of source or target sc-element 
// of `edgeAddr`.
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
auto const [subscriptionElementAddr, _1, _2] 
  = event.GetTriple();
// `subscriptionElementAddr` is sc-address of erasable 
// sc-element 
// (listen it in sc-event).
// `_1` is empty sc-address.
// `_2` is empty sc-address.
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
auto const [subscriptionElementAddr, _1, _2] 
  = event.GetTriple();
// `subscriptionElementAddr` is sc-address of sc-link 
// that has changable content.
// (listen it in sc-event).
// `_1` is empty sc-address.
// `_2` is empty sc-address.
...
      </code></pre>
    </td>
  </tr>
</table>

!!! note
    All described methods of `ScElementaryEvent` class are public and accessible from child classes.

### **ScEventAfterGenerateConnector**

`ScEventAfterGenerateConnector` is class that represents sc-event of generating sc-connector to or from specified sc-element.

#### **GetConnector**

Method `GetConnector` returns generated sc-connector to or from listen sc-element (subscription sc-element).

```cpp
...
ScAddr const connectorAddr = event.GetConnector();
...
```

#### **GetConnectorType**

It returns sc-type of generated sc-connector.

```cpp
...
ScType const connectorType = event.GetConnectorType();
...
```

#### **GetConnectorIncidentElements**

A sc-connector can be a sc-arc or a sc-edge, so method `GetConnectorIncidentElements` returns two incident elements for the generated sc-connector at once.

```cpp
...
auto const [elementAddr1, elementAddr2] = event.GetConnectorIncidentElements();
...
```

### **ScEventAfterGenerateOutgoingArc** and **ScEventAfterGenerateIncomingArc**

`ScEventAfterGenerateOutgoingArc` is class that represents sc-event of generating outgoing sc-arc from specified sc-element. `ScEventAfterGenerateIncomingArc` represents sc-event of generating incoming sc-arc to specified sc-element.

#### **GetArc**

Method `GetArc` returns generated sc-arc from (for `ScEventAfterGenerateOutgoingArc`) or to (for `ScEventAfterGenerateIncomingArc`) listen sc-element (subscription sc-element).

```cpp
...
ScAddr const arcAddr = event.GetArc();
...
```

#### **GetArcType**

It returns sc-type of generated sc-arc.

```cpp
...
ScType const arcType = event.GetArcType();
...
```

#### **GetArcSourceElement**

To get source and target sc-elements of generated sc-arc you can use `GetArcSourceElement` and `GetArcTargetElement` methods. For `ScEventAfterGenerateOutgoingArc` method `GetArcSourceElement` returns sc-address of listen sc-element, for `ScEventAfterGenerateIncomingArc` method `GetArcTargetElement` returns sc-address of listen sc-element.

```cpp
...
ScAddr const sourceElementAddr = event.GetArcSourceElement();
...
```

#### **GetArcTargetElement**

```cpp
...
ScAddr const targetElementAddr = event.GetArcTargetElement();
...
```

### **ScEventAfterGenerateEdge**

This class represents sc-event of adding sc-edge from or to specified sc-element.

#### **GetEdge**

Method `GetEdge` returns generated sc-edge from or to listen sc-element (subscription sc-element).

```cpp
...
ScAddr const edgeAddr = event.GetEdge();
...
```

#### **GetEdgeType**

It returns sc-type of generated sc-edge.

```cpp
...
ScAddr const edgeType = event.GetEdgeType();
...
```

#### **GetEdgeIncidentElements**

To get incident sc-elements of generated sc-edge you can use `GetEdgeIncidentElements` method.

```cpp
...
auto const [elementAddr1, elementAddr2] = event.GetEdgeIncidentElements();
...
```

### **ScEventBeforeEraseConnector**

`ScEventBeforeEraseConnector` is class that represents sc-event of erasing sc-connector to or from specified sc-element.

#### **GetConnector**

Method `GetConnector` returns erasable sc-connector to or from listen sc-element (subscription sc-element).

```cpp
...
ScAddr const connectorAddr = event.GetConnector();
...
```

#### **GetConnectorType**

It returns sc-type of erasable sc-connector.

```cpp
...
ScType const connectorType = event.GetConnectorType();
...
```

#### **GetConnectorIncidentElements**

Method `GetConnectorIncidentElements` returns two incident elements for the erasable sc-connector at once.

```cpp
...
auto const [elementAddr1, elementAddr2] = event.GetConnectorIncidentElements();
...
```

### **ScEventBeforeEraseOutgoingArc** and **ScEventBeforeEraseIncomingArc**

`ScEventBeforeEraseOutgoingArc` is class that represents sc-event of erasing outgoing sc-arc from specified sc-element. `ScEventBeforeEraseIncomingArc` represents sc-event of erasing incoming sc-arc to specified sc-element.

#### **GetArc**

Method `GetArc` returns erasable sc-arc from (for `ScEventBeforeEraseOutgoingArc`) or to (for `ScEventBeforeEraseIncomingArc`) listen sc-element (subscription sc-element).

```cpp
...
ScAddr const arcAddr = event.GetArc();
...
```

#### **GetArcType**

It returns sc-type of erasable sc-arc.

```cpp
...
ScAddr const arcType = event.GetArcType();
...
```

#### **GetArcSourceElement**

To get source and target sc-elements of erasable sc-arc you can use `GetArcSourceElement` and `GetArcTargetElement` methods. For `ScEventBeforeEraseOutgoingArc` method `GetArcSourceElement` returns sc-address of listen sc-element, for `ScEventBeforeEraseIncomingArc` method `GetArcTargetElement` returns sc-address of listen sc-element.

```cpp
...
ScAddr const arcSourceElement = event.GetArcSourceElement();
...
```

#### **GetArcTargetElement**

```cpp
...
ScAddr const arcTargetElement = event.GetArcTargetElement();
...
```

### **ScEventBeforeEraseEdge**

This class represents sc-event of erasing sc-edge from or to specified sc-element.

#### **GetEdge**

Method `GetEdge` returns erasable sc-edge from or to listen sc-element (subscription sc-element).

```cpp
...
ScAddr const edgeAddr = event.GetEdge();
...
```

#### **GetEdgeType**

It returns sc-type of erasable sc-edge.

```cpp
...
ScAddr const edgeType = event.GetEdgeType();
...
```

#### **GetEdgeIncidentElements**

To get incident sc-elements of erasable sc-edge you can use `GetEdgeIncidentElements` method.

```cpp
...
auto const [elementAddr1, elementAddr2] = event.GetEdgeIncidentElements();
...
```

### **ScEventBeforeEraseElement**

This class represents sc-event of erasing specified listen sc-element. You can use all methods from objects of this class that are accessible from `ScElementaryEvent` class.

### **ScEventBeforeChangeLinkContent**

This class represents sc-event of changing content for listen sc-link. You can use all methods from objects of this class that are accessible from `ScElementaryEvent` class.

!!! note
    For `ScEventBeforeEraseElement` and `ScEventBeforeChangeLinkContent` method `GetTriple` returns tuple of three sc-address. The first one is a sc-event subscription sc-element. The other ones should be empty sc-addresses.

--- 

## **Frequently Asked Questions**

<!-- no toc -->
- [Is there sc-event of creating sc-node?](#is-there-sc-event-of-creating-sc-node)
- [Is fact of what happened recorded in the knowledge base? Are sc-events recorded in the knowledge base?](#is-fact-of-what-happened-recorded-in-the-knowledge-base-are-sc-events-recorded-in-the-knowledge-base)
- [Why do we need connector events?](#why-do-we-need-connector-events)

### **Is there sc-event of creating sc-node?**

A sc-event is defined as the addition, modification or erasing of connections between sc-elements, or changing link content, or erasing sc-element. This is so because knowledge is not a single sc-element, and knowledge is construction of three sc-elements at least. A sc-element does not carry any knowledge in itself. Therefore, a sc-event is considered to be emergence of some new knowledge. But there is an exception, erasing sc-elements with no connections with other sc-elements is considered an event.

Also, even if events were defined differently, it doesn't negate the fact that one cannot subscribe to sc-event of creating sc-node, since we don't know about that node in advance, because it doesn't exist.

### **Is fact of what happened recorded in the knowledge base? Are sc-events recorded in the knowledge base?**

Right now, sc-events are not recorded in the knowledge base. It will be implemented in the future versions of the sc-machine.

### **Why do we need connector events?**

Event of generating (erasing) sc-connector is needed to subscribe to both generating (erasing) sc-arc and sc-edge.

For example, man B is a brother for woman A, and man B is a brother for man C. Here, the relation `to be a brother` between woman A and man B is oriented, i.e. they are connected by a sc-arc, not by a sc-edge, and the relation `to be a brother` between man C and man B is undirected, i.e. these men are connected by a sc-edge. In order for an agent to react to appearance of both a sc-arc and a sc-edge from man B to woman A and man C correspondingly, it is necessary to subscribe that agent to appearance of a sc-connector, i.e sc-arc or sc-edge from man B.

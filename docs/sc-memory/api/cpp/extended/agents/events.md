# **ScEvent API**

!!! warning
    This documentation is correct for only versions of sc-machine that >= 0.10.0.
--- 

This API describes how to work with sc-events.

## **What is event-driven model?**

The sc-machine uses event-driven model to manage processing information. The sc-memory stores SC-code constructions, which are are graph structures, then any kind of events, occurring in sc-memory, is related to changes in these graph constructions.

These are methods that generate events: 

- `CreateEdge`, 
- `EraseElement` 
- `SetLinkContent`).

They publish events to an event queue without needing to know which consumers will receive them. These components filter and distribute events to appropriate consumers. They manage the flow of events and ensure that they reach the correct destinations. Event consumers are the components that listen for and process events. Event consumers can be modules, agents or something else.

Within the OSTIS technology, events are considered only situations in which relationships have changed or new relationships have been generated, or link content have been changed.

## **ScEvent**

The sc-machine provides functionality for subscribing to the following syntactic elementary types of sc-events:

* `ScElementaryEvent` is base class for all sc-events, it can be used to handle all sc-events for specified sc-element;
* `ScEventGenerateConnector`, emits each time, when sc-connector from or to specified sc-element is generated;
* `ScEventGenerateOutgoingArc`, emits each time, when outgoing sc-arc from specified sc-element is generated;
* `ScEventGenerateIncomingArc`, emits each time, when ingoing sc-arc to specified sc-element is generated;
* `ScEventGenerateEdge`, emits each time, when sc-edge from or to specified sc-element is generated;
* `ScEventEraseConnector`, emits each time, when sc-connector from or to specified sc-element is erasing;
* `ScEventEraseOutgoingArc`, emits each time, when outgoing sc-arc from specified sc-element is erasing;
* `ScEventEraseIncomingArc`, emits each time, when ingoing sc-arc to specified sc-element is erasing;
* `ScEventEraseEdge`, emits each time, when sc-edge from or to specified sc-element is erasing;
* `ScEventEraseElement`, emits, when specified sc-element is erasing;
* `ScEventChangeLinkContent`, emits each time, when content of specified sc-link is changing.

The following classes correspond to them:

* `class ScElementaryEvent`;
* `template <ScType const & connectorType> class ScEventGenerateConnector`;
* `template <ScType const & arcType> class ScEventGenerateOutgoingArc`;
* `template <ScType const & arcType> class ScEventGenerateIncomingArc`;
* `template <ScType const & edgeType> class ScEventGenerateEdge`;
* `template <ScType const & connectorType> class ScEventEraseConnector`;
* `template <ScType const & arcType> class ScEventEraseOutgoingArc`;
* `template <ScType const & arcType> class ScEventEraseIncomingArc`;
* `template <ScType const & edgeType> class ScEventEraseEdge`;
* `class ScEventEraseElement`;
* `class ScEventChangeLinkContent`.

!!! note
    All these sc-events classes are derived from `TScElementaryEvent` class. `ScElementaryEvent` class is derived from `ScEvent` class that is interface class.

!!! note
    Types of sc-connectors are specified for sc-events of adding (erasing) sc-arc (sc-edge). 

!!! warning
    You can't create objects of these classes. All constructors of these classes are private.

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
  <td><strong>ScEventGenerateConnector</strong></td>
  <td>
      <scg src="../images/events/sc_event_edge_connector.gwf"></scg>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
...
auto const [subscriptionElementAddr, edgeAddr, otherAddr] = event.GetTriple();
// `subscriptionElementAddr` is sc-address of source sc-element 
// (listen it in sc-event).
// `edgeAddr` is sc-address of generated sc-connector to or from `subscriptionElementAddr`.
// `otherAddr` is sc-address of target sc-element of `arcAddr`.
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
auto const [subscriptionElementAddr, arcAddr, otherAddr] = event.GetTriple();
// `subscriptionElementAddr` is sc-address of source sc-element 
// (listen it in sc-event).
// `arcAddr` is sc-address of generated outgoing sc-arc from `subscriptionElementAddr`.
// `otherAddr` is sc-address of target sc-element of `arcAddr`.
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
auto const [subscriptionElementAddr, arcAddr, otherAddr] = event.GetTriple();
// `subscriptionElementAddr` is sc-address of target sc-element 
// (listen it in sc-event).
// `arcAddr` is sc-address of generated incoming sc-arc to `subscriptionElementAddr`.
// `otherAddr` is sc-address of source sc-element of `arcAddr`.
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
auto const [subscriptionElementAddr, edgeAddr, otherAddr] = event.GetTriple();
// `subscriptionElementAddr` is sc-address of source or target sc-element 
// (listen it in sc-event).
// `edgeAddr` is sc-address of generated sc-edge to or from `subscriptionElementAddr`.
// `otherAddr` is sc-address of source or target sc-element of `edgeAddr`.
...
      </code></pre>
    </td>
  </tr>

  <tr>
  <td><strong>ScEventEraseConnector</strong></td>
  <td>
    <scg src="../images/events/sc_event_edge.gwf"></scg>
    <strong>Example C++ code</strong>:
    <pre><code class="cpp">
...
auto const [subscriptionElementAddr, edgeAddr, otherAddr] = event.GetTriple();
// `subscriptionElementAddr` is sc-address of source sc-element 
// (listen it in sc-event).
// `edgeAddr` is sc-address of erasable sc-connector to or from `subscriptionElementAddr`.
// `otherAddr` is sc-address of target sc-element of `arcAddr`.
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
auto const [subscriptionElementAddr, arcAddr, otherAddr] = event.GetTriple();
// `subscriptionElementAddr` is sc-address of source sc-element 
// (listen it in sc-event).
// `arcAddr` is sc-address of erasable outgoing sc-arc from `subscriptionElementAddr`.
// `otherAddr` is sc-address of target sc-element of `arcAddr`.
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
auto const [subscriptionElementAddr, arcAddr, otherAddr] = event.GetTriple();
// `subscriptionElementAddr` is sc-address of target sc-element 
// (listen it in sc-event).
// `arcAddr` is sc-address of erasable incoming sc-arc to `subscriptionElementAddr`.
// `otherAddr` is sc-address of source sc-element of `arcAddr`.
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
auto const [subscriptionElementAddr, edgeAddr, otherAddr] = event.GetTriple();
// `subscriptionElementAddr` is sc-address of source or target sc-element 
// (listen it in sc-event).
// `edgeAddr` is sc-address of erasable sc-edge to or from `subscriptionElementAddr`.
// `otherAddr` is sc-address of source or target sc-element of `edgeAddr`.
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
auto const [subscriptionElementAddr, _1, _2] = event.GetTriple();
// `subscriptionElementAddr` is sc-address of erasable sc-element 
// (listen it in sc-event).
// `_1` is empty sc-address.
// `_2` is empty sc-address.
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
auto const [subscriptionElementAddr, _1, _2] = event.GetTriple();
// `subscriptionElementAddr` is sc-address of sc-link that has changable content 
// (listen it in sc-event).
// `_1` is empty sc-address.
// `_2` is empty sc-address.
...
      </code></pre>
    </td>
  </tr>
</table>

!!! note
    All described methods of `ScElementary` class are public and accessible from child classes.

### **ScEventGenerateConnector**

`ScEventGenerateConnector` is class that represents sc-event of generating sc-connector to or from specified sc-element.

#### **GetGeneratedConnector**

Method `GetGeneratedConnector` returns generated sc-connector to or from listen sc-element (subscription sc-element).

```cpp
...
ScAddr const connectorAddr = event.GetGeneratedConnector();
...
```

#### **GetGeneratedConnectorType**

It returns sc-type of generated sc-connector.

```cpp
...
ScType const connectorType = event.GetGeneratedConnectorType();
...
```

#### **GetConnectorIncidentElements**

A sc-connector can be a sc-arc or a sc-edge, so method `GetConnectorIncidentElements` returns two incident elements for the created sc-connector at once.

```cpp
...
auto const [elementAddr1, elementAddr2] = event.GetConnectorIncidentElements();
...
```

### **ScEventGenerateOutgoingArc** and **ScEventGenerateIncomingArc**

`ScEventGenerateOutgoingArc` is class that represents sc-event of generating outgoing sc-arc from specified sc-element. `ScEventGenerateIncomingArc` represents sc-event of generating incoming sc-arc to specified sc-element.

#### **GetGeneratedArc**

Method `GetGeneratedArc` returns generated sc-arc from (for `ScEventGenerateOutgoingArc`) or to (for `ScEventGenerateIncomingArc) listen sc-element (subscription sc-element).

```cpp
...
ScAddr const arcAddr = event.GetGeneratedArc();
...
```

#### **GetGeneratedArcType**

It returns sc-type of generated sc-arc.

```cpp
...
ScType const arcType = event.GetGeneratedArcType();
...
```

#### **GetArcSourceElement**

To get source and target sc-elements of generated sc-arc you can use `GetArcSourceElement` and `GetArcTargetElement` methods. For `ScEventGenerateOutgoingArc` method `GetArcSourceElement` returns sc-address of listen sc-element, for `ScEventGenerateIncomingArc` method `GetArcTargetElement` returns sc-address of listen sc-element.

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

### **ScEventGenerateEdge**

This class represents sc-event of adding sc-edge from or to specified sc-element.

#### **GetGeneratedEdge**

Method `GetGeneratedEdge` returns generated sc-edge from or to listen sc-element (subscription sc-element).

```cpp
...
ScAddr const edgeAddr = event.GetGeneratedEdge();
...
```

#### **GetGeneratedEdgeType**

It returns sc-type of generated sc-edge.

```cpp
...
ScAddr const edgeType = event.GetGeneratedEdgeType();
...
```

#### **GetEdgeSourceElement**

To get incident sc-elements of generated sc-edge you can use `GetEdgeSourceElement` and `GetEdgeTargetElement` methods.

```cpp
...
ScAddr const sourceElementAddr = event.GetEdgeSourceElement();
...
```

#### **GetEdgeTargetElement**

```cpp
...
ScAddr const targetElementAddr = event.GetEdgeTargetElement();
...
```

### **ScEventEraseConnector**

`ScEventEraseConnector` is class that represents sc-event of erasing sc-connector to or from specified sc-element.

#### **GetErasableConnector**

Method `GetErasableConnector` returns erasable sc-connector to or from listen sc-element (subscription sc-element).

```cpp
...
ScAddr const connectorAddr = event.GetErasableConnector();
...
```

#### **GetErasableConnectorType**

It returns sc-type of erasable sc-connector.

```cpp
...
ScType const connectorType = event.GetErasableConnectorType();
...
```

#### **GetConnectorIncidentElements**

Method `GetConnectorIncidentElements` returns two incident elements for the erasable sc-connector at once.

```cpp
...
auto const [elementAddr1, elementAddr2] = event.GetConnectorIncidentElements();
...
```

### **ScEventEraseOutgoingArc** and **ScEventEraseIncomingArc**

`ScEventEraseOutgoingArc` is class that represents sc-event of erasing outgoing sc-arc from specified sc-element. `ScEventEraseIncomingArc` represents sc-event of erasing incoming sc-arc to specified sc-element.

#### **GetErasableArc**

Method `GetErasableArc` returns erasable sc-arc from (for `ScEventEraseOutgoingArc`) or to (for `ScEventEraseIncomingArc) listen sc-element (subscription sc-element).

```cpp
...
ScAddr const arcAddr = event.GetErasableArc();
...
```

#### **GetErasableArcType**

It returns sc-type of erasable sc-arc.

```cpp
...
ScAddr const arcType = event.GetErasableArcType();
...
```

#### **GetArcSourceElement**

To get source and target sc-elements of erasable sc-arc you can use `GetArcSourceElement` and `GetArcTargetElement` methods. For `ScEventEraseOutgoingArc` method `GetArcSourceElement` returns sc-address of listen sc-element, for `ScEventEraseIncomingArc` method `GetArcTargetElement` returns sc-address of listen sc-element.

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

### **ScEventEraseEdge**

This class represents sc-event of erasing sc-edge from or to specified sc-element.

#### **GetErasableEdge**

Method `GetErasableEdge` returns erasable sc-edge from or to listen sc-element (subscription sc-element).

```cpp
...
ScAddr const edgeAddr = event.GetErasableEdge();
...
```

#### **GetErasableEdgeType**

It returns sc-type of erasable sc-edge.

```cpp
...
ScAddr const edgeType = event.GetErasableEdgeType();
...
```

#### **GetEdgeSourceElement**

To get incident sc-elements of erasable sc-edge you can use `GetEdgeSourceElement` and `GetEdgeTargetElement` methods.

```cpp
...
ScAddr const sourceElementAddr = event.GetEdgeSourceElement();
...
```

#### **GetEdgeTargetElement**

```cpp
...
ScAddr const targetElementAddr = event.GetEdgeTargetElement();
...
```

### **ScEventEraseElement**

This class represents sc-event of erasing specified listen sc-element. You can use all methods from objects of this class that arc accessible from `ScElementaryEvent` class.

### **ScEventChangeLinkContent**

This class represents sc-event of changing content for listen sc-link. You can use all methods from objects of this class that arc accessible from `ScElementaryEvent` class.

--- 

## **Frequently Asked Questions**

<!-- no toc -->
- [Is there sc-event of adding sc-node?](#is-there-sc-event-of-adding-sc-node)
- [Is fact of what happened recorded in the knowledge base? Are sc-events recorded in the knowledge base?](#is-fact-of-what-happened-recorded-in-the-knowledge-base-are-sc-events-recorded-in-the-knowledge-base)

### **Is there sc-event of adding sc-node?**

A sc-event is defined as the addition, modification or erasing of connections between sc-elements, or link content changing. This is so because knowledge is not a single sc-element, and knowledge is construction of three sc-elements at least. A sc-element does not carry any knowledge in itself. Therefore, a sc-event is considered to be emergence of some new knowledge.

### **Is fact of what happened recorded in the knowledge base? Are sc-events recorded in the knowledge base?**

Right now, sc-events are not recorded in the knowledge base. It will be implemented in the future versions of the sc-machine.

# **ScEvent API**

!!! примечание
    Это верно только для версий sc-machine, которые >= 0.10.0.
--- 

Этот API описывает, как работать с sc-событиями.

!!! примечание
    Чтобы подключить этот API, укажите "#include <sc-memory/sc_event.hpp>` в вашем исходном файле hpp.

## **Что такое событийно-ориентированная модель?**

Sc-машина использует событийно-управляемую модель для управления обработкой sc-конструкций. В sc-памяти хранятся конструкции SC-кода, которые представляют собой графовые структуры, тогда любые события, происходящие в sc-памяти, связаны с изменениями в этих графовых конструкциях.

Методы, которые генерируют события:

- `GenerateConnector`, 
- `EraseElement`,
- `SetLinkContent`.

Они публикуют события в очереди событий, не зная, какие пользователи их получат. Эти компоненты фильтруют и распределяют события среди соответствующих пользователей. Они управляют потоком событий и гарантируют, что они дойдут до нужных адресатов. Потребители событий - это компоненты, которые отслеживают и обрабатывают события. Потребителями событий могут быть модули, агенты или что-то еще. Смотрите [**FAQ**](#frequently-asked-questions), чтобы узнать, почему это работает именно так.

В рамках технологии OSTIS событиями считаются только ситуации, в которых изменились взаимосвязи или были созданы новые взаимосвязи, или содержание ссылок было изменено, или какой-либо sc-элемент был удален.

## **ScEvent**

sc-машина предоставляет функционал для подписки следующих элементарных типов sc-событий:

* `ScElementaryEvent` является базовым классом для всех sc-событий, его можно использовать для обработки всех sc-событий для указанного sc-элемента;
* `ScEventAfterGenerateConnector`, генерируется каждый раз, когда генерируется sc-коннектор от указанного sc-элемента или к нему;
* `ScEventAfterGenerateOutgoingArc`, генерируется каждый раз, когда генерируется исходящая sc-дуга от указанного sc-элемента;
* `ScEventAfterGenerateIncomingArc`, генерируется каждый раз, когда генерируется входящая sc-дуга для указанного sc-элемента;
* `ScEventAfterGenerateEdge`, генерируется каждый раз, когда создается sc-ребро от или к указанному sc-элементу;
* `ScEventBeforeEraseConnector`, генерируется каждый раз, когда sc-коннектор от указанного sc-элемента или к нему стирается;
* `ScEventBeforeEraseOutgoingArc`, генерируется каждый раз, когда удаляется исходящая sc-дуга из указанного sc-элемента;
* `ScEventBeforeEraseIncomingArc`, генерируется каждый раз, когда входящая sc-дуга к указанному sc-элементу удаляется;
* `ScEventBeforeEraseEdge`, генерируется каждый раз, когда удаляется sc-ребро от указанного sc-элемента или к нему;
* `ScEventBeforeEraseElement`, испускается, когда указанный sc-элемент удаляется;
* `ScEventBeforeChangeLinkContent`, срабатывает каждый раз, когда изменяется содержимое указанной sc-ссылки.

Им соответствуют следующие классы:

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

!!! примечание
    Все эти классы sc-событий унаследованы от класса `ScElementaryEvent`. Класс `ScElementaryEvent` унаследован от класса `ScEvent`, который является абстрактным классом.
    
!!! примечание
    Типы sc-коннекторов указаны для sc-событий добавления (удаления) sc-дуги (sc-ребра).
    
!!! предупреждение
    Вы не можете создавать объекты этих классов. Все конструкторы этих классов являются закрытыми(private).

!!! предупреждение
    Все объекты классов sc-event не подлежат копированию и перемещению.

### **ScElementaryEvent**

Этот класс предоставляет три общих метода для всех sc-событий: `GetUser`, `GetSubscriptionElement` и `GetTriple`.

#### **GetUser**

Используйте этот метод, чтобы получить sc-адрес того, кто инициировал текущее sc-событие.

```cpp
...
ScAddr const userAddr = event.GetUser();
...
```

#### **GetSubscriptionElement**

Каждое событие происходит в семантической окрестности некоторого sc-элемента. Этот sc-элемент называется sc-элементом подписки на sc-событие. Другими словами, это элемент, который постоянно отслеживает возникновение определенных sc-событий в пределах своей окрестности. Чтобы получить его, воспользуйтесь этим методом.

```cpp
...
ScAddr const subscriptionElement = event.GetSubscriptionElement();
...
```

#### **GetTriple**

Чтобы получить информацию о sc-элементах в инициированном sc-событии, вы можете использовать этот метод. Он возвращает кортеж из трех ScAddr. Для получения дополнительной информации смотрите таблицу ниже.

<table width="95%">
  <tr>
    <th>Класс</th>
    <th>Описание</th>
  </tr>

  <tr>
  <td><strong>ScEventAfterGenerateConnector</strong></td>
  <td>
      <scg src="../images/events/sc_event_connector.gwf"></scg>
      <strong>Пример кода на C++</strong>:
      <pre><code class="cpp">
...
auto const [subscriptionElementAddr, connectorAddr, otherAddr] 
  = event.GetTriple();
// `subscriptionElementAddr` — это sc-адрес начального или
// конечного sc-элемента (прослушивать на нем sc-событие).
// `connectorAddr` — это sc-адрес сгенерированного sc-коннектора
// к или от `subscriptionElementAddr`.
// `otherAddr` — это sc-адрес начального или конечного sc-элемента
// `arcAddr`.
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventAfterGenerateOutgoingArc</strong></td>
    <td>
      <scg src="../images/events/sc_event_outgoing_arc.gwf"></scg>
      <strong>Пример кода на C++</strong>:
      <pre><code class="cpp">
...
auto const [subscriptionElementAddr, arcAddr, otherAddr] 
  = event.GetTriple();
// `subscriptionElementAddr` - это sc-адрес начального sc-элемента 
// (прослушивать его в sc-событии).
// `arcAddr` - это sc-адрес сгенерированной исходящей sc-дуги 
// из `subscriptionElementAddr`.
// `otherAddr` - это sc-адрес конечного sc-элемента `arcAddr`.
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventAfterGenerateIncomingArc</strong></td>
    <td>
      <scg src="../images/events/sc_event_incoming_arc.gwf"></scg>
      <strong>Пример кода на C++</strong>:
      <pre><code class="cpp">
...
auto const [subscriptionElementAddr, arcAddr, otherAddr] 
  = event.GetTriple();
// `subscriptionElementAddr` - это sc-адрес конечного sc-элемента 
// (прослушивать его в sc-событии).
// `arcAddr` - это sc-адрес сгенерированной входящей sc-дуги 
// для `subscriptionElementAddr`.
// `otherAddr` - это sc-адрес начального sc-элемента `arcAddr`.
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventAfterGenerateEdge</strong></td>
    <td>
      <scg src="../images/events/sc_event_edge.gwf"></scg>
      <strong>Пример кода на C++</strong>:
      <pre><code class="cpp">
...
auto const [subscriptionElementAddr, edgeAddr, otherAddr] 
  = event.GetTriple();
// `subscriptionElementAddr` - это sc-адрес начального 
// или конечного sc-элемента (прослушивать его в sc-событии).
// `edgeAddr` - это sc-адрес сгенерированного sc-ребра 
// в "subscriptionElementAddr" или из "subscriptionElementAddr".
// `otherAddr` - это sc-адрес начального или конечного sc-элемента 
// из `edgeAddr`.
...
      </code></pre>
    </td>
  </tr>

  <tr>
  <td><strong>ScEventBeforeEraseConnector</strong></td>
  <td>
    <scg src="../images/events/sc_event_connector.gwf"></scg>
    <strong>Пример кода на C++</strong>:
    <pre><code class="cpp">
...
auto const [subscriptionElementAddr, connectorAddr, otherAddr] 
  = event.GetTriple();
// `subscriptionElementAddr` - это sc-адрес начального или
// конечного sc-элемента (прослушивать его в sc-событии).
// connectorAddr — это sc-адрес удаляемого sc-коннектора
// к или от subscriptionElementAddr.
// "otherAddr" - это sc-адрес начального или конечного sc-элемента 
// из `arcAddr`.
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventBeforeEraseOutgoingArc</strong></td>
    <td>
      <scg src="../images/events/sc_event_outgoing_arc.gwf"></scg>
      <strong>Пример кода на C++</strong>:
      <pre><code class="cpp">
...
auto const [subscriptionElementAddr, arcAddr, otherAddr] 
  = event.GetTriple();
// `subscriptionElementAddr` - это sc-адрес начального sc-элемента 
// (прослушивать его в sc-событии).
// `arcAddr` - это sc-адрес удаляемой исходящей sc-дуги 
// из `subscriptionElementAddr`.
// `otherAddr` - это sc-адрес конечного sc-элемента `arcAddr`.
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventBeforeEraseIncomingArc</strong></td>
    <td>
      <scg src="../images/events/sc_event_incoming_arc.gwf"></scg>
      <strong>Пример кода на C++</strong>:
      <pre><code class="cpp">
...
auto const [subscriptionElementAddr, arcAddr, otherAddr] 
  = event.GetTriple();
// `subscriptionElementAddr` - это sc-адрес конечного sc-элемента 
// (прослушивать его в sc-событии).
// `arcAddr` - это sc-адрес удаляемой входящей sc-дуги
// для `subscriptionElementAddr`.
// `otherAddr` - это sc-адрес начального sc-элемента `arcAddr`.
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventBeforeEraseEdge</strong></td>
    <td>
      <scg src="../images/events/sc_event_edge.gwf"></scg>
      <strong>Пример кода на C++</strong>:
      <pre><code class="cpp">
...
auto const [subscriptionElementAddr, edgeAddr, otherAddr] 
  = event.GetTriple();
// `subscriptionElementAddr` - это sc-адрес начального или
// конечного sc-элемента 
// (прослушивать его в sc-событии).
// `edgeAddr` - это sc-адрес удаляемого sc-ребра 
// в или из "subscriptionElementAddr".
// `otherAddr` - это sc-адрес начального или конечного sc-элемента 
// из `edgeAddr`.
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventBeforeEraseElement</strong></td>
    <td>
      <strong>Пример кода на C++</strong>:
      <pre><code class="cpp">
...
auto const [subscriptionElementAddr, _1, _2] 
  = event.GetTriple();
// `subscriptionElementAddr` - это sc-адрес удаляемого 
// sc-элемента 
// (прослушивать его в sc-событии).
// `_1` - пустой sc-адрес.
// `_2` - пустой sc-адрес.
...
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventBeforeChangeLinkContent</strong></td>
    <td>
      <scg src="../images/events/sc_event_content.gwf"></scg>
      <strong>Пример кода на C++</strong>:
      <pre><code class="cpp">
...
auto const [subscriptionElementAddr, _1, _2] 
  = event.GetTriple();
// `subscriptionElementAddr` - это sc-адрес sc-ссылки 
// с изменяемым содержимым.
// (прослушивать его в sc-событии).
// `_1` - пустой sc-адрес.
// `_2` - пустой sc-адрес.
...
      </code></pre>
    </td>
  </tr>
</table>

!!! примечание
    Все описанные методы класса `ScElementaryEvent` являются общедоступными и доступны из дочерних классов.
    
### **ScEventAfterGenerateConnector**

`ScEventAfterGenerateConnector` - это класс, который представляет sc-событие генерации sc-коннектора к или из указанного sc-элемента.

#### **GetConnector**

Метод `GetConnector` возвращает сгенерированный sc-коннектор к или из прослушиваемого sc-элемента (sc-элемент подписки).
```cpp
...
ScAddr const connectorAddr = event.GetConnector();
...
```

#### **GetConnectorType**

Он возвращает sc-тип сгенерированного sc-коннектора.

```cpp
...
ScType const connectorType = event.GetConnectorType();
...
```

#### **GetConnectorIncidentElements**

Sc-коннектором может быть sc-дуга или sc-ребро, поэтому метод `GetConnectorIncidentElements` возвращает сразу два инцидентных элемента для сгенерированного sc-коннектора.

```cpp
...
auto const [elementAddr1, elementAddr2] = event.GetConnectorIncidentElements();
...
```

### **ScEventAfterGenerateOutgoingArc** и **ScEventAfterGenerateIncomingArc**

`ScEventAfterGenerateOutgoingArc` - это класс, который представляет sc-событие генерации исходящей sc-дуги из указанного sc-элемента. `ScEventAfterGenerateIncomingArc` представляет sc-событие генерации входящей sc-дуги для указанного sc-элемента.

#### **GetArc**

Метод `GetArc` возвращает сгенерированную sc-дугу из (для `sceventaftergenerate outgoingarc`) или к (для `sceventaftergenerate Incomingarc`) прослушиваемого sc-элемента (sc-элемент подписки).

```cpp
...
ScAddr const arcAddr = event.GetArc();
...
```

#### **GetArcType**

Он возвращает sc-тип сгенерированной sc-дуги.

```cpp
...
ScType const arcType = event.GetArcType();
...
```

#### **GetArcSourceElement**

Чтобы получить начальные и конечные sc-элементы сгенерированной sc-дуги, вы можете использовать методы `GetArcSourceElement` и `GetArcTargetElement`. Для `ScEventAfterGenerateOutgoingArc` метод `GetArcSourceElement` возвращает sc-адрес прослушиваемого sc-элемента, для `Sceventaftergenerate incomingarc` метод `GetArcTargetElement` возвращает sc-адрес прослушиваемого sc-элемента.

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

Этот класс представляет sc-событие добавления sc-ребра из указанного sc-элемента или к нему.

#### **GetEdge**

Метод `GetEdge` возвращает сгенерированное sc-ребро из sc-элемента или для прослушивания sc-элемента (sc-элемент подписки).

```cpp
...
ScAddr const edgeAddr = event.GetEdge();
...
```

#### **GetEdgeType**

Он возвращает sc-тип сгенерированного sc-ребра.

```cpp
...
ScAddr const edgeType = event.GetEdgeType();
...
```

#### **GetEdgeIncidentElements**

Чтобы получить инцидентные sc-элементы сгенерированного sc-ребра, вы можете использовать метод `GetEdgeIncidentElements`.

```cpp
...
auto const [elementAddr1, elementAddr2] = event.GetEdgeIncidentElements();
...
```

### **ScEventBeforeEraseConnector**

`ScEventBeforeEraseConnector` - это класс, который представляет sc-событие удаления sc-коннектора из указанного sc-элемента.

#### **GetConnector**

Метод `GetConnector` возвращает удаляемый sc-коннектор в или из прослушиваемого sc-элемента (sc-элемент подписки).

```cpp
...
ScAddr const connectorAddr = event.GetConnector();
...
```

#### **GetConnectorType**

Он возвращает sc-тип  удаляемого sc-коннектора.

```cpp
...
ScType const connectorType = event.GetConnectorType();
...
```

#### **GetConnectorIncidentElements**

Метод `GetConnectorIncidentElements` возвращает сразу два инцидентных элемента для удаляемого sc-коннектора.

```cpp
...
auto const [elementAddr1, elementAddr2] = event.GetConnectorIncidentElements();
...
```

### **ScEventBeforeEraseOutgoingArc** и **ScEventBeforeEraseIncomingArc**

`ScEventBeforeEraseOutgoingArc` - это класс, который представляет sc-событие удаления исходящей sc-дуги из указанного sc-элемента. `ScEventBeforeEraseIncomingArc` представляет sc-событие удаления входящей sc-дуги для указанного sc-элемента.

#### **GetArc**

Метод `GetArc` возвращает удаляемую sc-дугу из (для `ScEventBeforeEraseOutgoingArc`) или к (для `ScEventBeforeEraseIncomingArc`) прослушиваемому sc-элементу (sc-элемент подписки).

```cpp
...
ScAddr const arcAddr = event.GetArc();
...
```

#### **GetArcType**

Он возвращает sc-тип удаляемой sc-дуги.

```cpp
...
ScAddr const arcType = event.GetArcType();
...
```

#### **GetArcSourceElement**

Чтобы получить начальные и конечные sc-элементы удаляемой sc-дуги, вы можете использовать методы `GetArcSourceElement` и `GetArcTargetElement`. Для `ScEventBeforeEraseOutgoingArc` метод `GetArcSourceElement` возвращает sc-адрес прослушиваемого sc-элемента, для `ScEventBeforeEraseIncomingArc` метод `GetArcTargetElement` возвращает sc-адрес прослушиваемого sc-элемента.

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

Этот класс представляет собой sc-событие удаления sc-ребра от или к указанному sc-элементу.

#### **GetEdge**

Метод `GetEdge` возвращает удаляемое sc-ребро от или к прослушиваемому sc-элементу (элементу подписки).

```cpp
...
ScAddr const edgeAddr = event.GetEdge();
...
```

#### **GetEdgeType**

Он возвращает sc-тип удаляемого sc-ребра.

```cpp
...
ScAddr const edgeType = event.GetEdgeType();
...
```

#### **GetEdgeIncidentElements**

Чтобы получить инцидентные sc-элементы удаляемого sc-ребра, вы можете использовать метод `GetEdgeIncidentElements`.

```cpp
...
auto const [elementAddr1, elementAddr2] = event.GetEdgeIncidentElements();
...
```

### **ScEventBeforeEraseElement**

Этот класс представляет sc-событие удаления указанного прослушиваемого sc-элемента. Вы можете использовать все методы из объектов этого класса, которые доступны из класса `ScElementaryEvent`.

### **ScEventBeforeChangeLinkContent**

Этот класс представляет sc-событие изменения содержимого для прослушивания sc-ссылки. Вы можете использовать все методы из объектов этого класса, которые доступны из класса `ScElementaryEvent`.

!!! примечание
    Для `ScEventBeforeEraseElement` и `ScEventBeforeChangeLinkContent` метод `GetTriple` возвращает кортеж из трех sc-адресов. Первый - это sc-элемент подписки на sc-событие. Остальные должны быть пустыми sc-адресами.

--- 

## **Часто Задаваемые Вопросы**

<!-- no toc -->
- [Существует ли sc-событие создания sc-узла?](#is-there-sc-event-of-creating-sc-node)
- [Регистрируется ли факт произошедшего в базе знаний? Регистрируются ли sc-события в базе знаний?](#is-fact-of-what-happened-recorded-in-the-knowledge-base-are-sc-events-recorded-in-the-knowledge-base)
- [Зачем нам нужны события connector?](#why-do-we-need-connector-events)

### **Существует ли sc-событие создания sc-узла?**

Sc-событие определяется как добавление, модификация или удаление связей между sc-элементами, или изменение содержимого ссылок, или удаление sc-элемента. Это так, потому что знание не является отдельным sc-элементом, а представляет собой конструкцию из как минимум трех sc-элементов. Sc-элемент сам по себе не несет в себе никаких знаний. Следовательно, sc-событием считается появление некоторого нового знания. Но есть исключение, удаление sc-элементов, не связанных с другими sc-элементами, считается событием.

Кроме того, даже если события были определены по-другому, это не отменяет того факта, что нельзя подписаться на sc-событие создания sc-узла, поскольку мы не знаем об этом узле заранее, потому что он не существует.

### **Регистрируется ли факт произошедшего в базе знаний? Регистрируются ли sc-события в базе знаний?**

На данный момент sc-события не регистрируются в базе знаний. Это будет реализовано в будущих версиях sc-машины.

### **Зачем нам нужны события connector?**

Событие генерации (удаления) sc-коннектора необходимо для подписки как на генерирующую (удаляющую) sc-дугу, так и на sc-ребро.

Например, мужчина В является братом для женщины А, а мужчина В является братом для мужчины С. Здесь отношение "быть братом" между женщиной А и мужчиной В ориентировано, т.е. они связаны дугой, а не ребром, и отношение `быть братом` между мужчиной С и мужчиной В нецелесообразно, т.е. эти люди связаны узким местом. Для того, чтобы агент реагировал на появление как sc-дуги, так и sc-ребра от мужчины B к женщине A и мужчине C соответственно, необходимо, чтобы этот агент подписался на появление sc-коннектора, то есть sc-дуги или sc-ребра от мужчины B.

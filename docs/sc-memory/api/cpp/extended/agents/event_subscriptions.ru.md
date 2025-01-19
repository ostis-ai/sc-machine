# **ScEventSubscription API**

!!! примечание
    Это верно только для версий sc-machine, которые >= 0.10.0.
--- 

**API C++ Event Subscriptions** предназначен для управления подписками на различные события в системе. В этой документации объясняется, как агенты могут подписываться на определенные события и получать уведомления о наступлении этих событий. Это важно для построения систем, требующих обновлений в режиме реального времени и взаимодействия на основе событий.

!!! примечание
    Чтобы подключить этот API, укажите "#include <sc-memory/sc_event_subscription.hpp>" в вашем исходном файле hpp.

## **ScEventSubscription**

`ScElementaryEventSubscription` является базовым классом для всех подписок на sc-события, его можно использовать для перехвата всех sc-событий для указанного sc-элемента.

Каждый конструктор подписки на sc-события, кроме конструктора `ScElementaryEventSubscription`, принимает 3 параметра:

* `context` - это объект `ScMemoryContext`, который будет использоваться для работы с sc-событием;
* `subscriptionElementAddr` - это объект `ScAddr` sc-элемента, который необходимо прослушивать для указанного sc-события;
* `delegateFunc` - это делегат функции обратного вызова, которая будет вызываться при каждом запуске события (`void delegateFunc(TScEvent const &)`), где `TScEvent` - это соответствующий класс sc-события.

!!! примечание
    Функция обратного вызова будет вызвана в другом потоке!

!!! предупреждение
    Sc-элемент подписки должен быть действительным sc-элементом.

Конструктор класса `ScElementaryEventSubscription` принимает 4 параметра:

* `context` - это объект `ScMemoryContext`, который будет использоваться для работы с sc-событием;
* `eventClassAddr` - это объект `ScAddr` класса sc-event;
* `subscriptionElementAddr` - это объект `ScAddr` sc-элемента, который необходимо прослушивать для указанного sc-события;
* `delegateFunc` - это делегат функции обратного вызова, которая будет вызываться при каждом возникновении события (`void delegateFunc(ScElementaryEvent const &)`).

Все эти конструкторы являются частными(private), вы не можете их вызывать. Мы предоставляем более безопасный API для создания подписки. Используйте [**C++ Agent Context API**](agent_context.md) для создания подписок на sc-события.

Все классы sc-event находятся в основных ключевых узлах:

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

Используйте их как "eventClassAddr" для "CreateElementaryEventSubscription`.

Таблица описания (параметры функции обратного вызова, указанные на картинках, если на картинке нет названия параметра,
то оно будет иметь пустое значение):

!!! примечание
    Здесь `context` - это указатель на объект класса `ScAgentContext`.

<table width="95%">
  <tr>
    <th>Класс</th>
    <th>Описание</th>
  </tr>

  <tr>
    <td><strong>ScElementaryEventSubscription</strong></td>
    <td>
      <strong>Пример кода на C++</strong>:
      <pre><code class="cpp">
...
auto subscription = context->CreateElementaryEventSubscription(
  eventClassAddr,
  subscriptionElementAddr, 
  [](ScElementaryEvent const & event) -> void
{
  // Обработать sc-событие.
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
  ScEventAfterGenerateConnector<ScType::ConstPermPosArc>>(
  subscriptionElementAddr, 
  [](ScEventAfterGenerateConnector<ScType::ConstPermPosArc> const & event) -> void
{
  // Обработать sc-событие.
});
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
auto subscription = context->CreateElementaryEventSubscription<
  ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc>>(
  subscriptionElementAddr, 
  [](ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc> const & event) -> void
{
  // Обработать sc-событие.
});
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
auto subscription = context->CreateElementaryEventSubscription<
  ScEventAfterGenerateIncomingArc<ScType::ConstPermPosArc>>(
  subscriptionElementAddr, 
  [](ScEventAfterGenerateIncomingArc<ScType::ConstPermPosArc> const & event) -> void
{
  // Обработать sc-событие.
});
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
auto subscription = context->CreateElementaryEventSubscription<
  ScEventAfterGenerateEdge<ScType::ConstCommonEdge>>(
  subscriptionElementAddr, 
  [](ScEventAfterGenerateEdge<ScType::ConstCommonEdge> const & event) -> void
{
  // Обработать sc-событие.
});
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
auto subscription = context->CreateElementaryEventSubscription<
  ScEventBeforeEraseConnector<ScType::ConstPermPosArc>>(
  subscriptionElementAddr, 
  [](ScEventBeforeEraseConnector<ScType::ConstPermPosArc> const & event) -> void
{
  // Обработать sc-событие.
});
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
auto subscription = context->CreateElementaryEventSubscription<
  ScEventBeforeEraseOutgoingArc<ScType::ConstPermPosArc>>(
  subscriptionElementAddr, 
  [](ScEventBeforeEraseOutgoingArc<ScType::ConstPermPosArc> const & event) -> void
{
  // Обработать sc-событие.
});
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
auto subscription = context->CreateElementaryEventSubscription<
  ScEventBeforeEraseIncomingArc<ScType::ConstPermPosArc>>(
  subscriptionElementAddr, 
  [](ScEventBeforeEraseIncomingArc<ScType::ConstPermPosArc> const & event) -> void
{
  // Обработать sc-событие.
});
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
auto subscription = context->CreateElementaryEventSubscription<
  ScEventBeforeEraseEdge<ScType::ConstCommonEdge>>(
  subscriptionElementAddr, 
  [](ScEventBeforeEraseEdge<ScType::ConstCommonEdge> const & event) -> void
{
  // Обработать sc-событие.
});
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
      <strong>Пример кода на C++</strong>:
      <pre><code class="cpp">
...
auto subscription = context->CreateElementaryEventSubscription<
  ScEventBeforeChangeLinkContent>(
  subscriptionElementAddr, 
  [](ScEventBeforeChangeLinkContent const & event) -> void
{
  // Обработать sc-событие.
});
...
      </code></pre>
    </td>
  </tr>
</table>

--- 

## **Часто Задаваемые Вопросы**

<!-- no toc -->
- [Считается ли функция агентом, если эта функция подписана на sc-событие и вызывается после возникновения этого sc-события?](#whether-a-function-is-considered-an-agent-if-this-function-is-subscribed-to-a-sc-event-and-which-is-called-after-that-sc-event-occurs)
- [Почему нельзя вызвать конструктор класса подписки для sc-события?](#why-cant-i-call-the-constructor-of-a-subscription-class-to-sc-event)
- [Запускаются ли события `ScEventAfterGenerateIncomingArc` или `ScEventAfterGenerateOutgoingArc` при запуске события `ScEventAfterGenerateEdge`?](#are-sceventaftergenerateincomingarc-or-sceventaftergenerateoutgoingarc-events-trigger-when-sceventaftergenerateedge-event-is-triggered)

### **Считается ли функция агентом, если эта функция подписана на sc-событие и вызывается после возникновения этого sc-события?**

Нет, такие функции не являются агентами. У агентов есть строгая спецификация. Смотрите [C++ Agents API](agents.md).

### **Почему нельзя вызвать конструктор класса подписки для sc-события?**

Во-первых, это небезопасно. Нам нужно больше проверок входных аргументов, потому что их больше. Во-вторых, это правильно с точки зрения ООП. Конструкторы не должны генерировать исключения. В-третьих, это правильно с точки зрения архитектуры, которую мы используем в sc-машине. `ScAgentContext` - это внешний вид всех возможных объектов, используемых агентами.

### **Запускаются ли события `ScEventAfterGenerateIncomingArc` или `ScEventAfterGenerateOutgoingArc` при запуске события `ScEventAfterGenerateEdge`?**

Нет, событие `ScEventAfterGenerateEdge` возникает только при создании sc-ребер.

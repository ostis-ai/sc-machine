# **C++ Agent context API**

!!! примечание
    Эта документация верна только для версий sc-machine >= 0.10.0.
---

**C++ Agent context API** предоставляет инструменты для управления m_context, в котором работают агенты. Этот API предлагает дополнительные методы sc-memory, которые упрощают работу с ним, а также дополнительные методы для работы с sc-событиями и агентами.

## **ScAgentContext**

Класс `ScAgentContext` наследуется от класса `ScMemoryContext` и предоставляет функциональность для управления операциями, специфичными для агента, включая подписки на события, действия и структуры внутри sc-машины.

!!! примечание
    Все методы API являются потокобезопасными.
    
!!! примечание
    Чтобы подключить этот API, укажите #include <sc-memory/sc_agent_context.hpp> в исходном файле hpp.

!!! предупреждение
    Объекты класса `ScAgentContext` можно перемещать, но нельзя копировать.

### **CreateElementaryEventSubscription**

Этот метод может быть полезен, если вы хотите создать подписку на sc-событие, но не хотите использовать для этого агенты. `CreateElementaryEventSubscription` генерирует подписку на sc-событие с указанным sc-элементом подписки (прослушивания) и обратным вызовом по событию. Чтобы узнать больше о sc-событиях, смотрите [**C++ Events API**](events.md), чтобы узнать больше о подписках sc-событий , смотрите [**C++ Events Subscriptions API**](event_subscriptions.md).

```cpp
...
// Создайте или используйте существующий m_context sc-агента.
ScAgentContext context;

// Создаем подписку на sc-событие (прослушивание) sc-элемента.
ScAddr const & nodeAddr = context.GenerateNode(ScType::ConstNode);
// Выберите тип sc-события, на которое хотите подписаться.
using MyEventType = ScEventAfterGenerateOutgoingArc<
  ScType::ConstPermPosArc>;
// Создаем подписку на sc-событие для сгенерированного sc-узла (прослушиваемый sc-элемент) 
// и обеспечиваем обратный вызов по событию.
auto eventSubscription 
  = context.CreateElementaryEventSubscription<MyEventType>(
  nodeAddr,
  [](MyEventType const & event) -> void
  {
    // Обработка sc-события.
  });

// Инициируем sc-событие. Создайте sc-дугу из nodeAddr.
ScAddr const & otherNodeAddr = context.GenerateNode(ScType:::ConstNode);
context.GenerateConnector(ScType::ConstPermPosArc, nodeAddr, otherNodeAddr);
// После того, как будет сгенерирована sc-дуга, произойдет sc-событие 
// и будет вызван указанный обратный вызов по событию.
...
```

!!! примечание
    Вы можете предоставить пустой обратный вызов по событию `{}` в `CreateElementaryEventSubscription` и вызвать `SetDelegate` из объекта подписки, чтобы установить новый обратный вызов по событию.

!!! примечание
    Вы можете вызвать `RemoveDelegate` из объекта подписки, чтобы удалить существующий обратный вызов по событию.

!!! предупреждение
    Вы должны предоставить действительный sc-элемент подписки. В противном случае будет выдано исключение.

Если вы заранее не знаете, на какое sc-событие вам нужно подписаться, вы можете использовать переопределенную версию этого метода.

```cpp
...
// Создание подписки на sc-событие (прослушивание) sc-элемента.
ScAddr const & nodeAddr = context.GenerateNode(ScType::ConstNode);
// Выберите тип sc-события, чтобы подписаться или найти его.
ScAddr const & eventClassAddr = GetSomeEventType(); // User-specified method.
// Создаем подписку на sc-событие для сгенерированного sc-узла (прослушиваемого sc-элемента) 
// и обеспечиваем обратный вызов по событию.
auto eventSubscription = context.CreateElementaryEventSubscription(
  eventClassAddr,
  nodeAddr,
  [](ScElementaryEvent const & event) -> void
  {
    // Handle sc-event.
  });

// Инициируем sc-событие. Создайте sc-дугу из nodeAddr.
ScAddr const & otherNodeAddr = context.GenerateNode(ScType:::ConstNode);
context.GenerateConnector(ScType::ConstPermPosArc, nodeAddr, otherNodeAddr);
// После того, как будет сгенерирована sc-дуга, произойдет sc-событие 
// и будет вызван указанный обратный вызов по событию.
...
```

!!! предупреждение
    Класс sc-события должен быть действительным и принадлежать классу `sc_event`.
  
!!! примечание 
    Все классы sc-событий, предоставляемые sc-machine, всегда принадлежат классу `sc_event`.

### **CreateEventWaiter**

Вы можете сгенерировать waiter для какого-либо sc-события. Это полезно, когда вашему агенту приходится ждать другого агента.

```cpp
...
// Находим подписку на sc-событие (прослушивание) sc-элемента.
ScAddr const & nodeAddr = context.SearchElementBySystemIdentifier("my_node");
// Выберите тип sc-события, на которое хотите подписаться.
using MyEventType = ScEventAfterGenerateOutgoingArc<
  ScType::ConstPermPosArc>;
// Создаем waiter sc-события для сгенерированного sc-узла (прослушивамого sc-элемента).
auto eventWaiter = context.CreateEventWaiter<MyEventType>(
  nodeAddr,
  []() -> void
  {
    // Здесь вы можете указать код, который будет вызываться
    // когда вы вызовете метод `Wait`.
    // Например, это будет код, инициирующий какое-то sc-событие.
    // По умолчанию этот обратный вызов пуст.
  });

// После создания вызываем метод `Wait` и указываем время, которое 
// будем ждать sc-событие для указанного sc-элемента подписки
eventWaiter->Wait(200); // миллисекунды
// По умолчанию это время ожидания равно 5000 миллисекунд.
// Вы будете ждать, пока не произойдет sc-событие или пока не истечет указанное время.

...
// Какой-то другой код должен инициировать sc-событие (генерировать sc-дугу из `nodeAddr`).
ScAddr const & otherNodeAddr = context.GenerateNode(ScType:::ConstNode);
context.GenerateConnector(ScType::ConstPermPosArc, nodeAddr, otherNodeAddr);
// После этого будет сгенерирована sc-дуга, и waiter будет разрешен.
...
```

!!! предупреждение
    Вы должны предоставить действительный sc-элемент подписки. В противном случае будет выдано исключение.

Как и в случае с методом создания подписки на sc-событие, если вы заранее не знаете класс sc-события, вы можете создавать waiters динамически.

```cpp
...
// Находим подписку на sc-событие (прослушивание) sc-элемента.
ScAddr const & nodeAddr = context.SearchElementBySystemIdentifier("my_node");
// Выберите тип sc-события, чтобы подписаться или найти его.
ScAddr const & eventClassAddr = GetSomeEventType(); // User-specified method.
// Создаем waiter sc-события для сгенерированного sc-узла (прослушиваемого sc-элемента)   
// и обеспечиваем обратный вызов по событию.
auto eventWaiter = context.CreateEventWaiter(
  eventClassAddr,
  nodeAddr);
...
```

!!! предупреждение
    Класс sc-события должен быть действительным и принадлежать классу `sc_event`.

### **CreateConditionWaiter**

Помимо времени ожидания, вы также можете указать проверку, которая будет вызываться при возникновении sc-события, на которое мы подписаны.

```cpp
...
// Находим подписку на sc-событие (прослушивание) sc-элемента.
ScAddr const & nodeAddr = context.SearchElementBySystemIdentifier("my_node");
// Выберите тип sc-события, на которое хотите подписаться.
using MyEventType = ScEventAfterGenerateOutgoingArc<
  ScType::ConstPermPosArc>;
// Создаем waiter sc-события для сгенерированного sc-узла (прослушиваемого sc-элемента) 
auto eventWaiter = context.CreateConditionWaiter<MyEventType>(
  nodeAddr,
  []() -> void
  {
// Здесь вы также можете указать код, который будет вызываться
    // когда вы вызовете метод `Wait`.
    // Например, это будет код, инициирующий какое-то sc-событие.
    // По умолчанию этот обратный вызов пуст.
  },
  [](MyEventType const & event) -> bool
  {
    // Здесь вы можете указать проверку на sc-событие, которое произойдет.
    // Эта проверка должна возвращать логическое значение(bool).
  });

eventWaiter->Wait(200); // миллисекунды
// Вы будете ждать, пока не произойдет sc-событие или пока не истечет указанное время.

...
// Какой-то другой код должен инициировать sc-событие (генерировать sc-дугу из `nodeAddr`).
ScAddr const & otherNodeAddr = context.GenerateNode(ScType:::ConstNode);
context.GenerateConnector(ScType::ConstPermPosArc, nodeAddr, otherNodeAddr);
// После этого будет сгенерирована sc-дуга, и waiter будет разрешен.
...
```

Waiters с условием очень полезны, когда вы хотите вызвать какого-нибудь агента и дождаться, пока этот агент закончит работу.

```cpp
...
// Генерируем действие и указываем для него класс.
ScAddr const & actionAddr = context.GenerateNode(ScType::ConstNode);
context.GenerateConnector(
    ScType::ConstPermPosArc, MyKeynodes::my_action_class, actionAddr);
// Создайте waiter для sc-события для сгенерированного действия (прослушиваемого sc-элемента).
// Вы должны подождать, пока sc-дуга в действии от `action_finished_successfully` 
// будет сгенерирована.
auto eventWaiter = context.CreateConditionWaiter<
  ScEventAfterGenerateIncomingArc<ScType::ConstPermPosArc>>(
  actionAddr,
  [&]() -> void
  {
    // Здесь инициируем sc-событие.
    context.GenerateConnector(
        ScType::ConstPermPosArc, 
        ScKeynodes::action_initiated, 
        actionAddr);
  },
  [](ScEventAfterGenerateIncomingArc<
        ScType::ConstPermPosArc> const & event) -> bool
  {
    // Здесь проверяем sc-событие.
    return event.GetOtherElement()
      == ScKeynodes::action_finished_successfully;
  });

eventWaiter->Wait(200); // миллисекунды
// Вы будете ждать, пока не произойдет sc-событие или пока не истечет указанное время.
...
```

Вы также можете динамически генерировать waiters с условиями, предоставляя класс sc-события из базы знаний.

---

### **SubscribeAgent**

`ScModule` предоставляет API для статической подписки агентов на sc-события (смотреть [**C++ Modules API**](modules.md)). `ScAgentContext` также предоставляет методы для подписки агентов на sc-события. Это может быть полезно, если вы хотите динамически подписывать агентов на sc-события.

Чтобы подписать класс агента на sc-событие, используйте SubscribeAgent.

```cpp
...
// Для классов агентов, унаследованных от класса `ScAgent`, напишите так.
context.SubscribeAgent<MyAgent>(mySubscriptionElementAddr);

// Для классов агентов, унаследованных от класса `ScActionInitiatedAgent`, напишите так.
context.SubscribeAgent<MyActionAgent>();
...
```

### **UnsubscribeAgent**

Чтобы отказаться от подписки класса агента на sc-event, используйте `UnsubscribeAgent`.

```cpp
...
// Для классов агентов, унаследованных от класса `ScAgent`, напишите так.
context.UnsubscribeAgent<MyAgent>(mySubscriptionElementAddr);

// Для классов агентов, унаследованных от класса `ScActionInitiatedAgent`, напишите так.
context.UnsubscribeAgent<MyActionAgent>();
...
```

!!! примечание
    Если вы подписали на какой-либо агент на sc-событие, вам следует впоследствии отписать его от этого sc-события. В противном случае, когда вы остановите sc-машину, будут получены предупреждения об этом.

!!! предупреждение
    Вы должны предоставить действительные sc-элементы подписки.

### **SubscribeSpecifiedAgent**

Также вы можете подписать агента в соответствии со спецификацией, представленной в базе знаний. Вместо sc-элементов подписки следует предоставить реализации агентов.
Метод `SubscribeSpecifiedAgent` находит спецификацию абстрактного агента для указанной реализации агента и использует эту спецификацию для подписки указанного агента на sc-событие.

```cpp
...
context.SubscribeSpecifiedAgent<MyAgent>(agentImplementationAddr);
...
```

!!! примечание
    Если некоторые элементы спецификации не будут найдены, будут выброшены исключения. Конечно, эта спецификация заполнена и загружена в базу знаний. Этот метод не загружает спецификацию в базу знаний для указанного класса агента.

Вы можете подписать одних и тех же агентов на sc-события в соответствии с несколькими реализациями агентов. Эти реализации агента могут быть включены в один и тот же абстрактный агент или в разные абстрактные агенты.

```cpp
...
context.SubscribeSpecifiedAgent<MyAgent>(agentImplementationAddr1);
context.SubscribeSpecifiedAgent<MyAgent>(agentImplementationAddr2);
...
```

### **UnsubscribeSpecifiedAgent**

Этот метод находит спецификацию абстрактного агента для указанной реализации агента и использует эту спецификацию для отмены подписки указанного агента на sc-событие.

```cpp
...
// Если ваш агент был подписан на sc-событие согласно реализации  
// агента, вам следует отписаться от sc-события согласно 
// этой реализации агента.
context.UnsubscribeSpecifiedAgent<MyAgent>(agentImplementationAddr);
...
```

```cpp
...
// Если ваш агент был подписан на sc-события по нескольким 
// реализации агента, вам следует отказаться от подписки на sc-события 
// согласно этим реализациям агента.
context.UnsubscribeSpecifiedAgent<MyAgent>(agentImplementationAddr1);
context.UnsubscribeSpecifiedAgent<MyAgent>(agentImplementationAddr2);
...
```

!!! примечание
    Если некоторые элементы спецификации не будут найдены, будут выброшены исключения. Конечно, эта спецификация заполнена и загружена в базу знаний. Этот метод не загружает спецификацию в базу знаний для указанного класса агента.

!!! предупреждение
    Вы должны предоставить действительные реализации агента.

---

### **GenerateAction**

Все агенты выполняют действия. Мы предоставляем API для работы с ними. Используйте `GenerateAction` для создания объекта класса `ScAction`. Дополнительные сведения о действиях смотрите в разделе [**C++ Action API**](actions.md).

```cpp
// Находим класс действия и генерируем действие.
...
ScAction action = context.GenerateAction(actionClassAddr);
...
```

!!! примечание
    Вы должны предоставить класс действия, который относится к одному из типов: `receptor_action`, `effector_action`, `behavioral_action` или `information_action`.

!!! примечание
    Sc-адрес класса действия должен быть действительным.

### **ConvertToAction**

```cpp
// Или найти действие и преобразовать его в объект класса `ScAction`.
...
ScAction action = context.ConvertToAction(actionAddr);
...
```

!!! примечание
    Sc-адрес действия должен быть действительным.

### **GenerateSet**

Мы предоставляем API для удобной работы с множествами и структурами в базе знаний.

```cpp
// Создаем новое множество.
...
ScSet set = context.GenerateSet();
...
```

### **ConvertToSet**

```cpp
// Или найдем множество и преобразуем его в объект класса `ScSet`.
...
ScSet set = context.ConvertToSet(setAddr);
...
```

!!! примечание
    Sc-адрес множества должен быть действительным.

### **GenerateStructure**

```cpp
// Создаем новую структуру
...
ScStructure structure = context.GenerateStructure();
...
```

### **ConvertToStructure**


```cpp
// Или найдем структуру и преобразуем ее в объект класса `ScStructure`.
...
ScStructure structure = context.ConvertToStructure(structureAddr);
...
```

!!! примечание
    Sc-адрес структуры должен быть действительным.

--- 

## **Часто Задаваемые Вопросы**

<!-- no toc -->
- [Как добавить свой метод в `ScAgentContext`?](#how-do-i-add-my-method-to-a-scagentcontext)
- [Как я могу ждать события бесконечно долго?](#how-can-i-wait-for-an-event-indefinitely)
- [Что делать, если мне нужно инициировать агент по событиям с разных узлов?](#what-if-i-need-to-initiate-agent-on-events-from-different-nodes)
- [Когда ScMultiset будет реализован?](#when-scmultiset-will-be-implemented)
- [Что мне нужно для запуска агента на `ScEventGeneratingIncomingArc` и `ScEventGeneratingOutgoingArc` для одного узла?](#what-do-i-need-to-initiate-agent-on-sceventgeneratingincomingarc-and-sceventgeneratingoutgoingarc-for-one-node)
- [В чем разница между `ScSet` и `ScStructure`?](#whats-the-difference-between-scset-and-scstructure)

### **How do I add my method to a `ScAgentContext`?**

Вы можете создать класс, который будет наследовать класс `ScAgentContext`. Если вы хотите использовать его в своем агенте, сделайте следующее:

```cpp
class MyContext : public ScAgentContext
{
public:
  // Новые методы.
};

class MyAgent : public ScAgent<MyEventType, MyContext>
{
public:
  // Методы агента.
};
```

Вы сможете использовать m_context указанного вами класса в методах агента..

### **Как я могу ждать события бесконечно долго?**

По умолчанию вы можете подождать появления события до 5 секунд. Вы можете подождать другое удобное для вас время. Но мы рекомендуем ждать столько, сколько вам нужно, а не больше. Вам не нужно генерировать waiters на несколько минут или часов, поскольку агент работает гораздо быстрее, за миллисекунды.

Вы не можете ждать события бесконечно, всегда есть максимальное время ожидания, после которого вы перестанете ждать. Но чтобы ждать какого-то события бесконечно долго, вы можете реализовать и подписать агент для этого события. Он сработает, когда произойдет событие. Смотреть [**C++ Agents API**](agents.md), чтобы узнать больше об агентах.

### **Что делать, если мне нужно инициировать агент по событиям с разных узлов?**

Вы можете подписать агента на множество различных sc-элементов, но на sc-события одного и того же типа. Для этого вы можете использовать `SubscribeAgent` and `UnsubscribeAgent`.

```cpp
...
context.SubscribeAgent<MyAgent>(nodeAddr1, nodeAddr2);
// Здесь вы можете предоставить только элементы массива подписок 
// для классов агентов, унаследованных от класса `ScAgent`.
```

Но помните, что вам придется отказаться от подписки агента, пока система отключается.

```cpp
...
context.UnsubscribeAgent<MyAgent>(nodeAddr1, nodeAddr2);
```

### **Когда ScMultiset будет реализован?**

Сейчас реализованы только `ScSet` и `ScStructure`. `ScSet` хранит уникальные sc-элементы. `ScMultiset` будет реализован в ближайшее время.

### **Что мне нужно для запуска агента на `ScEventGeneratingIncomingArc` и `ScEventGeneratingOutgoingArc` для одного узла?**

Вы можете подписать агента на sc-событие генерации sc-коннектора -- `ScEventGeneratingConnector`. Тогда агент будет срабатывать на входящих и исходящих sc-дугах.

### **В чем разница между `ScSet` и `ScStructure`?**

`ScSet` — это класс, представляющий sc-множество. Это сущность, которая связывает одну или несколько сущностей в единое целое. `ScStructure` — это класс, представляющий sc-структуру. Это совокупность sc-элементов, включающая sc-связи или sc-структуры, связывающие эти sc-элементы. Удаление одного из sc-элементов sc-структуры может привести к нарушению целостности этой sc-структуры. `ScSet` и `ScStructure` функционально не отличаются. «ScStructure» — это более строгое представление sc-множества.

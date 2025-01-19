# **C++ Modules API**

!!! примечание
    Это верно только для версий sc-machine >= 0.10.0.
--- 

**C++ Modules API** описывает, как создавать модули, группирующие агенты в sc-machine, и управлять ими. В этом разделе описывается, как зарегистрировать агентов в приложении.

!!! примечание
    Чтобы подключить этот API, укажите #include <sc-memory/sc_module.hpp> в исходном файле hpp.

---

## **ScModule**

Этот класс является базовым классом для подписки/отписки агентов на sc-события или от них. Это похоже на сложный компонент, который содержит подключенных агентов.

Чтобы подписать своих агентов на sc-события, реализуйте класс модуля и вызовите методы `Agent` для подписки на этих агентов.

```cpp
// Файл my_module.hpp
#pragma once

#include <sc-memory/sc_module.hpp>

class MyModule : public ScModule
{
};
```

```cpp
// Файл my_module.cpp:
#include "my-module/my_module.hpp"

#include "my-module/keynodes/my_keynodes.hpp"
#include "my-module/agent/my_agent.hpp"

SC_MODULE_REGISTER(MyModule)
// Инициализирует статический объект класса `MyModule`, который можно 
// использовать для вызова методов подписки агентов на sc-события.
  ->Agent<MyAgent>(); 
  // Он подписывает агента и возвращает объект `MyModule`.
  // `MyAgent` унаследован от `ScActionInitiatedAgent`.
  // Этот метод указывает на модуль, что класс агента `MyAgent` 
  // должен быть подписан на sc-событие добавления исходящей sc-дуги из 
  // sc-элемент `action_initiated`. Это параметр по умолчанию в этом методе,
  // если вы хотите подписаться на унаследованный класс агента. 
  // из `ScActionInitiatedAgent`.
```

Вы должны вызвать метод `Agent` для классов агентов, унаследованных от `ScActionInitiatedAgent`, без аргументов, но вы должны вызвать его, предоставив sc-элемент подписки sc-события для классов агентов, унаследованных от `ScAgent`.

Модуль подписывал агентов при инициализации sc-памяти и отписывался от них при отключении sc-памяти.
Также вы можете использовать модуль для подписки на множество агентов.

```cpp
// Файл my_module.cpp:
#include "my-module/my_module.hpp"

#include "my-module/agent/my_agent1.hpp"
#include "my-module/agent/my_agent2.hpp"
#include "my-module/agent/my_agent3.hpp"
#include "my-module/agent/my_agent4.hpp"
#include "my-module/agent/my_agent5.hpp"

SC_MODULE_REGISTER(MyModule)
  ->Agent<MyAgent1>()
  ->Agent<MyAgent2>()
  ->Agent<MyAgent3>()
  ->Agent<MyAgent4>()
  ->Agent<MyAgent5>()
  // ...
  ;
```

Если вам нужно инициализировать неагентские объекты в модуле, вы можете переопределить методы `Initialize` и `Shutdown` в классе вашего модуля.

```diff
// Файл my_module.hpp:
class MyModule : public ScModule
{
+ void Initialize(ScMemoryContext * context) override;
+ void Shutdown(ScMemoryContext * context) override;
};
```

```diff
// Файл my_module.cpp:
#include "my-module/my_module.hpp"

#include "my-module/agent/my_agent.hpp"

SC_MODULE_REGISTER(MyModule)
  ->Agent<MyAgent>(); 

+ // Этот метод будет вызван один раз. 
+ void MyModule::Initialize(ScMemoryContext * context)
+ {
+   // Здесь реализуем инициализацию ваших объектов.
+ }
+ // Этот метод будет вызван один раз. 
+ void MyModule::Shutdown(ScMemoryContext * context)
+ {
+   // Здесь реализуем выключение ваших объектов.
+ }
```

---

## **Спецификация динамического агента**

Модули позволяют подписывать агентов с динамической спецификацией, представленной в базе знаний или в коде. Динамическая спецификация может быть изменена другими агентами. Дополнительные сведения о типах спецификаций агентов смотреть в [C++ Agents API](agents.md).

Для этого класса `ScModule` есть метод `AgentBuilder`. Вы можете вызвать этот метод с классом агента, предоставляющим ключевой узел реализации агента, указанный в базе знаний, или вызвать методы после этого метода, чтобы установить элементы спецификации для данного агента.

### **ScAgentBuilder**

Метод `AgentBuilder` генерирует объект класса `ScAgentBuilder`, необходимый для инициализации спецификации агента из кода или из базы знаний.

### **Загрузка исходной спецификации агента на C++**

Вы можете указать начальную спецификацию вашего класса агента в коде с помощью `ScAgentBuilder`.

```cpp
// Файл my_module.cpp:
#include "my-module/my_module.hpp"

#include "my-module/agent/my_agent.hpp"

SC_MODULE_REGISTER(MyModule)
  ->AgentBuilder<MyAgent>()
    // Абстрактный агент должен принадлежать `abstract_sc_agent`.
    ->SetAbstractAgent(MyKeynodes::my_abstract_agent)
    ->SetPrimaryInitiationCondition({
        // Event class must belong to `sc_event`.
        ScKeynodes::sc_event_after_generate_outgoing_arc, 
        ScKeynodes::action_initiated
    }) 
    // Вы должны предоставить класс действия, который относится к одному из типов: 
    // `receptor_action`, `effector_action`, `behavioral_action` или
    // `information_action`.
    ->SetActionClass(MyKeynodes::my_action_class)
    ->SetInitiationConditionAndResult({
        MyKeynodes::my_agent_initiation_condition_template,
        MyKeynodes::my_agent_result_condition_template
    })
    ->FinishBuild();
```

Таким образом, вы можете загрузить исходную спецификацию вашего агента в базу знаний из кода. Вы можете изменить ее или нет, это зависит от вашей задачи.

!!! примечание
    Если спецификация на агента уже существует в базе знаний, то новые соединения создаваться не будут, т.е. не будет дубликатов.

!!! примечание
    Все предоставленные аргументы должны быть действительными, иначе ваш модуль не будет подписан, поскольку возникнут ошибки.

!!! предупреждение
    Если спецификации агента еще нет в базе знаний, вам следует вызвать все методы, перечисленные после вызова `AgentBuilder`.

!!! предупреждение
    В конце списка после вызова `AgentBuilder` необходимо вызвать метод `FinishBuild`, иначе ваш код не сможет скомпилироваться.

### **Загрузка спецификации агента из базы знаний**

Если у вас в базе знаний есть спецификация вашего агента, записанная в SCs-коде или SCg-коде, то вы можете просто указать реализацию вашего агента.

Напишите scs-спецификацию (или scg-спецификацию) вашего агента и используйте ее для подписки на ваш агент внутри модуля.

```scs
// Спецификация агента в базе знаний.
my_abstract_agent
<- abstract_sc_agent;
=> nrel_primary_initiation_condition: 
    (sc_event_after_generate_outgoing_arc => action_initiated); 
=> nrel_sc_agent_action_class:
    my_action_class; 
=> nrel_initiation_condition_and_result: 
    (my_agent_initiation_condition_template 
        => my_agent_result_condition_template);
<= nrel_sc_agent_key_sc_elements:
{
    action_initiated;
    my_action_class;
    my_class
};
=> nrel_inclusion: 
    my_agent_implementation 
    (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
            [github.com/path/to/agent/sources] 
            (* => nrel_format: format_github_source_link;; *)
        };; 
    *);;

my_agent_initiation_condition_template
= [*
    my_action_class _-> .._action;;
    action_initiated _-> .._action;;
    .._action _-> rrel_1:: .._parameter;;
*];; 

my_agent_result_condition_template
= [*
    my_class _-> .._my_node;;
*];;
```

<image src="../images/agents/my_agent_specification.png"></image>

```cpp
// Файл my_module.cpp:
#include "my-module/my_module.hpp"

#include "my-module/agent/my_agent.hpp"

SC_MODULE_REGISTER(MyModule)
  ->AgentBuilder<MyAgent>(ScKeynodes::my_agent_implementation)
    ->FinishBuild();
```

!!! примечание
    Если спецификация вашего агента не заполнена в базе знаний, то модуль не будет подписан, поскольку возникнут ошибки. Другие правильно указанные агенты будут подписаны без ошибок.

---

## **Часто Задаваемые Вопросы**

<!-- no toc -->
- [Можно ли подписаться на агента без вызова метода для его подписки?](#is-it-possible-to-subscribe-an-agent-without-calling-a-method-to-subscribe-it)
- [Можно ли сгенерировать один модуль и прописать в нем всех агентов?](#is-it-possible-to-generate-one-module-and-subscribe-all-agents-in-it)
- [Если есть разница, в каком порядке подписываться на агентов?](#if-there-is-a-difference-in-what-order-to-subscribe-agents)

### **Можно ли подписаться на агента без вызова метода для его подписки?**

Вы можете реализовать агент, который будет просматривать все спецификации агентов в базе знаний и подписывать агентов в соответствии с его спецификациями.

### **Можно ли сгенерировать один модуль и прописать в нем всех агентов?**

Вы можете это сделать, но мы советуем вам создавать компоненты из агентов. Это означает, что агенты должны быть включены в один и тот же модуль, если их наличие имеет смысл в одном компоненте. Привыкайте к тому, что каждый модуль является компонентом.

### **Если есть разница, в каком порядке подписываться на агентов?**

Наверное, нет. Агенты не должны зависеть друг от друга. Но если да, то лучше этого не делать.

# **C++ Modules API**

!!! note
    This is correct for only versions of sc-machine that >= 0.10.0.
--- 

The **C++ Modules API** outlines how to create and manage modules that group agents within sc-machine. This section describes how to register agents in an application.

!!! note
    To include this API provide `#include <sc-memory/sc_module.hpp>` in your hpp source.

---

## **ScModule**

This class is a base class for subscribing/unsubscribing agents to/from sc-events. It's like a complex component that contains connected agents.

To subscribe your agents to sc-events, implement module class and call `Agent` methods to subscribe these agents.

```cpp
// File my_module.hpp
#pragma once

#include <sc-memory/sc_module.hpp>

class MyModule : public ScModule
{
};
```

```cpp
// File my_module.cpp:
#include "my-module/my_module.hpp"

#include "my-module/keynodes/my_keynodes.hpp"
#include "my-module/agent/my_agent.hpp"

SC_MODULE_REGISTER(MyModule)
// It initializes static object of `MyModule` class that can be 
// used to call methods for subscribing agents to sc-events.
  ->Agent<MyAgent>(); 
  // It subscribes agent and returns object of `MyModule`.
  // `MyAgent` is inherited from `ScActionInitiatedAgent`.
  // This method pointers to module that agent class `MyAgent`
  // should be subscribed to sc-event of adding outgoing sc-arc from 
  // sc-element `action_initiated`. It is default parameter
  // in these method if you want to subscribe agent class inherited 
  // from `ScActionInitiatedAgent`.
```

You must call `Agent` method for agent classes inherited from `ScActionInitiatedAgent` without arguments, but you should call it providing sc-event subscription sc-element for agent classes inherited from `ScAgent`.

A module subscribed agents when the sc-memory initializes and it unsubscribes them when the sc-memory shutdowns.
Also, you can use module to subscribe a set of agents.

```cpp
// File my_module.cpp:
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

If you need to initialize not agent objects in module, you can override `Initialize` and `Shutdown` methods in your module class.

```diff
// File my_module.hpp:
class MyModule : public ScModule
{
+ void Initialize(ScMemoryContext * context) override;
+ void Shutdown(ScMemoryContext * context) override;
};
```

```diff
// File my_module.cpp:
#include "my-module/my_module.hpp"

#include "my-module/agent/my_agent.hpp"

SC_MODULE_REGISTER(MyModule)
  ->Agent<MyAgent>(); 

+ // This method will be called once. 
+ void MyModule::Initialize(ScMemoryContext * context)
+ {
+   // Implement initialize of your objects here.
+ }
+ // This method will be called once. 
+ void MyModule::Shutdown(ScMemoryContext * context)
+ {
+   // Implement shutdown of your objects here.
+ }
```

---

## **Dynamic agent specification**

Modules allow to subscribe agents with dynamic specification provided in knowledge base or in code. Dynamic specification can be changed by other agents. To learn more about types of agent specifications in [C++ Agents API](agents.md).

For this `ScModule` class has `AgentBuilder` method. You can call this method with agent class providing keynode of agent implementation specified in knowledge base or calling methods after this method to set the specification elements for the given agent.

### **ScAgentBuilder**

The `AgentBuilder` method generates object of `ScAgentBuilder` class that is needed to initialize agent specification from code or from knowledge base.

### **Loading initial agent specification in C++**

You can specify initial specification for your agent class in code with help of `ScAgentBuilder`.

```cpp
// File my_module.cpp:
#include "my-module/my_module.hpp"

#include "my-module/agent/my_agent.hpp"

SC_MODULE_REGISTER(MyModule)
  ->AgentBuilder<MyAgent>()
    // Abstract agent must belong to `abstract_sc_agent`.
    ->SetAbstractAgent(MyKeynodes::my_abstract_agent)
    ->SetPrimaryInitiationCondition({
        // Event class must belong to `sc_event`.
        ScKeynodes::sc_event_after_generate_outgoing_arc, 
        ScKeynodes::action_initiated
    }) 
    // You should provide action class that is include to the one of types: 
    // `receptor_action`, `effector_action`, `behavioral_action` or 
    // `information_action`.
    ->SetActionClass(MyKeynodes::my_action_class)
    ->SetInitiationConditionAndResult({
        MyKeynodes::my_agent_initiation_condition_template,
        MyKeynodes::my_agent_result_condition_template
    })
    ->FinishBuild();
```

So you can load the initial specification for your agent into the knowledge base from the code. You can change it or not, it depends on your problem.

!!! note
    If specification for an agent already exists in the knowledge base, no new connections will be generated, i.e. there will be no duplicates.

!!! note
    All provided arguments must be valid, otherwise you module will not be subscribed, because errors will occur.

!!! warning
    If specification for an agent isn't already in the knowledge base, you should call all the methods listed after `AgentBuilder` call.

!!! warning
    At the end of list after `AgentBuilder` call you should call `FinishBuild` method, otherwise your code can't be compiled.

### **Loading agent specification from knowledge base**

If you has specification for your agent in the knowledge base wrote in SCs-code or SCg-code, then you can just specify implementation of your agent.

Write scs-specification (or scg-specification) of your agent and use it with to subscribe your agent within module.

```scs
// Specification of agent in knowledge base.
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
// File my_module.cpp:
#include "my-module/my_module.hpp"

#include "my-module/agent/my_agent.hpp"

SC_MODULE_REGISTER(MyModule)
  ->AgentBuilder<MyAgent>(ScKeynodes::my_agent_implementation)
    ->FinishBuild();
```

!!! note
    If specification of your agent isn't full in the knowledge base, then module will not be subscribed, because errors will occur. Other correctly specified agents will be subscribed without errors.

---

## **Frequently Asked Questions**

<!-- no toc -->
- [Is it possible to subscribe an agent without calling a method to subscribe it?](#is-it-possible-to-subscribe-an-agent-without-calling-a-method-to-subscribe-it)
- [Is it possible to generate one module and subscribe all agents in it?](#is-it-possible-to-generate-one-module-and-subscribe-all-agents-in-it)
- [If there is a difference in what order to subscribe agents?](#if-there-is-a-difference-in-what-order-to-subscribe-agents)

### **Is it possible to subscribe an agent without calling a method to subscribe it?**

You can implement an agent that will traverse all agent specifications in the knowledge base and subscribe agents according its specifications.

### **Is it possible to generate one module and subscribe all agents in it?**

You can do that, but we advise you to build components from agents, meaning agents should be included in the same module if they make sense to be in the same component. Get used to having every module be a component.

### **If there is a difference in what order to subscribe agents?**

Probably, not. Agents shouldn't be dependent on each other. But if you did, it's better not to do so.

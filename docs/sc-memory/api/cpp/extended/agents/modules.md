# **C++ Modules API**

!!! warning
    This documentation is correct for only versions of sc-machine that >= 0.10.0.
--- 

This API provides functionality to implement modules for registering agents on C++.

---

## **ScModule**

This class is a base class for registering agents. It's like a complex component that contains connected agents.

To register your agents, implement module class and call `Agent` methods to register agents.

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
#include "my-module/agents/my_agent.hpp"

SC_MODULE_REGISTER(MyModule)
// It initializes static object of `MyModule` class that can be 
// used to call methods for registering agents.
  ->Agent<MyAgent>(); 
  // It registers agent and returns object of `MyModule`.
  // `MyAgent` is derived from `ScActionAgent`.
  // This method pointers to module that agent class `MyAgent`
  // should be subscribed to sc-event of adding output arc from 
  // sc-element `action_initiated`. It is default parameter
  // in these method if you want register agent class derived 
  // from `ScActionAgent`.
```

You must call `Agent` method for agent classes derived from `ScActionAgent` without arguments, but you should call it providing sc-event subscription sc-element for agent classes derived from `ScAgent`.

A module registers agents when the sc-memory initializes and it unregisters them when the sc-memory shutdowns.
Also, you can use module to register a set of agents.

```cpp
// File my_module.cpp:
#include "my-module/my_module.hpp"

#include "my-module/agents/my_agent1.hpp"
#include "my-module/agents/my_agent2.hpp"
#include "my-module/agents/my_agent3.hpp"
#include "my-module/agents/my_agent4.hpp"
#include "my-module/agents/my_agent5.hpp"

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

#include "my-module/agents/my_agent.hpp"

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

Modules allow to register agents with dynamic specification provided in knowledge base or in code. Dynamic specification can be changed by other agents. To learn more about types of agent specifications in [C++ Agents API](agents.md).

For this `ScModule` class has `AgentBuilder` method. You can call this method with agent class providing keynode of agent implementation specified in knowledge base or calling methods after this method to set the specification elements for the given agent.

### **ScAgentBuilder**

The `AgentBuilder` method creates object of `ScAgentBuilder` class that is needed to initialize agent specification from code or from knowledge base.

### **Loading initial agent specification in C++**

You can specify initial specification for your agent class in code with help of `ScAgentBuilder`.

```cpp
// File my_module.cpp:
#include "my-module/my_module.hpp"

#include "my-module/agents/my_agent.hpp"

SC_MODULE_REGISTER(MyModule)
  ->AgentBuilder<MyAgent>()
    ->SetAbstractAgent(MyKeynodes::my_abstract_agent)
    ->SetPrimaryInitiationCondition({
        ScKeynodes::sc_event_add_output_arc, 
        ScKeynodes::action_initiated
    })
    ->SetActionClass(MyKeynodes::my_action_class)
    ->SetInitiationConditionAndResult({
        MyKeynodes::my_agent_initiation_condition_template,
        MyKeynodes::my_agent_result_condition_template
    })
    ->FinishBuild();
```

So you can load the initial specification for your agent into the knowledge base from the code. You can change it or not, it depends on your task.

!!! note
    If specification for an agent already exists in the knowledge base, no new connections will be created, i.e. there will be no duplicates.

!!! note
    All provided arguments must be valid, otherwise you module will not be registered, because errors will occur.

!!! warning
    If specification for an agent isn't already in the knowledge base, you should call all the methods listed after `AgentBuilder` call.

!!! warning
    At the end of list after `AgentBuilder` call you should call `FinishBuild` method, otherwise your code can't be compiled.

### **Loading agent specification from knowledge base**

If you has specification for your agent in the knowledge base wrote in SCs-code or SCg-code, then you can just specify implementation of your agent.

Write scs-specification of your agent and connect it with module that register your agent.

```scs
// Specification of agent in kb.
// Abstract sc-agent
my_abstract_agent
<- abstract_sc_agent;
=> nrel_primary_initiation_condition: 
    (sc_event_add_output_arc => action_initiated); 
=> nrel_sc_agent_action_class:
    my_action_class; 
=> nrel_initiation_condition_and_result: 
    (my_agent_initiation_condition_template 
        => my_agent_result_condition_template);
<= nrel_sc_agent_key_sc_elements:
{
    action_initiated;
    action
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
*];; 

my_agent_result_condition_template
= [*
    my_action_class _-> .._action;;
    action_initiated _-> .._action;;
    action_finished_successfully _-> .._action;;
    .._action _=> nrel_result:: .._result;;
*];;
```

```cpp
// File my_module.cpp:
#include "my-module/my_module.hpp"

#include "my-module/agents/my_agent.hpp"

SC_MODULE_REGISTER(MyModule)
  ->AgentBuilder<MyAgent>(ScKeynodes::my_agent_implementation)
    ->FinishBuild();
```

!!! note
    If specification of your agent isn't full in the knowledge base, then module will not be registered, because errors will occur.

---

## **Frequently Asked Questions**

<!-- no toc -->
- [Is it possible to register an agent without calling a method to register it?](#is-it-possible-to-register-an-agent-without-calling-a-method-to-register-it)
- [Is it possible to create one module and register all agents in it?](#is-it-possible-to-create-one-module-and-register-all-agents-in-it)
- [If there is a difference in what order to register agents?](#if-there-is-a-difference-in-what-order-to-register-agents)

### **Is it possible to register an agent without calling a method to register it?**

You can implement an agent that will traverse all agent specifications in the knowledge base and register agents according its specifications.

### **Is it possible to create one module and register all agents in it?**

You can do that, but we advise you to build components from agents, meaning agents should be included in the same module if they make sense to be in the same component. Get used to having every module be a component.

### **If there is a difference in what order to register agents?**

Probably, not. Agents shouldn't be dependent on each other. But if you did, it's better not to do so.

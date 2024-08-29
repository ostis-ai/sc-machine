# **C++ Agents API**

!!! note
    This is correct for only versions of sc-machine that >= 0.10.0.
--- 

This API provides functionality to implement agents on C++.

## **What is the agent-driven model?**

The sc-machine implements the **agent-driven model** to manage processing knowledge. In the agent-based model, agents exchange messages only through shared memory, adding a new agent or eliminating one or more existing agents does not result in changes to other agents, agent initiation is decentralized and most often independent of each other. 

All agents within the OSTIS Technology are divided into two classes: platform-independent, i.e. implemented only by means of SC-code, and platform-dependent, implemented by means of sc-machine API. Platform-independent agents can be implemented with help of SCP language which is interpreted by [**scp-machine**](https://github.com/ostis-ai/scp-machine). The sc-machine presents a powerful, but simple API for developing and maintaining platform-dependent agents in C++.

All agents react to the occurrence of events in sc-memory (sc-events). That is, an agent is called implicitly when an sc-event occurs, for which type this agent is already subscribed. Knowledge about which sc-event will cause this agent to be called (awakening of this agent) is called primary initiation condition. Upon awakening, the agent checks for the presence of its full initiation condition. If the full initiation condition is successfully checked, the agent initiates an action of some class and starts performing it with a agent program (see [**C++ Actions API**](actions.md) to learn more about actions). After executing its program, the agent can check if there is a result.

## **What does agent specification represent?**

All knowledge about an agent: *primary initiation condition*, *class of actions* it can perform, *initiation condition*, and *result condition*, are part of **agent's specification**. This specification can be represented either in a knowledge base, using SC-code, or programably, using sc-machine API.

Let's describe specification for abstract sc-agent of counting power of specified set in SCs-code (SCg-code). An abstract sc-agent is a class of functionally equivalent agents, different instances of which can be implemented in different ways. Each abstract sc-agent has a specification corresponding to it.

```scs
// Abstract sc-agent
agent_calculate_set_power
<- abstract_sc_agent;
=> nrel_primary_initiation_condition: 
    // Class of sc-event and listen (subscription) sc-element
    (sc_event_after_generate_outgoing_arc => action_initiated); 
=> nrel_sc_agent_action_class:
    // Class of actions to be performed by agent
    action_calculate_set_power; 
=> nrel_initiation_condition_and_result: 
    (..agent_calculate_set_power_initiation_condition 
        => ..agent_calculate_set_power_result_condition);
<= nrel_sc_agent_key_sc_elements:
// Set of key sc-elements used by this agent
{
    action_initiated;
    action;
    action_calculate_set_power;
    nrel_set_power;
};
=> nrel_inclusion: 
    // Instance of abstract sc-agent; concrete implementation of agent in C++
    agent_calculate_set_power_implementation 
    (*
        <- platform_dependent_abstract_sc_agent;;
        // Set of links with paths to sources of agent programs
        <= nrel_sc_agent_program: 
        {
            [github.com/path/to/agent/sources] 
            (* => nrel_format: format_github_source_link;; *)
        };; 
    *);;

// Full initiation condition of agent
..agent_calculate_set_power_initiation_condition
= [*
    action_calculate_set_power _-> .._action;;
    action_initiated _-> .._action;;
    .._action _-> rrel_1:: .._parameter;;
*];; 
// Agent should check by this template that initiated action is instance of 
// class `action_calculate_set_power` and that it has argument.

// Full result condition of agent
..agent_calculate_set_power_result_condition
= [*
    action_calculate_set_power _-> .._action;;
    action_initiated _-> .._action;;
    action_finished_successfully _-> .._action;;
    .._action _-> rrel_1:: .._parameter;;
    .._action _=> nrel_result:: .._result (* <- sc_node_struct;; *);;
*];;
// Agent should check by this template that initiated action is finished 
// and that it has result.
```

<image src="../images/agents/agent_calculate_set_power_specification.png"></image>

## **What are ways of providing the agent's specification?**

The sc-machine API provides two types of functionalities to implement an agent in C++:

* when the agent's specification is represented in the knowledge base and used by the agent;
* when the agent's specification is represented in C++ code and also used by the agent.

In both cases, the agent's specification can be static, dynamic, or semi-dynamic.

1. **Static agent specification** is a specification provided externally in the agent's class (via overriding public getters). This specification isn't saved in the knowledge base because changes in the knowledge base can't automatically alter user-specified code. To provide this type of agent specification, you can use overriding public getters in your agent's class (see documentation below).
2. **Dynamic agent specification** is a specification provided in the knowledge base or initially in the code but automatically saved into the knowledge base. After that, the first agent changes the specification of the second agent, and the second agent uses the changed specification. To implement this type of specification, use the API of `ScModule` class and the API of `ScAgentBuilder` class (see [**C++ Modules**](modules.md)).
3. **Semi-dynamic agent specification** can be obtained when the specification is provided in the knowledge base or initially in the code and appended externally (via overriding public getters).

Static agent specification can be useful if you are implementing an agent in C++ for the first time or if you want to minimize the number of searches in the knowledge base. Dynamic agent specification provides the opportunity to analyze and change this specification by other agents. Semi-dynamic specification can be useful if you want to change some parts of this specification but still want the agent's calls to be quick.

---

## **Static agent specification**

Here, the API for implementing an agent with a static specification will be discussed. This is the easiest implementation variant to understand for you. To learn about dynamic agent specification, see [**C++ Modules API**](modules.md).

There are two main classes that you can use to implement an agent: `ScAgent` and `ScActionInitiatedAgent`.

### **ScAgent**

There is a base class for agents in C++. This class provides implemented methods to retrieve elements of the agent's specification from the knowledge base. All these methods can be overridden in your agent class.

You should distinguish between an abstract sc-agent as some class of functional equivalent sc-agents described in the knowledge base and `ScAgent` as a C++ class that implements an API to work with abstract sc-agents in the knowledge base.

This class can be used for all types of platform-dependent agents. The example using this class is represented below.

```cpp
// File my_agent.hpp
#pragma once

#include <sc-memory/sc_agent.hpp>

// Inherit your agent class from `ScAgent` class and specify template argument 
// as sc-event class. Here `ScEventAfterGenerateIncomingArc<
// ScType::EdgeAccessConstPosPerm>` is type of event to which the given 
// agent reacts.
class MyAgent : public ScAgent<
  ScEventAfterGenerateIncomingArc<ScType::EdgeAccessConstPosPerm>>
{
public:
  // Here you should specify class of actions which the given agent performs. 
  // Here `GetActionClass` overrides `GetActionClass` 
  // in `ScAgent` class. This overriding is required.
  ScAddr GetActionClass() const override;
  // Here you should implement program of the given agent. 
  // This overriding is required.
  ScResult DoProgram(
    ScEventAfterGenerateIncomingArc<
      ScType::EdgeAccessConstPosPerm> const & event, 
    ScAction & action) override;

  // Other user-defined methods.
};
```

You can't override `DoProgram` without sc-event argument. There can be override one of these methods. 

```cpp
// File my_agent.hpp
#pragma once

#include <sc-memory/sc_agent.hpp>

class MyAgent : public ScAgent<
  ScEventAfterGenerateIncomingArc<ScType::EdgeAccessConstPosPerm>>
{
public:
  ScAddr GetActionClass() const override;
  ScResult DoProgram(ScAction & action) override;

  // Other user-defined methods.
};
```

See [**C++ Events API**](events.md) and [**C++ Event subscriptions API**](event_subscriptions.md) to learn how to use and handle sc-events.

!!! note
    Define action class as keynode in agent class or keynodes class.

!!! warning
    You must override methods `GetActionClass` and `DoProgram`.

You can specify any existing event types as a template argument to the `ScAgent` class. For example, you can create agent that will be triggered to sc-event of removing sc-element.

```cpp
// File my_agent.hpp
#pragma once

#include <sc-memory/sc_agent.hpp>

class MyAgent : public ScAgent<ScEventBeforeEraseElement>
{
public:
  ScAddr GetActionClass() const override;
  ScResult DoProgram(
    ScEventBeforeEraseElement const & event, ScAction & action) override;

  // Other user-defined methods.
};
```

If you want to change specification of this agent in knowledge base, then write like this:

```cpp
// File my_agent.hpp
#pragma once

#include <sc-memory/sc_agent.hpp>

class MyAgent : public ScElementaryEventAgent
{
public:
  ScAddr GetActionClass() const override;
  ScResult DoProgram(
    ScElementaryEvent const & event, ScAction & action) override;

  // Other user-defined methods.
};
```

This implementation allows to provide any sc-event type to `DoProgram`.

!!! note
    `ScElementaryEventAgent` is alias for `ScAgent<ScElementaryEvent>`.

!!! note
    If you provide specification of your agent in knowledge base, then you don't need to override `GetActionClass`. See [**C++ Modules API**](modules.md) to learn how to implement agents with specification in the knowledge base.

### **ScActionInitiatedAgent**

In multi-agent systems most of the agents are implemented to execute actions initiated by other agents. While `ScAgent` is useful to create broad event handling logic, using it to handle action initiations requires some boilerplate. We've implemented another agent class to make it easier for our users to implement action-executing agents. These agents are named action agents. Implementing these agents requires passing action class node rather than checking initiation condition manually.

This class can be only used for agents that should be triggered by generating an output sc-arc from `action_initiated` class node.

```cpp
// File my_agent.hpp
#pragma once

#include <sc-memory/sc_agent.hpp>

// Inherit your agent class from `ScActionInitiatedAgent` class.
class MyAgent : public ScActionInitiatedAgent
{
public:
  // Here you should specify class of actions which the given agent performs. 
  // This overriding is required.
  ScAddr GetActionClass() const override;
  // Here you should implement program of the given agent. 
  // This overriding is required.
  ScResult DoProgram(ScActionInitiatedEvent const & event, ScAction & action) override;
  // Here `ScActionInitiatedEvent` is type of event to which 
  // the given agent reacts.

  // Other user-defined methods.
};
```

!!! note
    `ScActionInitiatedAgent` has default `GetInitiationConditionTemplate` that returns template that can be used to check that initiated action is action with class of specified agent.

!!! note
    `ScActionInitiatedEvent` is alias for `ScEventAfterGenerateOutgoingArc<ScType::EdgeAccessConstPosPerm>` with subscription sc-element `action_initiated`.

---

### **Required methods**

#### **GetActionClass**

It gets action class that the agent of specified class performs. If the abstract sc-agent for this agent class does not have an action class, then method will throw `utils::ExceptionItemNotFound`.

```cpp
// File my_agent.cpp
#include "my_agent.hpp"

#include "keynodes/my_keynodes.hpp"

ScAddr MyAgent::GetActionClass() const
{
  // You must specify valid sc-address of action class. 
  // In other case, the given sc-agent can’t be subscribed to sc-event.
  return MyKeynodes::my_action;
}
```

See [**C++ Keynodes API**](keynodes.md) and learn how to define keynodes and use them for your agent.

#### **DoProgram**

This method is executed when agent checked initiation condition successfully. Using program, the agent processes an input construction and generates an output construction.
Each agent performs action with the help of its program.

```cpp
ScResult MyAgent::DoProgram(ScActionInitiatedEvent const & event, ScAction & action)
{
  // Class `ScAction` encapsulates information about sc-action. The provided 
  // action is action that the given agent performs right now. 
  // It belongs to `MyKeynodes::my_action` class. 
  // Actions are copyable and movable. `ScAction` is inherited from `ScAddr`.

  // `ScActionInitiatedEvent` class is event type on which the given agent 
  // is triggered. It is encapsulate information about sc-event. 
  // The provided event is event on which the agent is triggered right now. 
  // It has methods to get information about initiated sc-event: `GetUser`, 
  // `GetArc`, `GetSubscriptionElement`, `GetArcSourceElement`.
 
  // Implement logic of your agent...

  // You must specify action state in all ends of your agent program. 
  // `FinishSuccessfully` sets action as `action_finished_successfully`.
  // You can’t create object of `ScResult` via constructor, because it is 
  // private.
  return action.FinishSuccessfully(); 
}
```

`ScAgent` class has field `m_context`. It is an object of `ScAgentContext` class. You can use to complete operations in sc-memory. See [**C++ Agent context API**](agent_context.md) to learn more about accessible methods.

!!! warning
    If you don't catch sc-exceptions in `DoProgram` then sc-machine will catch them and will finish action with error and will warn you about it.

##### Handling action arguments

There are many ways of methods that get action arguments. Use them, they can help you to simplify code.

```cpp
ScResult MyAgent::DoProgram(ScActionInitiatedEvent const & event, ScAction & action)
{
  auto [argAddr1, argAddr] = action.GetArguments<2>();

  // Some logic...

  return action.FinishSuccessfully(); 
}
```

```cpp
ScResult MyAgent::DoProgram(ScActionInitiatedEvent const & event, ScAction & action)
{
  ScAddr const & argAddr1 = action.GetArgument(ScKeynodes::rrel_1);
  // Parameter has ScAddr type.

  // Some logic...

  return action.FinishSuccessfully();
}
```

```cpp
ScResult MyAgent::Program(ScActionInitiatedEvent const & event, ScAction & action)
{
  ScAddr const & argAddr1 = action.GetArgument(1); // size_t
  // It would be the same if we pass ScKeynodes::rrel_1 instead of 1.

  // Some logic...

  return action.FinishSuccessfully();
}
```

```cpp
ScResult MyAgent::Program(MyEventType const & event, ScAction & action)
{
  ScAddr const & argAddr1 
    = action.GetArgument(1, MyKeynodes::default_text_link);
  // If action hasn’t the first argument, then MyKeynodes::default_text_link 
  // will return.

  // Some logic...

  return action.FinishSuccessfully();
}
```

```cpp
ScResult MyAgent::DoProgram(ScActionInitiatedEvent const & event, ScAction & action)
{
  // You can use object of ScAction as object of ScAddr.
  ScIterator3Ptr const it3 = m_context.Iterator3(action, ..., ...);

  // Some logic...

  return action.FinishSuccessfully();
}
```

##### Handling action result

```cpp
ScResult MyAgent::DoProgram(ScActionInitiatedEvent const & event, ScAction & action)
{
  // Some logic...
 
  action.FormResult(foundAddr1, generatedAddr1, ...); 
  // Or you can use `UpdateResult` method.
  return action.FinishSuccessfully();
}
```

To learn more about methods of `ScAction` class, see [**C++ Actions API**](actions.md).

```cpp
ScResult MyAgent::DoProgram(ScActionInitiatedEvent const & event, ScAction & action)
{
  // Some logic...
 
  action.SetResult(structureAddr);
  return action.FinishSuccessfully();
}
```

##### Handling action finish state

```cpp
ScResult MyAgent::DoProgram(ScActionInitiatedEvent const & event, ScAction & action)
{
  // Some logic...

  if (/* case 1 */)
    return action.FinishSuccessfully();
  else if (/* case 2 */)
    return action.FinishUnsuccessfully();
  else
    return action.FinishWithError();
}
```

```cpp
ScResult MyAgent::Program(MyEventType const & event, ScAction & action)
{
  action.IsInitiated(); // result: true
  action.IsFinished(); // result: false
  action.IsFinishedSuccessfully(); // result: false

  // Some logic...

  return action.FinishSuccessfully();
}
```

!!! warning
    The API of `ScAction` provides other methods. Don't use `GetResult` for initiated but not finished action and `Initiate` for initiated or finished action.

---

### **Not required methods**

#### **GetAbstractAgent**

It searches abstract agent for agent of specified class. If the agent implementation for this agent class is not included in any abstract sc-agent, then `GetAbstractAgent` will throw `utils::ExceptionItemNotFound`.

You can redefine this method in your agent class.

```cpp
ScAddr MyAgent::GetAbstractAgent() const
{
  // You must specify valid sc-address of abstract agent. 
  // In other case, the given sc-agent can’t be subscribed to sc-event.
  return MyKeynodes::my_abstract_agent;
}
```

!!! warning
    Remember, if you override only this method and required methods, then other getters will return elements of specification for specified abstract agent. All not override getters call `GetAbstractAgent`. 

#### **GetEventClass**

It searches the sc-event class to which the agent class is subscribed. It will throw `utils::ExceptionItemNotFound` if the abstract sc-agent for this agent class does not have primary initiation condition.

You can redefine this method in your agent class.

```cpp
ScAddr MyAgent::GetEventClass() const
{
  // You must specify valid sc-address of event class. 
  // In other case, the given sc-agent can’t be subscribed to sc-event.
  return ScKeynodes::sc_event_after_generate_outgoing_arc;
}
```

#### **GetEventSubscriptionElement**

This method searches sc-event subscription sc-element for which sc-event initiates. It will throw `utils::ExceptionItemNotFound` if the abstract sc-agent for this agent class does not have primary initiation condition.

You can redefine this method in your agent class.

```cpp
ScAddr MyAgent::GetEventSubscriptionElement() const
{
  // You must specify valid sc-address of sc-event subscription sc-element. 
  // In other case, the given sc-agent can’t be subscribed to sc-event.
  return ScKeynodes::action_initiated;
}
```

!!! warning
    Don't override `GetEventClass` and `GetEventSubscriptionElement` for agent with statically specified sc-event type. Your code can't be compiled. Override them, if your agent class inherit `ScAgent<ScElementaryEvent>` (`ScElementaryEventAgent`).

#### **GetInitiationCondition**

It gets initiation condition for agent of this class. Initiation condition is a sc-template (sc-structure) that is used by the agent to check necessary connections between sc-elements of sc-event triple and sc-elements in the knowledge base before the agent performs an action. This method will throw `utils::ExceptionItemNotFound` if the abstract sc-agent for this agent class does not have initiation condition.

You can redefine this method in your agent class.

```cpp
ScAddr MyAgent::GetInitiationCondition() const
{
  // You must specify valid sc-address or sc-template of initiation condition. 
  // In other case, the given sc-agent can’t be called.
  return ScKeynodes::my_initiation_condition;
}
```

```cpp
// sc_keynodes.hpp
...
// Here you should specify template to check initiation condition before 
// starting agent.
static inline ScTemplateKeynode const & my_initiation_condition
  = ScTemplateKeynode(my_initiation_condition)
    // You must specify valid sc-address of initiation condition. In other case, 
    // the given sc-agent can’t be called. For sc-event of generating (erasing) 
    // sc-connector (sc-arc or sc-edge), you must specify in template of 
    // initiation condition a triple in place of which agent have to substitute 
    // sc-elements involved in initiated event. These elements will be the 
    // sc-connector (sc-arc or sc-edge) and its incident sc-elements. 
    // For sc-event of changing sc-link content (or erasing sc-element), you 
    // should use sc-link (sc-element) in template of initiation condition 
    // explicitly. Otherwise your agent will not be called because its 
    // initiation condition is fulfilled for the action of another agent.
    .Triple(
      ScKeynodes::action_initiated,
      ScType::EdgeAccessVarPosPerm,
      ScType::NodeVar >> "_action"
    )
    // After you should specify triples that only apply to your agent.
    .Triple(
      MyKeynodes::my_action,
      ScType::EdgeAccessVarPosPerm,
      "_action"
    )
    .Quintuple(
      "_action",
      ScType::EdgeAccessVarPosPerm,
      ScType::NodeVar,
      ScType::EdgeAccessVarPosPerm,
      ScKeynodes::rrel_1
    );
...
```

!!! warning
    Agent must have initiation condition template with only one triple that can be used to substitute sc-elements involved in initiated event, otherwise agent will not started performing sc-action.

#### **GetInitiationConditionTemplate**

You can specify the initiation condition template in code rather than in the knowledge base. It can be useful if you want to quickly check agent initiation condition without loading the template into the knowledge base.

```cpp
ScTemplate MyAgent::GetInitiationConditionTemplate(
  ScActionInitiatedEvent const & event) const
{
  // In this method, you shouldn't specify triple for sc-event sc-elements.
  // You should specify triples that only apply to your agent.
  initiationCondition.Triple(
    MyKeynodes::my_action,
    ScType::EdgeAccessVarPosPerm,
    event.GetOtherElement() // It returns sc-action.
  );
  return initiationCondition;
}
```

#### **CheckInitiationCondition**

For speed, you can implement the agent initiation condition in the form of checks on iterators.

```cpp
bool MyAgent::CheckInitiationCondition(ScActionInitiatedEvent const & event)
{
 // ScActionInitiatedEvent is event type on which the given agent triggered. 
 // It is encapsulate information about sc-event. The provided event is event 
 // on which the agent is triggered right now. It has methods to get 
 // information about initiated sc-event: GetUser, GetArc, 
 // GetSubscriptionElement, GetArcSourceElement, GetArcTargetElement.
 // All events are not copyable and movable.
 return m_context.HelperCheckEdge(
   ScType::EdgeAccessConstPosPerm, 
   MyKeynodes::my_action, 
   event.GetArcTargetElement());
}
// You can use all opportunities of sc-memory API to check initiation 
// condition of your agent.
```

!!! note
    Each of these methods will be called before `DoProgram` call.

!!! warning
    You can specify only one of methods: `GetInitiationCondition`, `GetInitiationConditionTemplate` and `CheckInitiationCondition`. Otherwise, your code can't be compiled.

#### **GetResultCondition**

It gets result condition for agent of this class. Result condition is a sc-template (sc-structure) that is used by the agent to check necessary connections between sc-elements of sc-event triple and sc-elements in the knowledge base after the agent has performed an action. This method will throw `utils::ExceptionItemNotFound` if the abstract sc-agent for this agent class does not have result condition.

You can redefine this method in your agent class.

```cpp
ScAddr MyAgent::GetResultCondition() const
{
  // Here you should specify template to check action result content.
  // You must specify valid sc-address or sc-template of result condition. 
  // In other case, the given sc-agent can’t be finished successfully.
  return ScKeynodes::my_result_condition;
}
```

```cpp
// sc_keynodes.hpp
...
// Here you should specify template to check action result content.
static inline ScTemplateKeynode const & my_result_condition
  = ScTemplateKeynode(my_result_condition)
    .Triple(
      MyKeynodes::my_class,
      ScType::EdgeAccessVarPosPerm,
      ScType::NodeVar
    );
    // This template is used to check that agent generated sc-arc
    // between `MyKeynodes::my_class` and some sc-node.
...
```

#### **GetResultConditionTemplate**

You can specify the result condition template in code rather than in the knowledge base.

```cpp
ScTemplate MyAgent::GetResultConditionTemplate(
  ScActionInitiatedEvent const & event, ScAction & action) const
{
  // Here you should specify template to check action result content.
  ScTemplate resultCondition;
  resultCondition.Triple(
    MyKeynodes::my_class,
    ScType::EdgeAccessVarPosPerm,
    ScType::NodeVar
  );
  return resultCondition;
}
```

!!! note
    `GetResultCondition` and `GetResultConditionTemplate` are used to search sc-constructions in action result only.

#### **CheckResultCondition**

For speed, you can implement the agent result condition in the form of checks on iterators.

```cpp
bool MyAgent::CheckResult(ScActionInitiatedEvent const & event, ScAction & action)
{
  ScStructure const & actionResult = action.GetResult();
  ScIterator3Ptr const it3 = m_context.Iterator3(
    MyKeynodes::my_class,
    ScType::EdgeAccessConstPosPerm,
    ScType::NodeConst,
    ScType::EdgeAccessConstPosPerm,
    actionResult
  );
  return it3->Next() 
    && actionResult.HasElement(MyKeynodes::my_class) 
    && actionResult.HasElement(it3->Get(2))
    && !it3->Next() // Check that action result has no more triples.
    && m_context.GetOutputArcsCount(actionResult) == 3;
}
```

!!! note
    Each of these methods will be called after `DoProgram` call.

!!! warning
    You can specify only one of methods: `GetResultCondition`, `GetResultConditionTemplate` and `CheckResultCondition`. Otherwise, your code can't be compiled.

--- 

## **Frequently Asked Questions**

<!-- no toc -->
- [Is it possible to subscribe an agent for more than one sc-event?](#is-it-possible-to-subscribe-an-agent-for-more-than-one-sc-event)
- [What happens if I don't specify full initiation condition in agent class?](#what-happens-if-i-dont-specify-full-initiation-condition-in-agent-class)
- [Can there be an agent without primary initiation condition?](#can-there-be-an-agent-without-primary-initiation-condition)
- [Is it possible to create object of `ScAgent` class and call any of class methods?](#is-it-possible-to-create-object-of-scagent-class-and-call-any-of-class-methods)
- [Is agent's call protocol preserved?](#is-agents-call-protocol-preserved)
- [What is advisable to do as an agent and what is not?](#what-is-advisable-to-do-as-an-agent-and-what-is-not)
- [What's the purpose of result condition? What is it used for?](#whats-the-purpose-of-result-condition-what-is-it-used-for)

### **Is it possible to subscribe an agent for more than one sc-event?**

Future versions of the sc-machine will implement complex sc-events. Complex sc-events will be a set of elementary events. It will be possible to subscribe agents to such sc-events.

If it is a question of whether it is possible to subscribe the one and the same agent to two different elementary sc-events, it is better not to do it. Each agent should be subscribed to only one type of sc-event. If you need to subscribe an agent to multiple types of sc-events, you're probably doing something wrong.

Consider an example. Suppose you want to implement an agent to recalculate power of a set, which will react when a sc-arc is removed from the set and when a sc-arc is added from it. You should not do such an agent. It is better to split these two logics into two independent objects: an agent that will react when a sc-arc is erased from the specified set and an agent that will react when a sc-arc is generated from this set.

### **What happens if I don't specify full initiation condition in agent class?**

We allow not specifying full initiation condition for agents. However, remember, your agents will be executed every time an event occurs over the listen sc-element your agent is subscribed to.

### **Can there be an agent without primary initiation condition?**

No, agents can't be without primary initiation condition. Agents react to events in sc-memory. A primary initiation condition indicates which sc-event agent is subscribed to, i.e. when agent will be called.

### **Is it possible to create object of `ScAgent` class and call any of class methods?**

Yes, you can call a method of agent class through object of that class. But remember that you can't create necessary arguments for all methods.

### **Is agent's call protocol preserved?**

Until the sc-machine implements functionality for logging processes in sc-memory. It will be added in the next versions. However, the sc-machine does not remove anything by itself.

### **What is advisable to do as an agent and what is not?**

It depends on your goal. If you want to make an ostis-system, then the whole functionality of your system should be a collective of agents. If you want to use a machine as a database to store and process information in a convenient and simple way, not everything should be agents. To improve performance, use agents with static specification or without specification, combine multiple agents into one, optimize sc-memory requests and etc.

### **What's the purpose of result condition? What is it used for?**

Now, it has no effect on agent finishing. But in the future it can be used for verification and debugging of agents' work.

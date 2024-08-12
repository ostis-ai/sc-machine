# **Implementing agent in C++**

!!! warning
    This documentation is correct for only versions of sc-machine that >= 0.10.0.
--- 

All agents in C++ represent some classes in C++. To implement an agent in C++, you need to perform the following common steps:

1. Write input (initial) construction and output (result) construction of your future agent in SC-code.
2. Create folder with source and header files for sc-agent implementation.
3. Also you need write `CMakeLists.txt` file. We use cmake to build projects in C++.
4. In header file, define a class in C++ for this agent and specifies in it at least class of actions that this agent interprets and its program. In such class you can also specify primary initiation condition, initiation condition, and result condition.
5. In source file, implement all declared methods of agent's class. You can also implement your own methods and use them in an agent program. You can use all C++ and OOP tools as much as possible. 
6. Create file and implement class for keynodes used by implemented agent.
7. Implement class for module for registering implemented agent. 

---

Let's consider an example of implementing an agent to count power of the given set:

### **1. Specify inputs and outputs of your future agent.**

The initial construction of agent might look like this:

```scs
..action
<- action_for_сalculating_set_power;
<- action_initiated;
-> rrel_1: ..some_set;;

..some_set
-> ..element_1;
-> ..element_2;
-> ..element_3;;
```

The result construction of agent might look like this:

```scs
..action
<- action_for_сalculating_set_power;
<- action_initiated;
<- action_finished_successfully;
-> rrel_1: ..some_set;
=> nrel_answer: ..answer;;

..answer = [*
  ..some_set => nrel_set_power: [3];;
*];;
```

In addition to agents that initiate actions themselves and then interpret these actions, there is a need to implement agents that interpret actions initiated by other agents. For this class of agents, it is much easier to create a initial initiation construction in the knowledge base.

--- 

### **2. Create folder with source and header files for agent and `CMakeLists.txt.` file.**

You should get something like this structure:

```plaintext
set-agents-module/
├── CMakeLists.txt
├── agents/
│   ├── sc_agent_for_сalculating_set_power.hpp
│   └── sc_agent_for_сalculating_set_power.сpp
```

---

### **3. Write `CMakeLists.txt` file.**
  
The `CMakeLists.txt` file should describe a process of building your agent code. It should contain various instructions and parameters needed to compile and link agent source code to sc-machine libraries.

**CMakeLists.txt**

```cmake
file(GLOB SOURCES CONFIGURE_DEPENDS
    "*.cpp" "*.hpp"
    "*/*.cpp" "*/*.hpp"
)

# Create and link your library with using libraries.
add_library(set-agents SHARED ${SOURCES})
target_link_libraries(set-agents LINK_PRIVATE sc-memory)
target_include_directories(set-agents PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
```

---

### **4. Define a class in C++ for this agent and specifies class of actions that this agent interprets and its program.**

An agent's class to be implemented must comply with the following requirements:

a. It must inherit one of the common classes for implementing agents: 
  
  - `template <class TScEvent> class ScAgent`,
  - or `class ScActionAgent`.

The base class `ScAgent` contains API to implement agents that react to any sc-events. The base class `ScActionAgent` inherits base class `ScAgent` and provides API to implement agents that react to sc-events of initiating sc-action.

b. It must override at least methods `ScAddr GetAction() const` and `ScResult DoProgram(ScActionEvent const & event, ScAction & action)`.

c. Overriden methods must be public. Otherwise, you won't be able to build your code because the sc-machine won't be able to call methods on your agent class.

d. You can implement other methods in agent's class.

**sc_agent_for_сalculating_set_power.hpp**

```cpp
#pragma once

#include <sc-memory/sc_agent.hpp>

class ScAgentForCalculatingPower : public ScActionAgent
{
public:
  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScActionEvent const & event, ScAction & action) override;
};
```

---

### **5. Implement all declared methods of agent's class.**

**sc_agent_for_сalculating_set_power.cpp**

```cpp
#include "sc_agent_for_сalculating_set_power.hpp"

ScAddr ScAgentForCalculatingPower::GetActionClass() const
{
  return m_memoryCtx.HelperFindBySystemIdtf("action_for_сalculating_set_power");
}
// You must specify valid action class. In other case, the agent can’t be 
// registered in sc-memory.

ScResult ScAgentForCalculatingPower::DoProgram(ScActionEvent const & event, ScAction & action)
{
  // `ScActionEvent` class is event type on which the given agent 
  // is triggered. It is encapsulate information about sc-event. 
  // The provided event is event on which the agent is triggered right now. 
  // It has methods to get information about initiated sc-event: `GetUser`, 
  // `GetAddedArc`, `GetSubscriptionElement`, `GetArcSourceElement`, 
  // `GetArcTargetElement`. All events are not copyable and not movable. 
  // ScAction is derived from ScObject.

  // `ScAction` class encapsulates information about sc-action. 
  // The provided action is action that the given agent interpretes right now. 
  // It belongs to class action_for_сalculating_set_power`. 
  // Actions are copyable and movable. ScAction is derived from ScAddr.

  auto const & [setAddr] = action.GetArguments<1>(); 
  // This method finds construction `action => rrel_1: setAddr`.
  // Here the 1 is number of arguments which action must have. In step 1, 
  // we specified that an action should have a set as its the first and 
  // only one argument. But the one who calls this agent may not specify argument 
  // for the action. So we need to check that the action has argument. 
  if (!m_memoryCtx.IsElement(setAddr))
  {
    SC_AGENT_LOG_ERROR("Action has not argument."); 
    // output: "ScAgentForCalculatingPower: Action has not argument."
    return action.FinishWithError();
  }
  // There may be a situation where someone is trying to specify a number of 
  // arguments more then it is needed. You can also check this by specifying, 
  // for example, number 2 instead of number 1. But you don't always need 
  // to do this.

  // To calculate power of the set, we can traverse all accessory constant 
  // positive permanents arcs from the set and count number of these arcs. 
  // But, in any task, the presence of NON-factors must be considered, 
  // but this is omitted here.
  size_t setPower = 0;
  ScIterator3Ptr const it3 = m_memoryCtx.Iterator3( 
    setAddr,
    ScType::EdgeAccessConstPosPerm,
    ScType::NodeConst
  );
  while (it3->Next())
    ++setPower;

  ScAddr const & setPowerAddr = m_memoryCtx.CreateLink(ScType::LinkConst);
  m_memoryCtx.SetLinkContent(setPowerAddr, setPower);
  ScAddr const & arcCommonAddr 
    = m_memoryCtx.CreateEdge(ScType::EdgeDCommonConst, setAddr, setPowerAddr);
  ScAddr const & nrelSetPowerAddr 
    = m_memoryCtx.HelperFindBySystemIdtf("nrel_set_power");
  ScAddr const & arcAccessAddr 
    = m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, nrelSetPowerAddr, arcAddr);

  action.FormAnswer(
    setAddr, arcCommonAddr, setPowerAddr, arcAccessAddr, nrelSetPowerAddr);
  SC_AGENT_LOG_DEBUG("Set power was counted.");

  // At the end of the agent's program, you must call one of three methods 
  // (`FinishSuccessfully`, `FinishUnsuccessfully`, `FinishWithError`) to indicate 
  // that the agent's interpretation of action is complete:
  // - Method `FinishSuccessfully` indicates that action was interpreted by agent 
  // successfully (sets class `action_finished_successfully`). 
  // It means that the agent solved specified task.
  // - Method `FinishUnsuccessfully` indicates that action was interpreted by agent
  // unsuccessfully (sets class `action_finished_unsuccessfully`). 
  // It means that the agent didn't solved specified task.
  // - Method `FinishWithError` indicates that action was interpreted by agent
  // with error (sets class `action_finished_with_error`). 
  // It means that some incorrect situation in knowledge base occured.
  // All these methods return objects of `ScResult` class. 
  // You can’t create object of ScResult via constructor, because it is private.
  return action.FinishSuccessfully();
}
```

---

### **6. Define keynodes for implemented agent and integrate their in agent program.**

For each agent, you can specify key sc-elements that this agent uses during the execution of its program. These key sc-elements are sc-elements that agent does not create, but uses in the process of searching for or creating connections between entities in knowledge base. Key sc-elements are named keynodes. You can find these keynodes by its system identifiers (method `HelperFindBySystemIdtf`) if they have such identifiers. Also, you can use class `ScKeynode` to define keynodes as static objects and use them in agents.

```diff
set-agents-module/
  ├── CMakeLists.txt
  ├── agents/
  │   ├── sc_agent_for_сalculating_set_power.hpp
  │   └── sc_agent_for_сalculating_set_power.сpp
+├── keynodes/
+│   └── sc_set_keynodes.hpp
```

**sc_set_keynodes.hpp**

```cpp
#include <sc-memory/sc_keynodes.hpp>

// This class unites keynodes that used by agents of one module 
// (with one sense). You can implement hierarchies of keynodes.
class ScSetKeynodes : public ScKeynodes
{
public:
  static inline ScKeynode const action_for_сalculating_set_power{
    "action_for_сalculating_set_power", ScType::NodeConstClass};
  static inline ScKeynode const nrel_set_power{
    "nrel_set_power", ScType::NodeConstNoRole};
  // Here the first argument in constructor is system identifier of 
  // sc-keynode and the second argument is sc-type of this sc-keynode.
  // If there is no sc-keynode with such system identifier in 
  // knowledge base, then the one with specified sc-type will be created.
  // You don't should specify type of sc-keynode here, be default it is 
  // `ScType::NodeConst`. But you sure that your code will use this 
  // keynode with type `ScType::NodeConst` correctly.
};
```

!!! warning
    You can't specify sc-keynode with empty system identifier. It can be invalid.

!!! warning
    All keynodes must be static objects. You can define keynodes as static objects everywhere (not only in classes).

`ScKeynodes` class is base class for all classes with keynodes. It contains core keynodes, that can be used in each agent.

**sc_agent_for_сalculating_set_power.cpp**

```diff
#include "sc_agent_for_сalculating_set_power.hpp"

#include "keynodes/sc_set_keynodes.hpp"

ScAddr ScAgentForCalculatingPower::GetActionClass() const
{
- return m_memoryCtx.HelperFindBySystemIdtf("action_for_сalculating_set_power");
+ return ScSetKeynodes::action_for_сalculating_set_power;
}

ScResult ScAgentForCalculatingPower::DoProgram(ScActionEvent const & event, ScAction & action)
{
  ...

- ScAddr const & nrelSetPowerAddr = m_memoryCtx.HelperFindBySystemIdtf("nrel_set_power");
- ScAddr const & arcAccessAddr 
-   = m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, nrelSetPowerAddr, arcAddr);
+ ScAddr const & arcAccessAddr 
+   = m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, ScSetKeynodes::nrel_set_power, arcAddr);

  ...
}
```

---

### **7. Implement module class to register your keynodes and agent.**

Someone should subscribe your agent to event. It can be other agent, or any code at all. You can implement class, that allows register keynodes and agents. This class is named sc-module. Each sc-module should register keynodes and agents with common sense.

```diff
  set-agents-module/
  ├── CMakeLists.txt
  ├── agents/
  │   ├── sc_agent_for_сalculating_set_power.hpp
  │   └── sc_agent_for_сalculating_set_power.сpp
  ├── keynodes/
  │   └── sc_set_keynodes.hpp
+├── sc_set_module.hpp
+└── sc_set_module.cpp
```

**sc_set_module.hpp**

```cpp
#pragma once

#include <sc-memory/sc_module.hpp>

class ScSetModule : public ScModule
{
  // Here class is empty. You doesn't need to implement any methods. 
  // `ScModule` class contains all nessary API to register your keynodes 
  // and agents as separate sc-module.
};
```

**sc_set_module.cpp**

```cpp
#include "sc_set_module.hpp"

#include "keynodes/sc_set_keynodes.hpp"
#include "agents/sc_agent_for_сalculating_set_power.hpp"

SC_MODULE_REGISTER(ScSetModule)
  ->Keynodes<ScSetKeynodes>()
  // This method pointers to module that all keynodes in `ScSetKeynodes` should
  // be initialized.
  ->Agent<ScAgentForCalculatingPower>();
  // This method pointers to module that agent class `ScAgentForCalculatingPower`
  // should be subcribed to sc-event of adding output arc from sc-element
  // `action_initiated`. It is default parameter in these method if you want
  // register agent class derived from `ScActionAgent`.

// This way of registering keynodes and agents makes it easier to write code. 
// You don't have to think about deregistering keynodes and agents after 
// the system shutdown - your module will do it all by itself.
```

If you want to initialize something else in your module besides keynodes and agents, you can override methods `Initialize(ScMemoryContext * context) override;` and `Shutdown(ScMemoryContext * context) override;`.

**sc_set_module.hpp**

```diff
class ScSetModule : public ScModule
{
+ void Initialize(ScMemoryContext * context) override;
+ void Shutdown(ScMemoryContext * context) override;
};
```

**sc_set_module.cpp**

```diff
SC_MODULE_REGISTER(ScSetModule)
  ->Keynodes<ScSetKeynodes>()
  ->Agent<ScAgentForCalculatingPower>();

+ // This method will be called once. 
+ void ScSetModule::Initialize(ScMemoryContext * context)
+ {
+   // Implement initialize of your objects here.
+ }
+ // This method will be called once. 
+ void ScSetModule::Shutdown(ScMemoryContext * context)
+ {
+   // Implement shutdown of your objects here.
+ }
```

--- 

## **Frequently Asked Questions**

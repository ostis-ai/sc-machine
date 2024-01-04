# **C++ Agents API**

> This documentation is correct for only versions of sc-machine that >= 0.9.0.
 
This API provides functionality to implement sc-agents on C++.

Each sc-agent on C++ refers to program entity that performs actions in sc-memory and can be called only by emission of
sc-event on which this program entity is registered. In C++ all sc-agents are registered by sc-memory or sc-modules
connected to sc-memory. Each sc-module can register certain sc-agents, not only one. Division by sc-modules are usually 
performed logically by developer.

## **ScModule**

It is a dynamic library. It implements specified module class. To create module you need to do next steps:

1. Create directory for your module in project using sc-machine. For example, `my-module`.
2. Go to created directory.
3. Create file `CMakeLists.txt` with content (replace `my_module` (module name) to your one):

**CMakeLists.txt**

```bash
# Define alias for directory path with your sc-module and sc-agents in it.
set(MY_AGENT_SRC ${CMAKE_CURRENT_LIST_DIR})

# Collect your sc-agent sources.
set(SOURCES
    "my_module.cpp"
)
set(HEADERS
	"my_module.hpp"
)

# Include using other directories.
include_directories(${MY_AGENT_SRC} ${SC_MEMORY_SRC})

# Create and link your library with using libraries.
add_library(my_lib SHARED ${SOURCES} ${HEADERS})
add_dependencies(my_lib sc-memory)
target_link_libraries(my_lib sc-memory)

# Specify where code for your sc-agent will be generated.
sc_codegen_ex(my_lib ${MY_AGENT_SRC} "${MY_AGENT_SRC}/generated")
```

After that you need to create two files (.cpp and .hpp):

**my_module.hpp**

```cpp
#pragma once

#include <sc-memory/sc_module.hpp>

#include "my_module.generated.hpp"

// Your module class for sc-agents
class MyModule : public ScModule
{
  // Priority that can be used during load all sc-modules.
  SC_CLASS(LoadOrder(10))
  SC_GENERATED_BODY()

  virtual sc_result InitializeImpl() override;
  virtual sc_result ShutdownImpl() override;
}
```

**my_module.cpp**

```cpp
#include "my_module.hpp"

SC_IMPLEMENT_MODULE(MyModule)

// This method will be called once. 
// It is usually used to register sc-agents and sc-keynodes.
sc_result MyModule::InitializeImpl()
{
  return SC_RESULT_OK;
}

// This method will be called once. 
// It is usually used to unregister sc-agents and sc-keynodes.
sc_result MyModule::ShutdownImpl()
{
  return SC_RESULT_OK;
}
```

## **ScAgent**

To create sc-agent implementation you need:

1. Create source and header file for sc-agent implementation.
2. Add created files into `CMakeLists.txt`.
3. Register implemented sc-agent in created sc-module.

**CMakeLists.txt**

```diff
...

set(SOURCES
  "my_module.cpp"
+ "my_agent.cpp"
)
set(HEADERS
  "my_module.hpp"
+ "my_agent.hpp"
)

...
```

**my_agent.hpp**

```cpp
#pragma once

#include "sc-memory/kpm/sc_agent.hpp"
#include "sc-memory/sc_keynodes.hpp"

#include "my_agent.generated.hpp"

// Use namespaces to user can import in his code only needed sc-agents.
namespace myAgents
{

class MyAgent : public ScAgent
{
  // Specify sc-event on which this sc-agent will be called.
  SC_CLASS(
      Agent, 
      Event(ScKeynodes::question_initiated, ScEvent::Type::AddOutputEdge)
  )
  // This sc-agent can be called if sc-connector is created from sc-element 
  // with sc-address `ScKeynodes::question_initiated`.
  SC_GENERATED_BODY()
}

}
```

**my_agent.cpp**

```cpp
#include "my_agent.hpp"

#include "sc-memory/sc_memory.hpp"

namespace myAgents
{

SC_AGENT_IMPLEMENTATION(MyAgent)
{
  // Implement sc-agent logic here. 
  // Use Core C++ API to implement logic of your sc-agent.
  return SC_RESULT_OK;
}

}
```

`SC_AGENT_IMPLEMENTATION` is a macro that inserts generated code to minimize work. You just insert this one
(with class name as parameter), and implement it like a function. During implementation, you can use such variables:

* `listenAddr` - sc-address of sc-element listening for sc-events in sc-memory (see [ScEvent](#scevent));
* `edgeAddr` - sc-address of sc-connector that will be created to or from listening sc-element with sc-address 
`listenAddr` (if sc-event type is `ScEventEraseElement` or `ScEventContentChanged`, then value of this variable is empty 
sc-address);
* `otherAddr` - sc-address of target or source sc-element of sc-connector with sc-address `edgeAddr` (if sc-event type 
is `ScEventEraseElement` or `ScEventContentChanged`, then value of this variable is empty sc-address);
* `m_memoryCtx` - sc-memory context of calling sc-agent that can be used to work with sc-memory.

If sc-agent finished work without any errors, then it must return `SC_RESULT_OK`, otherwise - one of code error
`SC_RESULT_ERROR_...`.

Possible results:

* `SC_RESULT_ERROR` - unknown error;
* `SC_RESULT_OK` - no any errors (successfully finish);
* `SC_RESULT_ERROR_INVALID_PARAMS` - invalid parameters of agent;
* `SC_RESULT_ERROR_INVALID_TYPE` - invalid type error (invalid type of data of something else);
* `SC_RESULT_ERROR_INVALID_STATE` - invalid state of processing data;
* `SC_RESULT_ERROR_NOT_FOUND` - something wasn't found (for example: can't resolve element by identifier).

**my_module.cpp**

```diff
#include "my_module.hpp"

+ #include "my_agent.hpp"

SC_IMPLEMENT_MODULE(MyModule)

sc_result MyModule::InitializeImpl()
{
+ SC_AGENT_REGISTER(myAgents::MyAgent);
  return SC_RESULT_OK;
}

sc_result MyModule::ShutdownImpl()
{
+ SC_AGENT_UNREGISTER(myAgents::MyAgent);
  return SC_RESULT_OK;
}
```

**Some important rules:**

* You can implement more than one `ScAgent` class in on source/header file;
* Don't use any other memory contexts instead of `m_memoryCtx` in `ScAgent` implementation;
* You need always include `<you_header>.generated.hpp` file into your header, if you have any metadata.
  This include must be a last one in a file.

## **ScEvent**

This type of objects allows you to subscribe to any events in sc-memory. There are C++ classes that correspond to
specified event types:

* `ScEventAddOutputEdge` - emits each time, when output (outgoing) sc-connector (from specified sc-element) is created;
* `ScEventAddInputEdge` - emits each time, when input (ingoing) sc-connector (into specified sc-element) is created;
* `ScEventRemoveOutputEdge` - emits each time, when output (outgoing) sc-connector (from specified sc-element) is erased;
* `ScEventRemoveInputEdge` - emits each time, when input (ingoing) sc-connector (into specified sc-element) is erased;
* `ScEventEraseElement` - emits, when specified sc-element is erased;
* `ScEventContentChanged` - emits each time, when content of specified sc-link is changed.

Each event constructor takes 3 parameters:

* `ctx` - `ScMemoryContext` that will be used to work with event;
* `addr` - `ScAddr` of sc-element that need to be listened for a specified event;
* `func` - delegate to a callback function, that will be called on each event emit
  (`bool func(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr)`).
  Description of parameters for this function you can see in table below (for each event type).

> Note: Callback function will be called in another thread!

The table of description (parameters of callback function named on pictures, if there are no parameter name on picture,
then it's would have an empty value):
<table width="95%">
  <tr>
    <th>Class</th>
    <th>Description</th>
  </tr>

  <tr>
    <td><strong>ScEventAddOutputEdge</strong></td>
    <td>
      <scg src="../images/sc-events/sc-event-output-edge.gwf"></scg>
      <br/>
      Callback calls each time, when some sc-connector <code>edgeAddr</code> between listening sc-element with <code>listenAddr</code> and some sc-element with <code>otherAddr</code> is created.
      <hr/>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
auto const callback = [](ScAddr const & listenAddr,
                         ScAddr const & edgeAddr,
                         ScAddr const & otherAddr)
{
    // listenAddr - sc-address of source sc=element
    //  (listen it in sc-event)
    // edgeAddr - sc-address of added output sc-connector
    // otherAddr - sc-address of target element of added 
    //  sc-connector
    ...
    return SC_TRUE; // if failed, then return SC_FALSE
};
ScEventAddOutputEdge event(ctx, addr, callback);
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventAddInputEdge</strong></td>
    <td>
      <scg src="../images/sc-events/sc-event-input-edge.gwf"></scg>
      <br/>
      Callback calls each time, when some sc-connector <code>edgeAddr</code> between some sc-element with <code>otherAddr</code> and listening sc-element with <code>listenAddr</code> is created.
      <hr/>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
auto const callback = [](ScAddr const & listenAddr,
                         ScAddr const & edgeAddr,
                         ScAddr const & otherAddr)
{
    // listenAddr - sc-address of target sc-element
    //  (listen it in sc-event)
    // edgeAddr - sc-address of added input sc-connector
    // otherAddr - sc-address of source sc-element of added 
    //  sc-connector
    ...
    return SC_TRUE; // if failed, then return SC_FALSE
};
ScEventAddInputEdge event(ctx, addr, callback);
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventRemoveOutputEdge</strong></td>
    <td>
      <scg src="../images/sc-events/sc-event-output-edge.gwf"></scg>
      <br/>
      Callback calls each time, when some sc-connector <code>edgeAddr</code> between listening sc-element with <code>listenAddr</code> and some sc-element with <code>otherAddr</code> is erased.
      <hr/>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
auto const callback = [](ScAddr const & listenAddr,
                         ScAddr const & edgeAddr,
                         ScAddr const & otherAddr)
{
    // listenAddr - sc-address of source sc-element
    //  (listen it in sc-event)
    // edgeAddr - sc-address of erased output sc-connector
    // otherAddr - sc-address of target sc-element of erased 
    //  sc-connector
    ...
    return SC_TRUE; // if failed, then return SC_FALSE
};
ScEventRemoveOutputEdge event(ctx, addr, callback);
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventRemoveInputEdge</strong></td>
    <td>
      <scg src="../images/sc-events/sc-event-input-edge.gwf"></scg>
      <br/>
      Callback calls each time, when some sc-connector <code>edgeAddr</code> between some sc-element with <code>otherAddr</code> and listening sc-element with <code>listenAddr</code> is erased.
      <hr/>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
auto const callback = [](ScAddr const & listenAddr,
                         ScAddr const & edgeAddr,
                         ScAddr const & otherAddr)
{
    // listenAddr - sc-address of target sc-element
    //  (listen it in sc-event)
    // edgeAddr - sc-address of erased input sc-connector
    // otherAddr - sc-address of sc-element of erased
    //  sc-connector
    ...
    return SC_TRUE; // if failed, then return SC_FALSE
};
ScEventRemoveOutputEdge event(ctx, addr, callback);
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventEraseElement</strong></td>
    <td>
    Callback calls when listening sc-element with <code>listenAddr</code> is erased.
    <hr/>
    <strong>Example C++ code</strong>:
    <pre><code class="cpp">
auto const callback = [](ScAddr const & listenAddr,
                         ScAddr const & edgeAddr,
                         ScAddr const & otherAddr)
{
  // listenAddr - sc-address of erased sc-element
  //  (listen it in sc-event)
  // edgeAddr - empty sc-address
  // otherAddr - empty sc-address
  ...
  return SC_TRUE; // if failed, then return SC_FALSE
};
ScEventEraseElement event(ctx, addr, callback);
    </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ScEventContentChanged</strong></td>
    <td>
      <scg src="../images/sc-events/sc-event-content.gwf"></scg>
      <br/>
      Callback calls when content of listening sc-link with <code>listenAddr</code> is changed.
      <hr/>
      <strong>Example C++ code</strong>:
      <pre><code class="cpp">
auto const callback = [](ScAddr const & listenAddr,
                         ScAddr const & edgeAddr,
                         ScAddr const & otherAddr)
{
  // listenAddr - sc-address of element that has
  //   content changed (listen it in sc-event)
  // edgeAddr - empty sc-address
  // otherAddr - empty sc-address
  ...
  return SC_TRUE; // if failed, then return SC_FALSE
};
ScEventContentChanged evt(ctx, addr, callback);
      </code></pre>
    </td>
  </tr>
</table>

## **ScWait**

This type of objects used to wait until some event emits. It usually used, when on of an `ScAgent` want to wait result
of another one. There are next kind of `ScWait` objects:

* `ScWait`- lock run flow until simple event emits. You can see the list of these events in the Class properties table (
  ScEvent property);
* `ScWaitCondition` - lock run flow until simple event emits and specified conditional check returns `SC_TRUE`. In other
  words, this works like an `ScWait`, but returns to run flow if special condition function returns `SC_TRUE`. Condition
  function receives 3 parameters (see [ScEvent](#scevent) for more details about them).

There are some examples of usage for specified `ScWait` objects:

* Wait input sc-connector into sc-element with `addr`:

```cpp
ScWait<ScEventAddInputEdge> waiter(ctx, addr);
waiter.Wait();
```

* Wait input sc-connector into sc-element with `listenAddr`, with condition:

```cpp
auto const check = [](ScAddr const & listenAddr,
                      ScAddr const & edgeAddr,
                      ScAddr const & otherAddr)
{
  ... // Check condition there
  return SC_FALSE; // Return SC_TRUE or SC_TRUE depending on condition
};
ScWaitCondition<ScEventAddInputEdge> waiter(ctx, addr, SC_WAIT_CHECK(check));
waiter.Wait(10000); // Provide wait time value
```

There are some yet implemented most common waiters:

* `ScWaitActionFinished` - wait until specified agent will be finished. Example:

```cpp
...
ScWaitActionFinished waiter(ctx, commandAddr);
waiter.Wait();
...
```

## **FAQ**

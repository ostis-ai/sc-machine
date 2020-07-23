

There is a code generator that allows to create some common code by using metadata. It runs before code compilation and generate files with a name `<header_name>.generated.<header_extension>`.
For example if you run it for a file `text.h` it would produce a file `text.generated.h`

To make a metadata for you code you can use this set of macros:
* **SC_CLASS** - allows you to specify metadata for a class;
* **SC_GENERATED_BODY** - macros that need to be used after **SC_CLASS**, because it would be replaced in during compilation time with generated declaration for this class;
* **SC_PROPERTY** - allows to specify metadata for members of a class (including static members).

> You should to specify **SC_CLASS** and **SC_GENERATED_BODY** for all child classes of *ScObject*

## Syntax
There is a syntax rule that used for a metadata specification:
```
[<PropertyName> [ (<PropertyValue>, <PropertyValue>, ...) ] ], …
```

For example:

```cpp
SC_CLASS(Agent, CmdClass("command_update_power_usage"))
```
```cpp
SC_CLASS(CmdClass("command_generate_text_from_template"), Agent)
```
```cpp
SC_PROPERTY(Keynode("nrel_real_energy_usage"), ForceCreate)
```
```cpp
SC_CLASS(Agent, Event(ActionManager::msActionPeriodical, SC_EVENT_ADD_OUTPUT_ARC))
```

You should to use **SC_CLASS** and **SC_GENERATED_BODY** in class declaration:
```cpp
class AWhoAreYouAgent : public ScAgentAction
{
 SC_CLASS(Agent, CmdClass("command_who_are_you"))
 SC_GENERATED_BODY()
};
```

## Classes
Table of available properties of class metadata (**SC_CLASS**):

<table>
  <tr>
    <th>Property</td>
    <th>Description</td>
  </tr>
  <tr>
    <td><strong>Agent</strong></td>
    <td><strong>Parent class:</strong> ScAgent and all childs
      <br/>You should always use it for all <strong>ScAgent</strong> child classes
    </td>
  </tr>

  <tr>
    <td><strong>CmdClass</strong></td>
    <td>Determine system identifier of command class that implemented by sc-agent.
    <br/><strong>Parent class:</strong> ScAgentAction
    <br/><strong>Arguments:</strong>
    <ul>
      <li>System identifier of command class.</li>
    </ul>
    <hr/>
    <pre><code class="cpp hljs">
class AAddContentAgent : public ScAgentAction
{
 SC_CLASS(Agent, CmdClass("command_add_content"))
 SC_GENERATED_BODY()
};
    </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>Event</strong></td>
    <td>Specify condition to start sc-agent implementation.<br/>
      <br/><strong>Parent class:</strong> ScAgent
      <br/><strong>Arguments:</strong>
      <ul>
        <li><strong>ScAddr</strong> of element which will be used to subscribe for an event;</li>
        <li><strong>ScEventType</strong> type of event to subscribe.</li>
      </ul>
      Another words, we specify sc-element and event on it, that runs implementation of sc-agent. Possible event types:
      <ul>
        <li>SC_EVENT_ADD_OUTPUT_ARC</li>
        <li>SC_EVENT_ADD_INPUT_ARC</li>
        <li>SC_EVENT_REMOVE_OUTPUT_ARC</li>
        <li>SC_EVENT_REMOVE_INPUT_ARC</li>
        <li>SC_EVENT_REMOVE_ELEMENT</li>
        <li>SC_EVENT_CONTENT_CHANGED</li>
      </ul>
      <hr/>
      <pre><code class="cpp hljs">
class ANewPeriodicalActionAgent : public ScAgent
{
 SC_CLASS(Agent, Event(msActionPeriodical, SC_EVENT_ADD_OUTPUT_ARC))
 SC_GENERATED_BODY()
};
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>LoadOrder</strong></td>
    <td>Specify order (priority) of module loading. Can be used just in ScModule child classes.
      <br/><strong>Parent class:</strong> ScModule
      <br/><strong>Arguments:</strong>
      <ul>
        <li>Priority as <i>unsigned int</i> number</li>
      </ul>
      System loads modules by ascending order. If two module has an equal load order, then they can load in any order (relative to each other).
      <hr/>
      <pre><code class="cpp hljs">
class nlModule : public ScModule
{
 SC_CLASS(LoadOrder(11))
 SC_GENERATED_BODY()

 sc_result initializeImpl();
 sc_result shutdownImpl();
};
      </code></pre>
    </td>
  </tr>
</table>


## Members
Table of available properties of class members metadata (**SC_PROPERTY**):
<table>

  <tr>
    <th>Property</th>
    <th>Description</th>
  </tr>

  <tr>
    <td><strong>Keynode</strong></td>
    <td>
      <strong>Arguments:</strong>
      <ul>
        <li>String with system identifier of sc-element.</li>
      </ul>
      Specify that this member is a keynode. After module starts, this member will contains <strong>ScAddr</strong> of specified sc-element or invalid <strong>ScAddr</strong> if sc-element not found. Just add <strong>ForceCreate</strong> to create sc-element in case when it didn't found.
      <br/>You can use this property just for members that has <strong>ScAddr</strong> type.
      <hr/>
      <pre><code class="cpp hljs">
SC_PROPERTY(Keynode("device"), ForceCreate)
static ScAddr m_device;
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>Template</strong></td>
    <td>
      <strong>Arguments:</strong>
      <ul>
        <li>String system identifier of template sc-structure in sc-memory</li>
      </ul>
      Specify that this member is a template. After module starts, this template will be parsed from sc-memory. So you will be able use it to search/generate constructions.
      <br/>You can use this property just for members that has <strong>ScTemplate</strong> type.
      <hr/>
      <pre><code class="cpp hljs">
SC_PROPERTY(Template("test_template"))
ScTemplate m_testTemplate;
      </code></pre>
    </td>
  </tr>

  <tr>
    <td><strong>ForceCreate</strong></td>
    <td>
      <strong>Arguments:</strong>
      <ul>
        <li>[<strong>optional</strong>] type of sc-element. Any value from `ScType::Node...`</li>
      </ul>
      <br/>Used just with <strong>Keynode</strong> property. Using of this property force sc-element creation, if it didn't found by system identifier.
      <hr/>
      <pre><code class="cpp hljs">
SC_PROPERTY(Keynode("device"), ForceCreate(ScType::NodeConstClass))
static ScAddr m_device;

SC_PROPERTY(Keynode("device"), ForceCreate) // default value ScType::Node
static ScAddr m_node;
      </code></pre>
    </td>
  </tr>

</table>


## FAQ

1. How to include one `MyObject` into `OtherObject`
```cpp
/* In CPP file you should include header file for object, that implemented in this file
 * For example in file MyObject.cpp we should make order
 */
#include "otherObject.hpp"
#include "otherObject2.hpp"
...

#include "myObject.hpp"

// other includes (that doesn't contains ScObject derived classes)
...

// Implementation
...
```

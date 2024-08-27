# **C++ Keynodes API**

!!! warning
    This documentation is correct for only versions of sc-machine that >= 0.10.0.
--- 

This API provides functionality to define keynodes on C++.

A keynode is a key sc-element used by some program (agent) during its operation. Typically, keynodes can be classes and relations, which are sc-elements that program (agent) uses to find constructions in sc-memory or create new ones. However, keynodes can be any sc-elements (incl. connectors between sc-elements).

## **ScKeynode**

It is a base class for all keynodes. Use it to create keynodes in the `ScKeynodes` class for use in agent programs.

```cpp
#include <sc-memory/sc_keynodes.hpp>

static ScKeynode const my_keynode_class{
    "my_keynode_class", ScType::NodeConstClass};
```

!!! note
    We recommend to name keynode as a system identifier of key element.

!!! note
    Keynodes can be defined not only in classes inherited from the `ScKeynodes` class. You can define them as static objects anywhere.

!!! warning
    Use this class only for static object declarations.

Also, you can convert any keynode to string.

```cpp
std::string const & systemIdtf = my_keynode_class;
```

It can be useful if you want logging keynode system identifiers.

## **ScTemplateKeynode**

It is a base class to create sc-templates as keynodes. This class is useful when you want use programmly represented sc-template in different places in the code or when you don't want specify sc-template in knowledge base and use it in `ScAgentBuilder`.

```cpp
#include <sc-memory/sc_keynodes.hpp>

static ScTemplateKeynode const & my_agent_initiation_condition =
  ScTemplateKeynode("my_agent_initiation_condition")
     .Triple(
        ScKeynodes::action_initiated,
        ScType::EdgeAccessVarPosPerm,
        ScType::NodeVar >> "_action")
     .Triple(
        MyKeynodes::my_action,
        ScType::EdgeAccessVarPosPerm,
        "_action");
```

!!! note
    Objects of this class can be used as `ScTemplate` and `ScAddr`.

!!! warning
    Use this class only for static object declarations.

### **ScKeynodes**

This class is a base class for keynodes declaration. It's like a namespace. Use it as a base class for own keynodes. This class already contains frequently used keynodes. Use them in your code.

```cpp
// File my_keynodes.hpp:
#pragma once

#include <sc-memory/sc_keynodes.hpp>

class MyKeynodes : public ScKeynodes
{
public:
  static inline ScKeynode const my_keynode_class_a{
    "my_keynode_class_a", ScType::NodeConstClass};
  static inline ScKeynode const my_keynode_relation_b{
    "my_keynode_relation_b", ScType::NodeConstNoRole};
};
```

You don't have to inherit class `ScKeynodes`, you can do it this way.

```cpp
// File my_keynodes.hpp:
#pragma once

#include <sc-memory/sc_keynodes.hpp>

class MyKeynodes
{
public:
  static inline ScKeynode const my_keynode_class_a{
    "my_keynode_class_a", ScType::NodeConstClass};
  static inline ScKeynode const my_keynode_relation_b{
    "my_keynode_relation_b", ScType::NodeConstNoRole};
};
```

--- 

## **Frequently Asked Questions**

<!-- no toc -->
- [Is it possible to create the same keynodes in different modules but with different types?](#is-it-possible-to-create-the-same-keynodes-in-different-modules-but-with-different-types)
- [Is it possible to create a keynode inside an agent class?](#is-it-possible-to-create-a-keynode-inside-an-agent-class)
- [When are keynodes initialized?](#when-are-keynodes-initialized)

### **Is it possible to create the same keynodes in different modules but with different types?**

You can create the same keynodes in different modules. The main thing is that there should be no name conflict in the classes in which these keynodes are declared. However, you cannot create identical keynodes in different modules with different types, for example, as structure and as class. All sc-elements with the same system identifiers are automatically glued together.

### **Is it possible to create a keynode inside an agent class?**

Yes, the algorithm is exactly the same if you were to create this keynode in a separate special class for keynodes.

### **When are keynodes initialized?**

They are initialized during sc-memory initialization. Do not use them before the memory has been initialized.

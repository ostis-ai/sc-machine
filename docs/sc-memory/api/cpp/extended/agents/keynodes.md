# **C++ Keynodes API**

!!! warning
    This documentation is correct for only versions of sc-machine that >= 0.10.0.
--- 

This API provides functionality to define keynodes on C++.

A keynode is a key sc-element used by some agent during its operation. Typically, keynodes can be classes and relations, which are sc-elements that agents use to find constructions in sc-memory or create new ones. However, keynodes can be any sc-elements, even connectors between sc-elements.

## **ScKeynode**

It is a base class for all keynodes. Use it to create keynodes in the `ScKeynodes` class for use in agent programs.

```cpp
#include <sc-memory/sc_keynodes.hpp>

static ScKeynode const my_keynode_class{
    "my_keynode_class", ScType::NodeConstClass};
```

!!! note
    Keynodes can be defined not only in classes derived from the `ScKeynodes` class. You can define them as static objects anywhere.

!!! warning
    Use this class only for static object declarations.

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

--- 

## **Frequently Asked Questions**

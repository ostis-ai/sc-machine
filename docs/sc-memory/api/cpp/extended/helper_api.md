# **ScHelper API**

!!! note
    This is correct for only versions of sc-machine that >= 0.9.0.
---

This API allows to work with system identifiers of sc-elements. All methods of this API are the part of class 
`ScMemoryContext`. So, you can use they from object of class `ScMemoryContext`.

A system identifier is an identifier that is unique within the entire knowledge base. This identifier is usually used in 
the source texts of the lower-level knowledge base. To ensure internationalization, it is recommended that system 
identifiers be written in English. The characters used in the system identifier can be letters of the Latin alphabet, 
numbers, underscores and dashes. Thus, it is most appropriate to form the system identifier of the sc-element from 
the main English one by replacing all characters not included in the alphabet described above with the character “_”.

!!! note
    All system identifiers must satisfy the regular pattern `([a-z]|[A-Z]|'_'|'.'|[0-9])+`.

!!! note
    Now only sc-nodes and sc-links can have system identifiers.

There is a quintuple of sc-node with its system identifier.

<scg src="../images/helper/system_identifier_example_1.gwf"></scg>

!!! note
    To include this API provide `#include <sc-memory/sc_memory.hpp>` in your hpp source.

## **SetElementSystemIdentifier**

To set system identifier for some sc-node or sc-link, use the method `SetElementSystemIdentifier`. If passed system identifier
is not valid then the method `SetElementSystemIdentifier` will throw the exception `utils::ExceptionInvalidParams` with 
description of the error. If passed system identifier is already used for other sc-element then the method will return
`false`.

```cpp
...
ScAddr const & nodeAddr = context.GenerateNode(ScType::NodeConst);

bool const & isSystemIdentifierSet 
    = context.SetElementSystemIdentifier("my_node", nodeAddr);
// The value of `isSystemIdentifierSet` must be equal to `true`.
...
```

If you want to get creating quintuple you can provide variable of type `ScSystemIdentifierQuintuple` as out parameter.

```cpp
...
ScAddr const & nodeAddr = context.GenerateNode(ScType::NodeConst);

ScSystemIdentifierQuintuple quintuple;
bool const & isSystemIdentifierSet 
    = context.SetElementSystemIdentifier("my_node", nodeAddr, quintuple);
ScAddr const & myNodeAddr = quintuple.addr1;
// The value of `myNodeAddr` must be equal to the value of `nodeAddr`.
ScAddr const & arcToSystemIdtfLinkAddr = quintuple.addr2;
// The sc-address of common sc-arc between your sc-node and 
// sc-link with system identifier of your sc-node.
ScAddr const & systemIdtfLinkAddr = quintuple.addr3;
// The sc-address of sc-link with system identifier of your sc-node.
ScAddr const & arcToArcToSystemIdtfLinkAddr = quintuple.addr4;
// The sc-address of access sc-arc between binary sc-relation 
// with system identifier `nrel_system_identifier` and the common sc-arc
// between your sc-node and sc-link with system identifier of your sc-node.
ScAddr const & nrelSystemIdtfAddr = quintuple.addr5;
// The sc-address of binary sc-relation with system identifier 
// `nrel_system_identifier`.
...
```

## **GetElementSystemIdentifier**

To get system identifier of sc-element you can use the method `GetElementSystemIdentifier`. If sc-element hasn't system 
identifier then the method will return empty string.

```cpp
...
ScAddr const & nodeAddr = context.GenerateNode(ScType::NodeConst);

bool const & isSystemIdentifierSet 
    = context.SetElementSystemIdentifier("my_node", nodeAddr);
// The value of `isSystemIdentifierSet` must be equal to `true`.

std::string const & systemIdtf = context.GetElementSystemIdentifier(nodeAddr);
// The value of `systemIdtf` must be equal to `"my_node"`.
...
```

## **SearchElementBySystemIdentifier**

You can find sc-element by its system identifier. For this use the method `SearchElementBySystemIdentifier`. It will return 
`true` if there is sc-element with provided system identifier, otherwise `false`.

```cpp
...
ScAddr const & nodeAddr = context.GenerateNode(ScType::NodeConst);

bool const & isSystemIdentifierSet 
    = context.SetElementSystemIdentifier("my_node", nodeAddr);
// The value of `isSystemIdentifierSet` must be equal to `true`.

ScAddr resultAddr;
bool const & isElementWithSystemIdtfFound 
    = context.SearchElementBySystemIdentifier("my_node", resultAddr);
// The value of `isElementWithSystemIdtfFound` must be equal to `true` 
// and the value of `resultAddr` must be equal to the value of `nodeAddr`.

// Or use the another definition of this method.
resultAddr = context.SearchElementBySystemIdentifier("my_node");
// The value of `resultAddr` must be equal to the value of `nodeAddr`.

// Or use the another definition of this method.
resultAddr = context.SearchElementBySystemIdentifier("not_my_node");
// The value of `resultAddr` must be invalid.
...
```

If you want to find quintuple sc-element with its system identifier you can provide variable of type 
`ScSystemIdentifierQuintuple` as out parameter.

```cpp
...
ScAddr const & nodeAddr = context.GenerateNode(ScType::NodeConst);

bool const & isSystemIdentifierSet 
    = context.SetElementSystemIdentifier("my_node", nodeAddr);
// The value of `isSystemIdentifierSet` must be equal to `true`.

ScSystemIdentifierQuintuple quintuple;
bool const & isElementWithSystemIdtfFound 
    = context.SearchElementBySystemIdentifier("my_node", quintuple);
ScAddr const & myNodeAddr = quintuple.addr1;
// The value of `myNodeAddr` must be equal to the value of `nodeAddr`.
ScAddr const & arcToSystemIdtfLinkAddr = quintuple.addr2;
// The sc-address of common sc-arc between your sc-node 
// and sc-link with system identifier of your sc-node.
ScAddr const & systemIdtfLinkAddr = quintuple.addr3;
// The sc-address of sc-link with system identifier of your sc-node.
ScAddr const & arcToArcToSystemIdtfLinkAddr = quintuple.addr4;
// The sc-address of access sc-arc between binary sc-relation 
// with system identifier `nrel_system_identifier` and the common sc-arc 
// between your sc-node and sc-link with system identifier of your sc-node.
ScAddr const & nrelSystemIdtfAddr = quintuple.addr5;
// The sc-address of binary sc-relation with system identifier 
// `nrel_system_identifier`.
...
```

## **ResolveElementSystemIdentifier**

Very often you can need to resolve sc-element by system identifier. Resolve sc-element by system identifier refers to
finding for sc-element with providing system identifier and (if such sc-element is not found) creating sc-element with
provided system identifier and sc-type.

```cpp
...
ScAddr const & nodeAddr 
    = ResolveElementSystemIdentifier("my_node", ScType::NodeConst);
// If there is no sc-element with system identifier `"my_node"` 
// then the method will create sc-element with this system identifier 
// and provided sc-type `ScType::NodeConst`.
...
```

!!! note
    Provided sc-type must be a subtype of sc-type of sc-node or sc-type of sc-link.

If you want to resolve quintuple sc-element with its system identifier you can provide variable of type
`ScSystemIdentifierQuintuple` as out parameter.

```cpp
...
ScSystemIdentifierQuintuple quintuple;
bool const & isSystemIdentifierResolved 
    = context.ResolveElementSystemIdentifier("my_node", ScType::NodeConst, quintuple);
ScAddr const & myNodeAddr = quintuple.addr1;
// The sc-address of resolved sc-node by provided system identifier.
ScAddr const & arcToSystemIdtfLinkAddr = quintuple.addr2;
// The sc-address of common sc-arc between your sc-node and sc-link 
// with system identifier of your sc-node.
ScAddr const & systemIdtfLinkAddr = quintuple.addr3;
// The sc-address of sc-link with system identifier of your sc-node.
ScAddr const & arcToArcToSystemIdtfLinkAddr = quintuple.addr4;
// The sc-address of access sc-arc between binary sc-relation
// with system identifier `nrel_system_identifier` and the common sc-arc 
// between your sc-node and sc-link with system identifier of your sc-node.
ScAddr const & nrelSystemIdtfAddr = quintuple.addr5;
// The sc-address of binary sc-relation with system identifier 
// `nrel_system_identifier`.
...
```

--- 

## **Frequently Asked Questions**

- [Can I specify empty system identifier for sc-element?](#can-i-specify-empty-system-identifier-for-sc-element)

### **Can I specify empty system identifier for sc-element?**

You can not specify empty system identifier for sc-element. All system identifiers must satisfy the regular 
pattern `([a-z]|[A-Z]|'_'|'.'|[0-9])+`.

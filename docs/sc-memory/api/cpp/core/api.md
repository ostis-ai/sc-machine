# **C++ Core API**

!!! note
    This is correct for only versions of sc-machine that >= 0.10.0.
---

The API provides core functionality for creating, retrieving and erasing sc-elements in sc-memory.
This sc-memory is designed to represent knowledge in a structured and graph-based manner.

A sc-memory consists of sc-elements. Sc-elements can be grouped into sc-constructions (semantic graphs or knowledge graphs). Among sc-elements there are sc-nodes, sc-links and sc-connectors between them. All sc-elements have syntactic and semantic sc-types. Sc-connectors are divided into non-oriented sc-connectors (sc-edges) and oriented sc-connectors (sc-arcs). Each sc-connector have source and target sc-elements. These sc-elements can be sc-nodes, sc-links or sc-connectors. All these attributes of sc-elements and sc-element structures are hidden from user, but you can get these features using this API.

!!! note
    All API methods are thread-safe.

!!! note
    To include this API provide `#include <sc-memory/sc_memory.hpp>` in your hpp source.

!!! warning
    Objects of `ScMemoryContext` class are movable, but not copyable.

## **ScAddr**

Each sc-element has a sc-address with which you can obtain information about this sc-element.

```cpp
...
ScAddr elementAddr1 = // some call of the API method 1
ScAddr elementAddr2 = // some call of the API method 2
// Compare sc-addresses.
bool const isAddrsEqual = elementAddr1 == elementAddr2;
```

## **ScMemoryContext**

It is a class provided all API methods for creating, manipulating, retrieving and erasing sc-elements in sc-memory.
It refers to set of attributes and rights of some subject of action in sc-memory (user or agent) that are used in 
during API methods completion.

```cpp
...
// To create such context use constructor of ScMemoryContext.
ScMemoryContext context;
// After you can use this object to call any API methods.
```

### **GenerateNode**

To create sc-nodes you can use the method `GenerateNode`, to create sc-links - the method `GenerateLink`, to create
sc-connectors between them - the method `GenerateConnector`. All these methods check passed sc-types. If specified sc-type is
not valid, then a method throws the exception `utils::ExceptionInvalidParams` with description of this error.

```cpp
...
// Generate sc-node and get sc-address in sc-memory of it.
ScAddr const & nodeAddr = context.GenerateNode(ScType::ConstNode);
// Specified sc-type must be one of ScType::...Node... type.
```

### **GenerateLink**

```cpp
...
// Generate sc-link and get sc-address in sc-memory of it.
ScAddr const & linkAddr = context.GenerateLink(ScType::ConstNodeLink);
// Specified sc-type must be one of ScType::...NodeLink... type.
```

!!! note
    Now all sc-links are not sc-nodes. It can be fixed in the further versions of sc-machine. But you can use 
    the method `GenerateNode` to create sc-links.

### **GenerateConnector**

```cpp
...
// Generate sc-arc between sc-node and sc-link and get sc-address in 
// sc-memory of it.
ScAddr const & arcAddr = context.GenerateConnector(
    ScType::ConstPermPosArc, nodeAddr, linkAddr);
// Specified sc-type must be one of ScType::Edge... type.
```

If specified sc-addresses of source and target sc-elements are not valid, then the method throws exception
`utils::ExceptionInvalidParams` with description that some of specified sc-addresses is not valid.

!!! note
    Although this method is called incorrectly and may be misleading, but you can create any sc-connectors using it.

### **IsElement**

To check if specified sc-address is valid in sc-memory you can use the method `IsElement`. Valid sc-address refers to
sc-address that exists in sc-memory and that corresponds some sc-element in it.

```cpp
...
// Check if all created sc-elements are valid.
bool const isNodeValid = context.IsElement(nodeAddr);
bool const isLinkValid = context.IsElement(linkAddr);
bool const isArcValid = context.IsElement(arcAddr);
```

!!! note
    You can check if specified sc-address is not empty calling from this sc-address object method `IsValid`.
    But it is preferable to use `IsElement`, it checks if provided sc-address exists and valid in sc-memory. 

### **GetElementType**

If it needs you can get sc-element types by its sc-addresses. If specified sc-address is not valid, then the method 
throws exception `utils::ExceptionInvalidParams` with description that specified sc-address is not valid.

```cpp
...
// Get created sc-elements sc-types.
ScType const & nodeType = context.GetElementType(nodeAddr);
ScType const & linkType = context.GetElementType(linkAddr);
ScType const & arcType = context.GetElementType(arcAddr);
```

### **SetElementSubtype**

You can change semantic sc-type of sc-element. Use the method `SetElementSubtype` and provide semantic sc-type for
syntactic sc-type for sc-element.

```cpp
...
// Generate sc-node and get sc-address in sc-memory of it.
ScAddr const & nodeAddr = context.GenerateNode(ScType::Node);
bool const isSubtypeElementChanged 
    = context.SetElementSubtype(nodeAddr, ScType::ConstNode);
// The value of `isSubtypeElementChanged` must be equal to `true`.
```

!!! note
    Don't use this method to change syntactic sc-type for sc-element. It is misleading.

### **GetConnectorIncidentElements**

To get incident (source and target) sc-elements you can use methods `GetConnectorIncidentElements`, `GetArcSourceElement` and `GetArcTargetElement`.
If specified sc-address is not valid, then this methods throw exception `utils::ExceptionInvalidParams` with description 
that specified sc-address of sc-connector is not valid.

```cpp
...
// Get sc-arc incident sc-elements.
auto const [sourceAddr, targetAddr] 
  = context.GetConnectorIncidentElements(arcAddr);
// The sc-address `sourceAddr` must be equal to the sc-address `nodeAddr` 
// and the sc-address `targetAddr` must be equal to the sc-address `linkAddr`.
...
// Or get sc-arc incident source and target sc-elements separately.
ScAddr const & sourceAddr = context.GetArcSourceElement(arcAddr);
// The sc-address `sourceAddr` must be equal to the sc-address `nodeAddr`.
ScAddr const & targetAddr = context.GetArcTargetElement(arcAddr);
// The sc-address `targetAddr` must be equal to the sc-address `linkAddr`.
```

To find previously created sc-constructions you can use 3-element sc-iterators and 5-element sc-iterators from C++ API.
The image below shows numeration of sc-elements in 3-element (left) and 5-element (right) sc-iterators.

<scg src="../images/iterators_scheme.gwf">**Iterators element numeration**</scg>

For both of them you have two approaches.
First approach is to use simple while loop. It is suitable, when you need to break this loop depending on logic:

### **ScIterator3**

```cpp
...
// Create sc-iterator for searching all sc-element sc-addresses 
// with unknown sc-type belonging to sc-set with sc-address `setAddr`.
ScIterator3Ptr it3 = context.CreateIterator3(
    setAddr,
    ScType::ConstPermPosArc,
    ScType::Unknown);
// Use it3-Next() to go to the next appropriate by condition sc-construction.
while (it3->Next())
{
  // To get values use `it3->Get(index)`, where index in range [0; 2]. 
  // It returns `true`, if the next appropriate construction is found, 
  // otherwise `false`.
  ... // Write your code to handle found sc-construction.
}
```

### **ScIterator5**

```cpp
...
// Create sc-iterator for searching all sc-node sc-addresses, 
// which pairs with sc-element with address `setAddr` belong to the relation 
// with sc-address `nrelDecompositionAddr`.
ScIterator5Ptr it5 = context.CreateIterator5(
    setAddr,
    ScType::ConstCommonArc,
    ScType::ConstNode,
    ScType::ConstPermPosArc,
    nrelDecompositionAddr);
// Use `it5-Next()` to go to the next appropriate by condition sc-construction. 
// It returns `true`, if the next appropriate construction is found, 
// otherwise `false`.
while (it5->Next())
{
  // To get values use `it5->Get(index)`, where index in range [0; 4].
  ... // Write your code to handle found sc-construction.
}
```

Second approach allows you to iterate 3-element and 5-element constructions with less code, and it is suitable when
you need to iterate all results.

!!! note
    Use next methods if you need to iterate all results. Because it more clearly.

### **ForEach**

```cpp
...
// Create callback-based sc-iterator for searching all sc-element sc-addresses 
// with unknown sc-type belonging to sc-set with sc-address `setAddr`.
context.ForEach(
    setAddr,
    ScType::ConstPermPosArc,
    ScType::Unknown,
    [] (ScAddr const & srcAddr, ScAddr const & edgeAddr, ScAddr const & trgAddr)
{
  // srcAddr equal to the 0th value of iterator
  // edgeAddr equal to the 1st value of iterator
  // trgAddr equal to the 2d value of iterator
  ... // Write your code to handle found sc-construction.
});
```

```cpp
...
// Create callback-based sc-iterator for searching all sc-node sc-addresses, 
// which pairs with sc-element with address `setAddr` belong to the relation 
// with sc-address `nrelDecompositionAddr`.
context.ForEach(
  setAddr,
  ScType::ConstCommonArc,
  ScType::ConstNode,
  ScType::ConstPermPosArc,
  nrelDecompositionAddr
  [] (ScAddr const & srcAddr, 
      ScAddr const & connectorAddr, 
      ScAddr const & trgAddr, 
      ScAddr const & connectorAttrAddr, 
      ScAddr const & attrAddr)
{
  // srcAddr equal to the 0th value of sc-iterator
  // connectorAddr equal to the 1st value of sc-iterator
  // trgAddr equal to the 2d value of sc-iterator
  // connectorAttrAddr equal to the 3rd value of sc-iterator
  // attrAddr equal to the 4th value of sc-iterator
  ... // Write your code to handle found sc-construction.
});
```

### **EraseElement**

All sc-elements can be erasing from sc-memory. For this you can use the method `EraseElement`.

```cpp
...
// Erase all created sc-elements.
bool const isNodeErased = context.EraseElement(nodeAddr);
// The sc-elements with sc-addresses `nodeAddr` and `arcAddr` must be deleted.
bool const isArcErased = context.EraseElement(arcAddr);
// The sc-element with sc-address `targetAddr` must be deleted.
```

### **SetLinkContent**

Besides creating and checking elements, the API also supports updating and removing content of sc-links.
If specified sc-element is a sc-link, you can set content into it. Re-installing new content into a sc-link removes
the previous content from this sc-link. If specified sc-address is not valid, then the method throws exception 
`utils::ExceptionInvalidParams` with description that specified sc-address is not valid.

```cpp
...
ScAddr const & linkAddr1 = context.GenerateLink(ScType::ConstNodeLink);
// Set string content into created sc-link.
context.SetLinkContent(linkAddr1, "my content");

ScAddr const & linkAddr2 = context.GenerateLink(ScType::ConstNodeLink);
// Set numeric content into created sc-link.
context.SetLinkContent(linkAddr2, 10f);
...
```

!!! note
    Don't use result value, it doesn't mean anything.

### **GetLinkContent**

To get existed content from sc-link you can use the method `GetLinkContent`. A content can be represented as numeric or 
string. If content doesn't exist in sc-link, then the method `GetLinkContent` will return `false` and result content
will be empty.

```cpp
...
// Get string content from sc-link.
std::string stringContent;
bool const stringContentExist 
  = context.GetLinkContent(linkAddr1, stringContent);

// Get numeric content from sc-link.
float numericContent;
bool const numericContentExist 
  = context.GetLinkContent(linkAddr1, numericContent);
...
```

!!! note
    You can set empty content into sc-link, but it means that this sc-link has content and this method for this 
    sc-link returns `true`.

### **SearchLinksByContent**

You can find sc-links by its content. For this use the method `SearchLinksByContent`.

```cpp
...
// Find sc-links with specified string content.
ScAddrSet const & linkAddrs1 = context.SearchLinksByContent("my content");
// The set `linkAddrs1` must contain sc-address `linkAddr1`.

// Find sc-links with specified numeric content.
ScAddrSet const & linkAddrs2 = context.SearchLinksByContent(10f);
// The set `linkAddrs2` must contain sc-address `linkAddr2`.
```

### **SearchLinksByContentSubstring**

And you can find sc-links by its content substring. For this use the method `SearchLinksByContentSubstring`.

```cpp
...
// Find sc-links with specified string content substring.
ScAddrSet const & linkAddrs1 
  = context.SearchLinksByContentSubstring("my cont");
// The set `linkAddrs1` must contain sc-address `linkAddr1`.
```

Also, you can provide a filter to search sc-links to define criteria for which links should be included in the search results. This allows for customization of which links are considered based on user-defined conditions. The filter must be an instance of a class derived from `ScLinkFilter`, which implements the `CheckLink` and `RequestLink` methods.

```cpp
class TestScLinkFilter final : public ScLinkFilter
{
public:
  // You can specify custom fields and methods.
  ScMemoryContext * m_context;
  ScAddr m_linkClassAddr;
  size_t m_foundLinksCount = 0u;

  bool CheckLink(ScAddr const & linkAddr) override
  {
    // Don't include sc-link in result if it doesn't belong 
    // to `m_linkClassAddr`.
    return m_context->CheckConnector(
      m_linkClassAddr, linkAddr, ScType::ConstPermPosArc);
  }

  ScLinkFilterRequest RequestLink(ScAddr const & linkAddr) override
  {
    // Iterate over only the first 5 sc-links.
    ++m_foundLinksCount;

    if (m_foundLinksCount == 5u)
      return ScLinkFilterRequest::STOP;

    return ScLinkFilterRequest::CONTINUE;
  }
};


// Some method A
...
// For example, generate sc-links, set contents for them and add only 
// one of them to some set or class.
ScAddr const & linkClassAddr = m_ctx->GenerateNode(ScType::ConstNodeClass);
ScAddr const & linkAddr1 = m_ctx->GenerateLink(ScType::ConstNodeLink);
ScAddr const & linkAddr2 = m_ctx->CreateLink(ScType::ConstNodeLink);

m_ctx->SetLinkContent(linkAddr1, "content 1");
m_ctx->SetLinkContent(linkAddr2, "content 2");

m_ctx->GenerateConnector(ScType::ConstPermPosArc, linkClassAddr, linkAddr1);
...

// Some method B
...
// Create object of custom sc-link filter class and search sc-links 
// with this filter.
ScMemoryContext context;
CustomScLinkFilter filter;
filter.m_context = &context;
filter.m_linkClassAddr = linkClassAddr;

ScAddrSet const & links 
  = m_ctx->SearchLinksByContentSubstring("content", filter);
// The `links` must have only one sc-link sc-address `linkAddr1`.
...
```

### **SearchLinksContentsByContentSubstring**

Also, you can find contents of sc-links by its content substring. For this use the method `SearchLinksContentsByContentSubstring`.

```cpp
...
// Find contents of sc-links with specified string content substring.
std::set<std::string> const & linkContents 
  = context.SearchLinksContentsByContentSubstring("my cont");
// The set `linkContents` must contain string `my content`.
```

### **ScException**

To declare your own exceptions inherit from class `ScException`.

```cpp
class MyException final : public ScException
{
public:
  explicit MyException(
    std::string const & description, std::string const & message) 
    : ScException("MyException: " + description, message)
  {}
};
```

To throw exceptions use `SC_THROW_EXCEPTION(exceptionName, message);`.

```cpp
SC_THROW_EXCEPTION(MyException, "It is my exception.");
```

Throw exception for a non-implemented part of code.

```cpp
SC_NOT_IMPLEMENTED("This code is not implemented.");
```

### **ScLogger**

The `ScLogger` class provides a robust logging mechanism for your code.

To create an instance of ScLogger, you can use the constructor that specifies the logging type (console or file), the log file name (if applicable), and the initial log level. Here’s how to do it:

```cpp
// Create a logger that logs to the console with Info level
utils::ScLogger logger(
  utils::ScLogger::ScLogType::Console, "", utils::ScLogLevel::Info);

// Create a logger that logs to a file with Debug level
utils::ScLogger fileLogger(
  utils::ScLogger::ScLogType::File, "log_file.txt", utils::ScLogLevel::Debug);
```

Once you have created an instance of ScLogger, you can call its various methods to manage logging.

#### **Message** (**Error**, **Warning**, **Info**, **Debug**)

Use the `Message` method to log messages at different severity levels:

```cpp
logger.Info("This is an informational message.");
// or 
// logger.Message(
//   utils::ScLogLevel::Info, 
//   "This is an informational message.",
//   ScConsole::Color::Grey);

fileLogger.Error("An error occurred while processing.");
// or
// fileLogger.Message(
//    utils::ScLogLevel::Error,
//    "An error occurred while processing.",
//    ScConsole::Color::Red);
```

It prints colored info message. Look color constants in `ScConsole::Color`.

#### **SetLogLevel**

You can change the log level dynamically:

```cpp
logger.SetLogLevel(utils::ScLogLevel::Warning);
```

#### **Mute** and **Unmute**

Control whether logs are outputted:

```cpp
logger.Mute();   // Mute logging
logger.Unmute(); // Unmute logging
```

#### **SetPrefix**

Customize prefix for logs:

```cpp
logger.SetPrefix("MyAgent: ");

logger.Info("It is info message");
// Output: MyAgent: Is info message 
```

#### **Clear**

Clear any open file streams or flush logs if necessary:

```cpp
logger.Clear();
```

#### **Log levels**

The following macros are available for logging messages at different severity levels:

| Method    | Message prefix | Message color                 | Log levels                  |
|-----------|----------------|-------------------------------|-----------------------------|
| `Debug`   | `[Debug]`      | `ScConsole::Color::Grey`      | Debug                       |
| `Info`    | `[Info]`       | `ScConsole::Color::LightBlue` | Debug, Info                 |
| `Warning` | `[Warning]`    | `ScConsole::Color::Yellow`    | Debug, Info, Warning        |
| `Error`   | `[Error]`      | `ScConsole::Color::Red`       | Debug, Info, Warning, Error |

These macros simplify the process of logging messages by automatically formatting them with appropriate prefixes and colors based on their severity levels.

## **Extended API**

Described methods are part of Core C++ API of sc-memory. You can see and use Extended C++ API of sc-memory:

- [ScHelper C++ API](../extended/helper_api.md) to manipulate with system identifiers of sc-elements; 
- [ScTemplate C++ API](../extended/template_api.md), providing functionality for creating, manipulating and retrieving 
large graph structures.

## **Frequently Asked Questions**

- [What is the difference between ScType::ConstCommonArc and ScType::ConstPermPosArc?](#what-is-the-difference-between-sctypeconstcommonarc-and-sctypeconstpermposarc)
- [How can I specify empty ScAddr?](#how-can-i-specify-empty-scaddr)

### **What is the difference between ScType::ConstCommonArc and ScType::ConstPermPosArc?**

`ScType::ConstCommonArc` is a sc-type of sc-arc that connects two sc-elements in some relation.
`ScType::ConstPermPosArc` is a sc-type of sc-arc that denotes membership of target sc-element to source sc-element.
The sc-arc with sc-type `ScType::ConstCommonArc` between some two sc-elements can be transformed to sc-node to which 
this two sc-elements belong.

### **How can I specify empty ScAddr?**

Empty `ScAddr` is the sc-address that has hash that equals to `0`.

```cpp
...
ScAddr addr;
// Here `addr` is empty.

myFunc(addr);
// or
myFunc(ScAddr::Empty);
...
```

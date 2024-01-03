## Basic generate API

The API provides functionality for creating, manipulating, and erasing sc-elements in common sc-memory.
This sc-memory is designed to represent knowledge in a structured and graph-based manner.

```cpp
...
// Create sc-node and get sc-address in sc-memory of it.
ScAddr const & nodeAddr = ctx.CreateNode(ScType::NodeConst);
// Specified sc-type must be one of ScType::Node... type.
```

```cpp
...
// Create sc-link and get sc-address in sc-memory of it.
ScAddr const & linkAddr = ctx.CreateLink(ScType::LinkConst);
// Specified sc-type must be one of ScType::Link... type.
```

```cpp
...
// Create sc-arc between sc-node and sc-link and get sc-address in sc-memory of it.
ScAddr const & arcAddr = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, nodeAddr, linkAddr);
// Specified sc-type must be one of ScType::Edge... type.
```

```cpp
...
// Check if all created sc-elements are valid.
bool const isNodeValid = ctx.IsElement(nodeAddr);
bool const isLinkValid = ctx.IsElement(linkAddr);
bool const isEdgeValid = ctx.IsElement(arcAddr);
```

Note: You can check if specified sc-address is not empty calling from this sc-address object method `IsValid`.
But it is preferable to use `IsElement`, it checks if provided sc-address exists and valid in sc-memory.

```cpp
...
// Get created sc-elements sc-types.
ScType const & nodeType = ctx.GetElementType(nodeAddr);
ScType const & linkType = ctx.GetElementType(linkAddr);
ScType const & arcType = ctx.GetElementType(arcAddr);
```

```cpp
...
// Get sc-arc incident sc-elements.
ScAddr sourceAddr, targetAddr;
ctx.GetEdgeInfo(arcAddr, sourceAddr, targetAddr);
// The sc-address `sourceAddr` must be equal to the sc-address `nodeAddr` 
// and the sc-address `targetAddr` must be equal to the sc-address `linkAddr`.
...
// Or get sc-arc incident source and target sc-elements separately.
ScAddr const & sourceAddr = ctx.GetEdgeSource(arcAddr);
// The sc-address `sourceAddr` must be equal to the sc-address `nodeAddr`.
ScAddr const & targetAddr = ctx.GetEdgeTarget(arcAddr);
// The sc-address `targetAddr` must be equal to the sc-address `linkAddr`.
```

```cpp
...
// Erase all created sc-elements.
ctx.EraseElement(nodeAddr);
// The sc-elements with sc-addresses `nodeAddr` and `arcAddr` must be deleted.
ctx.EraseElement(arcAddr);
// The sc-element with sc-address `targetAddr` must be deleted.
```

Besides creating and checking elements, the API also supports updating and removing content of sc-links.
If specified sc-element is a sc-link, you can set content into it. Re-installing new content into a sc-link removes
the previous content from this sc-link.

```cpp
...
ScAddr const & linkAddr1 = ctx.CreateLink(ScType::LinkConst);
// Set string content into created sc-link
ctx.SetLinkContent(linkAddr1, "my content");

ScAddr const & linkAddr2 = ctx.CreateLink(ScType::LinkConst);
// Set numeric content into created sc-link
ctx.SetLinkContent(linkAddr2, 10f);
```

```cpp
...
// Get string content from sc-link
std::string stringContent;
ctx.GetLinkContent(linkAddr1, stringContent);

// Get numeric content from sc-link
float numericContent;
ctx.GetLinkContent(linkAddr1, numericContent);
```

```cpp
...
// Find sc-links with specified string content
ScAddrVector const & linkAddrs1 = ctx.FindLinksByContent("my content");
// The vector `linkAddrs1` must contain sc-address `linkAddr1`.

// Find sc-links with specified numeric content
ScAddrVector const & linkAddrs2 = ctx.FindLinksByContent(10f);
// The vector `linkAddrs2` must contain sc-address `linkAddr2`.
```

```cpp
...
// Find sc-links with specified string content substring
ScAddrVector const & linkAddrs1 = ctx.FindLinksByContentSubstring("my cont");
// The vector `linkAddrs1` must contain sc-address `linkAddr1`.
```

## Basic search API

To find previously created sc-constructions you can use 3-element sc-iterators and 5-element sc-iterators from C++ API.
The image below shows numeration of sc-elements in 3-element (left) and 5-element (right) sc-iterators.

![Iterators element numeration](images/iterators_scheme.png)

For both of them you have two approaches.
First approach to use simple while loop. It suitable, when you need to break this loop depending on logic:

```cpp
...
// Create sc-iterator for searching all sc-element sc-addresses 
// with unknown sc-type belonging to sc-set with sc-address `setAddr`.
ScIterator3Ptr it3 = ctx.Iterator3(
            setAddr,
            ScType::EdgeAccessConstPosPerm,
            ScType::Unknown);
// Use it3-Next() to go to the next appropriate by condition sc-construction.
while (it3->Next())
{
  // To get values use it3->Get(index), where index in range [0; 2]. 
  // It returns SC_TRUE, if the next appropriate construction is found, otherwise SC_FALSE.
  ... // Write your code to handle found sc-construction.
}

...
// Create sc-iterator for searching all sc-node sc-addresses, 
// which pairs with sc-element with address `setAddr` belong to the relation with sc-address `nrelDecompositionAddr`.
ScIterator5Ptr it5 = ctx.Iterator5(
        setAddr,
        ScType::EdgeDCommonConst,
        ScType::NodeConst,
        ScType::EdgeAccessConstPosPerm,
        nrelDecompositionAddr);
// Use it5-Next() to go to the next appropriate by condition sc-construction. 
// It returns SC_TRUE, if the next appropriate construction is found, otherwise SC_FALSE.
while (it5->Next())
{
  // To get values use it5->Get(index), where index in range [0; 4].
  ... // Write your code to handle found sc-construction.
}
```

Second approach allows you to iterate 3-element and 5-element constructions with less code, and suitable when
you need to iterate all results.

**Use it if you need to iterate all results. Because it more clearly.**

```cpp
...
// Create callback-based sc-iterator for searching all sc-element sc-addresses 
// with unknown sc-type belonging to sc-set with sc-address `setAddr`.
ctx.ForEachIter3(
    setAddr,
    ScType::EdgeAccessConstPosPerm,
    ScType::Unknown,
    [] (ScAddr const & srcAddr, ScAddr const & edgeAddr, ScAddr const & trgAddr)
{
  // srcAddr equal to the 0th value of iterator
  // edgeAddr equal to the 1st value of iterator
  // trgAddr equal to the 2d value of iterator
  ... // Write your code to handle found sc-construction.
});

...
// Create callback-based sc-iterator for searching all sc-node sc-addresses, 
// which pairs with sc-element with address `setAddr` belong to the relation with sc-address `nrelDecompositionAddr`.
ctx.ForEachIter5(
  setAddr,
  ScType::EdgeDCommonConst,
  ScType::NodeConst,
  ScType::EdgeAccessConstPosPerm,
  nrelDecompositionAddr
  [] (ScAddr const & srcAddr, ScAddr const & edgeAddr, ScAddr const & trgAddr, ScAddr const & edgeAttrAddr, ScAddr const & attrAddr)
{
  // srcAddr equal to the 0th value of sc-iterator
  // edgeAddr equal to the 1st value of sc-iterator
  // trgAddr equal to the 2d value of sc-iterator
  // edgeAttrAddr equal to the 3rd value of sc-iterator
  // attrAddr equal to the 4th value of sc-iterator
  ... // Write your code to handle found sc-construction.
});
```

## Expanded search API

Sc-templates is a very powerful mechanism to work with semantic network (graph). You can search and generate any
constructions using sc-templates. There are list of available classes to work with sc-templates:

* `ScTemplate` - class that represents sc-template.
* `ScTemplateParams` - parameters that contains values of sc-variables in sc-template. This class usually used when you
  generate sc-construction by sc-template.
* `ScTemplateSearchResult` - contains result of search by sc-template (list of found sc-constructions).
* `ScTemplateResultItem` - represents on search or generation result item.

### ScTemplate

The class to work with sc-templates in C++ API. Before reading this paragraph you need to read
common [information about sc-element types](sc-element-types.md).

Let use `f` symbols for constant parameter of sc-template. Let use `a` symbol for a variable parameter of sc-template. 
Then sc-template to search all output connectors from specified sc-element will be a triple:

* where first element is known `f`;
* second and third elements need to be found `a`.

There are possible 3 types of simple sc-templates:

* `f_a_a` - sc-template to find all outgoing connectors from a specified sc-element;
* `f_a_f` - sc-template to find all connectors between two specified sc-elements;
* `a_a_f` - sc-template to find all ingoing connectors to a specified sc-element.

There are some methods available for `ScTemplate` class:

* `triple` - method that adds triple sc-construction into sc-template. There are some examples of using this function to
  produce simple sc-templates:

<table>
  <tr>
    <th>Template</th>
    <th>Description</th>
  </tr>

  <tr>
    <td>f_a_a</td>
    <td>
      <strong>Graphical representation</strong>
      <br/><scg src="../images/templates/template_triple_f_a_a_example.gwf"></scg>
      <br/><strong>Equal C++ code</strong>
      <br/>
<pre><code class="cpp hljs">
ScTemplate templ;
templ.Triple(
  param1,
  ScType::EDGE_ACCESS_VAR_POS_PERM,
  ScType::NODE_VAR
);
</code></pre>
      <br/>This tripled sc-template using to traverse output edges from specified sc-element.
      <br/>There <code>param1</code> is a known <code>ScAddr</code> of sc-element. It must be a valid (use <code>ScAddr::IsValid</code> method to check). Where <code>_param2</code> and <code>_param3</code> are <code>ScType</code> for compare by search engine. When search engine will traverse output edges from <code>param1</code>. Construction will be added into traverse result, where output connector from <code>param1</code>, will suitable to specified type <code>_param2</code>, and type of target element of this edge will be sutable for a type <code>_param3</code>.
      <br/>You can use any sc-type of <code>_param3</code> (including edges) depending on sc-construction you want to find. But <code>_param2</code> should be any sc-type of variable edge.
    </td>
  </tr>

  <tr>
    <td>f_a_f</td>
    <td>
      <strong>Graphical representation</strong>
      <br/><scg src="../images/templates/template_triple_f_a_f_example.gwf"></scg>
      <br/><strong>Equal C++ code</strong>
      <br/>
<pre><code class="cpp hljs">
ScTemplate templ;
templ.Triple(
  param1,
  ScType::EDGE_ACCESS_VAR_POS_PERM,
  param3
);
</code></pre>
      <br/>This triple sc-template using to find edge between <code>param1</code> and <code>param3</code>.
      <br/>There are <code>param1</code> and <code>param3</code> a known <code>ScAddr</code> of sc-elements. Edge type <code>_param2</code> should be variable.
    </td>
  </tr>
  <tr>
    <td>a_a_f</td>
    <td>
      <strong>Graphical representation</strong>
      <br/><scg src="../images/templates/template_triple_a_a_f_example.gwf"></scg>
      <br/><strong>Equal C++ code</strong>
      <br/>
<pre><code class="cpp hljs">
ScTemplate templ;
templ.Triple(
  ScType::NODE_VAR,
  ScType::EDGE_ACCESS_VAR_POS_PERM,
  param3
);
</code></pre>
      <br/>This triple sc-template using to traverse input edges from specified sc-element.
      <br/>There <code>param3</code> is a known <code>ScAddr</code> of sc-element. You can use any type of <code>_param1</code> (including edges) depending on construction you want to find. But <code>_param2</code> should be any type of variable edge.
    </td>
  </tr>
</table>

When sc-template search engine works, it tries to traverse graph by simple (triple) sc-template in order they specified. For
example, we need to check if specified sc-element (`_device`) is included into `device` set and `device_enabled` set:

<scg src="../images/templates/template_example_2.gwf"></scg>

**Code** that generates equal sc-template

```cpp
ScAddr device_addr, device_enabled_addr;
...

ScTemplate templ;
templ.Triple(
  device_addr,    // sc-addr of device node
  ScType::EDGE_ACCESS_VAR_POS_PERM,
  ScType::NODE_VAR >> "_device_instance"
);
templ.Triple(
  device_enabled_addr,    // sc-addr of device_enabled node
  ScType::EDGE_ACCESS_VAR_POS_PERM,
  "_device_instance"
);
```

In code, you can see a construction `ScType::NODE_VAR >> "_device_instance"` - this is a naming for a sc-template element.
It allows to set name for a specified sc-element in sc-template, and use it many times in different triples. You can see,
that in the second triple we use this name `"_device_instance"`. That means, that we need to place search result from a
first triple into the second. So the second triple is a `f_a_f` style triple.

So if you want to use the same element `_x` in different triples, and you doesn't know it `ScAddr`, then just use two
main rules:

* set name of this element in a first occurrence of this element in sc-template triples. You need to use `>>` operator to
  do this (see code below, last element of first triple);
* when you need to use named element in next triples, then just use it name instead of `ScType` or `ScAddr` (see code
  below, first element if second triple).

**Example code with naming**

```cpp
ScTemplateTempl;
templ.Triple(
  any_addr, // sc-addr of known sc-element
  ScType::EDGE_ACCESS_VAR_POS_PERM,  // type of unknown edge
  ScType::NODE_VAR >> "_x"  // type and name for an unknown sc-element
);
templ.Triple(
  "_x",  // say that is the same element as the last on in a previous triple
  ScType::EDGE_ACCESS_VAR_POS_PERM,  // type of unknown edge
  ScType::NODE_VAR  // type of unknown sc-element
);
```

Also, you can generate sc-templates using [SCs-code](../../scs/scs.md). Example code:

```cpp
ScTemplate templ; 
sc_char const * data = 
  "_device_instance"
  "  _<- device;"
  "  _<- device_enabled;;";
ctx.HelperBuildTemplate(templ, data);
```

During sc-template building all constants will be resolved by their system identifier (in
example: `device`, `device_enabled`), so in result `templ` will be contained sc-template:

<scg src="../images/templates/template_example_2.gwf"></scg>

## Search

Search algorithm trying to find all possible variants of specified construction. It uses any constants (
available `ScAddr`'s from parameters to find equal sc-constructions in sc-memory).


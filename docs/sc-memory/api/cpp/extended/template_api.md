# **ScTemplate API**

!!! warning 
    This documentation is correct for only versions of sc-machine that >= 0.9.0.
---

Sc-templates is a very powerful mechanism to work with semantic network (graph). You can generate and search any
constructions using sc-templates. In the following picture the sc-template and the isomorphic sc-construction are shown.

<scg src="../images/templates/template_example_1.gwf"></scg>

!!! note
    To include this API provide `#include <sc-memory/sc_memory.hpp>` in your hpp source.

## **ScTemplate**

The class to work with sc-templates in C++ API. Before reading this paragraph you need to read
common [information about sc-element types](../../../../scs/sc_element_types.md).

Let use `f` symbols for constant parameter of sc-template. Let use `a` symbol for a variable parameter of sc-template. 
Then sc-template to search all output sc-connectors from specified sc-element will be a triple:

* where the first sc-element is known `f`;
* second and the third sc-elements need to be found `a`.

There are possible 3 types of triple sc-templates:

* `f_a_a` - sc-template to find all outgoing sc-connectors from a specified sc-element;
* `f_a_f` - sc-template to find all sc-connectors between two specified sc-elements;
* `a_a_f` - sc-template to find all ingoing sc-connectors to a specified sc-element.

And there are possible 7 types of quintuple sc-templates:

* `f_a_a_a_a` - sc-template to find all outgoing sc-connectors from a specified sc-element with all attributes of this sc-connectors;
* `f_a_f_a_a` - sc-template to find all sc-connectors between two specified sc-elements with all attributes of this sc-connectors;
* `f_a_a_a_f` - sc-template to find all outgoing sc-connectors from a specified sc-element with specified attribute;
* `f_a_f_a_f` - sc-template to find all sc-connectors between two specified sc-elements with specified attribute;
* `a_a_f_a_a` - sc-template to find all ingoing sc-connectors to a specified sc-element with all attributes of this sc-connectors;
* `a_a_f_a_f` - sc-template to find all ingoing sc-connectors to a specified sc-element with specified attribute;
* `a_a_a_a_f` - sc-template to find all sc-connectors with specified attribute.

Here attribute is sc-element from which the sc-connector is outgoing to the searchable sc-connectors.

There are some methods available for `ScTemplate` class:

### **Triple**

It is the method that adds triple sc-construction into sc-template. There are some examples of using this function to
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
<pre><code class="cpp">
ScTemplate templ;
templ.Triple(
  param1,
  ScType::EdgeAccessVarPosPerm,
  ScType::NodeVar
);
</code></pre>
      <br/>This triple sc-template is used to traverse output edges from specified sc-element.
      <br/>There <code>param1</code> is a known sc-address of sc-element. It must be a valid (use <code>IsElement</code> method to check). Where <code>_param2</code> and <code>_param3</code> are sc-types for compare by search engine. When search engine will traverse output edges from <code>param1</code>. Construction will be added into traverse result, where output sc-connector from <code>param1</code>, will suitable to specified type <code>_param2</code>, and type of target sc-element of this edge will be sutable for a type <code>_param3</code>.
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
<pre><code class="cpp">
ScTemplate templ;
templ.Triple(
  param1,
  ScType::EdgeAccessVarPosPerm,
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
<pre><code class="cpp">
ScTemplate templ;
templ.Triple(
  ScType::NodeVar,
  ScType::EdgeAccessVarPosPerm,
  param3
);
</code></pre>
      <br/>This triple sc-template using to traverse input edges from specified sc-element.
      <br/>There <code>param3</code> is a known sc-address of sc-element. You can use any type of <code>_param1</code> (including edges) depending on construction you want to find. But <code>_param2</code> should be any type of variable edge.
    </td>
  </tr>
</table>

### **Quintuple**

It is the method that adds quintuple sc-construction into sc-template. There are some examples of using this 
function to produce simple sc-templates:

<table>
  <tr>
    <th>Template</th>
    <th>Description</th>
  </tr>

  <tr>
    <td>f_a_a_a_a</td>
    <td>
      <strong>Graphical representation</strong>
      <br/><scg src="../images/templates/template_quintuple_f_a_a_a_a_example.gwf"></scg>
      <br/><strong>Equal C++ code</strong>
      <br/>
<pre><code class="cpp">
ScTemplate templ;
templ.Quintuple(
  param1,
  ScType::EdgeAccessVarPosPerm,
  ScType::NodeVar,
  ScType::EdgeAccessVarPosPerm,
  ScType::NodeVar
);
</code></pre>
    </td>
  </tr>

  <tr>
    <td>f_a_f_a_a</td>
    <td>
      <strong>Graphical representation</strong>
      <br/><scg src="../images/templates/template_quintuple_f_a_f_a_a_example.gwf"></scg>
      <br/><strong>Equal C++ code</strong>
      <br/>
<pre><code class="cpp">
ScTemplate templ;
templ.Quintuple(
  param1,
  ScType::EdgeAccessVarPosPerm,
  param3,
  ScType::EdgeAccessVarPosPerm,
  ScType::NodeVar
);
</code></pre>
    </td>
  </tr>

  <tr>
    <td>f_a_a_a_f</td>
    <td>
      <strong>Graphical representation</strong>
      <br/><scg src="../images/templates/template_quintuple_f_a_a_a_f_example.gwf"></scg>
      <br/><strong>Equal C++ code</strong>
      <br/>
<pre><code class="cpp">
ScTemplate templ;
templ.Quintuple(
  param1,
  ScType::EdgeAccessVarPosPerm,
  ScType::NodeVar,
  ScType::EdgeAccessVarPosPerm,
  param5
);
</code></pre>
    </td>
  </tr>

  <tr>
    <td>f_a_f_a_f</td>
    <td>
      <strong>Graphical representation</strong>
      <br/><scg src="../images/templates/template_quintuple_f_a_f_a_f_example.gwf"></scg>
      <br/><strong>Equal C++ code</strong>
      <br/>
<pre><code class="cpp">
ScTemplate templ;
templ.Quintuple(
  param1,
  ScType::EdgeAccessVarPosPerm,
  param3,
  ScType::EdgeAccessVarPosPerm,
  param5
);
</code></pre>
    </td>
  </tr>

  <tr>
    <td>a_a_f_a_a</td>
    <td>
      <strong>Graphical representation</strong>
      <br/><scg src="../images/templates/template_quintuple_a_a_f_a_a_example.gwf"></scg>
      <br/><strong>Equal C++ code</strong>
      <br/>
<pre><code class="cpp">
ScTemplate templ;
templ.Quintuple(
  ScType::NodeVar,
  ScType::EdgeAccessVarPosPerm,
  param3,
  ScType::EdgeAccessVarPosPerm,
  ScType::NodeVar
);
</code></pre>
    </td>
  </tr>

  <tr>
    <td>a_a_f_a_f</td>
    <td>
      <strong>Graphical representation</strong>
      <br/><scg src="../images/templates/template_quintuple_a_a_f_a_f_example.gwf"></scg>
      <br/><strong>Equal C++ code</strong>
      <br/>
<pre><code class="cpp">
ScTemplate templ;
templ.Quintuple(
  ScType::NodeVar,
  ScType::EdgeAccessVarPosPerm,
  param3,
  ScType::EdgeAccessVarPosPerm,
  param5
);
</code></pre>
    </td>
  </tr>

  <tr>
    <td>a_a_a_a_f</td>
    <td>
      <strong>Graphical representation</strong>
      <br/><scg src="../images/templates/template_quintuple_a_a_a_a_f_example.gwf"></scg>
      <br/><strong>Equal C++ code</strong>
      <br/>
<pre><code class="cpp">
ScTemplate templ;
templ.Quintuple(
  ScType::NodeVar,
  ScType::EdgeAccessVarPosPerm,
  ScType::NodeVar,
  ScType::EdgeAccessVarPosPerm,
  param5
);
</code></pre>
    </td>
  </tr>
</table>

When sc-template search engine works, it tries to traverse graph by simple (triple or quintuple) sc-template in order they specified. For
example, we need to check if specified sc-element (`_set`) is included into `concept_set` class and `concept_binary_relation` class:

<scg src="../images/templates/template_example_2.gwf"></scg>

There is example code that generates equal sc-template.

```cpp
...
// Find key concepts that should be used in sc-template.
ScAddr const & conceptSetAddr = context.HelperFindBySystemIdtf("concept_set");
ScAddr const & conceptBinaryRelationAddr 
    = context.HelperFindBySystemIdtf("concept_binary_relation");

// Create sc-template and add triples into this sc-template.
ScTemplate templ;
templ.Triple(
  conceptSetAddr,    // sc-address of concept set node
  ScType::EdgeAccessVarPosPerm,
  ScType::NodeVar >> "_set"
);
templ.Triple(
  conceptBinaryRelationAddr,    // sc-address of concept binary relation node
  ScType::EdgeAccessVarPosPerm,
  "_set"
);
..
```

In code, you can see a construction `ScType::NodeVar >> "_set"` - this is a naming for a sc-template sc-element.
It allows to set alias for a specified sc-element in sc-template, and use it many times in different triples. You can see,
that in the second triple we use this alias `"_set"`. That means, that we need to place search result from a
first triple into the second. So the second triple is a `f_a_f` style triple.

So if you want to use the same sc-element `_x` in different triples, and you doesn't know it `ScAddr`, then just use two
main rules:

* Set alias of this sc-element in a first occurrence of this sc-element in sc-template triples. You need to use `>>` operator to
  do this (see code below, last sc-element of first triple).
* When you need to use aliased sc-element in next triples, then just use it alias instead of `ScType` or `ScAddr` (see code
  below, first sc-element if second triple).

There is the example code with naming.

```cpp
...
ScTemplate templ;
templ.Triple(
  anyAddr, // sc-address of known sc-element
  ScType::EdgeAccessVarPosPerm,  // type of unknown sc-edge
  ScType::NodeVar >> "_x"  // type and alias for an unknown sc-element
);
templ.Triple(
  "_x",  // say that is the same sc-element as the last on in a previous triple
  ScType::EdgeAccessVarPosPerm,  // type of unknown sc-edge
  ScType::NodeVar  // type of unknown sc-node
);
...
```

Inside a program object of a sc-template all its constructions are represented as triples.

### **HasReplacement**

To check that sc-template has an aliased sc-element you can use the method `HasReplacement`.

```cpp
...
ScTemplate templ;
templ.Triple(
  anyAddr,
  ScType::EdgeAccessVarPosPerm,
  ScType::NodeVar >> "_x"
);
bool const hasAliasX = templ.HasReplacement("_x");
// The value of `hasAliasX` be equal to `SC_TRUE`.
...
```

### **Size**

To get count of triples in sc-template, use the method `Size`. It may be useful if your program can choose optimal for 
manipulating sc-template.

```cpp
ScTemplate templ;
templ.Triple(
  anyAddr, // sc-address of known sc-element
  ScType::EdgeAccessVarPosPerm,  // type of unknown sc-edge
  ScType::NodeVar >> "_x"  // type and alias for an unknown sc-element
);
templ.Triple(
  "_x",  // say that is the same sc-element as the last on in a previous triple
  ScType::EdgeAccessVarPosPerm,  // type of unknown sc-edge
  ScType::NodeVar  // type of unknown sc-node
);

size_t const tripleCount = templ.Size();
// The triple count must be equal to `2`.
...
```

### **IsEmpty**

If you need sc-template to be empty you don't have to add any constructions into it. But you should know
that result of generation by this sc-template is always `SC_TRUE` and result of searching by this sc-template is always
`SC_FALSE`. To check that sc-template is empty use the method `IsEmpty`.

```cpp
...
ScTemplate templ;
bool const isEmpty = templ.IsEmpty();
// The value of `isEmpty` be equal to `SC_TRUE`.
...
```

## **ScTemplateBuild**

Also, you can build sc-templates using [SCs-code](../../../../scs/scs.md).

```cpp
...
// Describe your sc-template on SCs-code.
sc_char const * data = 
  "_set"
  "  _<- concept_set;"
  "  _<- concept_binary_set;;";

// Build program object by this sc-template.
ScTemplate templ;
bool const isTemplateBuilt = context.HelperBuildTemplate(templ, data);
// The value of `isTemplateBuilt` must be equal to `SC_TRUE`.
...
```

During sc-template building all constants will be resolved by their system identifier (in
example: `concept_set`, `concept_binary_set`), so in result `templ` will contain sc-template:

<scg src="../images/templates/template_example_2.gwf"></scg>

Or you can it by specifying valid sc-address of some sc-template in sc-memory.

```cpp
...
// Find by system identifier your sc-template in sc-memory.
ScAddr const & templAddr = context.HelperFindBySystemIdtf("my_template");

// Build program object by this sc-template.
ScTemplate templ;
bool const isTemplateBuilt = context.HelperBuildTemplate(templ, templAddr);
// The value of `isTemplateBuilt` must be equal to `SC_TRUE`.
...
```

## **ScTemplateParams**

You can replace existing sc-variables in sc-templates by your ones. To provide different replacements for sc-variables 
in different cases there is class `ScTemplateParams`. It stores a map between sc-variables and specified values (replacements).

```cpp
...
ScTemplateParams params;
...
```

### **Add**

You can add replacement for sc-variable by specifying system identifier of this sc-variable if sc-template is built from
SCs-code.

```cpp
...
// Describe your sc-template on SCs-code.
sc_char const * data = 
  "_set"
  "  _<- concept_set;"
  "  _<- concept_binary_set;;";

// Create replacement in sc-memory.
ScAddr const & setAddr = context.CreateNode(ScType::NodeConst);
// Also you can find some replacement from sc-memory.

// Define replacements for sc-variables in sc-template.
ScTemplateParams params;
params.Add("_set", setAddr);

// Build program object by this sc-template, specifying replacements.
ScTemplate templ;
context.HelperBuildTemplate(templ, data, params);
...
```

Or you can add replacement for sc-variable by specifying sc-address of this sc-variable if sc-template is built from 
sc-address of sc-structure in sc-memory.

```cpp
...
// Find by system identifier your sc-template in sc-memory.
ScAddr const & templAddr = context.HelperFindBySystemIdtf("my_template");

// Find by system identifier sc-address of sc-variable in your sc-template.
ScAddr const & setVarAddr = context.HelperFindBySystemIdtf("_set");

// Create replacement in sc-memory.
ScAddr const & setAddr = context.CreateNode(ScType::NodeConst);
// Also you can find some replacement from sc-memory.

// Define replacements for sc-variables in sc-template.
ScTemplateParams params;
params.Add(setVarAddr, setAddr);

// Build program object by this sc-template, specifying replacements.
ScTemplate templ;
context.HelperBuildTemplate(templ, templAddr, params);
...
```

### **Get**

Sometimes you need to check if there is replacement in params yet. You can do it using the method `Get`. It works with
system identifiers and sc-addresses of sc-variables also.

```cpp
...
// Create replacement in sc-memory.
ScAddr const & setAddr = context.CreateNode(ScType::NodeConst);
// Also you can find some replacement from sc-memory.

// Define replacements for sc-variables in sc-template.
ScTemplateParams params;
params.Add("_set", setAddr);

ScAddr const & replAddr = params.Get("_set");
// The value of `replAddr` be equal to value of `setAddr`.
...
```

```cpp
...
// Find by system identifier sc-address of sc-variable in your sc-template.
ScAddr const & setVarAddr = context.HelperFindBySystemIdtf("_set");

// Create replacement in sc-memory.
ScAddr const & setAddr = context.CreateNode(ScType::NodeConst);
// Also you can find some replacement from sc-memory.

// Define replacements for sc-variables in sc-template.
ScTemplateParams params;
params.Add(setVarAddr, setAddr);

ScAddr const & replAddr = params.Get(setVarAddr);
// The value of `replAddr` be equal to value of `setAddr`.
...
```

!!! note
    If there are no replacements by specified system identifier or sc-address of sc-variable of sc-template then the 
    method `Get` will return empty sc-address.

### **IsEmpty**

To check that replacements map is empty use the method `IsEmpty`.

```cpp
...
ScTemplateParams params;
bool const isEmpty = params.IsEmpty();
// The value of `isEmpty` be equal to `SC_TRUE`.
...
```

## **HelperGenTemplate**

Use sc-template to generate graphs in sc-memory and get replacements from result.

```cpp
...
// Specify sc-template for searching sc-constructions in sc-memory.
// You can use `ScTemplate` methods or method ScTemplateBuild to translate 
// sc-template from SCs-code or sc-memory into program representation.
ScTemplateResultItem result;
bool const isGeneratedByTemplate = context.HelperGenTemplate(templ, result);
// Sc-elements sc-addresses of generated sc-construction may be gotten from 
// `result`.
...
```

!!! note 
    Remember, that sc-template must contain only valid sc-address of sc-elements and all sc-connectors in it must be
    sc-variables. Otherwise, this method can throw `utils::ExceptionInvalidParams` with description of this error.

## **ScTemplateResultItem**

It is a class that stores information about sc-construction.

### **Safe Get**

You can get sc-addresses of sc-elements of generated sc-construction by system identifier or sc-address of sc-variable 
in sc-template. To do it safely (without throwing exceptions if there are no replacements by specified system identifier 
or sc-address of sc-variable of sc-template) use the methods `Get` and provide result of replacement as out parameter in
this method.

```cpp
...
ScTemplate templ;
templ.Triple(
  conceptSetAddr,
  ScType::EdgeAccessVarPosPerm,
  ScType::NodeVar >> "_x"
);

ScTemplateResultItem result;
bool const isGeneratedByTemplate = context.HelperGenTemplate(templ, result);

ScAddr setAddr;
bool replExist = result.Get("_x", setAddr);
// The value of `replExist` be equal to `SC_TRUE`.

bool replExist = result.Get("_y", setAddr);
// The value of `replExist` be equal to `SC_FALSE`.
...
```

### **Get**

If you want to catch exceptions, if there are no replacements by specified system identifier or sc-address of sc-variable 
of sc-template, use the method `Get` and get replacement as result of this method. Then this method will throw 
`utils::ExceptionInvalidParams` with description of error.

```cpp
...
ScTemplate templ;
templ.Triple(
  conceptSetAddr,
  ScType::EdgeAccessVarPosPerm,
  ScType::NodeVar >> "_x"
);

ScTemplateResultItem result;
bool const isGeneratedByTemplate = context.HelperGenTemplate(templ, result);

ScAddr setAddr = result.Get("_x");

setAddr = result.Get("_y", setAddr);
// It will throw the exception `utils::ExceptionInvalidParams`.
...
```

### **Has**

To check that there is replacement by specified system identifier or sc-address of sc-variable of sc-template use the
method `Has`.

```cpp
...
ScTemplate templ;
templ.Triple(
  conceptSetAddr,
  ScType::EdgeAccessVarPosPerm,
  ScType::NodeVar >> "_x"
);

ScTemplateResultItem result;
bool const isGeneratedByTemplate = context.HelperGenTemplate(templ, result);

bool const replExist = result.Has("_x");
// The value of `replExist` be equal to `SC_TRUE`.
...
```

### **operator[]**

To get all replacements in result you can use the `operator[]`. It returns replacement by index of sc-variable in 
sc-template. If there is no sc-variable with specified index this method will throw the exception 
`utils::ExceptionInvalidParams` with description of the error.

```cpp
...
ScTemplate templ;
templ.Triple(
  conceptSetAddr,
  ScType::EdgeAccessVarPosPerm,
  ScType::NodeVar >> "_x"
);

ScTemplateResultItem result;
bool const isGeneratedByTemplate = context.HelperGenTemplate(templ, result);

ScAddr const & setAddr = result[2];
// It is equal to `result.Get("_x")`.
...
```

### **Size**

If you want to iterate all replacement in the result you need to know size of this result.

```cpp
...
ScTemplate templ;
templ.Triple(
  conceptSetAddr,
  ScType::EdgeAccessVarPosPerm,
  ScType::NodeVar >> "_x"
);

ScTemplateResultItem result;
bool const isGeneratedByTemplate = context.HelperGenTemplate(templ, result);

// Iterate by all replacements in result.
for (size_t i = 0; i < result.Size(); ++i)
{
  ScAddr const & addr = result[i];
  // Implement your code to handle replacements.
}
...
```

!!! note
    The method `Size` returns summary count of indexes of replacements in each triple in sc-template. If there are 
    `2` triples in sc-template, then there are `2 * 3 = 6` different indexes of replacements in sc-template.

## **HelperSearchTemplate**

You can search sc-construction in sc-memory by sc-templates. This search refers to isomorphic search by graph-template.
Search algorithm trying to find all possible variants of specified construction. It uses any constants (available
sc-addresses from parameters to find equal sc-constructions in sc-memory).

```cpp
...
// Specify sc-template for searching sc-constructions in sc-memory.
// You can use `ScTemplate` methods or method ScTemplateBuild to translate 
// sc-template from SCs-code or sc-memory into program representation.
ScTemplateSearchResult result;
bool const isFoundByTemplate = context.HelperSearchTemplate(templ, result);
// Program representation of sc-constructions in `ScTemplateResultItem` 
// may be gotten from `result`.
...
```

!!! note 
    Remember, that sc-template must contain only valid sc-address of sc-elements and all sc-connectors in it must be
    sc-variables. Otherwise, this method can throw `utils::ExceptionInvalidParams` with description of this error.

## **ScTemplateSearchResult**

It is a class that stores in information about sc-constructions represented in `ScTemplateResultItem`.
An object of class `ScTemplateSearchResult` can be referred to a vector of objects of class `ScTemplateResultItem`.

### **Safe Get**

To get object of class `ScTemplateResultItem` you can use the method `Get`. If you want to get objects safely, use the
method `Get` and provide `ScTemplateResultItem` as out parameter in this method.

```cpp
...
ScTemplate templ;
templ.Triple(
  conceptSetAddr,
  ScType::EdgeAccessVarPosPerm,
  ScType::NodeVar >> "_x"
);
// There is one sc-construction that is isomorphic this sc-template.

ScTemplateSearchResult result;
bool const isFoundByTemplate = context.HelperSearchTemplate(templ, result);

ScTemplateResultItem item;
bool constrExist = result.Get(0, item);
// The value of `constrExist` be equal to `SC_TRUE`.

constrExist = result.Get(1, item);
// The value of `constrExist` be equal to `SC_FALSE` and item is not valid.
...
```

### **Get**

If you want to catch exceptions use the method `Get` and get result as return value. If there is no sc-construction with 
specified index this method will throw the exception utils::ExceptionInvalidParams with description of the error.

```cpp
...
ScTemplate templ;
templ.Triple(
  conceptSetAddr,
  ScType::EdgeAccessVarPosPerm,
  ScType::NodeVar >> "_x"
);
// There is one sc-construction that is isomorphic this sc-template.

ScTemplateSearchResult result;
bool const isFoundByTemplate = context.HelperSearchTemplate(templ, result);

ScTemplateResultItem item = result.Get(0);
// It is a valid item.

item = result.Get(1);
// It throws `utils::ExceptionInvalidParams`.
...
```

### **operator[]**

Also, you can use the method `operator[]` to do this. If there is no sc-construction with specified index this method 
will throw the exception utils::ExceptionInvalidParams with description of the error.

```cpp
...
ScTemplate templ;
templ.Triple(
  conceptSetAddr,
  ScType::EdgeAccessVarPosPerm,
  ScType::NodeVar >> "_x"
);
// There is one sc-construction that is isomorphic this sc-template.

ScTemplateSearchResult result;
bool const isFoundByTemplate = context.HelperSearchTemplate(templ, result);

ScTemplateResultItem item = result[0];
// It is a valid item.

item = result[1];
// It throws `utils::ExceptionInvalidParams`.
...
```

### **Size**

To get count of found sc-constructions by sc-template you can use the method `Size`.

```cpp
...
ScTemplate templ;
templ.Triple(
  conceptSetAddr,
  ScType::EdgeAccessVarPosPerm,
  ScType::NodeVar >> "_x"
);
// There is one sc-construction that is isomorphic this sc-template.

ScTemplateSearchResult result;
bool const isFoundByTemplate = context.HelperSearchTemplate(templ, result);

size_t const count = result.Size();
// The value of `count` be equal to `1`.
...
```

### **IsEmpty**

To check if found result is empty use the method `IsEmpty`.

```cpp
...
ScTemplate templ;
templ.Triple(
  conceptSetAddr,
  ScType::EdgeAccessVarPosPerm,
  ScType::NodeVar >> "_x"
);
// There is one sc-construction that is isomorphic this sc-template.

ScTemplateSearchResult result;
bool const isFoundByTemplate = context.HelperSearchTemplate(templ, result);

bool const count = result.IsEmpty();
// The value of `count` be equal to `SC_FALSE`.
...
```

### **Clear**

To clear all information about found sc-constructions use the method `Clear`.

```cpp
...
ScTemplate templ;
templ.Triple(
  conceptSetAddr,
  ScType::EdgeAccessVarPosPerm,
  ScType::NodeVar >> "_x"
);
// There is one sc-construction that is isomorphic this sc-template.

ScTemplateSearchResult result;
bool const isFoundByTemplate = context.HelperSearchTemplate(templ, result);

result.Clear();
// After that `result` does not contain any information about sc-constructions.
...
```

### **ForEach**

To iterate all program objects of found sc-constructions by sc-template you can use for-each cycle.

```cpp
...
ScTemplate templ;
templ.Triple(
  conceptSetAddr,
  ScType::EdgeAccessVarPosPerm,
  ScType::NodeVar >> "_x"
);
// There is one sc-construction that is isomorphic this sc-template.

ScTemplateSearchResult result;
bool const isFoundByTemplate = context.HelperSearchTemplate(templ, result);

for (size_t i = 0; i < result.Size(); ++i)
{
  ScTemplateResultItem const & item = result.Get(i);
  // Implement logic to handle found sc-constructions.
}
...
```

Or you can use the method `ForEach` to do this.

```cpp
...
ScTemplate templ;
templ.Triple(
  conceptSetAddr,
  ScType::EdgeAccessVarPosPerm,
  ScType::NodeVar >> "_x"
);
// There is one sc-construction that is isomorphic this sc-template.

ScTemplateSearchResult result;
bool const isFoundByTemplate = context.HelperSearchTemplate(templ, result);

result.ForEach([](ScTemplateResultItem const & item) {
  // Implement logic to handle found sc-constructions.
});
...
```

## **HelperSmartSearchTemplate**

This method searches constructions by isomorphic sc-template and pass found sc-constructions to `callback` 
lambda-function. Lambda-function `callback` must return a request command value to manage sc-template search:

- ScTemplateSearchRequest::CONTINUE,
- ScTemplateSearchRequest::STOP,
- ScTemplateSearchRequest::ERROR.

When ScTemplateSearchRequest::CONTINUE returns, sc-template search will be continued. If ScTemplateSearchRequest::STOP 
or ScTemplateSearchRequest::ERROR returns, then sc-template search stops. If sc-template search stopped by 
ScTemplateSearchRequest::ERROR, then HelperSmartSearchTemplate thrown utils::ExceptionInvalidState. If `filterCallback` 
passed, then all found sc-constructions triples are filtered by `filterCallback` condition.

```cpp
...
ScAddr const & structureAddr = context.HelperFindBySystemIdtf("my_structure");
ScAddr const & setAddr = context.HelperFindBySystemIdtf("my_set");
ScAddr const & classAddr = context.HelperFindBySystemIdtf("my_class");

ScTemplate templ;
templ.Triple(
  classAddr,
  ScType::EdgeAccessVarPosPerm >> "_edge",
  ScType::Unknown >> "_addr2"
);
m_context->HelperSmartSearchTemplate(templ, [&context](
    ScTemplateSearchResultItem const & item) -> ScTemplateSearchRequest 
{
  ScAddr const & edgeAddr = item["_edge"];
  if (context->HelperCheckEdge(
      structureAddr, edgeAddr, ScType::EdgeAccessConstPosPerm))   
    return ScTemplateSearchRequest::CONTINUE;

  if (context.CreateEdge(
      ScType::EdgeAccessConstPosTemp, setAddr, item["_addr2"]))
    return ScTemplateSearchRequest::STOP;

  return ScTemplateSearchRequest::ERROR;
});
...
```

--- 

## **Frequently Asked Questions**

- [What's the best way to describe sc-templates for search? By sc-template C++ API or on the SC-code?](#what-is-the-best-way-to-describe-sc-templates-for-search-by-sc-template-c-api-or-on-the-sc-code)
- [Which is better: searching by sc-template or by iterator?](#which-is-better-searching-by-sc-template-or-by-iterator)

### **What is the best way to describe sc-templates for search? By sc-template C++ API or on the SC-code?**

The description of sc-templates in the knowledge base encourages the use of reflection for them. The sc-templates 
described in the knowledge base can be easily expanded and improved. However, sc-templates presented through the API 
do not require preprocessing (translation from the knowledge base), so the speed of the program used by such 
sc-templates may be higher if the sc-templates have significant size.

### **Which is better: searching by sc-template or by iterator?**

In all cases sc-iterator is faster the searching by sc-template. If you want to search large sc-constructions, then don't
search they by one large sc-template, divide it into certain sc-templates or use sc-iterators instead of sc-template.

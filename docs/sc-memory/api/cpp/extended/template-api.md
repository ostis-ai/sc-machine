# **ScTemplate API**

Sc-templates is a very powerful mechanism to work with semantic network (graph). You can generate and search any
constructions using sc-templates.

## **ScTemplate**

The class to work with sc-templates in C++ API. Before reading this paragraph you need to read
common [information about sc-element types](../../sc-element-types.md).

Let use `f` symbols for constant parameter of sc-template. Let use `a` symbol for a variable parameter of sc-template. 
Then sc-template to search all output sc-connectors from specified sc-element will be a triple:

* where the first element is known `f`;
* second and the third elements need to be found `a`.

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
      <br/>There <code>param1</code> is a known sc-address of sc-element. It must be a valid (use <code>IsElement</code> method to check). Where <code>_param2</code> and <code>_param3</code> are sc-types for compare by search engine. When search engine will traverse output edges from <code>param1</code>. Construction will be added into traverse result, where output sc-connector from <code>param1</code>, will suitable to specified type <code>_param2</code>, and type of target element of this edge will be sutable for a type <code>_param3</code>.
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

When sc-template search engine works, it tries to traverse graph by simple (triple) sc-template in order they specified. For
example, we need to check if specified sc-element (`_set`) is included into `concept_set` class and `concept_binary_relation` class:

<scg src="../images/templates/template_example_2.gwf"></scg>

Code that generates equal sc-template.

```cpp
ScAddr conceptSetAddr, conceptBinaryRelationAddr;
...

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
```

In code, you can see a construction `ScType::NodeVar >> "_set"` - this is a naming for a sc-template element.
It allows to set name for a specified sc-element in sc-template, and use it many times in different triples. You can see,
that in the second triple we use this name `"_set"`. That means, that we need to place search result from a
first triple into the second. So the second triple is a `f_a_f` style triple.

So if you want to use the same element `_x` in different triples, and you doesn't know it `ScAddr`, then just use two
main rules:

* Set name of this element in a first occurrence of this element in sc-template triples. You need to use `>>` operator to
  do this (see code below, last element of first triple).
* When you need to use named element in next triples, then just use it name instead of `ScType` or `ScAddr` (see code
  below, first element if second triple).

**Example code with naming**

```cpp
ScTemplateTempl;
templ.Triple(
  anyAddr, // sc-address of known sc-element
  ScType::EdgeAccessVarPosPerm,  // type of unknown edge
  ScType::NodeVar >> "_x"  // type and name for an unknown sc-element
);
templ.Triple(
  "_x",  // say that is the same element as the last on in a previous triple
  ScType::EdgeAccessVarPosPerm,  // type of unknown edge
  ScType::NodeVar  // type of unknown sc-element
);
```

## **ScTemplateBuild**

Also, you can generate sc-templates using [SCs-code](../../../../scs/scs.md).

```cpp
...
ScTemplate templ; 
sc_char const * data = 
  "_set"
  "  _<- concept_set;"
  "  _<- concept_binary_set;;";
ctx.HelperBuildTemplate(templ, data);
...
```

During sc-template building all constants will be resolved by their system identifier (in
example: `concept_set`, `concept_binary_set`), so in result `templ` will be contained sc-template:

<scg src="../images/templates/template_example_2.gwf"></scg>

Or you can it when specifying valid sc-address of some sc-template in sc-memory.

```cpp
...
ScAddr const & templAddr = ctx.HelperFindBySystemIdtf("my_template");

ScTemplate templ;
ctx.HelperBuildTemplate(templ, templAddr);
...
```

## **ScTemplateParams**

## **HelperGenTemplate**

Use sc-template to generate large graphs in sc-memory.

```cpp
...
// Specify sc-template for searching sc-constructions in sc-memory.
// You can use `ScTemplate` methods or method ScTemplateBuild to translate 
// sc-template from SCs-code or sc-memory into program representation.
ScTemplateResultItem result;
bool const isGeneratedByTemplate = ctx.HelperGenTemplate(templ, result);
...
```

> Note: Remember, that sc-template must contain only valid sc-address of sc-elements and all sc-connectors in it must be
> sc-variables. Otherwise, this method can throw `utils::ExceptionInvalidParams` with description of this error.

## **ScTemplateResultItem**

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
bool const isFoundByTemplate = ctx.HelperSearchTemplate(templ, result);
...
```

> Note: Remember, that sc-template must contain only valid sc-address of sc-elements and all sc-connectors in it must be
> sc-variables. Otherwise, this method can throw `utils::ExceptionInvalidParams` with description of this error.

## **ScTemplateSearchResult**

## **HelperSmartSearchTemplate**

## **FQA**

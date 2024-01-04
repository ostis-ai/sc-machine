# **ScTemplate API**

Sc-templates is a very powerful mechanism to work with semantic network (graph). You can search and generate any
constructions using sc-templates. There are list of available classes to work with sc-templates:

* `ScTemplate` - class that represents sc-template.
* `ScTemplateParams` - parameters that contains values of sc-variables in sc-template. This class usually used when you
  generate sc-construction by sc-template.
* `ScTemplateSearchResult` - contains result of search by sc-template (list of found sc-constructions).
* `ScTemplateResultItem` - represents on search or generation result item.

## **ScTemplate**

The class to work with sc-templates in C++ API. Before reading this paragraph you need to read
common [information about sc-element types](../../sc-element-types.md).

Let use `f` symbols for constant parameter of sc-template. Let use `a` symbol for a variable parameter of sc-template. 
Then sc-template to search all output connectors from specified sc-element will be a triple:

* where first element is known `f`;
* second and third elements need to be found `a`.

There are possible 3 types of simple sc-templates:

* `f_a_a` - sc-template to find all outgoing connectors from a specified sc-element;
* `f_a_f` - sc-template to find all connectors between two specified sc-elements;
* `a_a_f` - sc-template to find all ingoing connectors to a specified sc-element.

There are some methods available for `ScTemplate` class:

* `Triple` - method that adds triple sc-construction into sc-template. There are some examples of using this function to
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
example, we need to check if specified sc-element (`_set`) is included into `concept_set` class and `concept_binary_relation` class:

<scg src="../images/templates/template_example_2.gwf"></scg>

**Code that generates equal sc-template**

```cpp
ScAddr conceptSetAddr, conceptBinaryRelationAddr;
...

ScTemplate templ;
templ.Triple(
  conceptSetAddr,    // sc-address of concept set node
  ScType::EDGE_ACCESS_VAR_POS_PERM,
  ScType::NODE_VAR >> "_set"
);
templ.Triple(
  conceptBinaryRelationAddr,    // sc-address of concept binary relation node
  ScType::EDGE_ACCESS_VAR_POS_PERM,
  "_set"
);
```

In code, you can see a construction `ScType::NODE_VAR >> "_set"` - this is a naming for a sc-template element.
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
  ScType::EDGE_ACCESS_VAR_POS_PERM,  // type of unknown edge
  ScType::NODE_VAR >> "_x"  // type and name for an unknown sc-element
);
templ.Triple(
  "_x",  // say that is the same element as the last on in a previous triple
  ScType::EDGE_ACCESS_VAR_POS_PERM,  // type of unknown edge
  ScType::NODE_VAR  // type of unknown sc-element
);
```

## **ScTemplateBuild**

Also, you can generate sc-templates using [SCs-code](../../../../scs/scs.md). Example code:

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

## **ScTemplateGen**

## **ScTemplateSearch**

Search algorithm trying to find all possible variants of specified construction. It uses any constants (available 
`ScAddr`'s from parameters to find equal sc-constructions in sc-memory).


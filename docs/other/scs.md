### Common

SCs-code - is a text representation of SC-code. Whole text consist of sentences, that
separated by `;;` symbols.

You can use C-style comments in SCs text:
```cpp
// one line comment
fruit -> apple;
/* Multiline
 * comment
 */
```

To make an sc-link into specified file you can use special type identifier:
```
"file://<file name>"
```

* `"file://<file name>"` - is a relative path to a file. According to a file, where it used;
* `"file:///<file_name>"` - is an absolute path to a file.

There are some tricks with object names:

* `...` - is an unnamed object;
* `_<object name>` - all object names, that starts with symbol `_` represents a variable objects.

Objects identifier visibility. By default all objects with name `x` are visible anywhere.
After translating it into memory this object will have a **system identifier** equal to `x`.
So if you use `x` in different *scs* files, then you designate the same object in them
(would be the same element in a knowledge base).

Sometimes you need to designate the same objects in different files, but do not generate a
**system identifier** in memory for it. In this case you should to prefix it name with a `.` symbol.
For example: `.x`.

In case, when you need to make a named object just local for a *scs* file,
then you should to use `..` prefix (example: `..x`).

So a rule to build identifier is:

```
[visibility][variable]<identifier>
```

For example identifier `.._x` localy visible variable identifier.

<div class="note">
SCs-code is splitted into levels. Each level allows to minimize number
of string symbols to represent the same structures.
</div>

### SCs level 1

SCs-code level 1 - is a simple representation of SC-code. It represents it
with just simple triples. Each triple contains `subject`, `predicate`, `object`
that are splitted by `|` symbol. Line `subject | predicate | object;;` is a sentence.

Identifier of `subject`, `predicate`, `object` build with rule:

```
<type>#<identifier>
```

Where `type` - is an element type specification. It can be one of possible values:

* `sc_node` - equal to ![SCg-node](../images/scg/scg_node.png);
* `sc_link` - equal to ![SCg-link](../images/scg/scg_link.png);
* `sc_edge_dcommon` - equal to ![SCg-edge](../images/scg/scg_edge_common_orient.png);
* `sc_edge_ucommon` - equal to ![SCg-edge](../images/scg/scg_edge_common.png);
* `sc_edge_main` - equal to ![SCg-edge](../images/scg/scg_edge_const_pos_perm.png);
* `sc_edge_access` - equal to ![SCg-edge](../images/scg/scg_edge_access.png).

<hr/>

**Examples**

<table>
  <tr>
    <th>SCg construction</th>
    <th>Equal SCs-level 1 text</th>
  </tr>

  <tr>
    <td><scg src="../../images/scs/scs_example_level_1.gwf"></scg></td>
    <td>
      <pre>
        <code class="js hljs javascript">
// append set of apples into fruit set
sc_node#fruit | sc_edge_main#... | sc_node#apple;;
// append set of bananas into fruit set
sc_node#fruit | sc_edge_main#... | sc_node#banana;;
        </code>
      </pre>
    </td>
  </tr>

  <tr>
    <td width="50%"><scg src="../../images/scs/scs_example_level_1_2.gwf"></scg></td>
    <td>
      <pre>
        <code class="js hljs javascript">
sc_node#apple | sc_edge_dcommon#..e | "file://apple.png";;
/*append edge from nrel_image relation into
  edge between apple set and it's image*/
sc_node#nrel_image | sc_edge_main#... | sc_edge_dcommon#..e;;
        </code>
      </pre>
    </td>
  </tr>
</table>

SCs-level 1 allows you to represent any kind of SC-code construction. It's a low-level
representation and commonly used as a transport format, that is very simple for parsing.

### SCs level 2

This level of SCs-text add two new features:

* using of extended alphabet of edges;
* using of compound identifiers of an edges.

On this level you can make sentences by rule:
```
<element> <edge> <element>;;
```

Where `<edge>` can be on of values:
<table>
  <tr>
    <th>SCs</th>
    <th>SCg equivalent</th>
  </tr>
  <tr>
    <td>`<` or `>`</td>
    <td><img src="../../images/scg/scg_edge_common_orient.png"></img></td>
  </tr>
  <tr>
    <td>`<>`</td>
    <td><img src="../../images/scg/scg_edge_common.png"></img></td>
  </tr>
  <tr>
    <td>`<..` or `..>`</td>
    <td><img src="../../images/scg/scg_edge_access.png"></img></td>
  </tr>
  <tr>
    <td>`<=>`</td>
    <td><img src="../../images/scg/scg_edge_const_common.png"></img></td>
  </tr>
  <tr>
    <td>`_<=>`</td>
    <td><img src="../../images/scg/scg_edge_var_common.png"></img></td>
  </tr>
  <tr>
    <td>`<=` or `=>`</td>
    <td><img src="../../images/scg/scg_edge_const_common_orient.png"></img></td>
  </tr>
  <tr>
    <td>`_<=` or `_=>`</td>
    <td><img src="../../images/scg/scg_edge_var_common_orient.png"></img></td>
  </tr>
  <tr>
    <td>`<-` or `->`</td>
    <td><img src="../../images/scg/scg_edge_const_pos_perm.png"></img></td>
  </tr>
  <tr>
    <td>`_<-` or `_->`</td>
    <td><img src="../../images/scg/scg_edge_var_pos_perm.png"></img></td>
  </tr>
  <tr>
    <td>`<|-` or `-|>`</td>
    <td><img src="../../images/scg/scg_edge_const_neg_perm.png"></img></td>
  </tr>
  <tr>
    <td>`_<|-` or `_-|>`</td>
    <td><img src="../../images/scg/scg_edge_var_neg_perm.png"></img></td>
  </tr>
  <tr>
    <td>`</-` or `-/>`</td>
    <td><img src="../../images/scg/scg_edge_const_fuz_perm.png"></img></td>
  </tr>
  <tr>
    <td>`_</-` or `_-/>`</td>
    <td><img src="../../images/scg/scg_edge_var_fuz_perm.png"></img></td>
  </tr>
  <tr>
    <td>`<~` or `~>`</td>
    <td><img src="../../images/scg/scg_edge_const_pos_temp.png"></img></td>
  </tr>
  <tr>
    <td>`_<~` or `_~>`</td>
    <td><img src="../../images/scg/scg_edge_var_pos_temp.png"></img></td>
  </tr>
  <tr>
    <td>`<|~` or `~|>`</td>
    <td><img src="../../images/scg/scg_edge_const_neg_temp.png"></img></td>
  </tr>
  <tr>
    <td>`_<|~` or `_~|>`</td>
    <td><img src="../../images/scg/scg_edge_var_neg_temp.png"></img></td>
  </tr>
  <tr>
    <td>`</~` or `~/>`</td>
    <td><img src="../../images/scg/scg_edge_const_fuz_temp.png"></img></td>
  </tr>
  <tr>
    <td>`_</~` or `_~/>`</td>
    <td><img src="../../images/scg/scg_edge_var_fuz_temp.png"></img></td>
  </tr>
</table>

<hr/>

<table>
  <tr>
    <th>SCg construction</th>
    <th>Equal SCs-level 2 text</th>
  </tr>

  <tr>
    <td><scg src="../../images/scs/scs_example_level_1.gwf"></scg></td>
    <td>
      <pre>
        <code class="js hljs javascript">
fruit -> apple;;
// backward direction
banana <- fruit;;
        </code>
      </pre>
    </td>
  </tr>
</table>

<hr/>
Compound identifier of an edge builds as a sentence in SCs-level 2, but without
`;;` separator and inside brackets `()`: `(<element> <edge> <element>)`.
So that allows to simplify usage of an edge as a source or target of another one.

<hr/>

**Examples**
<table>
  <tr>
    <th>SCg construction</th>
    <th>Equal SCs-level 2 text</th>
  </tr>

  <tr>
    <td width="50%"><scg src="../../images/scs/scs_example_level_1_2.gwf"></scg></td>
    <td>
      <pre>
        <code class="js hljs javascript">
nrel_image -> (fruit => "file://apple.png");;
        </code>
      </pre>
    </td>
  </tr>

  <tr>
    <td><scg src="../../images/scs/scs_example_level_2.gwf"></scg></td>
    <td>
      <pre>
        <code class="js hljs javascript">
d -> (c -> (a -> b));;
        </code>
      </pre>
    </td>
  </tr>

  <tr>
    <td><scg src="../../images/scs/scs_example_level_2_2.gwf"></scg></td>
    <td>
      <pre>
        <code class="js hljs javascript">
(a -> b) -> (c <- d);;
        </code>
      </pre>
    </td>
  </tr>

</table>

### SCs level 3

This level of SCs-text allows to minimize symbols to represent constructions like this one:
<scg src="../../images/scs/scs_example_level_3.gwf"></scg>

```js
c -> (a -> b);;
```

To do that you should use sentence like this:
```
<object> <edge> <attribute>: <object>
```

For this example it would be like this:
```js
a -> c: b;;
```

In case, when output edge from `c` is an variable, then use `::` splitter instead of `:`:
```js
a -> c:: b;;
```
equal to:
```js
c _-> (a -> b);;
```

<div class="note">
<b>Note</b>: you can use <code>:</code>, <code>::</code> just to replace <code>-></code> or <code>_-></code> edges.
</div>

<hr/>

**Examples**
<table>
  <tr>
    <th>SCg construction</th>
    <th>Equal SCs-level 3 text</th>
  </tr>

  <tr>
    <td><scg src="../../images/scs/scs_example_level_1_2.gwf"></scg></td>
    <td>
      <pre>
        <code class="js hljs javascript">
apple => nrel_image: "file://apple.png";;
        </code>
      </pre>
    </td>
  </tr>

  <tr>
    <td><scg src="../../images/scs/scs_example_level_3_2.gwf"></scg></td>
    <td>
      <div class="note">
        <b>Note</b>: it is possible to use any number of <code>:</code>, <code>::</code> in one sentence.
      </div>
      <pre>
        <code class="js hljs javascript">
a -> c: d:: b;;
        </code>
      </pre>
    </td>
  </tr>
</table>

### SCs level 4

This level of SCs-text allows to combine many setences with one element into one.
For that pupropses used ';' symbol. So if we have some sentences like:
```js
x -> y;;
x <- z;;
x => h: r;;
```

Then using SCs level 4 we can write them like this:
```js
x -> y;
  <- z;
  => h: r;;
```

Another words, this level of SCs text, allows to use source element one time.

<hr/>

**Examples**
<table>
  <tr>
    <th>SCg construction</th>
    <th>Equal SCs-level 4 text</th>
  </tr>

  <tr>
    <td><scg src="../../images/scs/scs_example_level_1.gwf"></scg></td>
    <td>
      <pre>
        <code class="js hljs javascript">
fruit -> apple;
      -> banana;;
        </code>
      </pre>
    </td>
  <tr>

  <tr>
    <td><scg src="../../images/scs/scs_example_level_4.gwf"></scg></td>
    <td>
      <pre>
        <code class="js hljs javascript">
a -> c: d: b;
  -> e;
  -> g: f;;
        </code>
      </pre>
    </td>
  </tr>
<table>

### SCs level 5

### SCs level 6

### Keynodes

There are a list of element type keynodes, that can be used to specify type of an element:

| Keynode                           | Equal sc-type                       | Equal SCg-element
| --------------------------------- | ----------------------------------- | --------------
| sc_node                           | ScType::Node                        | ![SCg-edge](../images/scg/scg_node.png)
| sc_link                           | ScType::Link                        | ![SCg-edge](../images/scg/scg_link.png)
| sc_edge_dcommon                   | ScType::EdgeDCommon                 | ![SCg-edge](../images/scg/scg_edge_common_orient.png)
| sc_edge_ucommon                   | ScType::EdgeUCommon                 | ![SCg-edge](../images/scg/scg_edge_common.png)
| sc_edge_main                      | ScType::EdgeAccessConstPosPerm      | ![SCg-edge](../images/scg/scg_edge_const_pos_perm.png)
| sc_edge_access                    | ScType::EdgeAccess                  | ![SCg-edge](../images/scg/scg_edge_access.png)
| sc_node_tuple                     | ScType::NodeTuple                   | ![SCg-edge](../images/scg/scg_node_const_tuple.png) ![SCg-edge](../images/scg/scg_node_var_tuple.png)
| sc_node_struct                    | ScType::NodeStruct                  | ![SCg-edge](../images/scg/scg_node_const_struct.png) ![SCg-edge](../images/scg/scg_node_var_struct.png)
| sc_node_role_relation             | ScType::NodeRole                    | ![SCg-edge](../images/scg/scg_node_const_role.png) ![SCg-edge](../images/scg/scg_node_var_role.png)
| sc_node_norole_relation           | ScType::NodeNoRole                  | ![SCg-edge](../images/scg/scg_node_const_norole.png) ![SCg-edge](../images/scg/scg_node_var_norole.png)
| sc_node_class                     | ScType::NodeClass                   | ![SCg-edge](../images/scg/scg_node_const_class.png) ![SCg-edge](../images/scg/scg_node_var_class.png)
| sc_node_abstract                  | ScType::NodeAbstract                | ![SCg-edge](../images/scg/scg_node_const_abstract.png) ![SCg-edge](../images/scg/scg_node_var_abstract.png)
| sc_node_material                  | ScType::NodeMaterial                | ![SCg-edge](../images/scg/scg_node_const_material.png) ![SCg-edge](../images/scg/scg_node_var_material.png)

There are old keynodes, that used for backward compatibility:

| Keynode                   | Equal sc-type | New keynode |
| ------------------------- | ------------- | ----------- |
| sc_arc_main               | ScType::EdgeAccessConstPosPerm | sc_edge_main |
| sc_edge                   | ScType::EdgeUCommon | sc_edge_ucommon |
| sc_arc_common             | ScType::EdgeDCommon | sc_edge_dcommon |
| sc_arc_access             | ScType::EdgeAccess  | sc_edge_access |
| sc_node_not_relation      | ScType::NodeClass | sc_node_class |
| sc_node_not_binary_tuple  | ScType::NodeTuple | sc_node_tuple |

There is an example of usage:

<table>
  <tr>
    <th>SCs-code</th>
    <th>Equal SCg-code</th>
  </tr>
  <tr>
    <td>
      <pre>
a <- sc_node_class;;
a _-> _b;;
_b <- sc_node_material;;
      </pre>
    </td>
    <td><scg src="../images/scs_keynodes_old_example_1.gwf"></scg></td>
  </tr>
  <tr>
    <td>
      <pre>
_x => nrel_y: t;;
nrel_y <- sc_node_norole_relation;;
      </pre>
    </td>
    <td><scg src="../images/scs_keynodes_old_example_2.gwf"></scg></td>
  </tr>
</table>
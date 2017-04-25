# Python

You can implement agents using python. All python modules should be implemented with specified rules.

1. Each module, that implement agent should be names by this rule: `a_<agentName>.py`. That allows system to
know which modules could be used to run. Util modules and others should be named NOT using this rule.

2. You can specify search paths of a python modules by `python.modules_path` value in a [configuration file](../other/config.md).

# Classes

## ScAddr

This class represents `ScAddr` in C++. Methods of this class:

- `IsValid` - if addr is valid, then returns `True`; otherwise - `False`
- `ToInt` - convert addr value to integer (equal C++ `ScAddr::Hash`). _Useful for debug purposes_.
- `__eq__` - equal operator `==`. Can be used like this:
```python
if addr1 == addr2:
  pass # do something
```

---

- `__ne__` - not equal operator `!=`. Can be used like this:
```python
if addr1 != addr2:
  pass # do something
```

## ScType

This class equal to `ScType` in C++. Methods of this class:

- `constructor` - you can create this class with input type combinations
```python
type1 = ScType() # equal to ScType.Unknown
type2 = ScType.Node # see more constants below
type3 = ScType.Node | ScType.Const # equal to ScType.NodeConst
```

---

- `IsValid()` - if type is not `Unknown`, then returns `True`; otherwise - `False`
- `__eq__` - equal operator `==`. Can be used like this:
```python
if type1 == type2:
  pass # do something
```

---

- `__eq__` - not equal operator `!=`. Can be used like this:
```python
if type1 != type2:
  pass # do something
```

---

- `__or__` - bitwise or operator `|`. Can be used like this:
```python
type1 = ScType.Node
type2 = ScType.Const

type3 = type1 | type2
type3 == ScType.NodeConst # will be True
```

---

- `__and__` - bitwise and operator `&`. Can be used like this:
```python
type1 = ScType.NodeConst
type2 = ScType.Node

type3 = type1 & type2

type3 == type1 # will be True
```

---

- `IsLink()` - if type represents a link, then returns `True`; otherwise - `False`
```python
type1 = ScType.LinkConst
type1.IsLink() # returns True
```

---

- `IsEdge()` - if type represents an edge, then returns `True`; otherwise - `False`
```python
type1 = ScType.EdgeAccessConstFuzPerm
type1.IsEdge() # return True
```

---

- `IsNode()` - if type represents a node, then returns `True`; otherwise - `False`
```python
type1 = ScType.NodeConst
type1.IsNode() # returns True
```

---

- `IsValid()` - if type is not `Unknown`, then returns `True`; otherwise - `False`
```python
type1 = ScType.Node
type1.IsValid() # return True

type2 = ScType()
type2.IsValid() # return False
```

---

- `IsConst()` - if type represents constant, then returns `True`; otherwise - `False`
```python
type1 = ScType()
type1.IsConst() # returns False

type2 = ScType.NodeVar()
type2.IsConst() # return False

type3 = ScType.Node()
type3.IsConst() # return False

type4 = ScType.NodeConst()
type4.IsConst() # returns True
```

---

- `IsVar()` - if type represents variable, then returns `True`; otherwise - `False`
```python
type1 = ScType()
type1.IsVar() # returns False

type2 = ScType.NodeVar()
type2.IsVar() # returns True

type3= ScType.Node()
type3.IsVar() # returns False

type4 = ScType.NodeConst
type4.IsVar() # returns False
```

- `ToInt()` - returns integer, that represents a type. Useful for debug purposes

**There are some predefined types**. You can find them in [types table](../cpp/el_types.md) (see C++ table). In **Python** you should use `ScType.Node` instead of `ScType::Node`


## ScIterator3

This class represents iterator for a triples (see [iterators description](../cpp/common.md#iterators)). There are a list of available methods:

- `Next()` - move iterator to next triple. Returns `True` if moved to next triple; otherwise - `False`. Example of usage:
```python
while (it3.Next())
  ... # do something
```

---

- `Get(idx)` - returns `ScAddr` of specified element in current triple (should be in range `[0; 2]`). Example of usage:
```python
while (it3.Next())
  src = it3.Get(0)  # source element in triple
  edge = it3.Get(1) # edge in triple
  trg = it3.Get(2)  # target element in triple
  ...
```

---

- `IsValid()` - returns `True` if iterator is valid; otherwise - `False`

## ScIterator5

This class represents iterator of 5-element constructions (see [iterators description](../cpp/common.md#iterators)). There are a list of available methods:

- `Next()` - move iterator to next 5-element construction. Returns `True` if moved to next construction; otherwise - `False`. Example of usage:
```python
while (it5.Next())
  ... # do something
```

---

- `Get(idx)` - returns `ScAddr` of specified element in current construction (should be in range `[0; 4]`). Example of usage:
```python
while (it5.Next())
  src = it5.Get(0)      # source element in triple
  edge = it5.Get(1)     # edge in triple
  trg = it5.Get(2)      # target element in triple
  attrEdge = it5.Get(3) # edge from attribute set
  attr = it5.Get(4)     # attribute set
  ...
```

---

- `IsValid()` - returns `True` if iterator is valid; otherwise - `False`


## ScMemoryContext

This class implements context, that allows you to work with memory.
<div class="note"><b>You shouldn't share one context between two or more threads</b></div>
There are methods of this class:

- `CreateNode(type)` - create node with specified type. Where `type` is an instance of `ScType` (just **edge** types could be used). Returns `ScAddr` of created node. If returned `ScAddr` is not valid (`IsValid()`), then node wasn't created.
```python
nodeAddr = ctx.CreateNode(ScType.NodeConst)
```

---

- `CreateEdge(type, src, trg)` - create edge between `src` and `trg` elements. Where `type` is an instance
of `ScType` (just **edge** types could be used); `src` is a `ScAddr` of source element (could be any but not `Unknown`); `trg` is a `ScAddr` of target element (could be any but not `Unknown`). Returns `ScAddr` of created edge, but if returned `ScAddr` is not valid, then edge wasn't created.
```python
edgeAddr = ctx.CreateEdge(ScType.EdgeAccessConstPosPerm, srcAddr, trgAddr)
```

---

- `CreateLink()` - create link. Returns `ScAddr` of created link, but if returned `ScAddr` is not valid, then link wasn't created
```python
linkAddr = ctx.CreateLink()
```

---

- `GetName()` - returns name of context. Useful in debug purposes

---

- `IsElement(addr)` - if element with specified `addr` exist, then returns `True`; otherwise - `False`
```python
ctx.IsElement(elementAddr)
```

---

- `GetElementType(addr)` - returns type of specified element. If element doesn't exist, then returns `ScType.Unknown`
```python
t = ctx.GetElementType(elementAddr)
```

---

- `GetEdgeInfo(addr)` - returns tuple `(src, trg)`, where `src` - is a `ScAddr` of adge source element; `trg` - target element. If `addr` point to element that doesn't exist, or is not an edge, then returns `(None, None)`.
```python
src, trg = ctx.GetEdgeInfo(edgeAddr)
```

---

- `SetLinkContent(addr, content)` - sets specified `content` into link (specified with `addr`). Type of `content` should be one of: `int`, `float`, `string`. If content changed, then returns `True`; otherwise - `False`
```python

...
ctx.SetLinkContent(linkAddr1, 56)
...
ctx.SetLinkContent(linkAddr1, 56.0)
...
ctx.SetLinkContent(linkAddr1, "any text")
...

```

---

- `GetLinkContent(addr)` - returns content of a specified link. If specified `addr` is not a link, or it doesn't exist, then returns `None`. Returned value has type `string`. It can be unpacked with [struct module](https://docs.python.org/2/library/struct.html) depending on semantic.
```python
...

value = ctx.GetLinkContent(elementAddr)
if value:
  print (value)

...
```

---

- `Iterator3(param1, param2, param3)` - create iterator for a specified triple template. Parameters `param1`, `param2`, `param3` could be on of a type: `ScAddr`, `ScType`. For more information about iterators see [iterators description](../cpp/common.md#iterators). If iterator created, then return `ScIterator3` object; otherwise - `None`

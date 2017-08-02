# Python

You can implement agents using **Python 3**. All python modules should be implemented with specified rules.

1. You can specify search paths of a python modules by `python.modules_path` value in a [configuration file](../other/config.md).


# Usage example
There are two ways to run python script:

- when you need run script and wait it finished, then just use this code:
```cpp
#include <sc-memory/cpp/python/sc_python_interp.hpp>
...
void doSomething()
{
  ...
  py::ScPythonInterpreter::RunScript("lg/tv_service.py", m_bridge);
  ...
}
```
Then in python you can write any code you want:
```python
import any_module

# create context (with minimal access)
ctx = createScMemoryContext('context name')

# create node
nodeAddr = ctx.CreateNode(ScType.NodeConst)
...

# you can use any installed python libraries
any_module.doSomething()
```

- if you want to run script as a service and communicate with it, then use code:

**my_service.hpp**
```cpp
#pragma once

#include <sc-memory/cpp/python/sc_python_service.hpp>

class MyService : public py::ScPythonService
{
public:
  MyService();

private:
  virtual void RunImpl() override;
  virtual void StopImpl() override;
};

}
```

**my_service.cpp**
```cpp
#include "my_service.hpp"

MyService::MyService()
  : ScPythonService("my_service.py")
{
}

void MyService::RunImpl()
{
  // your run code that need to be run after service started
}

void MyService::StopImpl()
{
  // your run code that need to be run after service stopped
}
```

Then in python code use common module:

**my_service.py**
```python
from common import ScModule

class MyModule(ScModule):

    kNrelMainIdtf = 'nrel_main_idtf'
    kDevice = 'device'

    def __init__(self):
        ScModule.__init__(self,
            createScMemoryContext('MyModule'), keynodes=[
                # there we can initialize keynodes that we need
                MyModule.kNrelMainIdtf,
                MyModule.kDevice,
            ], cpp_bridge=cpp_bridge)
        # cpp_bridge - is a global variable that store CPP-Python bridge object


    def OnCppUpdate(self):
        super.OnCppUpdate(self)
        # you can overload more functions for work
        # see more details in sc-kpm/python/common/sc_module.py

    def printMainIdtf(self):
        # template to update current volume value
        templ = ScTemplate()
        templ.TripleWithRelation(
            self.keynodes[self.kDevice], # get ScAddr of keynode
            ScType.EdgeDCommonVar,
            ScType.Link >> '_link',
            ScType.EdgeAccessVarPosPerm,
            self.keynodes[self.kNrelMainIdtf])

        # self.sc_context - is a context to work with sc-memory (use just this one)
        searchResult = self.sc_context.HelperSearchTemplate(templ)

        linkAddr = None
        if searchResult.Size() > 0:
            # we found old value change it
            linkAddr = searchResult[0]['_link']
        else:
            genResult = self.sc_context.HelperGenTemplate(templ, ScTemplateGenParams())
            if genResult:
                linkAddr = genResult['_link']

        if linkAddr:
            print(self.sc_context.GetLinkContent(linkAddr))

service = MyModule()
service.Run()
```

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


## ScLinkContent

This class wrap content of link. It allows to unpack it to `string`, `int` or `float`. There are methods of this class:

- `AsString` - get content of a link as `string` value.
```python
content = ctx.GetLinkContent(linkAddr)
stringValue = content.AsString()
```

----

- `AsInt` - get content of a link as `int` value. If length of content not equal to 8 bytes, then `ExceptionInvalidType` will be raised.
```python
content = ctx.GetLinkContent(linkAddr)
intValue = content.AsInt()
```

---

- `AsFloat` - get content of a link as `float` value. If length of content not equal to 8 bytes, then `ExceptionInvalidType` will be raised.
```python
content = ctx.GetLinkContent(linkAddr)
floatValue = content.AsFloat()
```

## ScTemplateGenParams

This class accumulate parameters for a template generation. There are methods of this class:

- `Add(paramName, valueAddr)` - add new parameter value, where `paramName` - string with a replacement name; `valueAddr` - `ScAddr` of element that need to be placed instead of specified replacement (see more in [templates description](../cpp/templates.md))
```python
params = ScTemplateGenParams()
params.Add("_item", itemAddr);
...
```

---

- `Get(paramName)` - return value of parameter with a specified name `paramName`. If parameter with specified identifier exists, then returns `ScAddr` value of it; otherwise - `None`
```python
addr = params.Get("_item")
```

---

- `IsEmpty()` - if there are no any parameters added, then returns `True`; otherwise - `False`


## ScTemplateGenResult

This class wrap template generation result. There are methods of this class:

- `Size()` - return number of elements

---

- `__getitem__(replName)` - return specified result by replacement name (`replName` - string). Returns `ScAddr` type. If there are no replacement with a specified name, then raise `ExceptionItemNotFound` exception
```python
addr = genResult["node1"]
```

## ScTemplateSearchResultItem

This class represents one result for a search by template. There are methods of this class:

- `Size()` - return size of result (number of addrs equal to search construction)

---

- `__getitem__` - allows to get result items by any index: `int`, `str`. In case of `int` index you will just get `ScAddr` by index in result array (length equal to `Size()`) this case suitable, when you need to iterate all addrs in result. <div class="note">You will receive duplicate `ScAddr`'s, because result stored as array of founded triples.'</div> When you try to get `ScAddr` by name it will be find it by replacement name (see [templates](../cpp/templates.md) for more info). If there are no element with specified index, then returns `None`
```python
resultSize = searchResultItem.Size()
for i in range(resultSize):
  addr = searchResultItem[i] # iterate all addrs

addr1 = searchResultItem["replacement name"] # get by replacement name
```

## ScTemplateSearchResult

This class represent list of results by template search. There are methods of this class:

- `Size()` - return number of results.

---

- `__getitem__` - get result by specified index (`int` should be less then `Size()`).
```python
resultNum = searchResult.Size()
for i in range(resultNum):
  searchResultItem = searchResult[i]

  # work with searchResultItem there see (ScTemplateSearchResult)
```

## ScMemoryContext

This class implements context, that allows you to work with memory.
<div class="note"><b>DO NOT use one context in different threads</b></div>
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

- `GetLinkContent(addr)` - returns content of a specified link. If specified `addr` is not a link, or it doesn't exist, then returns `None`. Returned value has type `ScLinkContent`.
```python
...

value = ctx.GetLinkContent(elementAddr)
if value:
  print (value)

...
```

---

- `Iterator3(param1, param2, param3)` - create iterator for a specified triple template. Parameters `param1`, `param2`, `param3` could be on of a type: `ScAddr`, `ScType`. For more information about iterators see [iterators description](../cpp/common.md#iterators). If iterator created, then return `ScIterator3` object; otherwise - `None`
```python
itFAA = ctx.Iterator3(addr1, ScType.EdgeAccessConstPosPerm, ScType.NodeVar)
while itFAA.Next():
  pass # process iterated constructions there

...

itFAF = ctx.Iterator3(addr1, ScType.EdgeAccessConstPosPerm, addr2)
while itFAF.Next():
  pass # process iterated constructions there

...

itAAF = ctx.Iterator3(ScType.NodeConst, ScType.EdgeAccessConstPosPerm, addr2)
while itAAF.Next():
  pass # process iterated constructions there
```

---

- `Iterator5(param1, param2, param3, param4, param5)` - create iterator for a specified 5-element construction. Parameters `param1`, `param2`, `param3`, `param4`, `param5` could be on of a type `ScAddr`, `ScType`. For more information about iterators see [iterators description](../cpp/common.md#iterators). If iterator created, then return `ScIterator5` object; otherwise - `None`
```python
itFAFAF = ctx.Iterator5(
            addr1,
            ScType.EdgeAccessConstPosPerm,
            addr2,
            ScType.EdgeAccessVarPosTemp,
            attr)
while itFAFAF.Next():
  pass # process iterated constructions there

...

itFAFAA = ctx.Iterator5(
            addr1,
            ScType.EdgeAccessConstPosPerm,
            addr2,
            ScType.EdgeAccessVarPosTemp,
            ScType.Node)
while itFAFAA.Next():
  pass # process iterated constructions there

...

itFAAAF = ctx.Iterator5(
            addr1,
            ScType.EdgeAccessConstPosPerm,
            ScType.NodeVar,
            ScType.EdgeAccessVarPosTemp,
            attr)
while itFAAAF.Next():
  pass # process iterated constructions there

...

itFAAAA = ctx.Iterator5(
            addr1,
            ScType.EdgeAccessConstPosPerm,
            ScType.NodeVar,
            ScType.EdgeAccessVarPosTemp,
            ScType.Node)
while itFAAAA.Next():
  pass # process iterated constructions there

...
itAAFAF = ctx.Iterator5(
            ScType.NodeConst,
            ScType.EdgeAccessConstPosPerm,
            addr2,
            ScType.EdgeAccessVarPosTemp,
            attr)
while itAAFAF.Next():
  pass # process iterated constructions there

...

itAAFAA = ctx.Iterator5(
            ScType.NodeConst,
            ScType.EdgeAccessConstPosPerm,
            addr2,
            ScType.EdgeAccessVarPosTemp,
            ScType.Node)
while itAAFAA.Next():
  pass # process iterated constructions there
```

---

- `HelperResolveSystemIdtf(idtf, type)` - resolve element by system identifier. This function tries to find element with specified system identifier - `idtf`. If element wasn't found and type is `ScType`, then new element will be created with specified system identifier and type. If `type` is `None`, then new element wouldn't created and function returns invalid `ScAddr`. In other cases function returns `ScAddr` of element with specified system identifier.
```python
addr = ctx.HelperResolveSystemIdtf("nrel_main_idtf", ScType.NodeConstNoRole)
```

---

- `HelperSetSystemIdtf(idtf, addr)` - set specified system identifier - `idtf`, to element with specified `addr`. If identifier changed, then returns `True`; otherwise - `False`
```python
if ctx.HelperSetSystemIdtf("new_idtf", addr):
  pass # identifier changed
else:
  pass # identifier not changed
```

---

- `HelperGetSystemIdtf(addr)` - returns system identifier of a specified element. If system identifier exist, then it returns as a non empty string; otherwise result string would be empty.
```python
idtfValue = ctx.HelperGetSystemIdtf(addr)
```

---

- `HelperCheckEdge(srcAddr, trgAddr, edgeType)` - if there are one or more edge with a specified type (`edgeType`) between `srcAddr` and `trgAddr` elements, then returns `True`; otherwise - `False`
```python
if ctx.HelperCheckEdge(addr1, addr2, ScType.EdgeAccessConstPosPerm):
  pass # edge between addr1 and addr2 exist
else:
  pass # edge doesn't exists
```

---

- `HelperGenTemplate(templ, params)` - generate construction by specified template (`templ` should be instance of `ScTemplate`) with specified parameters (`params` should be instance of `ScTemplateGenParams`). If construction generated, then returns `ScTemplateGenResult` that contains generated construction; otherwise - `None`.
```python
templ = ScTemplate()
... # fill template

params = ScTemplateGenParams()
... # fill parameters

result = ctx.HelperGenTemplate(templ, params)
```

---

- `HelperSearchTemplate(templ)` - search construction by specified template (`templ` should be instance of `ScTemplate`). Returns `ScTemplateSearchResult` object instance. If it `Size()` equal to 0, then no anything found
```python
templ = ScTemplate()
... # fill template

result = ctx.HelperSearchTemplate(templ)
```

---

- `HelperBuildTemplate(templAddr)` - build template from construction in memory. Returns `ScTemplate` instance. If template wasn't built, then return `None`
```python
templ = ctx.HelperBuildTemplate(templAddr)
... # work with template
```

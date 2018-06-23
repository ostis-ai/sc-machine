## Global

- `getScConfigValue` - function that returns value from a sc-memory config file.
```python
configValue = getScConfigValue('group', 'value')
```

## ScAddr

This class represents `ScAddr` in C++. Methods of this class:
??? tip "IsValid()" 
    if `ScAddr` is valid, then returns `True`; otherwise - `False`

??? tip "ToInt()"
    convert addr value to integer (equal C++ `ScAddr::Hash`). _Useful for debug purposes_.

??? tip "\_\_eq\_\_()"
    equal operator `==`. Can be used like this:

    ```python
    if addr1 == addr2:
      pass # do something
    ```

??? tip "\_\_ne\_\_()"
    not equal operator `!=`. Can be used like this:
    ```python
    if addr1 != addr2:
      pass # do something
    ```

## ScType

This class equal to `ScType` in C++. Methods of this class:

??? tip "constructor"
    you can create this class with input type combinations
    ```python
    type1 = ScType() # equal to ScType.Unknown
    type2 = ScType.Node # see more constants below
    type3 = ScType.Node | ScType.Const # equal to ScType.NodeConst
    ```

??? tip "IsValid()"
    if type is not `ScType.Unknown`, then returns `True`; otherwise - `False`

??? tip "\_\_eq\_\_()"
    equal operator `==`. Can be used like this:
    ```python
    if type1 == type2:
      pass # do something
    ```

??? tip "\_\_ne\_\_()"
    not equal operator `!=`. Can be used like this:
    ```python
    if type1 != type2:
      pass # do something
    ```

??? tip "\_\_or\_\_()"
    bitwise or operator `|`. Can be used like this:
    ```python
    type1 = ScType.Node
    type2 = ScType.Const

    type3 = type1 | type2
    type3 == ScType.NodeConst # will be True
    ```

??? tip "\_\_and\_\_()"
    bitwise and operator `&`. Can be used like this:
    ```python
    type1 = ScType.NodeConst
    type2 = ScType.Node

    type3 = type1 & type2

    type3 == type1 # will be True
    ```

??? tip "IsLink()"
    if type represents a link, then returns `True`; otherwise - `False`
    ```python
    type1 = ScType.LinkConst
    type1.IsLink() # returns True
    ```

??? tip "IsEdge()"
    if type represents an edge, then returns `True`; otherwise - `False`
    ```python
    type1 = ScType.EdgeAccessConstFuzPerm
    type1.IsEdge() # return True
    ```

??? tip "IsNode()"
    if type represents a node, then returns `True`; otherwise - `False`
    ```python
    type1 = ScType.NodeConst
    type1.IsNode() # returns True
    ```

??? tip "IsUnknown()"
    if type is not `Unknown`, then returns `True`; otherwise - `False`
    ```python
    type1 = ScType.Node
    type1.IsUnknown() # return True

    type2 = ScType()
    type2.IsUnknown() # return False
    ```

??? tip "IsConst()"
    if type represents constant, then returns `True`; otherwise - `False`
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

??? tip "IsVar()"
    if type represents variable, then returns `True`; otherwise - `False`
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

??? tip "ToInt()"
    returns integer, that represents a type. Useful for debug purposes

**There are some predefined types**. You can find them in [types table](../cpp/el_types.md) (see C++ table). In **Python** you should use `ScType.Node` instead of `ScType::Node`


## ScIterator3

This class represents iterator for a triples (see [iterators description](../cpp/common.md#iterators)). There are a list of available methods:

??? tip "Next()" 
    move iterator to next triple. Returns `True` if moved to next triple; otherwise - `False`. Example of usage:
    ```python
    while it3.Next():
      ... # do something
    ```

??? tip "Get(idx)"
    * **idx** - `number` that represents values index. Ыhould be in range `[0; 2]`

    returns `ScAddr` of specified element in current triple. Example of usage:
    ```python
    while it3.Next():
      src = it3.Get(0)  # source element in triple
      edge = it3.Get(1) # edge in triple
      trg = it3.Get(2)  # target element in triple
      ...
    ```

??? tip "IsValid()"
    returns `True` if iterator is valid; otherwise - `False`

## ScIterator5

This class represents iterator of 5-element constructions (see [iterators description](../cpp/common.md#iterators)). There are a list of available methods:

??? tip "Next()"
    move iterator to next 5-element construction. Returns `True` if moved to next construction; otherwise - `False`. Example of usage:
    ```python
    while it5.Next():
      ... # do something
    ```

??? tip "Get(idx)"
    * **idx** - `number`  that represents values index. Ыhould be in range `[0; 4]`

    returns `ScAddr` of specified element in current construction. Example of usage:
    ```python
    while it5.Next():
      src = it5.Get(0)      # source element in triple
      edge = it5.Get(1)     # edge in triple
      trg = it5.Get(2)      # target element in triple
      attrEdge = it5.Get(3) # edge from attribute set
      attr = it5.Get(4)     # attribute set
      ...
    ```

??? tip "IsValid()"
    returns `True` if iterator is valid; otherwise - `False`


## ScLinkContent

This class wrap content of link. It allows to unpack it to `string`, `int` or `float`. There are methods of this class:

??? tip "AsString()"
    get content of a link as `string` value.
    ```python
    content = ctx.GetLinkContent(linkAddr)
    stringValue = content.AsString()
    ```

??? tip  "AsInt()"
    get content of a link as `int` value. If length of content not equal to 8 bytes, then `ExceptionInvalidType` will be raised.
    ```python
    content = ctx.GetLinkContent(linkAddr)
    intValue = content.AsInt()
    ```

??? tip "AsFloat()"
    get content of a link as `float` value. If length of content not equal to 8 bytes, then `ExceptionInvalidType` will be raised.
    ```python
    content = ctx.GetLinkContent(linkAddr)
    floatValue = content.AsFloat()
    ```

??? tip "AsBinary()"
    get content of a link as `memoryview` value.
    ```python
    content = ctx.GetLinkContent(linkAddr)
    binaryValue = content.AsBinary()
    ```

    !!! danger 
        Object `content` should be alive until memory used
    
??? tip "GetType()"
    return type of content. There are possible values:

    * `ScLinkContent.String`
    * `ScLinkContent.Int`
    * `ScLinkContent.Float`

## ScTemplateGenParams

This class accumulate parameters for a template generation. There are methods of this class:

??? tip "Add(paramName, valueAddr)"
    * **paramName** - parameter name (`str`)
    * **valueAddr** - `ScAddr` of element that should be used with specified name (see more in [templates description](../cpp/templates.md))

    ```python
    params = ScTemplateGenParams()
    params.Add("_item", itemAddr)
    ...
    ```

??? tip "Get(paramName)"
    * **paramName** - name of parameter (`str`)

    returns value of parameter with a specified name. If parameter with specified identifier exists, then returns it `ScAddr`; otherwise - `None`
    ```python
    addr = params.Get("_item")
    ```

??? tip "IsEmpty()"
    if there are no any parameters added, then returns `True`; otherwise - `False`


## ScTemplateGenResult

This class wrap template generation result. There are methods of this class:

??? tip "Aliases()"
    returns a `dict` of all used aliases in template (where key - `alias`, value - index in generate result). Keys can be used to get result values by `__getitem__`

??? tip "Size()"
    return number of elements

??? tip "\_\_getitem\_\_(alias)"
    * **alias** - name of result parameter (`str`)

    returns `ScAddr` by specified name. If there are no value with a specified name, then returns `None`
    ```python
    addr = genResult["node1"]
    ```

## ScTemplateSearchResultItem

This class represents one result for a search by template. There are methods of this class:

??? tip "Size()"
    return size of result (number of `ScAddr`'s equal to search construction)

??? tip "\_\_getitem\_\_()"
    allows to get result items by any index: `int`, `str`. In case of `int` index you will just get `ScAddr` by index in result array (length equal to `Size()`) this case suitable, when you need to iterate all addrs in result.
    !!! warning 
        You will receive duplicate `ScAddr`'s, because result stored as array of founded triples.

    When you try to get `ScAddr` with `str` it will be found by alias (see [templates](../cpp/templates.md) for more info). If there are no element with specified index, then returns `None`
    ```python
    resultSize = searchResultItem.Size()
    for i in range(resultSize):
      addr = searchResultItem[i] # iterate all addrs

    addr1 = searchResultItem["replacement name"] # get by replacement name
    ```

## ScTemplateSearchResult

This class represent list of results by template search. There are methods of this class:

??? tip "Aliases()"
    returns a `dict` of all used aliases in template (where key - `alias`, value - index in search result). Keys can be used to get result values by `__getitem__`

??? tip "Size()"
    returns number of results.

??? tip "\_\_getitem\_\_()"
    get result by specified index (`int` should be less then `Size()`).
    ```python
    resultNum = searchResult.Size()
    for i in range(resultNum):
      searchResultItem = searchResult[i]

      # work with searchResultItem there see (ScTemplateSearchResult)
    ```

## ScMemoryContext

This class implements context, that allows you to work with memory.
!!! danger
    **DO NOT use same context in different threads**
There are methods of this class:

??? tip "CreateNode(type)"
    * **type** - `ScType` of a node

    create node with specified type. Returns `ScAddr` of created node. If returned `ScAddr` is not valid (`IsValid()`), then node wasn't created.
    ```python
    nodeAddr = ctx.CreateNode(ScType.NodeConst)
    ```
??? tip "CreateEdge(type, src, trg)"
    * **type** - `ScType` of edge
    * **src** - `ScAddr` of source edge element
    * **trg** - `ScAddr` of target edge element

    create edge between `src` and `trg` elements. Returns `ScAddr` of created edge, but if returned `ScAddr` is not valid, then edge wasn't created.

    **Example:**
    ```python
    edgeAddr = ctx.CreateEdge(ScType.EdgeAccessConstPosPerm, srcAddr, trgAddr)
    ```

??? tip "CreateLink()"
    create link. Returns `ScAddr` of created link, but if returned `ScAddr` is not valid, then link wasn't created

    **Example:**
    ```python
    linkAddr = ctx.CreateLink()
    ```

??? tip "GetName()"
    returns name of context. Useful in debug purposes

??? tip "IsElement(addr)"
    * **addr** - `ScAddr` of element to check
    
    if element exist, then returns `True`; otherwise - `False`

    **Example:**
    ```python
    ctx.IsElement(elementAddr)
    ```

??? tip "GetElementType(addr)"
    * **addr** - `ScAddr` of element to get type

    returns type of specified element. If element doesn't exist, then returns `ScType.Unknown`

    **Example:**
    ```python
    t = ctx.GetElementType(elementAddr)
    ```

??? tip "GetEdgeInfo(addr)"
    * **addr** - `ScAddr` of edge
    
    returns tuple `(src, trg)`, where:

    * `src` - is a `ScAddr` of edge source element;
    * `trg` - target element.
    
    If `addr` point to element that doesn't exist, or is not an edge, then returns `(None, None)`.

    **Example:**
    ```python
    src, trg = ctx.GetEdgeInfo(edgeAddr)
    ```

??? tip "SetLinkContent(addr, content)"
    * **addr** - `ScAddr` of sc-link to set content
    * **content** - content of sc-link, that should be set. Type of `content` should be one of: `int`, `float`, `string`.
    
    Change content of sc-link. If content changed, then returns `True`; otherwise - `False`

    **Example:**
    ```python

    ...
    ctx.SetLinkContent(linkAddr1, 56)
    ...
    ctx.SetLinkContent(linkAddr1, 56.0)
    ...
    ctx.SetLinkContent(linkAddr1, "any text")
    ...

    ```

??? tip "GetLinkContent(addr)"
    * **addr** - `ScAddr` of sc-link
    
    returns content of a specified link. If specified `addr` is not a link, or it doesn't exist, then returns `None`. Returned value has type `ScLinkContent`.

    **Example:**
    ```python
    ...

    value = ctx.GetLinkContent(elementAddr)
    if value:
      print (value)

    ...
    ```

??? tip "Iterator3(param1, param2, param3)"
    * **param1**, **param2**, **param3** - could be on of a type: `ScAddr`, `ScType`

    create iterator for a specified triple template. For more information about iterators see [iterators description](../cpp/common.md#iterators). If iterator created, then return `ScIterator3` object; otherwise - `None`

    **Example:**
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

??? tip "Iterator5(param1, param2, param3, param4, param5)"
    * **param1**, **param2**, **param3**, **param4**, **param5** - could be on of a type `ScAddr`, `ScType`

    create iterator for a specified 5-element construction. For more information about iterators see [iterators description](../cpp/common.md#iterators). If iterator created, then return `ScIterator5` object; otherwise - `None`

    **Example:**
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

??? tip "HelperResolveSystemIdtf(idtf, type)"
    * **idtf** - `str` that represents a system identifier of sc-element
    * **type** - `ScType` of sc-element

    resolve element by system identifier. This function tries to find element with specified system identifier - `idtf`. If element wasn't found and type is valid (`ScType.IsValid()`), then new element will be created with specified system identifier and type. If `type` is `None`, then new element wouldn't created and function returns invalid `ScAddr`. In other cases function returns `ScAddr` of element with specified system identifier.

    **Example:**
    ```python
    addr = ctx.HelperResolveSystemIdtf("nrel_main_idtf", ScType.NodeConstNoRole)
    ```

??? tip "HelperSetSystemIdtf(idtf, addr)"
    * **idtf** - `str` new identifier of sc-element
    * **addr** - `ScAddr` of element

    set specified system identifier - `idtf`, to element with specified `addr`. If identifier changed, then returns `True`; otherwise - `False`

    **Example:**
    ```python
    if ctx.HelperSetSystemIdtf("new_idtf", addr):
      pass # identifier changed
    else:
      pass # identifier not changed
    ```

??? tip "HelperGetSystemIdtf(addr)"
    * **addr** - `ScAddr` of sc-element to get identifier

    returns system identifier of a specified element. If system identifier exist, then it returns as a non empty string; otherwise result string would be empty.

    **Example:**
    ```python
    idtfValue = ctx.HelperGetSystemIdtf(addr)
    ```

??? tip "HelperCheckEdge(srcAddr, trgAddr, edgeType)"
    * **srcAddr** - `ScAddr` of source edge sc-element
    * **trgAddr** - `ScAddr` of target edge sc-element
    * **edgeType** - `ScType` of edge to check

    if there are one or more edge with a specified type (`edgeType`) between `srcAddr` and `trgAddr` elements, then returns `True`; otherwise - `False`

    **Example:**
    ```python
    if ctx.HelperCheckEdge(addr1, addr2, ScType.EdgeAccessConstPosPerm):
      pass # edge between addr1 and addr2 exist
    else:
      pass # edge doesn't exists
    ```

??? tip "HelperGenTemplate(templ, params)"
    * **templ** - `ScTemplate` to generate construction
    * **params** - `ScTemplateGenParams` parameters for construction generation

    generates construction by specified template with specified parameters. If construction generated, then returns instance of `ScTemplateGenResult`; otherwise - `None`.

    **Example:**
    ```python
    templ = ScTemplate()
    ... # fill template

    params = ScTemplateGenParams()
    ... # fill parameters

    result = ctx.HelperGenTemplate(templ, params)
    ```

??? tip "HelperSearchTemplate(templ)"
    * **templ** - `ScTemplate` to search construction

    searches construction by specified template. Returns `ScTemplateSearchResult` object instance. If it `Size()` equal to 0, then nothing was found

    **Example:**
    ```python
    templ = ScTemplate()
    ... # fill template

    result = ctx.HelperSearchTemplate(templ)
    ```

??? tip "HelperBuildTemplate(data)"
    * **data** - `ScAddr` or `str`. If it's a `ScAddr` then it should point to sc-struct in memory, that is a template. It it's a `str`, then it should contains `SCs-code` that describes template

    returns `ScTemplate` instance. If template wasn't built, then return `None`

    **Example:*
    ```python
    templFromMemory = ctx.HelperBuildTemplate(templAddr)
    templFromStr = ctx.HelperBuildTemplate('person _=> nrel_email:: _[];;')
    ... # work with template
    ```

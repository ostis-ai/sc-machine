Python library contains python implementation of useful classes and functions to work with sc-memory.

There are a list of classes:
 
 * [ScAgent](#scagent)
 * [ScKeynodes](#sckeynodes)
 * [ScHelper](#schelper)
 * [ScSet](#scset)

## ScAgent

## ScKeynodes

Object that implements caching of keynodes. This objects cache requested `ScAddr`'s so you can access it when you need from any places. Create it with such parameters:

* **ctx** - [`ScMemoryContext`](cpp_wrap#scmemorycontext) that will be used to access sc-memory

---

**Methods**

??? tip "\_\_getitem\_\_(sys_idtf)"
    * **sys_idtf** - system identifier of keynode

    returns `ScAddr` of keynodes with specified system identifier. If keynode doesn't exist, then returns invalid `ScAddr`

    **Example:**
    ```python
    keynodes = ScKeynodes(ctx)
    addr1 = keynodes["keynode_idtf"]    # will find it in sc-memory
    addr2 = keynodes["keynode_idtf_2"]  # will find it in sc-memory
    addr3 = keynodes["keynode_idtf"]    # will return cached value equal to addr1
    ```

## ScHelper

Object that wrap some common functions to work with knowledge base.
You can create it with such parameters:

* **ctx** - [`ScMemoryContext`](cpp_wrap#scmemorycontext) that will be used to access sc-memory

---
** Methods **

??? tip "kbSetRelationLinkValue(`_addr`, `_relAddr`, `_value`)"
    * **_addr** - `ScAddr` of sc-element to change value by relation
    * **_relAddr** - `ScAddr` of relation node
    * **_value** - `int | float | str` new value of sc-link

    Set value of sc-link connected to `_addr` with `_relAddr` relation:
    
    <scg src="../../images/python/sc_helper_kbSetRelationLinkValue.gwf"></scg>

    If there are not value linked with `_addr` by relation `_relAddr`, then this function create it; otherwise just change value of sc-link to new one.

??? tip "kbGetRelationLinkValue(`_addr`, `_relAddr`)"
    * **_addr** - `ScAddr` of sc-element to get value by relation
    * **_relAddr** - `ScAddr` of relation node
    
    Return `ScLinkContent` value of sc-link by template:
    <scg src="../../images/python/sc_helper_kbSetRelationLinkValue.gwf"></scg>

    If there are no such construction, then returns `None`.

??? tip "kbReplaceBinaryRelation(`_addr`, `_relAddr`, `_newTarget`)"
    * **_addr** - `ScAddr`
    * **_relAddr** - `ScAddr`
    * **_newTarget** - `ScAddr`

    Remove any construction that corresponds to template:
    
    <scg src="../../images/python/sc_helper_kbReplaceBinaryRelation_1.gwf"></scg>

    After that new construction will be created with template:
    <scg src="../../images/python/sc_helper_kbReplaceBinaryRelation_2.gwf"></scg>

    Returns `True`, when new relation created; otherwise - `False`.


## ScSet

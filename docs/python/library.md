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

??? tip "kbSetBinaryRelationLinkValue(`_addr`, `_relAddr`, `_value`)"
    * **_addr** - `ScAddr` of sc-element to change value by relation
    * **_relAddr** - `ScAddr` of relation node
    * **_value** - `int | float | str` new value of sc-link

    Set value of sc-link connected to `_addr` with `_relAddr` relation:
    
    ```scs
    _addr _=> _relAddr:
      [value];;
    ```

    If there are not value linked with `_addr` by relation `_relAddr`, then this function create it; otherwise just change value of sc-link to new one.


??? tip "kbGetBinaryRelationLinkValue(`_addr`, `_relAddr`)"
    * **_addr** - `ScAddr` of sc-element to get value by relation
    * **_relAddr** - `ScAddr` of relation node
    
    Return `ScLinkContent` value of sc-link by template:
    ```scs
    _addr _=> _relAddr: 
      [value];;
    ```    

    If there are no such construction, then returns `None`.

??? tip "kbReplaceBinaryRelation(`_addr`, `_relAddr`, `_newTarget`)"
    * **_addr** - `ScAddr`
    * **_relAddr** - `ScAddr`
    * **_newTarget** - `ScAddr`

    Remove any construction that corresponds to template:
    
    ```scs
    _addr _=> _relAddr: 
      _...;;
    ```

    After that new construction will be created with template:
    
    ```scs
    _addr _=> _relAddr: 
      _newTarget;;
    ```

    Returns `True`, when new relation created; otherwise - `False`.

??? tip "kbUpdateStructureValues(`_addr`, `_values`)"
    * **_addr** - `ScAddr` of structure to update
    * **_values** - list of tuples: `(_rrel_addr, _new_value)`. Where `_new_value` can be `ScAddr`, `str`, `float`, `int`. In case, when `_new_value` is **not** a `ScAddr` `ScLink` will be used with a content that equal to `_new_value`

    This function setup values for each item in a tuple `_values` by template:
    ```scs
    _addr _-> _rrel_addr:: _new_value;;
    ```

    Example code:
    ```python
    
    helper.kbUpdateStructureValues(structAddr, [
      (rrel_name, 'name'),
      (rrel_height, 178),
      (rrel_mass, 79.6),
      (rrel_other_addr, any_addr)
    ])
    ```

    will produce such sc-construction:
    ```scs
    structAddr
      -> rrel_name: [name];
      -> rrel_height: [178];
      -> rrel_mass: [79.6];
      -> rrel_other_addr: any_addr;;
    ```

    **If any of field, already exist, then it would be replaced by a new value.**

## ScSet

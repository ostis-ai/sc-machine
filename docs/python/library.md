Python library contains python implementation of useful classes and functions to work with sc-memory.

There are a list of classes:
 
 * [ScAgent](#scagent)
 * [ScKeynodes](#sckeynodes)
 * [ScHelper](#schelper)
 * [ScSet](#scset)

## ScAgent

Object that implements `ScAgent` behaviour. This is a smart wrapper on event listener. It runs code on registered event emit. Create it with such parameters:

* **module** - [`ScModule`](#scmodule)

---

***Methods***

??? tip "Register(addr, evt_type)"
    * **addr** - `ScAddr` of element to listen events
    * **evt_type** - `ScPythonEventType` type of event to listen

    Register `ScAgent` to run on spcified event emit.

    **Example:**
    ```python
    agent = MyScAgent()
    agent.Register(self.keynodes[ScAgent.kCmdInitiated], ScPythonEventType.AddOutputEdge)
    ```

??? tip "Unregister()"
    Unregister `ScAgent` from previously registered event

??? abstract "CheckImpl(evt)"
    * **evt** - [`ScEventParams`](#sceventparams) structure that describes emited event

    This method calls after event emit to check if `ScAgent` should run on this event.

    You can override this method to implement your own check. It should return `True` when check passed; otherwise - `False`.

    Default implementation returns `True`.

    **Example:**
    ```python
    class MyScAgent(ScAgent):

      def CheckImpl(self, evt):
        # do extra check there
        return ...
    ```

??? abstract "RunImpl(evt)"
    * **evt** - [`ScEventParams`](#sceventparams) structure that describes emited event

    This method should be everriden. It calls after passed `CheckImpl` returns `True`. Whole logic of agent should be implemented there.

## ScAgentCommand

Object that implement `ScAgent` logic to work with a commands. It built on top of `ScAgent` object, but **you should not override RunImpl of this one**. Instead of that you should write your custom logic in overriden `DoCommand` method.

To create this object you should provide two parameters:

* **module** - [`ScModule`](#scmodule)
* **cmd_class_addr** - `ScAddr` of command class

---

**Methods**

??? abstract "DoCommand()"

    Returns command process result `ScResult`.

    You can access command and result addrs in this function with
    `self.cmd_addr` and `self.result_set`

**StaticMethods**

??? tip "CreateCommand(ctx, cmd_class_addr, params)"

    * **ctx** - [`ScMemoryContext`](/python/cpp_wrap/#scmemorycontext) that will be used to access sc-memory
    * **cmd_class_addr** - `ScAddr` of command class
    * **params** - `ScAddr[]` array of parameters for a command

    Create instance of a specified command class with a parameters and returns `ScAddr` of generated one.

    **Example:**
    ```python
    ScAgentCommand.CreateCommand(my_cmd_class, [param_1, param_2])
    ```

    will generate such command in `ScMemory`

    ```scs
    cmd_instance
      <- my_cmd_class;
      -> rrel_1: param_1;
      -> rrel_2: param_2;;
    ```

??? tip "RunCommand(ctx, cmd_addr)"

    * **ctx** - [`ScMemoryContext`](/python/cpp_wrap/#scmemorycontext) that will be used to access sc-memory
    * **cmd_addr** - `ScAddr` of command to run

    Returns `True` on command run; otherwise - `False`.

    **Improtant:** this function **doesn't block** thread. It runs command asynchronously.

    **Example:**
    ```python
    cmd = ScAgentCommand.Create(my_cmd_class, [param_1, param_2])
    if ScAgentCommand.RunCommand(cmd):
      print("Command run")
    else:
      print("Can't run command")
    ```

??? tip "RunCommandWait(ctx, cmd_addr, wait_timeout_ms)"

    * **ctx** - [`ScMemoryContext`](/python/cpp_wrap/#scmemorycontext) that will be used to access sc-memory
    * **cmd_addr** - `ScAddr` of command to run
    * **wait_timeout_ms** - wait timeout in milliseconds

    Returns `True` on command run and finished in specified timeout; otherwise - `False`.

    **Improtant:** this function **block** thread until command finishes. It runs comman synchronous.

    **Example:**
    ```python
    cmd = ScAgentCommand.Create(my_cmd_class, [param_1, param_2])
    if ScAgentCommand.RunCommandWait(cmd):
      print("Command run and finished")
    else:
      print("Can't run command")
    ```

??? tip "GetCommandResultAddr(ctx, cmd_addr)"
    * **ctx** - [`ScMemoryContext`](/python/cpp_wrap/#scmemorycontext) that will be used to access sc-memory
    * **cmd_addr** - `ScAddr` of command

    Returns `ScAddr` of result structure for a specified comand. If there are no result structure, then returns empty `ScAddr`


## ScEventParams

## ScHelper

Object that wrap some common functions to work with knowledge base.
You can create it with such parameters:

* **ctx** - [`ScMemoryContext`](/python/cpp_wrap/#scmemorycontext) that will be used to access sc-memory

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

## ScKeynodes

Object that implements caching of keynodes. This objects cache requested `ScAddr`'s so you can access it when you need from any places. Create it with such parameters:

* **ctx** - [`ScMemoryContext`](/python/cpp_wrap/#scmemorycontext) that will be used to access sc-memory

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

## ScModule



## ScSet

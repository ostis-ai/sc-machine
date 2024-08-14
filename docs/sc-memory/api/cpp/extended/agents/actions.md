# **C++ Actions API**

!!! warning
    This documentation is correct for only versions of sc-machine that >= 0.10.0.
--- 

This API provides functionality to handle actions on C++.

## **What is action?**

All actions are processes. Any process is performed by some subject. Each action performed can be interpreted as a process of solving some task, i.e. as a process of achieving the given goal with the given conditions. Each action denotes some transformation carried out in the external environment or in the memory of some system.

Like an agent, an action has a specification. This specification includes: class, arguments, state, result and others. The specification of an action is called a problem (task).

## **ScAction**

The sc-machine provides `ScAction` class to handle actions in sc-memory. You can't call constructors of this class, because they are private. Use `ScAgentContext` to create action with provided class or use the existed one.

```cpp
// Find action class and create action.
...
ScAction action = context.CreateAction(actionClassAddr);
...
```

```cpp
// Or find action and use it.
...
ScAction action = context.UseAction(actionAddr);
...
```

### **GetClass**

To get class of action use this method.

```cpp
...
ScAddr const & actionClassAddr = action.GetClass();
...
```

### **GetArgument**

Actions can have arguments. Action arguments are some objects on which this action should be performed. If action has arguments in knowledge base, then you will get them. There are a couple of getters for this.

```cpp
...
// Provide relation here if your argument has specific role in action. 
ScAddr const & argAddr = action.GetArgument(ScKeynodes::rrel_1);
// If there is no argument with such role, then `argAddr` will be empty.
...
```

```cpp
...
// Or provide index of argument. 1 is equal to `ScKeynodes::rrel_1`,
// 2 is equal to `ScKeynodes::rrel_2` and etc.
ScAddr const & argAddr = action.GetArgument(1);
// If there is no argument with such role, then `argAddr` will be empty.
...
```

!!! warning
    This method will throw `utils::ExceptionInvalidParams` if you provide invalid index for argument (for example, 0).

```cpp
...
ScAddr const & argAddr = action.GetArgument(1, defaultArgumentAddr);
// If there is no argument with such role, then `argAddr` will equal 
// to `defaultArgumentAddr`.
...
```

### **GetArguments**

You can get all arguments using one getters.

```cpp
...
// Here, 2 is number arguments in specified action.
auto const & [argAddr1, argAddr2] = action.GetArguments<2>();
// If action doesn't have some argument, it will be empty.
...
```

```cpp
...
// You can't ignore some arguments.
auto const & [argAddr1, argAddr2, _] = action.GetArguments<3>();
...
```

!!! note
    `GetArguments` uses `GetArgument` for each argument.

### **SetArgument**

You can set arguments for specified action.

```cpp
...
// Provide relation here if your argument should have specific role in action. 
action.SetArgument(ScKeynodes::rrel_1, argAddr);
...
```

```cpp
...
// Or provide index of argument. 1 is equal to `ScKeynodes::rrel_1`,
// 2 is equal to `ScKeynodes::rrel_2` and etc.
action.SetArgument(1, argAddr);
...
```

!!! note
    If action has already argument with specified role, then connection between action and this argument will be removed and created new one between action and new argument.

### **SetArguments**

```cpp
...
// Set several arguments simultaneously.
action.SetArguments(argAddr1, argAddr2);
...
```

!!! note
    `SetArguments` uses `SetArgument` for each argument.

### **Action result**

All action should have result (result situation). Result situation is structure that contains all sc-constructions that which indicate result of interpreting action.

#### **GetResult**

You can result for any actions.

```cpp
...
// Use this method after that some agent finished 
// interpreting action.
ScStructure const & actionResult = action.GetResult();
...
```

!!! warning
    You can call this method for finished action only.

#### **SetResult**

```cpp
...
// Use this method in agent interpreting action 
// to set new result.
action.SetResult(resultStructure);
...
```

!!! note
    If action has result, then will be removed and the new one will be set.

!!! warning
    You can call this method for not finished, but initiated action only.

#### **FormResult**

You must not create result structure. You can provide only elements of result for action.

```cpp
...
// Use this method in agent interpreting action 
// to form new result with provided sc-elements.
action.FormResult(elementAddr1, elementAddr2);
...
```

!!! note
    If action has result, then will be removed and the new one will be set.

#### **UpdateResult**

```cpp
...
// Use this method in agent interpreting action
// for update existing result by new sc-elements.
action.UpdateResult(elementAddr1, elementAddr2);
...
```

!!! note
    This method updates existing result for action.

!!! note
    `FormResult` and `UpdateResult` doesn't append sc-element twice.

!!! note
    If you don't form result then empty result for your action will be generated if you finish action.

### **Action states**

All actions have state. There are three states of actions provided by this API:

* action isn't initiated;
* action is initiated, but isn't finished, that is, action is performed;
* action is finished.

You can initiate, wait or finish actions.

#### **IsInitiated**

Use this method to check that specified action is initiated.

```cpp
...
sc_bool const isActionInitiated = action.IsInitiated();
...
```

#### **InitiateAndWait**

You can initiate action and wait while it will be finished.

```cpp
...
// Provide maximum time of waiting while action will be finished.
action.InitiateAndWait(100); // milliseconds
// This argument has default value, that equals to 5000 milliseconds.
...
```

#### **Initiate**

Or you can initiate and not wait while it will be finished.

```cpp
...
action.Initiate();
...
```

#### **IsFinished**

Use this method to check that specified action is finished.

```cpp
...
sc_bool const isActionFinished = action.IsFinished();
...
```

All finished actions should be:

* finished successfully;
* finished unsuccessfully;
* finished with error.

#### **IsFinishedSuccessfully**

The set of actions finished successfully includes actions that have been successfully completed from the point of view of subject who performed them, i.e., the goal has been achieved, for example, the solution and result to a problem have been obtained, a construction has been successfully transformed, etc.

```cpp
...
sc_bool const isActionFinishedSuccessfully = action.IsFinishedSuccessfully();
...
```

#### **FinishSuccessfully**

You can finish successfully action that not finished yet.

```cpp
...
ScResult const & result = action.FinishSuccessfully();
// Use result to return result from agent program.
...
```

#### **IsFinishedUnsuccessfully**

The set of actions finished unsuccessfully includes actions that were not successfully finished from the point of view of subject who performed them for some reasons. There are two main reasons why this situation may occur:

* corresponding problem is formulated incorrectly;
* formulation of corresponding problem is correct and understandable to the system, but solution of this problem at the current moment cannot be obtained in terms satisfactory from the point of view of executor.

```cpp
...
sc_bool const isActionFinishedUnsuccessfully = action.IsFinishedUnsuccessfully();
...
```

!!! warning
    You can't finish successfully action that finished or not initiated.

#### **FinishUnsuccessfully**

You can finish unsuccessfully action that not finished yet.

```cpp
...
ScResult const & result = action.FinishUnsuccessfully();
// Use result to return result from agent program.
...
```

!!! warning
    You can't finish unsuccessfully action that finished or not initiated.

#### **IsFinishedWithError**

The set of actions finished with error includes actions whose execution was not successfully finished from the point of view of subject who executed them, due to some error, such as incorrect specification of this action or violation of sc-memory integrity by some subject.

```cpp
...
sc_bool const isActionFinishedWithError = action.IsFinishedWithError();
...
```

#### **FinishWithError**

You can finish action with error that not finished yet.

```cpp
...
ScResult const & result = action.FinishWithError();
// Use result to return result from agent program.
...
```

!!! warning
    You can't finish action with error that finished or not initiated.


All these methods return object of `ScResult`. You should return it in agent program. You can't call constructor of `ScResult` to create new object.

--- 

## **Frequently Asked Questions**

<!-- no toc -->
- [What is difference between `ScAction` and `ScEvent`?](#what-is-difference-between-scaction-and-scevent)

### **What is difference between `ScAction` and `ScEvent`?**

`ScAction` is a class that represents sc-action. A sc-action is a process performed by some entity to accomplish specified problem (task). `ScEvent` represents a sc-event. A sc-event is a connection between process and its initial and result situation. Actions are created after the occurrence of some sc-event and actions are interpreted by agents. Emergence of events in sc-memory leads to generation of new processes.

# **C++ Actions API**

!!! note
    This is correct for only versions of sc-machine that >= 0.10.0.
--- 

This API provides functionality to handle sc-actions.

## **What is action?**

All actions are processes. Any process is performed by some subject. Each action can be defined as a process of solving some problem, i.e. as a process of achieving the given goal with the given conditions. Each action denotes some transformation carried out in the external environment or in the memory of some system.

Like an agent, an action has a specification. This specification includes: class, arguments, state, result. The specification of an action is called a problem.

## **ScAction**

The sc-machine provides `ScAction` class to handle actions in sc-memory. You can't use constructors of this class, because they are private. Use `ScAgentContext` to create action with provided class or use the existing one. All methods of this class work directly with the knowledge base, there is no local cache.

```cpp
// Find action class and create action.
...
ScAction action = context.CreateAction(actionClassAddr);
...
```

```cpp
// Or find action and convert it to object of `ScAction` class.
...
ScAction action = context.ConvertToAction(actionAddr);
...
```

<scg src="../images/actions/action_class.gwf"></scg>

### **GetClass**

To get class of action use this method.

```cpp
...
ScAddr const & actionClassAddr = action.GetClass();
...
```

### **GetArgument**

Actions can have arguments. Action arguments are some objects using which this action should be performed. If action has arguments in knowledge base, then you will get them. There are a couple of getters for this.

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

<scg src="../images/actions/action_argument.gwf"></scg>

!!! note
    There is a limit to the number of order role relations. You can get from `rrel_1` up to and including `rrel_20`.

!!! warning
    This method will throw `utils::ExceptionInvalidParams` if you provide invalid index for argument (for example, 0 or 21).

```cpp
...
ScAddr const & argAddr = action.GetArgument(1, defaultArgumentAddr);
// If there is no argument with such role, then `argAddr` will equal 
// to `defaultArgumentAddr`.
...
```

### **GetArguments**

You can get all arguments using one getter.

```cpp
...
// Here, 2 is number of arguments in specified action.
auto const & [argAddr1, argAddr2] = action.GetArguments<2>();
// If action doesn't have some argument, it will be empty.
...
```

```cpp
...
// You can ignore some arguments.
auto const & [argAddr1, argAddr2, _] = action.GetArguments<3>();
...
```

<scg src="../images/actions/action_arguments.gwf"></scg>

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
// 2 is equal to `ScKeynodes::rrel_2`, etc.
action.SetArgument(1, argAddr);
...
```

<scg src="../images/actions/action_argument.gwf"></scg>

!!! note
    If action already has an argument with specified role, then connection between action and existing argument will be removed and connection between action and new argument will be created.

### **SetArguments**

```cpp
...
// Set several arguments simultaneously.
action.SetArguments(argAddr1, argAddr2);
...
```

<scg src="../images/actions/action_arguments.gwf"></scg>

### **Action result**

All actions should have result (result situation). Result situation is structure that contains all sc-constructions that indicate result of performed (finished) action.

#### **GetResult**

You get can result of any action. Result may be empty.

```cpp
...
// Use this method after some agent finishes 
// performing action.
ScStructure const & actionResult = action.GetResult();
...
```

<scg src="../images/actions/action_result.gwf"></scg>

!!! warning
    If you call this method for not finished action, then this method will throw `utils::ExceptionInvalidState`. It prevents a situation where an agent that performs an action is still forming a result for that action, and you haven't waited for it and already want to get result for that action.

#### **SetResult**

```cpp
...
// Use this method in agent performing action 
// to set new result.
action.SetResult(resultStructure);
...
```

!!! note
    If action has result, then existing result will be removed and the new one will be set.

!!! warning
    If you call this method for not finished, but initiated action only, the this method will throw `utils::ExceptionInvalidState`.

#### **FormResult**

You may not create result structure. You can provide only elements of result for action.

```cpp
...
// Use this method in agent performing action 
// to form new result with provided sc-elements.
action.FormResult(elementAddr1, elementAddr2);
...
```

!!! note
    If action has result, then existing result will be removed and the new one will be set.

#### **UpdateResult**

```cpp
...
// Use this method in agent performing action
// for update existing result by new sc-elements.
action.UpdateResult(elementAddr1, elementAddr2);
...
```

!!! note
    This method updates existing result for action.

!!! note
    `FormResult` and `UpdateResult` don't append sc-element twice.

!!! note
    If you don't form result then empty result for your action will be generated if you finish action.

### **Action states**

All actions have state. There are three states of actions provided by this API:

* action isn't initiated;
* action is initiated, but isn't finished (that is, action is being performed);
* action is finished.

You can initiate, wait or finish actions.

#### **IsInitiated**

Use this method to check that specified action is initiated.

```cpp
...
bool const isActionInitiated = action.IsInitiated();
...
```

<scg src="../images/actions/is_action_initiated.gwf"></scg>

#### **InitiateAndWait**

You can initiate action and wait for it to finish.

```cpp
...
// Provide maximum time of waiting while action will be finished.
action.InitiateAndWait(100); // milliseconds
// This argument has default value, that equals to 5000 milliseconds.
...
```

#### **Initiate**

Or you can initiate and not wait for it to finish.

```cpp
...
action.Initiate();
...
```

<scg src="../images/actions/action_initiated.gwf"></scg>

#### **IsFinished**

Use this method to check that specified action is finished.

```cpp
...
bool const isActionFinished = action.IsFinished();
...
```

<scg src="../images/actions/is_action_finished.gwf"></scg>

All finished actions should be finished with one of the following statuses:

* finished successfully;
* finished unsuccessfully;
* finished with error.

#### **IsFinishedSuccessfully**

The set of actions finished successfully includes actions that have been successfully completed from the point of view of subject who performed them, i.e., the goal has been achieved, for example, the solution and result to a problem have been obtained, a construction has been successfully transformed, etc.

```cpp
...
bool const isActionFinishedSuccessfully = action.IsFinishedSuccessfully();
...
```

<scg src="../images/actions/is_action_finished_successfully.gwf"></scg>

#### **FinishSuccessfully**

You can successfully finish action that does not have yet been finished.

```cpp
...
ScResult const & result = action.FinishSuccessfully();
// Use result to return result from agent program.
...
```

<scg src="../images/actions/action_finished_successfully.gwf"></scg>

!!! warning
    If you finish action successfully that is finished or not initiated, then this method will throw `utils::ExceptionInvalidState`.

#### **IsFinishedUnsuccessfully**

The set of actions finished unsuccessfully includes actions that were not successfully finished from the point of view of subject who performed them for some reason. There are two main reasons why this situation may occur:

* corresponding problem is formulated incorrectly;
* formulation of corresponding problem is correct and understandable to the system, but solution of this problem at the current moment cannot be obtained in terms satisfactory from the point of view of executor.

```cpp
...
bool const isActionFinishedUnsuccessfully = action.IsFinishedUnsuccessfully();
...
```

<scg src="../images/actions/is_action_finished_unsuccessfully.gwf"></scg>

#### **FinishUnsuccessfully**

You can finish unsuccessfully action that not finished yet.

```cpp
...
ScResult const & result = action.FinishUnsuccessfully();
// Use result to return result from agent program.
...
```

<scg src="../images/actions/action_finished_unsuccessfully.gwf"></scg>

!!! warning
    If you finish action unsuccessfully that is finished or not initiated, then this method will throw `utils::ExceptionInvalidState`.

#### **IsFinishedWithError**

The set of actions finished with error includes actions whose execution was not successfully finished from the point of view of subject who executed them, because to some error, such as incorrect specification of this action or violation of sc-memory integrity by some subject.

```cpp
...
bool const isActionFinishedWithError = action.IsFinishedWithError();
...
```

<scg src="../images/actions/is_action_finished_with_error.gwf"></scg>

#### **FinishWithError**

You can finish action with error that not finished yet.

```cpp
...
ScResult const & result = action.FinishWithError();
// Use result to return result from agent program.
...
```

<scg src="../images/actions/action_finished_with_error.gwf"></scg>

!!! warning
    If you finish action with error that is finished or not initiated, then this method will throw `utils::ExceptionInvalidState`.


All these methods return object of `ScResult`. You should return it in agent program. You can't call constructor of `ScResult` to create new object.

--- 

## **Frequently Asked Questions**

<!-- no toc -->
- [What is difference between `ScAction` and `ScEvent`?](#what-is-difference-between-scaction-and-scevent)
- [What if I want to set some edge as action result and not structure with this edge?](#what-if-i-want-to-set-some-edge-as-action-result-and-not-structure-with-this-edge)
- [What's the difference between ScSet and ScStructure?](#what-is-difference-between-scaction-and-scevent)

### **What is difference between `ScAction` and `ScEvent`?**

`ScAction` is a class that represents sc-action. A sc-action is a process performed by some entity to solve specified problem. `ScEvent` represents a sc-event. A sc-event is a connection between process and its initial and result situation. Actions are created after the occurrence of some sc-event and actions are performed by agents. Emergence of events in sc-memory leads to generation of new processes.

### **What if I want to set some edge as action result and not structure with this edge?**

You're not allowed to do this. Action result should be a atomic formula (statement, situation or structure). Action result describes how action was performed. In the future, result can be non-atomic logical formula.

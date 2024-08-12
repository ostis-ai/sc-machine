# **C++ Actions API**

!!! warning
    This documentation is correct for only versions of sc-machine that >= 0.10.0.
--- 

This API provides functionality to handle actions on C++.

## **What is action?**

All actions are processes. Any process is performed by some subject. Each action performed can be interpreted as a process of solving some task, i.e. as a process of achieving the given goal with the given conditions. Each action denotes some transformation carried out in the external environment or in the memory of some system.

Like an agent, an action has a specification. This specification includes: class, arguments, finish state and others. The specification of an action is called a problem (task).

## **ScAction**

The sc-machine provides `ScAction` class to handle actions in sc-memory. You can't call constructors of this class, because they are private. Use `ScAgentContext` to create action with provided class or use the existed one.

```cpp
// Find action class and create action.
...
ScAction const & action = action.CreateAction(actionClassAddr);
...
```

```cpp
// Or find action and use it.
...
ScAction const & action = action.UseAction(actionAddr);
...
```

### **GetClass**

To get class of action use this method.

```cpp
...
ScAddr const & actionClassAddr = action.GetClass();
...
```

If action has arguments in knowledge base, then you will get them. There are a couple of getters for this.

### **GetArgument**

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
// Use 
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

### **SetArgument**

```cpp
...
action.SetArgument(ScKeynodes::rrel_1, argAddr);
...
```

```cpp
...
action.SetArgument(1, argAddr);
...
```

### **SetArguments**

```cpp
...
action.SetArguments(argAddr1, argAddr2);
...
```

### **GetResult**

```cpp
...
ScStruct const & actionResult = action.GetResult();
...
```

### **SetResult**

```cpp
...
action.SetResult(resultStruct);
...
```

### **FormResult**

```cpp
...
action.FormResult(elementAddr1, elementAddr2);
...
```

### **UpdateResult**

```cpp
...
action.UpdateResult(elementAddr1, elementAddr2);
...
```

### **IsInitiated**

```cpp
...
sc_bool const isActionInitiated = action.IsInitiated();
...
```

### **InitiateAndWait**

```cpp
...
action.InitiateAndWait(100);
...
```

### **Initiate**

```cpp
...
action.Initiate();
...
```

### **IsFinished**

```cpp
...
sc_bool const isActionFinished = action.IsFinished();
...
```

### **IsFinishedSuccessfully**

```cpp
...
sc_bool const isActionFinishedSuccessfully = action.IsFinishedSuccessfully();
...
```

### **FinishSuccessfully**

```cpp
...
ScResult const & result = action.FinishSuccessfully();
...
```

### **IsFinishedUnsuccessfully**

```cpp
...
sc_bool const isActionFinishedUnsuccessfully = action.IsFinishedUnsuccessfully();
...
```

### **FinishUnsuccessfully**

```cpp
...
ScResult const & result = action.FinishUnsuccessfully();
...
```

### **IsFinishedWithError**

```cpp
...
sc_bool const isActionFinishedWithError = action.IsFinishedWithError();
...
```

### **FinishWithError**

```cpp
...
ScResult const & result = action.FinishWithError();
...
```

--- 

## **Frequently Asked Questions**

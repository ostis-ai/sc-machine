# **User permissions API**

!!! note
    This documentation is correct for only versions of sc-machine that >= 0.10.0.
---

This API allows to handle users and their permissions in knowledge base.

## **Who are users of ostis-systems?**

Users interact with ostis-systems. Users can be other systems, agents or people. All users of an ostis-system can perform actions on its knowledge base. There are six classes of user actions in knowledge base:

* action_read_from_sc_memory;
* action_generate_in_sc_memory;
* action_erase_from_sc_memory;
* action_read_permissions_from_sc_memory;
* action_generate_permissions_in_sc_memory;
* action_erase_permissions_from_sc_memory.

Before a user action is performed, it is checked whether the user has permissions to perform actions of the specified class.

By default, user permissions are not handled by sc-machine. This is configured via the sc-machine config (`<config-name>.ini`). To enable user permissions handling in the sc-machine, go to the group `[sc-memory]` and set `user_mode` as `true`.

```ini
[sc-memory]
...
user_mode = true
...
```

## **How are users managed?**

You can only work with sc-memory through methods provided in `ScMemoryContext` class. Each object of this class can be considered as an object that handles information about a user (including his permissions) when that user invokes methods through that object.

To work with sc-memory, you should create object of `ScMemoryContext` class or use existing one. If you create context via constructor, then this context will have guest user (user with `concept_guest_user` class). 

```cpp
ScMemoryContext context;
ScAddr const & guestUserAddr = context.GetUser();
// This user should belongs `concept_guest_user` class.

// After you can use `context` to generate, search or erase sc-elements 
// in sc-memory.
```

You can get user from object of context, if you need to handle this user.

```cpp
ScAddr const & userAddr = context.GetUser();
```

`concept_guest_user` class is default class for all users in sc-memory. By default, all guest users haven't permissions. You can specify permissions for their class. See documentation below to learn how to do it.

When some user initiates a sc-event, an object of `ScMemoryContext` with this user is created for the agent that reacted to this sc-event. After this agent uses this context to manipulate with sc-constructions within sc-memory. Each agent class has `m_context` field. You should use it to call methods in sc-memory.

## **How does the sc-machine identifies users?**

You can identify user. User identification refers to the process of identifying a user on ostis-system, i.e., that the specified guest user is some user that is on the knowledge base.

It is useful when your ostis-system implements functionality to work with guest users. The sc-machine provides users identification. You can identify some guest user as user that had been already registered in the system. Users identification allows you to implement your own agent to register/authorize users by password, token or something else.

To identify user in the sc-memory, you should create connection between guest user and identified user.

```scs
..some_guest_user => nrel_identified_user: ..some_user;;
```

After that, all contexts that had specified guest user will have specified identified user. 

To unidentify identified user, you can create reverse connection between specified guest user and specified identified user.

## **How does the sc-machine authenticate users?**

Users can be known in advance and then guest users and their identification is unnecessary, but all users should be authenticated in sc-memory. User authentication involves the process of verifying the authenticity of identified user and storing information about that user in sc-memory.

To authenticate user in sc-memory, you should create connection between `concept_authentication_request` and user:

```scs
concept_authentication_request -> ..some_user;;
```

You should do it once, no more. After the sc-machine authenticates this user, it means that connection between `concept_authenticated_user` and user has been created and all contexts with this user have been authenticated.

```scs
concept_authenticated_user ~> ..some_user;;
```

If you remove this sc-arc between `concept_authenticated_user` and the user, then the user and all contexts with user will be also unauthenticated.

## **Examples of using user identification and authentication**

### **Example of user identification**

You should identify guest user as user that should be authenticated. 

```cpp
// Create context with guest user.
ScMemoryContext context;
// Get guest user from created context.
ScAddr const & guestUserAddr = context.GetUser();

// Find a user, which should be identified. You can provide your own logic 
// of user identification before.
ScAddr const & userAddr1 = context.SearchElementBySystemIdentifier("user_1");

// Before user identification, specify that user isn't identified. 
// This way, you can subscribe to sc-event of erasing negative sc-arc, 
// because when you identify user, all existing negative sc-arcs 
// between `nrel_identified_user` and sc-arc between guest user and user 
// to be identified are erased. This logic will help you to wait for user 
// to be identified.
ScAddr const & arcAddr = ScMemory::ms_globalContext->GenerateConnector(
  ScType::ConstCommonArc, guestUserAddr, userAddr1);
ScMemory::ms_globalContext->GenerateConnector(
  ScType::ConstTempNegArc, ScKeynodes::nrel_identified_user, arcAddr);

// Create sc-event waiter to wait user to be identified.
// You should subscribe to sc-event of erasing negative sc-arc incoming to
// sc-arc between guest user and user to be identified.
auto eventWaiter 
  = ScMemory::ms_globalContext->CreateConditionWaiter<
    ScEventBeforeEraseIncomingArc<ScType::ConstTempNegArc>>(
  arcAddr,
  [&]() -> void
  {
    // Identify guest user.
    ScMemory::ms_globalContext->GenerateConnector(
      ScType::ConstTempPosArc, ScKeynodes::nrel_identified_user, arcAddr);
    // Only `ScMemory::ms_globalContext` can identify users.
  },
  [&](ScEventBeforeEraseIncomingArc<ScType::ConstTempNegArc> const & event) 
    -> bool
  {
    // Check that sc-arc from `nrel_identified_user` is erased.
    return event.GetArcSourceElement() 
      == ScKeynodes::nrel_identified_user;
  });

// After creation, call method `Wait` and specify time while you 
// will wait sc-event for specified subscription sc-element.
eventWaiter->Wait(200); // milliseconds
// By default, this wait time equals to 5000 milliseconds.
// You will wait until sc-event occurs or until specified time expires.
```

After, you can authenticate identified user.

### **Example of user authentication**

To do this programly you can create waiter to wait sc-event of adding outgoing sc-arc from `concept_authenticated_user` and request user to be authenticated, i.e. create sc-arc between `concept_authentication_request` and user. After waiting this sc-event, your user will be authenticated.

```cpp
...
// Find a user, which you want to authenticate.
ScAddr const & userAddr1 = context.SearchElementBySystemIdentifier("user_1");

// Before user authentication, specify that user isn't authenticated. 
// This way, you can subscribe to sc-event of erasing negative sc-arc, 
// because when you authenticate user, all existing negative sc-arcs 
// between `concept_authenticated_user` and user are erased. This logic will 
// help you to wait for user to be authenticated.
ScMemory::ms_globalContext->GenerateConnector(
  ScType::ConstTempNegArc, 
  ScKeynodes::concept_authenticated_user,
  userAddr1);

// Create sc-event waiter to wait user to be authenticated.
// You should subscribe to sc-event of erasing negative sc-arc incoming to
// user.
auto eventWaiter 
  = ScMemory::ms_globalContext->CreateConditionWaiter<
    ScEventBeforeEraseIncomingArc<ScType::ConstTempNegArc>>(
  userAddr1,
  [&]() -> void
  {
    ScMemory::ms_globalContext->GenerateConnector(
      ScType::ConstPermPosArc, 
      ScKeynodes::concept_authentication_request, 
      userAddr1);

    // Only `ScMemory::ms_globalContext` can authenticate users.
  },
  [&](ScEventBeforeEraseIncomingArc<ScType::ConstTempNegArc> const & event) 
    -> bool
  {
    // Check that sc-arc from `concept_authenticated_user` is erased.
    return event.GetArcSourceElement() 
      == ScKeynodes::concept_authenticated_user;
  });

// After creation, call method `Wait` and specify time while you 
// will wait sc-event for specified subscription sc-element.
eventWaiter->Wait(200); // milliseconds
// By default, this wait time equals to 5000 milliseconds.
// You will wait until sc-event occurs or until specified time expires.
```

Yes, this way of waiting for user authentication is complicated, but it ensures that the NOT factors in the user information are accounted for. In the future, transaction mechanisms will be implemented in the sc-machine to simplify its API.

### **Example of user unauthentication**

To unauthenticate user, you must erase sc-arc between `concept_authenticated_user` and user.

```cpp
...
// Find a user, which you want to unauthenticate.
ScAddr const & userAddr1 = context.SearchElementBySystemIdentifier("user_1");

// Create sc-event waiter to wait user to be unauthenticated.
// You should subscribe to sc-event of adding negative sc-arc incoming to
// user, because it will be added when you erase sc-arcs between 
// `concept_authenticated_user` and user.
auto eventWaiter 
  = ScMemory::ms_globalContext->CreateConditionWaiter<
    ScEventAfterGenerateIncomingArc<ScType::ConstTempNegArc>>(
  userAddr1,
  [&]() -> void
  {
    // Find and erase connection between `concept_authenticated_user`
    // and user.
    ScIterator3Ptr it3 = ScMemory::ms_globalContext->Iterator3(
      ScKeynodes::concept_authenticated_user,
      ScType::EdgeAccessConstPosTemp,
      userAddr1
    );
    if (it3->Next())
    {
      ScMemory::ms_globalContext->EraseElement(it3->Get(1));
    }

    // Only `ScMemory::ms_globalContext` can unauthenticate users.
  },
  [&](ScEventAfterGenerateIncomingArc<ScType::ConstTempNegArc> const & event) 
    -> bool
  {
    // Check that sc-arc from `concept_authenticated_user` is added.
    return event.GetArcSourceElement() 
      == ScKeynodes::concept_authenticated_user;
  });

// After creation, call method `Wait` and specify time while you 
// will wait sc-event for specified subscription sc-element.
eventWaiter->Wait(200); // milliseconds
```

You can use these examples to identify guest users.

## **How are user permissions managed?**

### **Action classes used to handle user permissions**

In order for a user to successfully execute a method from `ScMemoryContext`, it is necessary that the knowledge base for that user specifies that it can perform the class of actions that corresponds to that method.

Description of action classes that users of ostis-system can perform in its knowledge base is represented below:

<table>
  <thead>
    <tr>
      <th>Action class</th>
      <th>System identifier of action class sc-node</th>
      <th>Abbreviation</th>
      <th>Logic if user has permissions to execute actions of the specified class</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>reading from sc-memory action</td>
      <td><code>action_read_from_sc_memory</code></td>
      <td>R</td>
      <td>The user can execute all methods that read sc-constructions from knowledge base.</td>
    </tr>
    <tr>
      <td>generating in sc-memory action</td>
      <td><code>action_generate_in_sc_memory</code></td>
      <td>G</td>
      <td>The user can execute all methods that generate sc-constructions in knowledge base.</td>
    </tr>
    <tr>
      <td>erasing from sc-memory action</td>
      <td><code>action_erase_from_sc_memory</code></td>
      <td>E</td>
      <td>The user can execute all methods that erase sc-constructions from knowledge base.</td>
    </tr>
    <tr>
      <td>reading permissions from sc-memory action</td>
      <td><code>action_read_permissions_from_sc_memory</code></td>
      <td>RP</td>
      <td>The user can read permissions of other users from knowledge base.</td>
    </tr>
    <tr>
      <td>generating permissions in sc-memory action</td>
      <td><code>action_generate_permissions_in_sc_memory</code></td>
      <td>GP</td>
      <td>The user can generate permissions for other users in knowledge base.</td>
    </tr>
    <tr>
      <td>erasing permissions from sc-memory action</td>
      <td><code>action_erase_permissions_from_sc_memory</code></td>
      <td>EP</td>
      <td>The user can erase permissions of other users from knowledge base.</td>
    </tr>
  </tbody>
</table>

Each core method of [**C++ Core API**](../core/api.md) checks user permissions. All methods of *C++ Extended API* use methods of *C++ Core API*. Description of ScMemoryContext API methods and user permissions required to execute these methods is represented below:

<table>
  <thead>
    <tr>
      <th>ScMemoryContext API method</th>
      <th>R</th>
      <th>G</th>
      <th>E</th>
      <th>Logic if user hasn't required permissions to perform method</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>GenerateNode, GenerateLink</td>
      <td>-</td>
      <td>-</td>
      <td>-</td>
      <td>These methods don't require any permissions for users.</td>
    </tr>
    <tr>
      <td>GenerateConnector</td>
      <td>-</td>
      <td>+</td>
      <td>-</td>
      <td>If user hasn't permissions to add sc-connector from specified source sc-element or to specified target sc-element, then method will throw <code>utils::ExceptionInvalidState</code>.</td>
    </tr>
    <tr>
      <td>IsElement</td>
      <td>+</td>
      <td>-</td>
      <td>-</td>
      <td>If user hasn't permissions to read specified sc-element, then method will throw <code>utils::ExceptionInvalidState</code>.</td>
    </tr>
    <tr>
      <td>GetElementType</td>
      <td>+</td>
      <td>-</td>
      <td>-</td>
      <td>If user hasn't permissions to read specified sc-element, then method will throw <code>utils::ExceptionInvalidState</code>.</td>
    </tr>
    <tr>
      <td>SetElementSubtype</td>
      <td>-</td>
      <td>+</td>
      <td>-</td>
      <td>If user hasn't permissions to update specified sc-element sc-type, then method will throw <code>utils::ExceptionInvalidState</code>.</td>
    </tr>
    <tr>
      <td>GetConnectorIncidentElements, GetArcSourceElement, GetArcTargetElement</td>
      <td>+</td>
      <td>-</td>
      <td>-</td>
      <td>If user hasn't permissions to read specified sc-connector or its incident sc-elements, then method will throw <code>utils::ExceptionInvalidState</code>.</td>
    </tr>
    <tr>
      <td>CreateIterator3, CreateIterator5, ForEach, ForEach</td>
      <td>+</td>
      <td>-</td>
      <td>-</td>
      <td>If user hasn't permissions to read some sc-connectors from or to specified sc-element, then method won't return these sc-connectors to user: method <code>ScIterator::Next</code> will skip these sc-connectors. If user has permissions to read sc-connector from or to specified sc-element, but he hasn't permissions to read other incident sc-elements, then method <code>ScIterator::Get</code> will throw <code>utils::ExceptionInvalidState</code> if user tries to get such permitted sc-element by index in found sc-construction. Methods <code>ForEach</code> will return empty sc-address in such case.</td>
    </tr>
    <tr>
      <td>EraseElement</td>
      <td>-</td>
      <td>-</td>
      <td>+</td>
      <td>If user hasn't permissions to erase specified sc-element, then method will return <code>false</code>.</td>
    </tr>
    <tr>
      <td>SetLinkContent</td>
      <td>-</td>
      <td>+</td>
      <td>+</td>
      <td>If user hasn't permissions to change (erase and write) content for specified sc-link, then method will throw <code>utils::ExceptionInvalidState</code>.</td>
    </tr>
    <tr>
      <td>GetLinkContent</td>
      <td>+</td>
      <td>-</td>
      <td>-</td>
      <td>If user hasn't permissions to read specified sc-link by specified content, then method will throw <code>utils::ExceptionInvalidState</code>.</td>
    </tr>
    <tr>
      <td>SearchLinksByContent, SearchLinksByContentSubstring</td>
      <td>+</td>
      <td>-</td>
      <td>-</td>
      <td>If user hasn't permissions to read some sc-links that have specified content, then method won't return these sc-links.</td>
    </tr>
  </tbody>
</table>

### **Global and local user permissions**

Depending on where the user can perform actions in the knowledge base, their permissions are divided into global and local permissions:

* global permissions is permissions over entire knowledge base;
* local permissions is permissions in some part (sc-structure) of knowledge base.

Permissions can be set on a specific user or group (set or class) of users.

All permissions can be represented as connections between users, action classes and sc-structures:

<table>
  <thead>
    <tr>
      <th>Permissions class</th>
      <th>System identifier of relation sc-node for concrete user</th>
      <th>Description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Global permissions</td>
      <td><code>nrel_user_action_class</code></td>
      <td>Binary relation between concrete user and action class</td>
    </tr>
    <tr>
      <td>Local permissions</td>
      <td><code>nrel_user_action_class_within_sc_structure</code></td>
      <td>Ternary relation between concrete user and action class with sc-structure where actions of specified class can be performed by this user</td>
    </tr>
  </tbody>
</table>

<table>
  <thead>
    <tr>
      <th>Permissions class</th>
      <th>System identifier of relation sc-node for users set</th>
      <th>Description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Global permissions</td>
      <td><code>nrel_users_set_action_class</code></td>
      <td>Binary relation between users set and action class</td>
    </tr>
    <tr>
      <td>Local permissions</td>
      <td><code>nrel_users_set_action_class_within_sc_structure</code></td>
      <td>Ternary relation between users set and action class with sc-structure where actions of specified class can be performed by this one of users</td>
    </tr>
  </tbody>
</table>

Examples of global and local read permissions for user and users set:

<table>
  <tr>
    <th>Permissions</th>
    <th>SCg-code example</th>
    <th>SCs-code example</th>
  </tr>

  <tr>
    <td>User has global read permissions</td>
    <td><scg src="../images/permissions/global_read_permissions_for_user.gwf"></scg></td>
    <td>
      <pre>
        <code class="js hljs javascript">
..user <~ concept_user;;

nrel_user_action_class
~> (..user => action_read_from_sc_memory);; 
        </code>
      </pre>
    </td>
  </tr>
  <tr>
    <td>User has local read permissions</td>
    <td><scg src="../images/permissions/local_read_permissions_for_user.gwf"></scg></td>
    <td>
      <pre>
        <code class="js hljs javascript">
..user <~ concept_user;;

..structure = [*
    concept_set ~> ..set;;
*];;

nrel_user_action_class_within_sc_structure
~> (..user => (action_read_from_sc_memory => ..structure));;
        </code>
      </pre>
    </td>
  </tr>
  <tr>
    <td>Each user in users set has global read permissions</td>
    <td><scg src="../images/permissions/global_read_permissions_for_users_set.gwf"></scg></td>
    <td>
      <pre>
        <code class="js hljs javascript">
nrel_users_set_action_class
~> (concept_user => action_read_from_sc_memory);;
        </code>
      </pre>
    </td>
  </tr>

  <tr>
    <td>Each user in users set has local read permissions</td>
    <td><scg src="../images/permissions/local_read_permissions_for_users_set.gwf"></scg></td>
    <td>
      <pre>
        <code class="js hljs javascript">
..structure = [*
    concept_set ~> ..set;;
*];;

nrel_users_set_action_class_within_sc_structure
~> (..user => (action_read_from_sc_memory => ..structure));;
        </code>
      </pre>
    </td>
  </tr>

  <tr>
    <td>User hasn't global read permissions</td>
    <td><scg src="../images/permissions/no_global_read_permissions_for_user.gwf"></scg></td>
    <td>
      <pre>
        <code class="js hljs javascript">
..user <~ concept_user;;

nrel_user_action_class
~|> (..user => action_read_from_sc_memory);;
        </code>
      </pre>
    </td>
  </tr>
  <tr>
    <td>User hasn't local read permissions</td>
    <td><scg src="../images/permissions/no_local_read_permissions_for_user.gwf"></scg></td>
    <td>
      <pre>
        <code class="js hljs javascript">
..user <~ concept_user;;

..structure = [*
concept_set ~> ..set;;
*];;

nrel_user_action_class_within_sc_structure
~|> (..user => (action_read_from_sc_memory => ..structure));;
        </code>
      </pre>
    </td>
  </tr>
  <tr>
    <td>Each user in users set hasn't global read permissions</td>
    <td><scg src="../images/permissions/no_global_read_permissions_for_users_set.gwf"></scg></td>
    <td>
      <pre>
        <code class="js hljs javascript">
nrel_users_set_action_class
~|> (concept_user => action_read_from_sc_memory);;
        </code>
      </pre>
    </td>
  </tr>

  <tr>
    <td>Each user in users set hasn't local read permissions</td>
    <td><scg src="../images/permissions/no_local_read_permissions_for_users_set.gwf"></scg></td>
    <td>
      <pre>
        <code class="js hljs javascript">
..structure = [*
    concept_set ~> ..set;;
*];;

nrel_users_set_action_class_within_sc_structure
~|> (..user => (action_read_from_sc_memory => ..structure));;
        </code>
      </pre>
    </td>
  </tr>
</table>

In these examples, you can use any other action class (`action_generate_in_sc_memory`, `action_erase_from_sc_memory`, `action_read_permissions_from_sc_memory`, `action_generate_permissions_in_sc_memory` or `action_erase_permissions_from_sc_memory`) instead of `action_read_from_sc_memory`.

All sc-arcs from permissions classes (relations) must be permanent or temporary. All sc-arcs from user groups (classes or sets) must be permanent or temporary.

!!! note
    If the knowledge base does not explicitly specify permissions for a user, it is equivalent to saying that this user does not have them.

!!! note
    In order to erase permissions for a user it is enough to erase an membership sc-arc from the relation that indicated permissions.
  
!!! note
    In the sc-machine there is a global system context -- `ScMemory::ms_globalContext`, that has all permissions. You can use it to update permissions that can be used to read, update or erase permissions for other users.

!!! warning
    Erasing a positive membership sc-arc between relation and sc-arc between the user and an action class, that the user can perform (pair with sc-structure and action class), automatically creates a negative membership sc-arc between the given relation and the sc-arc between the user and the action class (the pair with sc-structure and action class).

<scg src="../images/permissions/erase_local_read_permissions_for_user.gwf"></scg>

!!! warning
    Creating a positive membership sc-arc between relation and sc-arc between the user and an action class, that the user can perform (pair with sc-structure and action class), automatically erases all negative membership sc-arcs between the given relation and the sc-arc between the user and the action class (the pair with sc-structure and action class).

<scg src="../images/permissions/generate_local_read_permissions_for_user.gwf"></scg>

---

### **Compatibility of global and local user permissions**

Both global and local permissions can be described for the same user. It is important to know the logic of how they work together. For example, concrete user wants to handle some sc-construction. There may be several cases:

<table>
  <thead>
    <tr>
      <th>Case</th>
      <th>Criterion 1. Does specified sc-construction belong to any permitted sc-structure?</th>
      <th>Criterion 2. Does user have global permissions?</th>
      <th>Criterion 3. Does user have local permissions?</th>
      <th>Result. Can user handle the specified sc-construction?</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>1</td>
      <td>No, it doesn't.</td>
      <td>No, he doesn't.</td>
      <td>No, he doesn't.</td>
      <td>No, he doesn't.</td>
    </tr>
    <tr>
      <td>2</td>
      <td>No, it doesn't.</td>
      <td>Yes. He has global write permissions.</td>
      <td>No, he doesn't.</td>
      <td>Yes. He can only add sc-elements into the specified sc-construction, because he has global write permissions.</td>
    </tr>
    <tr>
      <td>3</td>
      <td>Yes. The sc-construction is in a sc-structure that indicates that certain permissions are required. Some users have permissions to this sc-structure.</td>
      <td>No, he doesn't.</td>
      <td>No, he doesn't.</td>
      <td>No, he doesn't.</td>
    </tr>
    <tr>
      <td>4</td>
      <td>Yes. The sc-construction is in a sc-structure that indicates that certain permissions are required. Some users have permissions to this sc-structure.</td>
      <td>Yes. He has global read permissions.</td>
      <td>No, he doesn't.</td>
      <td>No, he doesn't. The user hasn't local permissions for the specified sc-structure. But he can read any sc-constructions in knowledge base, which doesn't have permitted sc-structures.</td>
    </tr>
    <tr>
      <td>5</td>
      <td>Yes. The sc-construction is in a sc-structure that indicates that certain permissions are required. Some users have permissions to this sc-structure.</td>
      <td>No, he doesn't.</td>
      <td>Yes. He has local write permissions for the specified sc-structure.</td>
      <td>Yes. The user can only add sc-elements into the specified sc-structure. But he can't handle any sc-constructions in knowledge base other than the given one.</td>
    </tr>
    <tr>
      <td>6</td>
      <td>Yes. The sc-construction is in a sc-structure that indicates that certain permissions are required. Some users have permissions to this sc-structure.</td>
      <td>Yes. He has global write permissions.</td>
      <td>Yes. He has local write permissions for the specified sc-structure.</td>
      <td>Yes. The user can add sc-elements into the specified sc-structure. And he can add sc-element into any sc-constructions in knowledge base other than the given one.</td>
    </tr>
  </tbody>
</table>

---

### **Examples of using user permissions**

If you want to update permissions for a user, then you can describe them in the knowledge base in any format convenient for you (SCs-code or SCg-code) or do it programly.

#### **Example for adding (updating) user permissions**

If you want to add permissions for user constantly, specify permissions for this user in the knowledge base. See examples of local and global user permissions above.

To do this programly you can create waiter to wait sc-event of adding outgoing sc-arc from permissions class and add permissions for this user. After waiting this sc-event, your user will have new user permissions.

```cpp
...
// Find a user, for whom you need to add new permissions.
ScAddr const & userAddr1 = context.SearchElementBySystemIdentifier("user_1");
// Find a structure, within which the user should have permissions.
ScAddr const & structureAddr 
  = context.SearchElementBySystemIdentifier("my_structure");

// Before adding new user permissions, specify that user doesn't have these
// permissions. This way, you can subscribe to sc-event of erasing negative 
// sc-arc, because when you add new permissions for user, all existing 
// negative sc-arcs to permissions are erased. This logic will help you to 
// wait for user's permissions to be changed.
ScAddr const & _actionClassArcAddr = context.GenerateConnector(
  ScType::ConstCommonArc, 
  ScKeynodes::action_generate_in_sc_memory, 
  structureAddr);
ScAddr const & _userArcAddr = context.GenerateConnector(
  ScType::ConstCommonArc, 
  userAddr1, 
  _actionClassArcAddr);
context.GenerateConnector(
  ScType::ConstTempNegArc, 
  ScKeynodes::nrel_user_action_class_within_sc_structure, 
  _userArcAddr);

// Create sc-event waiter to wait adding new permissions for specified user.
// You should subscribe to sc-event of erasing negative sc-arc. 
// Negative sc-arc is erased when you added new permissions for user. 
auto eventWaiter 
  = context.CreateConditionWaiter<
    ScEventEraseOutgoingArc<ScType::ConstTempNegArc>>(
  ScKeynodes::nrel_user_action_class_within_sc_structure,
  [&]() -> void
  {
    // Update user permissions here.
    ScAddr const & actionClassArcAddr = context.GenerateConnector(
      ScType::ConstCommonArc, 
      ScKeynodes::action_generate_in_sc_memory, 
      structureAddr);
    ScAddr const & userArcAddr = context.GenerateConnector(
      ScType::ConstCommonArc, 
      userAddr1, 
      actionClassArcAddr);
    context.GenerateConnector(
      ScType::EdgeAccessConstPosTemp, 
      ScKeynodes::nrel_user_action_class_within_sc_structure, 
      userArcAddr);
    // You should create sc-arc from permissions class at 
    // the end of this callback.

    // And note, that if `context` user doesn't have permissions to create
    // permissions in sc-memory for other users, you should to update 
    // permissions for user of `context` or use `ScMemory::ms_globalContext` 
    // that has all permissions, by default.
  },
  [&](ScEventEraseOutgoingArc<ScType::ConstTempNegArc> const & event)
    -> bool
  {
    // Check that permissions for specified are updated.
    ScAddr targetElementAddr = event.GetArcTargetElement();
    if (context.GetElementType(targetElementAddr) 
        != ScType::ConstCommonArc)
      return false;

    auto [userAddr, permissionsAddr]
      = context.GetConnectorIncidentElements(targetElementAddr);

    return userAddr == userAddr1;
  });

// After creation, call method `Wait` and specify time while you 
// will wait sc-event for specified subscription sc-element.
eventWaiter->Wait(200); // milliseconds
// By default, this wait time equals to 5000 milliseconds.
// You will wait until sc-event occurs or until specified time expires.
```

Yes, this way of waiting for permissions to change is complicated, but it ensures that the NOT factors in the user information are accounted for. In the future, transaction mechanisms will be implemented in the sc-machine to simplify its API.

#### **Example for erasing user permissions**

To erase permissions for a user, you must erase sc-arc from permissions class.

```cpp
...
// Find a user, for whom you need to erase permissions.
ScAddr const & userAddr1 = context.SearchElementBySystemIdentifier("user_1");
// Find a structure, within which the user should no longer have rights
ScAddr const & structureAddr 
  = context.SearchElementBySystemIdentifier("my_structure");
// Create sc-event waiter to wait erasing permissions for specified user. 
// You should subscribe to sc-event of adding negative sc-arc. 
// Negative sc-arc is added when you erases user permissions. 
auto eventWaiter 
  = context.CreateConditionWaiter<
    ScEventAfterGenerateOutgoingArc<ScType::ConstTempNegArc>>(
  ScKeynodes::nrel_user_action_class_within_sc_structure,
  [&]() -> void
  {
    // Update user permissions here.
    ScTemplate permissionsTemplate;
    permissionsTemplate.Triple(
      ScKeynodes::action_generate_in_sc_memory,
      ScType::VarCommonArc >> "_action_class_arc",  
      structureAddr
    );
    permissionsTemplate.Quintuple(
      userAddr1, 
      ScType::VarCommonArc,
      "_action_class_arc",
      ScType::VarTempPosArc >> "_permissions_arc",
      ScKeynodes::nrel_user_action_class_within_sc_structure 
    );
    ScTemplateSearchResult result;
    if (context.SearchByTemplate(permissionsTemplate, result))
    {
      context.EraseElement(result[0]["_permissions_arc"]);
      // You should erase sc-arc from permissions class at 
      // the end of this callback.
    }

    // And note, that if `context` user doesn't have permissions to read
    // permissions in sc-memory for other users, you should to update 
    // permissions for user of `context` or use `ScMemory::ms_globalContext` 
    // that has all permissions, by default.
  },
  [&](ScEventAfterGenerateOutgoingArc<ScType::ConstTempNegArc> const & event)
    -> bool
  {
    // Check that permissions for specified are erased.
    ScAddr targetElementAddr = event.GetArcTargetElement();
    if (context.GetElementType(targetElementAddr) 
        != ScType::ConstCommonArc)
      return false;

    auto [userAddr, permissionsAddr]
      = context.GetConnectorIncidentElements(targetElementAddr);

    return userAddr == userAddr1;
  });

// After creation, call method `Wait` and specify time while you 
// will wait sc-event for specified subscription sc-element.
eventWaiter->Wait(200); // milliseconds
```

These examples can be reused for any of permissions classes described above.

---

## **Frequently Asked Questions**

- [How to specify role for users in knowledge base and set permissions for this role?](#how-to-specify-role-for-users-in-knowledge-base-and-set-permissions-for-this-role)
- [How to specify permissions to structures that are not described in knowledge base sources?](#how-to-specify-permissions-to-structures-that-are-not-described-in-knowledge-base-sources)

### **How to specify role for users in knowledge base and set permissions for this role?**

A role for users should be understood as a class of users. You can specify permission for user classes.

The following example shows how global and local permissions are specified for a class of users:

```scs
concept_users_class_1
~> ..user_1;;

nrel_users_set_action_class
~> (concept_users_class_1 => action_read_from_sc_memory);;

nrel_users_set_action_class_within_sc_structure
~> (concept_users_class_1 => (action_read_from_sc_memory => ..structure));;

..structure = [*
    concept_users_class_2 ~> ..user_2;;
    ..user_2 -> rrel_password: [password];;
*];;
```

This example shows that all users from `concept_users_class_1` can view the entire knowledge base besides the structure describing users from `concept_users_class_2`.

### **How to specify permissions to structures that are not described in knowledge base sources?**

You can generate and update structures in knowledge base while system is running. You can also generate and modify permissions to structures in knowledge base while system is running. To do this, you should use all available API methods.

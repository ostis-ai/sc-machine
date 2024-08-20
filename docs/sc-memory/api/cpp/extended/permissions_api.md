# **User permissions API**

!!! warning
    This documentation is correct for only versions of sc-machine that >= 0.10.0.
---

This API allows to handle users and their permissions in knowledge base.

## **What are user permissions?**

Users interact with ostis-systems. Users can be other systems, agents or people. All users of an ostis-system can perform actions on its knowledge base. There are six classes of user actions in knowledge base. Before a user action is  performed, it is checked whether the user has permissions to perform actions of the specified class.

By default, user permissions are not handled by sc-machine. This is configured via the sc-machine config (`<config-name>.ini`). To enable user permissions handling in the sc-machine, go to the group `[sc-memory]` and set `user_mode` as `true`.

```ini
[sc-memory]
...
user_mode = true
...
```

## **How are user permissions handled?**

You can only work with memory through methods provided in `ScMemoryContext`. Each object of this class can be considered as an object that handles information about a user (including his permissions) when that user invokes methods through that object.

You can't create object of `ScMemoryContext` with providing user. This happens automatically. When some user initiates a sc-event, an object of `ScMemoryContext` with this user is created for the agent that reacted to this sc-event. After your agent uses this context to call sc-memory methods.

You can get user from object of context, if you need to handle this user.

```cpp
ScAddr const & userAddr = context.GetUser();
```

In order for a user to successfully execute a method from `ScMemoryContext`, it is necessary that the knowledge base for that user specifies that it can perform the class of actions that corresponds to that method.

## **Action classes used to handle user permissions**

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
      <td>CreateNode, CreateLink</td>
      <td>-</td>
      <td>-</td>
      <td>-</td>
      <td>These methods don't require any permissions for users.</td>
    </tr>
    <tr>
      <td>CreateEdge</td>
      <td>-</td>
      <td>+</td>
      <td>-</td>
      <td>If user hasn't permissions to add sc-connector from specified begin sc-element or to specified end sc-element, then method will throw <code>utils::ExceptionInvalidState</code>.</td>
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
      <td>GetEdgeInfo, GetEdgeSource, GetEdgeTarget</td>
      <td>+</td>
      <td>-</td>
      <td>-</td>
      <td>If user hasn't permissions to read specified sc-connector or its incident sc-elements, then method will throw <code>utils::ExceptionInvalidState</code>.</td>
    </tr>
    <tr>
      <td>ScIterator3, ScIterator5, ForEachIter3, ForEachIter5</td>
      <td>+</td>
      <td>-</td>
      <td>-</td>
      <td>If user hasn't permissions to read some sc-connectors from or to specified sc-element, then method won't return these sc-connectors to user: method <code>ScIterator::Next</code> will skip these sc-connectors. If user has permissions to read sc-connector from or to specified sc-element, but he hasn't permissions to read other incident sc-elements, then method <code>ScIterator::Get</code> will throw <code>utils::ExceptionInvalidState</code> if user tries to get such permitted sc-element by index in found sc-construction. Methods <code>ForEachIter3</code> and <code>ForEachIter5</code> will return empty sc-address in such case.</td>
    </tr>
    <tr>
      <td>EraseElement</td>
      <td>-</td>
      <td>-</td>
      <td>+</td>
      <td>If user hasn't permissions to erase specified sc-element, then method will return <code>SC_FALSE</code>.</td>
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
      <td>FindLinksByContent, FindLinksByContentSubstring</td>
      <td>+</td>
      <td>-</td>
      <td>-</td>
      <td>If user hasn't permissions to read some sc-links that have specified content, then method won't return these sc-links.</td>
    </tr>
  </tbody>
</table>

## **Global and local user permissions**

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
    In order to erase permissions for a user it is enough to erase an access sc-arc from the relation that indicated permissions.
  
!!! note
    In the sc-machine there is a global system context -- `ScMemory::ms_globalContext`, that has all permissions. You can use it to update permissions that can be used to read, update or erase permissions for other users.

!!! warning
    Erasing a positive access sc-arc between relation and sc-arc between the user and an action class, that the user can perform, automatically creates a negative access sc-arc between the given relation and the sc-arc between the user and the action class.

!!! warning
    Creating a positive access sc-arc between relation and sc-arc between the user and an action class, that the user can perform, automatically erases all negative access sc-arcs between the given relation and the sc-arc between the user and the action class.

---

## **Compatibility of global and local user permissions**

Both global and local permissions can be described for the same user. It is important to know the logic of how they work together. For example, concrete user wants to handle some sc-construction. There may be several cases:

<table>
  <thead>
    <tr>
      <th>Case</th>
      <th>Does specified sc-construction belong to any permitted sc-structure?</th>
      <th>Does user have global permissions?</th>
      <th>Does user have local permissions?</th>
      <th>Can user handle the specified sc-construction?</th>
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

## **Examples of using user permissions**

If you want to update permissions for a user, then you can describe them in the knowledge base in any format convenient for you (SCs-code or SCg-code) or do it programly.

### **Example for adding (updating) user permissions**

If you want to add permissions for user constantly, specify permissions for this user in the knowledge base. See examples of local and global user permissions above.

To do this programly you can create waiter to wait sc-event of adding outgoing sc-arc from permissions class and add permissions for this user. After waiting this sc-event, your user will have new user permissions.

```cpp
...
// Find a user, for whom you need to add new permissions.
ScAddr const & userAddr1 = context.HelperFindBySystemIdtf("user_1");
// Find a structure, within which the user should have permissions.
ScAddr const & structureAddr = context.HelperFindBySystemIdtf("my_structure");

// Before adding new user permissions, specify that user doesn't have these
// permissions. This way, you can subscribe to sc-event of erasing negative 
// sc-arc, because when you add new permissions for user, all existing 
// negative sc-arcs to  permissions are erased. This logic will help you to 
// wait for user's permissions will be changed.
ScAddr const & _actionClassArcAddr = context.CreateEdge(
  ScType::EdgeDCommonConst, 
  ScKeynodes::action_generate_in_sc_memory, 
  structureAddr);
ScAddr const & _userArcAddr = context.CreateEdge(
  ScType::EdgeDCommonConst, 
  userAddr1, 
  _actionClassArcAddr);
context.CreateEdge(
  ScType::EdgeAccessConstNegTemp, 
  ScKeynodes::nrel_user_action_class_within_sc_structure, 
  _userArcAddr);

// Create sc-event waiter to wait adding new permissions for specified user.
// You should subscribe to sc-event of erasing negative sc-arc. 
// Negative sc-arc is erased when you added new permissions for user. 
auto eventWaiter 
  = context.CreateEventWaiter<
    ScEventEraseOutgoingArc<ScType::EdgeAccessConstNegTemp>>(
  ScKeynodes::nrel_user_action_class_within_sc_structure,
  [&]() -> void
  {
    // Update user permissions here.
    ScAddr const & actionClassArcAddr = context.CreateEdge(
      ScType::EdgeDCommonConst, 
      ScKeynodes::action_generate_in_sc_memory, 
      structureAddr);
    ScAddr const & userArcAddr = context.CreateEdge(
      ScType::EdgeDCommonConst, 
      userAddr1, 
      actionClassArcAddr);
    context.CreateEdge(
      ScType::EdgeAccessConstPosTemp, 
      ScKeynodes::nrel_user_action_class_within_sc_structure, 
      userArcAddr);
    // You should create sc-arc from permissions class at 
    // the end of this callback.

    // And note, that if `context` user doesn't have permissions to create
    // permissions in sc-memory for other users, you should to update 
    // permissions for user of `context` or use `ScMemory::ms_globalContext` 
    // that has all permissions, by default.
  });

// After creation, call method `Wait` and specify time while you 
// will wait sc-event for specified subscription sc-element
eventWaiter->Wait(200); // milliseconds
// By default, this wait time equals to 5000 milliseconds.
// You will wait until sc-event occurs or until specified time expires.
```

Yes, this way of waiting for permissions to change is complicated, but it ensures that the NOT factors in the user information are accounted for. In the future, transaction mechanisms will be implemented in the ыс0machine to simplify its API.

### **Example for erasing user permissions**

To erase permissions for a user, you must erase sc-arc from permissions class.

```cpp
...
// Find a user, for whom you need to erase permissions.
ScAddr const & userAddr1 = context.HelperFindBySystemIdtf("user_1");
// Find a structure, within which the user should no longer have rights
ScAddr const & structureAddr = context.HelperFindBySystemIdtf("my_structure");
// Create sc-event waiter to wait erasing permissions for specified user. 
// You should subscribe to sc-event of adding negative sc-arc. 
// Negative sc-arc is added when you erases user permissions. 
auto eventWaiter 
  = context.CreateEventWaiter<
    ScEventAddOutgoingArc<ScType::EdgeAccessConstNegTemp>>(
  ScKeynodes::nrel_user_action_class_within_sc_structure,
  [&]() -> void
  {
    // Update user permissions here.
    ScTemplate permissionsTemplate;
    permissionsTemplate.Triple(
      ScKeynodes::action_generate_in_sc_memory,
      ScType::EdgeDCommonVar >> "_action_class_arc",  
      structureAddr
    );
    permissionsTemplate.Quintuple(
      userAddr1, 
      ScType::EdgeDCommonVar,
      "_action_class_arc",
      ScType::EdgeAccessVarPosTemp >> "_permissions_arc",
      ScKeynodes::nrel_user_action_class_within_sc_structure 
    )
    ScTemplateSearchResult result;
    if (context.HelperSearchTemplate(permissionsTemplate, result))
    {
      context.EraseElement(result[0]["_permissions_arc"]);
      // You should erase sc-arc from permissions class at 
      // the end of this callback.
    }

    // And note, that if `context` user doesn't have permissions to read
    // permissions in sc-memory for other users, you should to update 
    // permissions for user of `context` or use `ScMemory::ms_globalContext` 
    // that has all permissions, by default.
  });

// After creation, call method `Wait` and specify time while you 
// will wait sc-event for specified subscription sc-element
eventWaiter->Wait(200); // milliseconds
// By default, this wait time equals to 5000 milliseconds.
// You will wait until sc-event occurs or until specified time expires.
```

These examples can be reused for any of permissions classes described above.

---

## **Frequently Asked Questions**

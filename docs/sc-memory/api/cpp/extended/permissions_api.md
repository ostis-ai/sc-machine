# **User permissions API**

!!! warning
    This documentation is correct for only versions of sc-machine that >= 0.10.0.
---

This API allows to handle users and their permissions in knowledge base.

Users interact with ostis-systems. Users can be other systems, agents or people. All users of an ostis-system can 
perform actions on its knowledge base. There are six classes of user actions in knowledge base. Before a user action is 
performed, it is checked whether the user has permissions to perform actions of the specified class.

Description of action classes that users of ostis-system can perform in its knowledge base:

| Action class                               | System identifier of action class sc-node  | Abbreviation | Logic if user has permissions to execute actions of the specified class            |
|--------------------------------------------|--------------------------------------------|--------------|------------------------------------------------------------------------------------|
| reading from sc-memory action              | `action_read_from_sc_memory`               | R            | The user can execute all methods that read sc-constructions from knowledge base.   |
| generating in sc-memory action             | `action_generate_in_sc_memory`             | G            | The user can execute all methods that generate sc-constructions in knowledge base. |
| erasing from sc-memory action              | `action_erase_from_sc_memory`              | E            | The user can execute all methods that erase sc-constructions from knowledge base.  |
| reading permissions from sc-memory action  | `action_read_permissions_from_sc_memory`   | RP           | The user can read permissions of other users from knowledge base.                  |
| generating permissions in sc-memory action | `action_generate_permissions_in_sc_memory` | GP           | The user can generate permissions for other users in knowledge base.               |
| erasing permissions from sc-memory action  | `action_erase_permissions_from_sc_memory`  | EP           | The user can erase permissions of other users from knowledge base.                 |

Description of ScMemoryContext API methods and user permissions required to execute these methods:

| ScMemoryContext API method                           | R | G | E | Logic is user hasn't required permissions to perform method                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     |
|------------------------------------------------------|---|---|---|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| CreateNode, CreateLink                               | - | - | - | These methods don't require any permissions for users.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          |
| CreateEdge                                           | - | + | - | If user hasn't permissions to add sc-connector from specified begin sc-element or to specified end sc-element, then method will throw `utils::ExceptionInvalidState`.                                                                                                                                                                                                                                                                                                                                                                                                                           |
| IsElement                                            | + | - | - | If user hasn't permissions to read specified sc-element, then method will throw `utils::ExceptionInvalidState`.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 |
| GetElementType                                       | + | - | - | If user hasn't permissions to read specified sc-element, then method will throw `utils::ExceptionInvalidState`.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 |
| SetElementSubtype                                    | - | + | - | If user hasn't permissions to update specified sc-element sc-type, then method will throw `utils::ExceptionInvalidState`.                                                                                                                                                                                                                                                                                                                                                                                                                                                                       |
| GetEdgeInfo, GetEdgeSource, GetEdgeTarget            | + | - | - | If user hasn't permissions to read specified sc-connector or its incident sc-elements, then method will throw `utils::ExceptionInvalidState`.                                                                                                                                                                                                                                                                                                                                                                                                                                                   |
| ScIterator3, ScIterator5, ForEachIter3, ForEachIter5 | + | - | - | If user hasn't permissions to read some sc-connectors from or to specified sc-element, then method won't return this sc-connectors to user: method `ScIterator::Next` will skip this sc-connectors. If user has permissions to read sc-connector from or to specified sc-element, but he hasn't permissions to read other incident sc-elements, then method `ScIterator::Get` will throw `utils::ExceptionInvalidState`, if user tries to get such permitted sc-element by index in found sc-construction. Methods `ForEachIter3` and `ForEachIter5` will return empty sc-address in such case. |
| EraseElement                                         | - | - | + | If user hasn't permissions to erase specified sc-element, then method will return `SC_FALSE`.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   | 
| SetLinkContent                                       | - | + | + | If user hasn't permissions to change (erase and write) content for specified sc-link, then method will throw `utils::ExceptionInvalidState`.                                                                                                                                                                                                                                                                                                                                                                                                                                                    |
| GetLinkContent                                       | + | - | - | If user hasn't permissions to read specified sc-link by specified content, then method will throw `utils::ExceptionInvalidState`.                                                                                                                                                                                                                                                                                                                                                                                                                                                               |
| FindLinksByContent, FindLinksByContentSubstring      | + | - | - | If user hasn't permissions to read some sc-links that have specified content, then method won't return this sc-links.                                                                                                                                                                                                                                                                                                                                                                                                                                                                           |


Depending on where the user can perform actions in the knowledge base, their permissions are divided into global and 
local permissions:
- global permissions is permissions over entire knowledge base;
- local permissions is permissions in some part (sc-structure) of knowledge base.

Permissions can be set on a specific user or group (set or class) of users.

All permissions can be represented as connections between users, action classes and sc-structures:

| Permissions class  | System identifier of relation sc-node for concrete user | Description                                                                                                                              |
|--------------------|---------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------|
| Global permissions | `nrel_user_action_class`                                | Binary relation between concrete user and action class                                                                                   |
| Local permissions  | `nrel_user_action_class_within_sc_structure`            | Ternary relation between concrete user and action class with sc-structure where actions of specified class can be performed by this user |


| Permissions class  | System identifier of relation sc-node for users set | Description                                                                                                                                  |
|--------------------|-----------------------------------------------------|----------------------------------------------------------------------------------------------------------------------------------------------|
| Global permissions | `nrel_users_set_action_class`                       | Binary relation between users set and action class                                                                                           |
| Local permissions  | `nrel_users_set_action_class_within_sc_structure`   | Ternary relation between users set and action class with sc-structure where actions of specified class can be performed by this one of users |


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

!!! note
    You can use any other action class (`action_generate_in_sc_memory`, `action_erase_from_sc_memory`, `action_read_permissions_from_sc_memory`, `action_generate_permissions_in_sc_memory` or `action_erase_permissions_from_sc_memory`) instead of `action_read_from_sc_memory`.

!!! note
    If the knowledge base does not explicitly specify permissions for a user, it is tantamount to the user doesn't have them.

!!! note
    In order to erase permissions for a user it is enough to erase the access arc from the relation that indicated permissions.
    
!!! note
    Erasing a positive access sc-arc between relation and sc-arc between the user and an action class, that the user can perform, automatically creates a negative access sc-arc between the given relation and the sc-arc between the user and the action class.
    Creating a positive access sc-arc between relation and sc-arc between the user and an action class, that the user can perform, automatically erasing all negative access sc-arcs between the given relation and the sc-arc between the user and the action class.

---

Both global and local permissions can be described for the same user. It is important to know the logic of how they work 
together. For example, concrete user wants to handle some sc-construction. There may be several cases:

| Case | Does specified sc-construction belong to any permitted sc-structure?                                                                                  | Does user have global permissions?    | Does user have local permissions?                                   | Can user handle the specified sc-construction?                                                                                                                                        |
|------|-------------------------------------------------------------------------------------------------------------------------------------------------------|---------------------------------------|---------------------------------------------------------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 1    | No, it doesn't.                                                                                                                                       | No, he doesn't.                       | No, he doesn't.                                                     | No, he doesn't.                                                                                                                                                                       |
| 2    | No, it doesn't.                                                                                                                                       | Yes. He has global write permissions. | No, he doesn't.                                                     | Yes. He can only add sc-elements into the specified sc-construction, because he has global write permissions.                                                                         |
| 3    | Yes. The sc-construction is in a sc-structure that indicates that certain permissions are required. Some users have permissions to this sc-structure. | No, he doesn't.                       | No, he doesn't.                                                     | No, he doesn't.                                                                                                                                                                       |
| 4    | Yes. The sc-construction is in a sc-structure that indicates that certain permissions are required. Some users have permissions to this sc-structure. | Yes. He has global read permissions.  | No, he doesn't.                                                     | No, he doesn't. The user hasn't local permissions for the specified sc-structure. But he can read any sc-constructions in knowledge base, which doesn't have permitted sc-structures. |
| 5    | Yes. The sc-construction is in a sc-structure that indicates that certain permissions are required. Some users have permissions to this sc-structure. | No, he doesn't.                       | Yes. He has local write permissions for the specified sc-structure. | Yes. The user can only add sc-elements into the specified sc-structure. But he can't handle any sc-constructions in knowledge base other than the given one.                          |
| 6    | Yes. The sc-construction is in a sc-structure that indicates that certain permissions are required. Some users have permissions to this sc-structure. | Yes. He has global write permissions. | Yes. He has local write permissions for the specified sc-structure. | Yes. The user can add sc-elements into the specified sc-structure. And he can add sc-element into any sc-constructions in knowledge base other than the given one.                    |

## **Frequently Asked Questions**



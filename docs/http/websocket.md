# WebSocket protocol

It used to communicate with knowledge base by network from a browser.
This protocol based is asynchronous and based on JSON.

!!! note ""
    JSON used instead of binary data to prevent problems with different byte (bit) order on different platforms.

!!! info "ScAddr"
    `ScAddr` encodes with `ScAddr::Hash` function

## Common

Each request has a common structure:

```json
{
  "id": 2,
  "type": "request type",
  "payload": {
    ...
  }
}
```

Where:

!!! tip ""
    * **id** - unique id of command. Used to identify responses;
    * **payload** - command specified data.

---

Response also has a common structure:

```json
{
  "id": 2,
  "status": true,
  "event": false, // flag that mark if it is an emited event or not
  "payload": {
    ...
  }
}
```

Where:

!!! tip ""
    * **id** - id of command;
    * **status** - has `true` value when command processed; otherwise has a `false` value;
    * **payload** - command specified result data.

---

### Authentificate

!!! warning "TODO"
    Need to make documentation

---

### CreateElements

**Request type**: `create_elements`

With this command you can create a batch of elements. `payload` of this command
contains list of element creation parameters (depend on element type).
All of this parameters have common structure:

```json
{
  "el": "node", // type of element to create. Possible values: node, edge, link
  "params": {}  // parameters that depends on element type
}
```

There are parameters requested by elements type:

* **node** - to create node use parameters:
```json
{
  "el": "node", // type of element to create
  "type": 342   // ScType value of node type
}
```
* **edge** - to create edge use parameters:
```json
{
  "el": "edge",
  "src": {          // source element of edge
    "type": "addr", // use exist ScAddr of element
    "value": 2313   // value of ScAddr
  },
  "trg": {          // target element of edge
    "type": "ref",  // reference to created element before this one
    "value": 0      // index of element in CreateElement payload list
  },
  "type": 32        // ScType value of edge type
}
```
* **link** - to create link use parameters:
```json
{
  "el": "link",
  "type": 2,         // ScType value of link type (const or var)
  "content": "data", // content data of link
  "content_type": "string"  // content type: int, float, string, binary
}
```

!!! example "Example"

    ```json
    // request
    {
      "id": 3,
      "type": "create_elements",
      "payload": [
        {
          "el": "node",
          "type": 1
        },
        {
          "el": "link",
          "type": 2,
          "content": 45.4
        },
        {
          "el": "edge",
          "src": {
            "type": "ref",
            "value": 0
          },
          "trg": {
            "type": "ref",
            "value": 1
          },
          "type": 32
        }
      ]
    }

    // response
    {
      "id": 3,
      "status": true,
      "payload": [
        323,
        534,
        342
      ]
    }
    ```

---

### CheckElements

**Request type**: `check_elements`

With this command you can check if specified elements exist. So you should pass
list of ScAddr to check, and this command will return list of each element type.
If type is not valid, then element doesn't exist. See `ScType` for more information.

!!! quote "Request"

    ```json
    {
      ..., // common request data
      "type": "check_elements",
      "payload": [
        // there are a list of ScAddr's to check
        23123,
        432,
        ...
      ]
    }
    ```

!!! quote "Response"

    ```json
    {
      ..., // common response data
      "payload": [
        // there is a list of element types (ScType)
        32, // valid type
        0,  // not valid type
        ...
      ]
    }
    ```

---

### DeleteElements

**Request type**: `delete_elements`

This request delete specified elements.

!!! quote "Request"

    ```json
    {
      ..., // common request data
      "type": "delete_elements",
      "payload": [
        // there are a list of ScAddr's to delete
        2323,
        4322,
        ...
      ]
    }
    ```

!!! quote "Response"

    If command processed, then response status is `true`,
    otherwise status - `false`

---

### SearchByTemplate

**Request type**: `search_template`

This request search constructions by specified template.

!!! quote "Request"
    ```json
    {
      ..., // common request data
      "type": "search_template",
      "payload": [
        // there are a list of template triples
        [
          // triple that represents F_A_A template
          {
            "type": "addr",
            "value": 23123  // ScAddr
          },
          {
            "type": "type",
            "value": 32,    // ScType (should be a variable type)
            "alias": "_edge1"
          },
          {
            "type": "type",
            "value": 2,     // ScType (should be a variable type)
            "alias": "_trg"  // can be used in next triples to ref this element
          }
        ],
        [
          {
            "type": "addr",
            "value": 231342
          },
          {
            "type": "type",
            "value": 2000,
            "alias": "_edge2"
          },
          {
            "type": "alias",
            "value": "_edge1"  // ref to first triple element
          }
        ],
        ...
      ]
    }
    ```

    !!! note ""
        Just elements with `aliases` will be exist in a result

!!! quote "Response"

    ```json
    {
      ..., // common response data
      "payload": {
        /* this map contains index of specified alias in
        * result addrs list
        */
        "aliases": {
          "trg": 2,
          "edge1": 1,
          "edge2": 4
        },
        "addrs": [
          [23123, 412, 423, 231342, 282, 412], // addrs for 1st result
          [23123, 6734, 85643, 231342, 4234, 6734], // addrs for 2nd result
          [23123, 7256, 252, 231342, 654, 7256],
          ...
        ]
      }
    }
    ```

You can use [SCs-text](../other/scs.md) language to make a search requests. Just replace `payload` field with a string, that contains `SCs-text`.
Each vairable identifier (start with `_` symbol) will be used as an alias. All other should be a system identifiers, that will be used to find `ScAddr` of elements.
**Example**:

!!! quote "Request"
    ```json
    {
      ..., // common request data
      "type": "search_template",
      "payload": "person _-> .._p (* _=> nrel_email:: _[] *);;"
    }
    ```

---

### GenerateByTemplate

**Request type**: `generate_template`

This request generate construction by specified template.

!!! quote "Request"

    ```json
    {
      ..., // common request data
      "type": "generate_template",
      "payload": {
        // there are a list of template triples
        "templ":
        [
          [
            // triple that represents F_A_A template
            {
              "type": "addr",
              "value": 23123  // ScAddr
            },
            {
              "type": "type",
              "value": 32,    // ScType (should be a variable type)
              "alias": "_edge1"
            },
            {
              "type": "type",
              "value": 2,     // ScType (should be a variable type)
              "alias": "_trg"  // can be used in next triples to ref this element
            }
          ],
          [
            {
              "type": "addr",
              "value": 231342
            },
            {
              "type": "type",
              "value": 2000,
              "alias": "_edge2"
            },
            {
              "type": "alias",
              "value": "_edge1"  // ref to first triple element
            }
          ],
          ...
        ],
        /* Map of parameters. Each parameter with specified alias in this map.
        * Will be used in template generation. Just `type` values can be replaced
        * by params. Also you can't use alias in parameters for an edges.
        */
        "params": {
          "_trg": 564
        }
      }
    }
    ```

!!! quote "Response"

    ```json
    {
      ..., // common response data
      "payload": {
        /* This map contains index of ScAddr (in `addrs` field) for each alias from template.
        */
        "aliases": {
          "_trg": 2,
          "_edge1": 1,
          "_edge2": 4
        },
        "addrs": [23123, 4953, 564, 231342, 533, 4953]
      }
    }
    ```

Like in [SearchByTemplate](#searchbytemplate) you can use SCs-template for generation. Just replace `templ` field of `payload` to do that. **Example**:
```json
{
  ..., // common request data
  "type": "generate_template",
  "payload": {
    // there are a list of template triples
    "templ": "person _-> .._p (* _=> nrel_email:: _[test@email.com] *);;",
    /* Map of parameters. Each parameter with specified alias in this map.
    * Will be used in template generation. Just `type` values can be replaced
    * by params. Also you can't use alias in parameters for an edges.
    */
    "params": {
      ".._p": 5314
    }
  }
}
```

---

### Events

**Request type**: `events`

!!! quote "Request"

    ```json
    {
      ..., // common request data
      "type": "events",
      "payload": {
        // there are a list of events to create
        "create": [
          {
            "type": "add_output_edge",  // event type
            "addr": 324                 // element to subscribe events
          }
        ],
        // there are a list of events id's to delete
        "delete": [
          2, 4, 5
        ]
      }
    }
    ```

List of possible event types:

* `add_outgoing_edge` - used to subscribe for events, when new outgoing edge added from a specified element;
* `add_ingoing_edge` - the same as `add_outgoing_edge`, but for ingoing edges;
* `remove_outgoing_edge` - used to subscribe for events, when outgoing edge removed from a specified element;
* `remove_ingoing_edge` - the same as `remove_outgoing_edge`, but for outgoing edges;
* `content_change` - used to subscribe for sc-link content changes event;
* `delete_element` - used to subscribe for element deletion.

!!! quote "Response"

    ```json
    {
      ..., // common response data
      "payload": [
        /* List of created event id's. The same order (index)
        * as in request `create` list
        */
        2, 3, 7
      ]
    }
    ```

**Events emit**

Each event can be emitted by server and passed to client. It has such structure:

```json
{
  "id": 2,
  "event": "true",
  // payload contains 1 or 3 elements
  "payload": [
    32,   // ScAddr of subscribed element
    324,  // added/removed edge ScAddr
    34    // source/target of edge
  ]
}
```

!!! info ""
    `2nd` and `3rd` elements used in add/remove edge events

---

### Keynodes

This command allows you to find/resolve keynodes.

**Request type**: `keynodes`

!!! quote "Request"

    ```json
    {
      ..., // common request data
      "type": "keynodes",
      "payload": [
        // there are a list of commands that you need to process
        {
          // command to find element by system identifier
          "command": "find",
          "idtf": "any system identifier"
        },
        {
          // Command to resolve element. If it doesn't exist, then create new one.
          // elType - element type, that will be used to create it.
          // Should be a any node type.
          "command": "resolve",
          "idtf": "any system identifier",
          "elType": 32
        },
        ... // any commands
      ]
    }
    ```


!!! quote "Response"

    ```json
    {
      ..., // common response data
      "payload": [
        /* List of resolved ScAddr values (numbers).
        * Each element with index N is a result of N command in a request.
        * So length of this array should be equal to length
        * of commands array in request.
        * 0 - is an invalid ScAddr (not found/resolved)
        */
      ]
    }
    ```

!!! example "Example"

    ```json
    // request
    {
      "id": 1,
      "type": "keynodes",
      "payload": [
        {
          "command": "find",
          "idtf": "any system identifier that NOT exist"
        },

        {
          "command": "find",
          "idtf": "any system identifier that exist"
        },

        {
          "command": "resolve",
          "idtf": "NOT exist",
          "elType": 1
        },

        {
          "command": "resolve",
          "idtf": "exist",
          "elType": 1
        }
      ]
    }

    // response
    {
      "id": 1,
      "status": true,
      "payload": [
        0,
        321,
        435,
        324
      ]
    }
    ```

---

### Content

This command allow to work with ScLink contents.

**Request type**: `content`

!!! quote "Request"

    ```json
    {
      ..., // common request data
      "type": "content",
      "payload": [
        // there are a list of commands that you need to process
        {
          // command to set ScLink content
          "command": "set",
          "type": "int",  // content type
          "data": 67,     // content could be a string, number
          "addr": 3123    // ScAddr of ScLink to change content
        },
        {
          // Comman to get link content
          "command": "get",
          "addr": 232       // ScAddr of ScLink to get content
        },
        ... // any commands
      ]
    }
    ```

List of possible content types:

* `int` - for integer values;
* `float` - for float values;
* `string` - for string values;
* `binary` - for binary values.

!!! quote "Response"

    ```json
    {
      ..., // common response data
      "payload": [
        // List of command results
        true,      // true or false for a set command result
        // for get command it returns content with a type
        {
          "value": 56.7,  // value will be a null, if content doesn't exist
          "type": "float"
        },
        ... // other command results
      ]
    }
    ```

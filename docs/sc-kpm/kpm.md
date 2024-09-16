## ScAgent Implementations

### Get Decomposition Agent

It is an agent that get decomposition of subject domain in JSON format

**Action class:**

`action_get_decomposition`

**Parameters:**

* `subjDomain` - subject domain node;
* `level` - link that stores decomposition level (unnecessary parameter, by default value of `level` parameter is `1`);
* `lang` - language node;
* `relation of decomposition` - relation node by which an entity is decomposed.

**Example:**

Example of an input structure:

<scg src="../images/get-decomposition-agent/input.gwf"/></scg>

Example of an output structure:

<scg src="../images/get-decomposition-agent/output.gwf"></scg>

NodeLink result:

```json
{
   <main_entity_addr>: {
      "decomposition": {
            <entity_1_addr>: {
               "decomposition": null,
               "idtf": "Entity 1",
               "position": 0
            },
            <entity_2_addr>: {
               "decomposition": null,
               "idtf": "Entity 2",
               "position": 1
            },
            <entity_3_addr>: {
               "decomposition": null,
               "idtf": "Entity 3",
               "position": 2
            },
            <entity_4_addr>: {
               "decomposition": {
                  <entity_4_1_addr>: {
                     "decomposition": null,
                     "idtf": "Entity 4.1",
                     "position": 0
                  },
                  <entity_4_2_addr>: {
                     "decomposition": null,
                     "idtf": "Entity 4.2",
                     "position": 1
                  },
               },
               "idtf": "Entity 4",
               "position": 3
            },
      },
      "idtf": "Main Entity",
      "position": 0
   }
}
```

**Result:**

* `SC_RESULT_OK` - result link is generated.
* `SC_RESULT_ERROR` - internal error in the link generation.
* `SC_RESULT_ERROR_INVALID_PARAMS` - internal error. This happens when subject domain node is invalid.

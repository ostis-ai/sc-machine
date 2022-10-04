Sc-elements identification, string literals and images storing, as well as their processing takes place in 
<b>a file storage</b> module. A file storage is a software functional module that uses a <b>sc-dictionary</b> 
that stores and processes pairs of the form <i><string, files(sc-links)></i>. 

<b>A sc-dictionary</b> itself is universal and is able to store objects and arrays of objects of any type by key in the 
form of a string. The dictionary structure is programmatically <b>a prefix tree</b>, where intermediate nodes are common 
substrings for strings stored in the tree, and terminal nodes are containers for storing data by value of strings added to 
a dictionary tree.

<div style="text-align:center"><img src ="../images/storage/sc-dictionary.png" /></div>

A sc-dictionary implementation based on the prefix tree allows:

+ to represent data in a hierarchical fashion;
+ to place and receive data for sublinear time from the length of the string (key);
+ to store common leading substrings of multiple strings in one place in the shared memory dedicated to the tree;
+ to reduce memory size for storing strings in multiple languages. Techniques such as alphabet reduction can alleviate 
high space complexity by reinterpreting the original string as a long string in a smaller alphabet;

## Sc-dictionary implementation

A file storage and sc-dictionary are implemented by С language means. Together with the implementation of 
the file storage and dictionary, support for the 
[C dictionary API](https://github.com/ostis-ai/sc-machine/tree/main/sc-memory/sc-core/sc-store/sc-container/sc-dictionary/sc-dictionary.h)
is provided.

Common API methods of sc-dictionary implementation and their description are represented in the next table:

<table>
  <tr>
    <th>C API method</th>
    <th>Specification</th>
  </tr>

  <tr>
    <td>sc_bool sc_dictionary_initialize(sc_dictionary ** dictionary)</td>
    <td>It initializes sc-dictionary tree structure with standard memory configuration. If a pointer to sc-dictionary had 
        been not <b>null_ptr</b> at the time of memory setup, then the method returns SC_FALSE.</td>
  </tr>

  <tr>
    <td>sc_bool sc_dictionary_destroy(sc_dictionary * dictionary)</td>
    <td>It destroys sc-dictionary tree structure, freeing memory for all nodes ib tree. If a pointer to sc-dictionary had 
        been <b>null_ptr</b> at the time of memory freeing, then the method returns SC_FALSE.</td>
  </tr>

  <tr>
    <td>sc_dictionary_node * sc_dictionary_append(sc_dictionary * dictionary, sc_char * sc_string, sc_uint32 size, void * value)</td>
    <td>It places data or data array by key string. Moreover, if earlier in the tree there was a string that has a common 
        prefix with to append, then the rest of the original string is appended to the tree node at which this prefix ends.
        If other data stored by the original string, then it will be replaced by new one. The method return terminal node,
        in which has been stored data by key string.</td>
  </tr>

  <tr>
    <td>sc_bool sc_dictionary_remove(sc_dictionary * dictionary, const sc_char * sc_string)</td>
    <td>It removes key string and data stored by it from sc-dictionary. If sc-dictionary has not such key string, 
        then method returns SC_FALSE.</td>
  </tr>

  <tr>
    <td>sc_bool sc_dictionary_is_in(sc_dictionary * dictionary, const sc_char * sc_string)</td>
    <td>It checks for key existence in sc-dictionary. If sc-dictionary has not such key string, 
        then method returns SC_FALSE.</td>
  </tr>

  <tr>
    <td>sc_list * sc_dictionary_get(sc_dictionary * dictionary, const sc_char * sc_string)</td>
    <td>It gets data or data array by key string. If sc-dictionary has not such key string, 
        then method returns empty sc-list.</td>
  </tr>

  <tr>
    <td>void sc_dictionary_visit_down_nodes(sc_dictionary * dictionary, void (*callable)(sc_dictionary_node *, void **), void ** dest)</td>
    <td>It visits up down all nodes and call specified routine for them. A result of procedure completion saves into 'dest'.</td>
  </tr>

  <tr>
    <td>void sc_dictionary_visit_up_nodes(sc_dictionary * dictionary, void (*callable)(sc_dictionary_node *, void **), void ** dest)</td>
    <td>It visits down up all nodes and call specified routine for them. A result of procedure completion saves into 'dest'.</td>
  </tr>
</table>

A sc-dictionary implementation uses [sc-list structure](https://github.com/ostis-ai/sc-machine/tree/main/sc-memory/sc-core/sc-store/sc-container/sc-list/sc-list.h)
to store arrays of data. A sc-container-iterator is used to iterate on sc-list elements.

## Usage example

```cpp
#include <stdio.h>

#include "sc-container/sc-dictionary/sc_dictionary.h"
#include "sc-container/sc-list/sc_list.h"

int main()
{
  sc_dictionary * dict;
  sc_dictionary_initialize(&dict);
  
  sc_char * sc_string = "sc_dictionary";
  sc_uint32 len = strlen(sc_string);
  sc_uint8 value = 10;
  sc_dictionary_append(sc_dictionary, sc_string, len, (void *)&value);
  
  sc_uint8 sum = 0;
  sc_list * list = sc_dictionary_get(sc_string);
  sc_iterator * it = sc_list_iterator(list);
  while (sc_iterator_next(it))
  {
    sc_uint8 v = *((sc_uint8)sc_iterator_get(it));
    sum += v;
    std::cout << v << std::endl; 
  }
  sc_iterator_destroy(it);
  
  sc_dictionary_destroy(dict);
  
  return 0;
}
```

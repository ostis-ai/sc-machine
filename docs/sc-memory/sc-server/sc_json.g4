grammar sc_json;

sc_json_text
  : sc_json_command
  | sc_json_command_answer
  ;

sc_json_command
  : '{'
        '"id"' ':' NUMBER ','
        sc_json_command_type_and_payload
    '}'
  ;

sc_json_command_type_and_payload
  : sc_json_command_healthcheck
  | sc_json_command_create_elements
  | sc_json_command_create_elements_by_scs
  | sc_json_command_check_elements
  | sc_json_command_delete_elements
  | sc_json_command_handle_keynodes
  | sc_json_command_handle_link_contents
  | sc_json_command_search_template
  | sc_json_command_generate_template
  | sc_json_command_handle_events
  | sc_json_command_answer_init_event
  ;

sc_json_command_answer
  : '{'
        '"id"' ':' NUMBER ','
        '"status"' ':' BOOL ','
        sc_json_command_answer_payload
    '}'
  ;

sc_json_command_answer_payload
  : sc_json_command_answer_healthcheck
  | sc_json_command_answer_create_elements
  | sc_json_command_answer_create_elements_by_scs
  | sc_json_command_answer_check_elements
  | sc_json_command_answer_delete_elements
  | sc_json_command_answer_handle_keynodes
  | sc_json_command_answer_handle_link_contents
  | sc_json_command_answer_search_template
  | sc_json_command_answer_generate_template
  | sc_json_command_answer_handle_events
  ;

sc_json_command_healthcheck
  : '"type"' ':' '"healthcheck"'
  ;

sc_json_command_answer_healthcheck
  : '"OK"'
  | '"NO"'
  ;

sc_json_command_create_elements
  : '"type"' ':' '"create_elements"' ','
    '"payload"' ':'
    '['(
        '{'
            '"el"' ':' '"node"' ','
            '"type"' ':' SC_NODE_TYPE ','
        '}' ','
        |
        '{'
            '"el"' ':' '"link"' ','
            '"type"' ':' SC_LINK_TYPE ','
            '"content"' ':' NUMBER_CONTENT | STRING_CONTENT
        '}' ','
        |
        '{'
            '"el"' ':' '"edge"' ','
            '"type"' ':' SC_EDGE_TYPE ','
            '"src"' ':' (
            '{'
                '"type"' ':' '"ref"' ','
                '"value"' ':' NUMBER
            '}' ','
            |
            '{'
                '"type"' ':' '"addr"' ','
                '"value"' ':' SC_ADDR_HASH
            '}' ','
            |
            '{'
                '"type"' ':' '"idtf"' ','
                '"value"' ':' SC_NODE_IDTF
            '}' ','
            )
            '"trg"' ':' (
            '{'
                '"type"' ':' '"ref"' ','
                '"value"' ':' NUMBER
            '}' ','
            |
            '{'
                '"type"' ':' '"addr"' ','
                '"value"' ':' SC_ADDR_HASH
            '}' ','
            |
            '{'
                '"type"' ':' '"idtf"' ','
                '"value"' ':' SC_NODE_IDTF
            '}' ','
            )
        '}' ','
        |
        scs_text ','
    )*']' ','
  ;

sc_json_command_answer_create_elements
  : '"payload"' ':'
    '['
        (SC_ADDR_HASH ',')*
    ']' ','
  ;

sc_json_command_create_elements_by_scs
  : '"type"' ':' '"create_elements_by_scs"' ','
    '"payload"' ':'
    '['(
        scs_text ','
    )*']' ','
  ;

sc_json_command_answer_create_elements_by_scs
  : '"type"' ':' '"check_elements"' ','
    '"payload"' ':'
    '['
        (BOOL ',')*
    ']' ','
  ;

sc_json_command_check_elements
  : '"type"' ':' '"check_elements"' ','
    '"payload"' ':'
    '['
        (SC_ADDR_HASH ',')*
    ']' ','
  ;

sc_json_command_answer_check_elements
  : '"payload"' ':'
    '['
        (SC_ADDR_TYPE ',')*
    ']' ','
  ;

sc_json_command_delete_elements
  : '"type"' ':' '"delete_elements"' ','
     '"payload"' ':'
     '['
         (SC_ADDR_HASH ',')*
     ']' ','
  ;

sc_json_command_answer_delete_elements
  : '"payload"' ':'
    '['']' ','
  ;

sc_json_command_handle_keynodes
  : '"type"' ':' '"keynodes"' ','
     '"payload"' ':'
     '['(
         '{'
             '"command"' ':' '"find"' ','
             '"idtf"' ':' SC_NODE_IDTF ','
         '}' ','
         |
         '{'
             '"command"' ':' '"resolve"' ','
             '"idtf"' ':' SC_NODE_IDTF ','
             '"elType"' ':' SC_ADDR_TYPE
         '}' ','
     )']' ','
  ;

sc_json_command_answer_handle_keynodes
  : '"payload"' ':'
    '['
        (SC_ADDR_HASH ',')*
    ']' ','
  ;

sc_json_command_handle_link_contents
  : '"type"' ':' '"content"' ','
     '"payload"' ':'
     '['(
         '{'
             '"command"' ':' '"set"' ','
             '"type"' ':' SC_LINK_CONTENT_TYPE ','
             '"data"' ':' NUMBER_CONTENT | STRING_CONTENT ','
             '"addr"' ':' SC_ADDR_HASH ','
         '}' ','
         |
         '{'
             '"command' ':' '"get"' ','
             '"addr"' ':' SC_ADDR_HASH ','
         '}' ','
         |
         '{'
             '"command' ':' '"find"' ','
             '"data"' ':' NUMBER_CONTENT | STRING_CONTENT ','
         '}' ','
         |
         '{'
              '"command' ':' '"find_links_by_substr"' ','
			  '"data"' ':' NUMBER_CONTENT | STRING_CONTENT ','
          '}' ','
         |
         '{'
              '"command' ':' '"find_strings_by_substr"' ','
              '"data"' ':' NUMBER_CONTENT | STRING_CONTENT ','
         '}' ','
     )*']' ','
  ;

sc_json_command_answer_handle_link_contents
  : '"payload"' ':'
     '['(
         BOOL
         |
         '{'
             '"value"' ':' NUMBER_CONTENT | STRING_CONTENT  ','
             '"type"' ':' SC_LINK_CONTENT_TYPE ','
         '}' ','
         |
         '['
             (SC_ADDR_HASH ',')*
         ']' ','
     )*']' ','
  ;

sc_json_command_search_template
  : '"type"' ':' '"search_template"' ','
    sc_json_command_template_payload
  ;

sc_json_command_template_payload
  : '"payload"' ':'
    '{'
        '"templ"' ':'
        ('['
            ('['
                (
                '{'
                    '"type"' ':' '"addr"' ','
                    '"value"' ':' SC_ADDR_HASH ','
                    ('"alias"' ':' SC_ALIAS ',')?
                '}' ','
                |
                '{'
                    '"type"' ':' '"type"' ','
                    '"value"' ':' SC_ADDR_TYPE ','
                    ('"alias"' ':' SC_ALIAS ',')?
                '}' ','
                )
                '{'
                    '"type"' ':' '"type"' ','
                    '"value"' ':' SC_EDGE_TYPE ','
                    ('"alias"' ':' SC_ALIAS ',')?
                '}' ','
                (
                '{'
                    '"type"' ':' '"addr"' ','
                    '"value"' ':' SC_ADDR_HASH ','
                    ('"alias"' ':' SC_ALIAS ',')?
                '}' ','
                |
                '{'
                    '"type"' ':' '"type"' ','
                    '"value"' ':' SC_ADDR_TYPE ','
                    ('"alias"' ':' SC_ALIAS ',')?
                '}' ','
                )
            ']' ',')*
        ']' ','
        |
		scs_text ','
		| '{' '"type"' ':' '"addr"' ',' '"value"' ':' SC_ADDR_HASH ',' '}' ','
	    | '{' '"type"' ':' '"idtf"' ',' '"value"' ':' SC_NODE_IDTF ',''}' ',')
        '"params"' ':'
        '{'
            (SC_ALIAS ':' (SC_ADDR_HASH | SC_ALIAS) ',')*
        '}' ','
    '}' ','
  ;

scs_text
  : STRING_CONTENT
  ;

sc_json_command_answer_search_template
  : '"payload"' ':'
    '{'
        '"addrs"' ':'
        '['
            ('['
                (SC_ADDR_HASH ',')*
            ']' ',')*
        ']' ','
        '"aliases"' ':'
        '{'
            (SC_ALIAS ':' NUMBER ',')*
        '}' ','
    '}' ','
  ;

sc_json_command_generate_template
  : '"type"' ':' '"generate_template"' ','
    sc_json_command_template_payload
  ;

sc_json_command_answer_generate_template
  : '"payload"' ':'
    '{'
        '"addrs"' ':'
        '['
            (SC_ADDR_HASH ',')*
        ']' ','
        '"aliases"' ':'
        '{'
            (SC_ALIAS ':' NUMBER ',')*
        '}' ','
    '}' ','
  ;

sc_json_command_handle_events
  : '"type"' ':' '"events"' ','
    '"payload"' ':'
    '{'
        ('"create"' ':'
        '['
            ('{'
                '"type"' ':' SC_EVENT_TYPE ','
                '"addr"' ':' SC_ADDR_HASH ','
            '}' ',')*
        ']' ',')?
        ('"delete"' ':'
        '['
            (NUMBER ',')*
        ']' ',')?
    '}' ','
  ;

sc_json_command_answer_handle_events
  : '"payload"' ':'
    '['
        (SC_ADDR_HASH ',')*
    ']' ','
  ;

sc_json_command_answer_init_event
  : '"event"' ':' '1' ','
    '"payload"' ':'
    '['
	    (SC_ADDR_HASH ',')*
    ']' ','
  ;

SC_LINK_CONTENT_TYPE
  : '"string"'
  | '"int"'
  | '"float"'
  | '"binary"'
  ;

SC_EVENT_TYPE
  : '"add_outgoing_edge"'
  | '"add_ingoing_edge"'
  | '"remove_outgoing_edge"'
  | '"remove_ingoing_edge"'
  | '"content_change"'
  | '"delete_element"'
  ;

SC_NODE_TYPE
  : '1'     // sc_type_node
  | '128'   // sc_type_node_tuple
  | '256'   // sc_type_node_struct
  | '512'   // sc_type_node_role
  | '1024'  // sc_type_node_norole
  | '2048'  // sc_type_node_class
  | '4096'  // sc_type_node_abstract
  | '8192'  // sc_type_node_material

  | '33'    // sc_type_node_const
  | '168'   // sc_type_node_const_tuple
  | '288'   // sc_type_node_const_struct
  | '544'   // sc_type_node_const_role
  | '1056'  // sc_type_node_const_norole
  | '2080'  // sc_type_node_const_class
  | '4128'  // sc_type_node_const_abstract
  | '8224'  // sc_type_node_const_material

  | '65'    // sc_type_node_var
  | '192'   // sc_type_node_var_tuple
  | '320'   // sc_type_node_var_struct
  | '576'   // sc_type_node_var_role
  | '1088'  // sc_type_node_var_norole
  | '2112'  // sc_type_node_var_class
  | '4160'  // sc_type_node_var_abstract
  | '8256'  // sc_type_node_var_material
  ;

SC_EDGE_TYPE
  : '4'     // sc_type_edge_common
  | '8'     // sc_type_arc_common
  | '16'    // sc_type_arc_access

  | '2212'  // sc_type_edge_common_const_pos_perm
  | '2216'  // sc_type_arc_common_const_pos_perm
  | '2224'  // sc_type_arc_access_const_pos_perm
  | '2340'  // sc_type_edge_common_const_neg_perm
  | '2344'  // sc_type_arc_common_const_neg_perm
  | '2352'  // sc_type_arc_access_const_neg_perm
  | '2596'  // sc_type_edge_common_const_fuz_perm
  | '2600'  // sc_type_arc_common_const_fuz_perm
  | '2608'  // sc_type_arc_access_const_fuz_perm

  | '1188'  // sc_type_edge_common_const_pos_temp
  | '1192'  // sc_type_arc_common_const_pos_temp
  | '1200'  // sc_type_arc_access_const_pos_temp
  | '1316'  // sc_type_edge_common_const_neg_temp
  | '1320'  // sc_type_arc_common_const_neg_temp
  | '1328'  // sc_type_arc_access_const_neg_temp
  | '1572'  // sc_type_edge_common_const_fuz_temp
  | '1576'  // sc_type_arc_common_const_fuz_temp
  | '1584'  // sc_type_arc_access_const_fuz_temp

  | '2244'  // sc_type_edge_common_var_pos_perm
  | '2248'  // sc_type_arc_common_var_pos_perm
  | '2256'  // sc_type_arc_access_var_pos_perm
  | '2372'  // sc_type_edge_common_var_neg_perm
  | '2376'  // sc_type_arc_common_var_neg_perm
  | '2384'  // sc_type_arc_access_var_neg_perm
  | '2628'  // sc_type_edge_common_var_fuz_perm
  | '2632'  // sc_type_arc_common_var_fuz_perm
  | '2640'  // sc_type_arc_access_var_fuz_perm

  | '1220'  // sc_type_edge_common_var_pos_temp
  | '1224'  // sc_type_arc_common_var_pos_temp
  | '1232'  // sc_type_arc_access_var_pos_temp
  | '1348'  // sc_type_edge_common_var_neg_temp
  | '1352'  // sc_type_arc_common_var_neg_temp
  | '1360'  // sc_type_arc_access_var_neg_temp
  | '1604'  // sc_type_edge_common_var_fuz_temp
  | '1608'  // sc_type_arc_common_var_fuz_temp
  | '1616'  // sc_type_arc_access_var_fuz_temp
  ;

SC_LINK_TYPE
  : '2'     // sc_type_link
  | '34'    // sc_type_link_const
  | '66'    // sc_type_link_var
  ;

SC_ADDR_TYPE
  : SC_NODE_TYPE
  | SC_EDGE_TYPE
  | SC_LINK_TYPE
  ;

fragment CONTENT_ESCAPED
  : '\\' ('[' | ']' | '\\')
  ;

fragment CONTENT_SYBMOL
  : (CONTENT_ESCAPED | ~('[' | ']' | '\\'))
  ;

fragment CONTENT_SYBMOL_FIRST_END
  : (CONTENT_ESCAPED | ~('[' | ']' | '\\' | '*'))
  ;

CONTOUR_BEGIN
  : '[*'
  ;

CONTOUR_END
  : '*]'
  ;

CONTENT
  : CONTENT_SYBMOL_FIRST_END CONTENT_SYBMOL*
  ;

CONTENT_BODY
  : '[]'
  | '[' CONTENT ']'
  ;

NUMBER_CONTENT
  : NUMBER
  ;

STRING_CONTENT
  : '"' CONTENT '"'
  ;

SC_ADDR_HASH
  : NUMBER
  ;

SC_NODE_IDTF
  : STRING_CONTENT
  ;

SC_ALIAS
  : STRING_CONTENT
  ;

NUMBER
  : '0'
  | '1'..'9' '0'..'9'*
  ;

BOOL
  : '0'
  | '1'
  ;

ID_SYSTEM
  : ('a'..'z'|'A'..'Z'|'_'|'.'|'0'..'9')+
  ;

WS
  : ( ' '
  | '\t'
  | '\r'
  | '\n'
  ) -> channel(HIDDEN)
  ;

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

SC_ADDR_TYPE
  : SC_NODE_TYPE
  | SC_EDGE_TYPE
  | SC_LINK_TYPE
  ;

fragment CONTENT_ESCAPED
  : '\\' ('[' | ']' | '\\')
  ;

fragment CONTENT_SYMBOL
  : (CONTENT_ESCAPED | ~('[' | ']' | '\\'))
  ;

fragment CONTENT_SYMBOL_FIRST_END
  : (CONTENT_ESCAPED | ~('[' | ']' | '\\' | '*'))
  ;

CONTOUR_BEGIN
  : '[*'
  ;

CONTOUR_END
  : '*]'
  ;

CONTENT
  : CONTENT_SYMBOL_FIRST_END CONTENT_SYMBOL*
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

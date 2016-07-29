grammar scs;

options 
{
    k = 3;
    language = C;
    output = AST;
    ASTLabelType=pANTLR3_BASE_TREE;
}

tokens {
  SEP_SENTENCE = ';;';
  SEP_SIMPLE  =  '|';
  SEP_LPAR = '(';
  SEP_RPAR = ')';
  SEP_ATTR_CONST = ':';
  SEP_ATTR_VAR = '::';
  SEP_IDTF = ';';
  SEP_LINT = '(*';
  SEP_RINT = '*)';
  SEP_LTUPLE = '<';
  SEP_RTUPLE = '>';
  SEP_LSET = '{';
  SEP_RSET = '}';
  SEP_LCONTENT = '[';
  SEP_RCONTENT = ']';
  SEP_ASSIGN = '=';
  
}

@lexer::includes {
    #include "../parseutils.h"
}

@parser::includes {
    #include "../parseutils.h"
}

@parser::apifuncs {
    RECOGNIZER->displayRecognitionError = displayRecognitionError;
}

@lexer::apifuncs {
    LEXER->rec->displayRecognitionError = displayLexerError;
}

@rulecatch
{
    if (HASEXCEPTION())
    {
        parseError((const char*)EXCEPTION->name, EXCEPTION->line);
    }
}

/* Parser rules */
syntax
    : (sentence SEP_SENTENCE!)* EOF
    ;
    
sentence
    : sentence_level1
    | sentence_level2_6
    | sentence_assignment
    ;
    

sentence_level2_6
    :
    | (idtf c1=CONNECTORS a1=attrs_idtf_list -> ^($c1 idtf $a1))
    (SEP_IDTF c2=CONNECTORS a2=attrs_idtf_list -> $sentence_level2_6 ^($c2 idtf $a2))*
    ;
    
sentence_level1
    : idtf_level1 SEP_SIMPLE idtf_level1 SEP_SIMPLE idtf_level1 -> ^(SEP_SIMPLE idtf_level1+)
    ;
    
    
sentence_internal_list
    : SEP_LINT  (sentence_internal SEP_SENTENCE)* SEP_RINT -> ^(SEP_LINT sentence_internal+)
    ;
    
sentence_assignment
    : idtf SEP_ASSIGN idtf -> ^(SEP_ASSIGN idtf+)
    ;
    
sentence_internal
    : CONNECTORS^ attrs_idtf_list
    ;
    
attrs_idtf_list
    : idtf_attrs  (SEP_IDTF! idtf_attrs)*
    ;
    
idtf_attrs
    : attrs_list idtf_internal
    ;
    
attrs_list
    : (attr_sep)* 
    ;
    
attr_sep
    : idtf_level1 SEP_ATTR_VAR^
    | idtf_level1 SEP_ATTR_CONST^
    ;
    
idtf_internal
    : idtf^ sentence_internal_list?
    ;

idtf_tuple
    : SEP_LTUPLE^ attrs_idtf_list  SEP_RTUPLE!
    ;
    
idtf_set
    : SEP_LSET^ attrs_idtf_list SEP_RSET!
    ;

idtf
    : idtf_level1
    | idtf_edge
    | idtf_tuple
    | idtf_set
    | CONTENT
    ;

idtf_level1
    : ID_SYSTEM
    | LINK
    ;

idtf_edge
    : SEP_LPAR^ idtf CONNECTORS idtf SEP_RPAR! 
    ;

// --------------- separators -----------------


ID_SYSTEM  :    ('a'..'z'|'A'..'Z'|'_'|'.'|'0'..'9'|'#')+
    ;

COMMENT
    :   '//' ~('\n'|'\r')* '\r'? '\n' {$channel=HIDDEN;}
    |   ('/*' | '/!*') ( options {greedy=false;} : . )* '*/' {$channel=HIDDEN;}
    ;

CONTENT
    @init{int count = 1;}
  	: '_'? SEP_LCONTENT
  	  (
  	  { count > 0 }? =>
  	   	 (
  		  ~ (SEP_LCONTENT | SEP_RCONTENT)
	  	  | SEP_LCONTENT { count++; } 
  		  | SEP_RCONTENT { count--; }
	  	  )
	  )*
  	;


LINK
     :  '"' (   ~('"')  | '\\"'  )* '"'
     ;
    
CONNECTORS  :  ( 
                '<>'
              | '>'
              | '<'
              | '..>'
              | '<..'
              | '->'
              | '<-'
              | '<=>'
              | '=>'
              | '<='
              | '-|>'
              | '<|-'
              | '-/>'
              | '</-'
              | '~>'
              | '<~'
              | '~|>'
              | '<|~'
              | '~/>'
              | '</~'
              | '_<>'
              | '_>'
              | '_<'
              | '_..>'
              | '_<..'
              | '_->'
              | '_<-'
              | '_<=>'
              | '_=>'
              | '_<='
              | '_-|>'
              | '_<|-'
              | '_-/>'
              | '_</-'
              | '_~>'
              | '_<~'
              | '_~|>'
              | '_<|~'
              | '_~/>'
              | '_</~' )
              ;

fragment
HEX_DIGIT : ('0'..'9'|'a'..'f'|'A'..'F') ;

WS  :   ( ' '
        | '\t'
        | '\r'
        | '\n'
        ) {$channel=HIDDEN;}
    ;

fragment
OCTAL_ESC
    :   '\\' ('0'..'3') ('0'..'7') ('0'..'7')
    |   '\\' ('0'..'7') ('0'..'7')
    |   '\\' ('0'..'7')
    ;

fragment
UNICODE_ESC
    :   '\\' 'u' HEX_DIGIT HEX_DIGIT HEX_DIGIT HEX_DIGIT
    ;


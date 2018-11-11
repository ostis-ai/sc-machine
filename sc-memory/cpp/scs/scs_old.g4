grammar scs;

options 
{
    k = 3;
    language = Cpp;
}

@lexer::header
{
#include "scs_traits.hpp"
#include "scs_parser.hpp"
}

@parser::header
{
#include "scs_traits.hpp"
#include "scsLexer.hpp"
#include "scs_parser.hpp"

#include "../sc_debug.hpp"
}

@parser::members {
public:
  void setParser(scs::Parser * parser)
  {
    SC_ASSERT(parser, ());
    m_parser = parser;
  }

  void displayRecognitionError(ANTLR_UINT8 ** tokenNames, Parser::ExceptionBaseType * ex)
  {
    scsParserImplTraits::StringStreamType errtext;
    // See if there is a 'filename' we can use
    //
    if( !ex->get_streamName().empty() )
    {
        errtext << "File:" << ex->get_streamName() << std::endl;
    }

    // Next comes the line number
    //
    errtext << "Line: " << this->get_rec()->get_state()->get_exception()->get_line() << std::endl;
    errtext << "Error: " << this->get_rec()->get_state()->get_exception()->getType()
                         << ". "
                         << this->get_rec()->get_state()->get_exception()->get_message();

    // Prepare the knowledge we know we have
    //
    const CommonTokenType* theToken   = this->get_rec()->get_state()->get_exception()->get_token();
    StringType ttext = theToken->toString();

    errtext << ", at offset "
            << this->get_rec()->get_state()->get_exception()->get_charPositionInLine();
            
    if  (theToken != NULL)
    {
        if (theToken->get_type() != CommonTokenType::TOKEN_EOF && !ttext.empty())
        {
            errtext << " near " << ttext;
        }
    }

//  ex->displayRecognitionError( tokenNames, errtext );
    SC_THROW_EXCEPTION(utils::ExceptionParseError, errtext.str());
  }
  
private:
  scs::Parser * m_parser;

public:
}

@lexer::namespace { scs }
@parser::namespace { scs }

@parser::members
{

}

ID_SYSTEM
    :   ('a'..'z'|'A'..'Z'|'_'|'.'|'0'..'9')+
    ;
   
EDGE_ATTR
    :   (':'|'::')
    ;

COMMENT
    :   '//' ~('\n'|'\r')* '\r'? '\n' {$channel=HIDDEN;}
    |   ('/*' | '/!*') ( options {greedy=false;} : . )* '*/' {$channel=HIDDEN;}
    ;


LINK
     :  '"' (   ~('"')  | '\\"'  )* '"'
     ;
     
CONTENT
    @init{int count = 1;}
    : ('_')? '['
      (
         { count > 0 }? =>
         (
           ~ ('[' | ']')
           | '[' { count++; } 
           | ']' { count--; }
         )
      )*
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


/* Parser rules */
syntax
    : (sentence ';;')* EOF
    ;
       
sentence
    // level 1
    : sentence_lvl1
    | sentence_lvl_common
    ;
    
idtf_lvl1_preffix returns [std::string text]
    : type=('sc_node'
    | 'sc_link'
    | 'sc_arc_common'
    | 'sc_edge'
    | 'sc_arc_main'
    | 'sc_arc_access')
    { text = $type->getText(); }
    ;
       
idtf_lvl1 returns [ElementHandle handle]
    : (type=idtf_lvl1_preffix '#')? ID_SYSTEM { $handle = m_parser->ProcessIdentifierLevel1($type.text, $ID_SYSTEM->getText()); }
    ;

idtf_system returns [ElementHandle handle]
    : ID_SYSTEM { $handle = m_parser->ProcessIdentifier($ID_SYSTEM->getText()); }
    ;

idtf_edge returns [ElementHandle handle]
    : '(' src=idtf_system
          c=connector attrs=attr_list?
          trg=idtf_system
      ')'
      
      {
        ElementHandle const edge = m_parser->ProcessConnector(c);
        m_parser->ProcessTriple(src, edge, trg);
        
        // append attributes
        APPEND_ATTRS(attrs, edge);
        
        handle = edge;
      }
    ;
    
idtf_set returns [ElementHandle handle]
    : '{' { handle = m_parser->ProcessIdentifier("..."); }
         a1=attr_list? i1=idtf_common
         {
           ElementHandle const edge = m_parser->ProcessConnector("->");
           m_parser->ProcessTriple(handle, edge, i1);
           
           APPEND_ATTRS(a1, edge);
         }
         (';'
          a2=attr_list? i2=idtf_common
          {
            ElementHandle const edge = m_parser->ProcessConnector("->");
            m_parser->ProcessTriple(handle, edge, i2);
            APPEND_ATTRS(a2, edge);
          }
         )*
       '}'
    ;

idtf_common returns [ElementHandle handle]
    : is=idtf_system { handle = is; }
    | ie=idtf_edge { handle = ie; }
    | it=idtf_set { handle = it; }
    | CONTENT { handle = m_parser->ProcessContent($CONTENT->getText()); }
    | LINK { handle = m_parser->ProcessLink($LINK->getText()); }
    ;

idtf_list returns [std::vector<ElementHandle> result]
    : i1=idtf_common { result.push_back(i1); }
      internal_sentence_list[i1]?
      (';'
       i2=idtf_common { result.push_back(i2); }
       internal_sentence_list[i2]?
      )*
    ;
    
internal_sentence [ElementHandle source]
    : c=connector attrs=attr_list? targets=idtf_list
      {
        for (auto const & trg : targets)
        {
          ElementHandle const edge = m_parser->ProcessConnector(c);
          m_parser->ProcessTriple(source, edge, trg);
          
          APPEND_ATTRS(attrs, edge);
        }
      }
    ;
    
internal_sentence_list [ElementHandle source]
    : '(*' (internal_sentence[source] ';;')+ '*)'
    ;

sentence_lvl1
    : src=idtf_lvl1 '|' edge=idtf_lvl1  '|' trg=idtf_lvl1
    { m_parser->ProcessTriple(src, edge, trg); }
    ;

sentence_lvl_4_list [ElementHandle source]
    : ( c=connector attrs=attr_list? targets=idtf_list
        { 
            for (auto const & t : targets)
            {
                ElementHandle const edge = m_parser->ProcessConnector(c);
                m_parser->ProcessTriple(source, edge, t);
                
                APPEND_ATTRS(attrs, edge);
            }
        }
      )
    ;

sentence_lvl_common
    : source=idtf_common 
      sentence_lvl_4_list[source] (';' sentence_lvl_4_list[source])*
    ;

attr_list returns [std::vector<std::pair<ElementHandle, bool>> attrs]
    : (
        ID_SYSTEM
        EDGE_ATTR
        {
          attrs.emplace_back(m_parser->ProcessIdentifier($ID_SYSTEM->getText()),
                             scs::TypeResolver::IsEdgeAttrConst($EDGE_ATTR->getText()));
        }
      )+
    ;


connector returns [std::string value]
     :  c=( 
         '<>'  | '>'   | '<'   |
         '..>' | '<..' | '->'  |
         '<-'  | '<=>' | '=>'  | '<=' |
         '-|>' | '<|-' | '-/>' | '</-' |
         '~>'  | '<~'  | '~|>' | '<|~' |
         '~/>' | '</~' | '_<>' | '_>'  |
         '_<'  | '_..>'| '_<..'| '_->' |
         '_<-' | '_<=>'| '_=>' | '_<=' |
         '_-|>'| '_<|-'| '_-/>'| '_</-'|
         '_~>' | '_<~' | '_~|>'| '_<|~'|
         '_~/>'| '_</~'
         )
         
         { value = c->getText(); }
      ;

// --------------- separators -----------------

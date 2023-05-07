grammar scs;

options
{
  language = Cpp;
}

tokens
{
}

@parser::header
{
#include "scsLexer.h"
#include "sc-memory/scs/scs_parser.hpp"

#include "sc-memory/sc_debug.hpp"

#define APPEND_ATTRS(__attrs, __connector) \
for (auto const & _a : __attrs) \
{ \
  ElementHandle const attrConnector = m_parser->ProcessConnector(_a.second ? "->" : "_->"); \
  m_parser->ProcessTriple(_a.first, attrConnector, __connector); \
}

#define PARSE_ERROR(line, pos, msg) \
  SC_THROW_EXCEPTION(utils::ExceptionParseError, \
                    "line " << line << ", " << pos << " " << msg);

}

@parser::members {

public:
  void setParser(scs::Parser * parser)
  {
    m_parser = parser;
  }

private:
  scs::Parser * m_parser;

public:

}

content[ElementHandle contentHandle = ElementHandle()]
  returns [ElementHandle handle]
  locals [ bool isVar = false;]
  @init{ $ctx->isVar = false; }
  : ('_' { $ctx->isVar = true; } )?
      c=CONTENT_BODY
    {
      std::string content = $ctx->c->getText();
      $ctx->handle = $contentHandle.IsValid()
        ? m_parser->ProcessLink($contentHandle, content)
        : m_parser->ProcessContent(content, $ctx->isVar);
    }
  ;

contour_begin
  : CONTOUR_BEGIN
  ;

contour_end
  : CONTOUR_END
  ;

contour[ElementHandle contourHandle = ElementHandle()]
  returns [ElementHandle handle]
  : contour_begin
    {
      $ctx->handle = $contourHandle.IsValid() ? $contourHandle : m_parser->ProcessEmptyContour();
      m_parser->ProcessContourBegin();
    }
    ( (sentence_wrap
	| (sentence_lvl_4_list_item[$ctx->handle] (';' sentence_lvl_4_list_item[$ctx->handle])* ';;') )* )
    CONTOUR_END
    {
      m_parser->ProcessContourEnd($ctx->handle);
    }
  ;

connector returns [std::string text]
  : c=('?<=>' | '?=>' | '<=?'
  | '<=>' | '_<=>' | '=>' | '<=' | '_=>' | '<=_'
  | '?.?>' | '<?.?' 
  | '.?>' | '<?.' | '_.?>' | '<?._'
  | '?.>' | '<.?' | '?.|>' | '<|.?' | '?/>' | '</?' 
  | '.>' | '<.' | '.|>' | '<|.' | '/>' | '</' 
  | '_.>' | '<._' | '_.|>' | '<|._' | '_/>' | '</_' 
  | '?-?>' | '<?-?' | '?..?>' | '<?..?' | '?~?>' | '<?~?' | '?%?>' | '<?%?' 
  | '?->' | '<-?' | '?-|>' | '<|-?' | '-?>' | '<?-' | '_-?>' | '<?-_' 
  | '?..>' | '<..?' | '?~>' | '<~?' | '?%>' | '<%?' | '?..|>' | '<|..?' | '?~|>' | '<|~?' | '?%|>' | '<|%?'
  | '..?>' | '<?..' | '~?>' | '<?~' | '%?>' | '<?%' | '_..?>' | '<?.._' | '_~?>' | '<?~_' | '_%?>' | '<?%_'
  | '->' | '<-' | '_->' | '<-_' | '-|>' | '<|-' | '_-|>' | '<|-_'
  | '..>' | '<..' | '_..>' | '<.._' | '~>' | '<~' | '_~>' | '<~_' | '%>' | '<%' | '_%>' | '<%_' 
  | '..|>' | '<|..' | '_..|>' | '<|.._' | '~|>' | '<|~' | '_~|>' | '<|~_' | '%|>' | '<|%' | '_%|>' | '<|%_'
  // deprecated
  | '>' | '<' | '<>' | '_<=' | '_<-' | '_<|-' | '_<~' | '_<|~' )
    {
      $ctx->text = $c->getText();
    }
  ;


// ------------- Rules --------------------

syntax 
  : sentence_wrap* EOF
  ;

sentence_wrap
  : (sentence ';;')
  ;

sentence
  : sentence_lvl1
  | sentence_assign
  | sentence_assign_link
  | sentence_assign_contour
  | sentence_lvl_common
  ;

idtf_alias returns [ElementHandle handle]
  : ALIAS_SYMBOLS
    {
      std::string const _alias = $ALIAS_SYMBOLS->getText();
      $ctx->handle = m_parser->ResolveAlias(_alias);
      if (!$ctx->handle.IsValid())
      {
        PARSE_ERROR($ctx->start->getLine(), $ctx->start->getCharPositionInLine(),
          "Can't resolve alias `" << _alias << "`. You should assign an alias before use.");
      }
    }
  ;

idtf_system returns [ElementHandle handle]
  : ID_SYSTEM
    { $ctx->handle = m_parser->ProcessIdentifier($ID_SYSTEM->getText()); }
  | '...'
    { $ctx->handle = m_parser->ProcessIdentifier("..."); }
  | name=idtf_lvl1_preffix
    { $ctx->handle = m_parser->ProcessIdentifier($ctx->name->text); }
  ;


sentence_assign
  : ALIAS_SYMBOLS '=' i=idtf_common
    {
      m_parser->ProcessAssign($ALIAS_SYMBOLS->getText(), $ctx->i->handle);
    }
  ;

sentence_assign_link
  : a=idtf_system '='
  (content[$ctx->a->handle] | idtf_url[$ctx->a->handle])
  ;

sentence_assign_contour
  : a=idtf_system '=' contour[$ctx->a->handle] (';' internal_sentence[$ctx->a->handle])*
  ;
    
idtf_lvl1_preffix returns [std::string text]
  : type=('sc_common_edge'
  | 'sc_common_arc'
  | 'sc_membership_arc'
  | 'sc_main_arc'

  | 'sc_node'
  | 'sc_link'
  | 'sc_link_class'
  | 'sc_node_tuple'
  | 'sc_node_structure'
  | 'sc_node_class'
  | 'sc_node_role_relation'
  | 'sc_node_non_role_relation'
  | 'sc_node_superclass'
  | 'sc_node_material'
  
  // deprecated
  | 'sc_edge'
  | 'sc_edge_ucommon'
  | 'sc_arc_common'
  | 'sc_edge_dcommon'
  | 'sc_edge_dcommon'
  | 'sc_arc_main'
  | 'sc_edge_main'
  | 'sc_arc_access'
  | 'sc_edge_access'
  | 'sc_node_not_binary_tuple'
  | 'sc_node_struct'
  | 'sc_node_not_relation'
  | 'sc_node_norole_relation')
    {
      $ctx->text = $ctx->type->getText();
    }
  ;
    
idtf_lvl1_value returns [ElementHandle handle]
   : type=idtf_lvl1_preffix '#' i=ID_SYSTEM
    {
      $ctx->handle = m_parser->ProcessIdentifierLevel1($ctx->type->text, $ctx->i->getText());
    }
  ;
    
idtf_lvl1 returns [ElementHandle handle]
  : id=idtf_lvl1_value { $ctx->handle = $ctx->id->handle; }
  | cn=content[] { $ctx->handle = $ctx->cn->handle; }
  | u=idtf_url[] { $ctx->handle = $ctx->u->handle; }
  ;

idtf_connector returns [ElementHandle handle]
  : '(' (src=idtf_atomic | se=idtf_connector)
        c=connector attrs=attr_list?
        (trg=idtf_atomic | te=idtf_connector)
    ')'
    {
      ElementHandle const connector = m_parser->ProcessConnector($ctx->c->text);
      m_parser->ProcessTriple(
        $ctx->src ? $ctx->src->handle : $ctx->se->handle, connector, $ctx->trg ? $ctx->trg->handle : $ctx->te->handle);

      // append attributes
      if ($ctx->attrs != nullptr)
      {
        APPEND_ATTRS($attrs.items, connector);
      }

      $ctx->handle = connector;
    }
  ;
  
idtf_set returns [ElementHandle handle]
  : '{' ctx_s=idtf_set_elements["set"] { $ctx->handle = $ctx->ctx_s->handle; } '}'
  | '<' ctx_v=idtf_set_elements["vector"] { $ctx->handle = $ctx->ctx_v->handle; } '>'
  ;

idtf_set_elements [std::string setType]
  returns [ElementHandle handle]
  locals [ElementHandle prevArc]
  : {
      std::string const setIdtf = "..set_" + std::to_string($ctx->start->getLine()) + "_" + std::to_string($ctx->start->getCharPositionInLine());
      $ctx->handle = m_parser->ProcessIdentifierLevel1("sc_node_tuple", setIdtf);
    }
  a1=attr_list? i1=idtf_common
    {
      ElementHandle const arc = m_parser->ProcessConnector("->");
      m_parser->ProcessTriple($ctx->handle, arc, $ctx->i1->handle);

      if ($ctx->a1 != nullptr)
      {
        APPEND_ATTRS($ctx->a1->items, arc);
      }

      if ($ctx->setType == "vector")
      {
        ElementHandle const relArc = m_parser->ProcessConnector("->");
        ElementHandle const rel = m_parser->ProcessIdentifier("rrel_1");
        m_parser->ProcessTriple(rel, relArc, arc);

        $ctx->prevArc = arc;
      }
    }
    internal_sentence_list[$ctx->i1->handle]?

    (';'
    a2=attr_list? i2=idtf_common
      {
        ElementHandle const arc = m_parser->ProcessConnector("->");
        m_parser->ProcessTriple($ctx->handle, arc, $ctx->i2->handle);

        if ($ctx->a2 != nullptr)
        {
          APPEND_ATTRS($ctx->a2->items, arc);
        }

        if ($ctx->setType == "vector")
        {
          ElementHandle const seqArc = m_parser->ProcessConnector("=>");
          m_parser->ProcessTriple($ctx->prevArc, seqArc, arc);

          ElementHandle const relArc = m_parser->ProcessConnector("->");
          ElementHandle const rel = m_parser->ProcessIdentifier("nrel_basic_sequence");
          m_parser->ProcessTriple(rel, relArc, seqArc);

          $ctx->prevArc = arc;
        }
      }
      internal_sentence_list[$ctx->i2->handle]?
    )*
  ;

idtf_atomic returns [ElementHandle handle]
  : a=idtf_alias { $ctx->handle = $ctx->a->handle; }
  | is=idtf_system { $ctx->handle = $ctx->is->handle; }
  ;

idtf_url[ElementHandle contentHandle = ElementHandle()]
  returns [ElementHandle handle]
  : LINK
    {
      std::string const content = $LINK->getText();
      $ctx->handle = $contentHandle.IsValid()
          ? m_parser->ProcessLink($contentHandle, content, true)
          : m_parser->ProcessFileURL(content);
    }
  ;

idtf_common returns [ElementHandle handle]
  : a=idtf_atomic { $ctx->handle = $ctx->a->handle; }
  | ic=idtf_connector { $ctx->handle = $ctx->ic->handle; }
  | iset=idtf_set { $ctx->handle = $ctx->iset->handle; }
  | ct=contour[] { $ctx->handle = $ctx->ct->handle; }
  | cn=content[] { $ctx->handle = $ctx->cn->handle; }
  | u=idtf_url[] { $ctx->handle = $ctx->u->handle; }
  ;

idtf_list returns [std::vector<ElementHandle> items]
  : i1=idtf_common { $ctx->items.push_back($ctx->i1->handle); }
    internal_sentence_list[$ctx->i1->handle]? 
    (';' 
      i2=idtf_common { $items.push_back($ctx->i2->handle); }
      internal_sentence_list[$ctx->i2->handle]?
    )*
  ;

internal_sentence [ElementHandle source]
  : c=connector attrs=attr_list? targets=idtf_list
    {
      for (auto const & trg : $ctx->targets->items)
      {
        ElementHandle const connector = m_parser->ProcessConnector($c.text);
        m_parser->ProcessTriple(source, connector, trg);
        
        if ($ctx->attrs != nullptr)
        {
          APPEND_ATTRS($ctx->attrs->items, connector);
        }
      }
    }
    | internal_sentence_list[source]
  ;

internal_sentence_list [ElementHandle source]
  : '(*' (internal_sentence[source] ';;')+ '*)'
  ;

sentence_lvl1
   : src=idtf_lvl1 '|' connector_el=idtf_lvl1 '|' trg=idtf_lvl1
    {
      m_parser->ProcessTriple($ctx->src->handle, $ctx->connector_el->handle, $ctx->trg->handle);
    }
  ;

sentence_lvl_4_list_item [ElementHandle source]
  : c=connector attrs=attr_list? targets=idtf_list
    { 
      for (auto const & t : $ctx->targets->items)
      {
          ElementHandle const connector = m_parser->ProcessConnector($c.text);
          m_parser->ProcessTriple($source, connector, t);
          
          if ($ctx->attrs != nullptr)
          {
            APPEND_ATTRS($ctx->attrs->items, connector);
          }
      }
    }
  ;

sentence_lvl_common
  : src=idtf_common sentence_lvl_4_list_item[$ctx->src->handle]
    (';' sentence_lvl_4_list_item[$ctx->src->handle])*
  ;

attr_list returns [std::vector<std::pair<ElementHandle, bool>> items]
  @init { $items = {}; }
  : (
<<<<<<< HEAD:sc-memory/sc-memory/src/scs/scs.g4
      ID_SYSTEM
      CONNECTOR_ATTR
      {
        $ctx->items.emplace_back(m_parser->ProcessIdentifier($ID_SYSTEM->getText()),
                                 scs::TypeResolver::IsConnectorAttrConst($CONNECTOR_ATTR->getText()));
=======
      id=idtf_system
      EDGE_ATTR
      {
        $ctx->items.emplace_back(m_parser->ProcessIdentifier($ctx->id->getText()),
                                 scs::TypeResolver::IsEdgeAttrConst($EDGE_ATTR->getText()));
>>>>>>> ea8cd3b0 ([memory][scs] Fix grammar terminal rules):sc-memory/sc-memory/scs/scs.g4
      }
    )+
  ;


// ----------------------------
ID_SYSTEM
  : ('a'..'z'|'A'..'Z'|'_'|'.'|'0'..'9')+
  ;

ALIAS_SYMBOLS
  : '@'('a'..'z'|'A'..'Z'|'_'|'0'..'9')+
  ;

fragment CONTENT_ESCAPED
  : '\\' ('[' | ']' | '\\' | '*' )
  ;

fragment CONTENT_SYMBOL
  : (CONTENT_ESCAPED | ~('[' | ']' | '\\'))
  ;

fragment CONTENT_SYMBOL_FIRST_END
  : (CONTENT_ESCAPED | ~('[' | ']' | '\\' | '*' ))
  ;

CONTOUR_BEGIN
  : '[*'
  ;

CONTOUR_END
  : '*]'
  ;

CONTENT_BODY
  : '[]'
  | '![]!'
  | '[' CONTENT_SYMBOL_FIRST_END CONTENT_SYMBOL* ']'
  | '![' CONTENT_SYMBOL_FIRST_END CONTENT_SYMBOL* ']!'
  ;

LINK
  : '"' (~('"') | '\\"' )* '"'
  ;
  
CONNECTOR_ATTR
  : ':'
  | '::';

LINE_TERMINATOR
  : [\r\n\u2028\u2029] -> channel(HIDDEN)
  ;
  
LINE_COMMENT
  : '//' ~('\n'|'\r')* '\r'? '\n' -> channel(HIDDEN)
  ;

MULTINE_COMMENT
  : '/*' .*? '*/' -> channel(HIDDEN)
  ;

WS :
  ( ' '
  | '\t'
  | '\r'
  | '\n'
  ) -> channel(HIDDEN)
  ;

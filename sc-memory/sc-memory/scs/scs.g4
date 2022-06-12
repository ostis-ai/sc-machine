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

#define APPEND_ATTRS(__attrs, __edge) \
for (auto const & _a : __attrs) \
{ \
  ElementHandle const attrEdge = m_parser->ProcessConnector(_a.second ? "->" : "_->"); \
  m_parser->ProcessTriple(_a.first, attrEdge, __edge); \
}

#define PARSE_ERROR(line, pos, msg) \
  SC_THROW_EXCEPTION(utils::ExceptionParseError, \
                    "line " << line << ", " << pos << " " << msg);

}

@parser::members {

public:
  void setParser(scs::Parser * parser)
  {
    SC_ASSERT(parser, ());
    m_parser = parser;
  }

private:
  scs::Parser * m_parser;

public:

}

content returns [ElementHandle handle]
  locals [ bool isVar = false;]
  @init{ $ctx->isVar = false; }
  : ('_' { $ctx->isVar = true; } )?
      c=CONTENT_BODY
    {
      std::string const v = $ctx->c->getText();
      SC_ASSERT(v.size() > 1, ());
      $ctx->handle = m_parser->ProcessContent(v.substr(1, v.size() - 2), $ctx->isVar);
    }
  ;


contour returns [ElementHandle handle]
  locals [int count = 0; ]
  @init{ $count = 1; }
  : CONTOUR_BEGIN
    {
      $ctx->handle = m_parser->ProcessContourBegin();
    }
    { $ctx->count > 0 }?
    ( (sentence_wrap
	| (sentence_lvl_4_list_item[$ctx->handle] ';;'))* )
    CONTOUR_END
    {
      $ctx->count--;
      if ($ctx->count == 0)
      {
        m_parser->ProcessContourEnd($ctx->handle);
      }
    }
  ;

contourWithJoin[ElementHandle contourHandle]
  returns [ElementHandle handle]
  locals [int count = 0; ]
  @init{ $count = 1; }
  : CONTOUR_BEGIN
    {
      m_parser->ProcessContourBegin();
    }
    { $ctx->count > 0 }?
    ( (sentence_wrap
	| (sentence_lvl_4_list_item[$contourHandle] ';;'))* )
    CONTOUR_END
    {
      $ctx->count--;
      if ($ctx->count == 0)
      {
        //$ctx->handle = m_parser->ProcessContourEnd();
      }
    }
  ;

connector returns [std::string text]
  : c=('<>' | '>' | '<' | '..>' | '<..'
    | '->' | '<-' | '<=>' | '=>' | '<='
    | '-|>' | '<|-' | '-/>' | '</-'
    | '~>' | '<~' | '~|>' | '<|~'
    | '~/>' | '</~' | '_<>' | '_>' | '_<'
    | '_..>' | '_<..' | '_->' | '_<-'
    | '_<=>' | '_=>' | '_<=' | '_-|>' | '_<|-'
    | '_-/>' | '_</-' | '_~>' | '_<~'
    | '_~|>' | '_<|~' | '_~/>' | '_</~' )

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
  | type=idtf_lvl1_preffix
    { $ctx->handle = m_parser->ProcessIdentifier($ctx->type->text); }
  ;


sentence_assign
  : ALIAS_SYMBOLS '=' i=idtf_common
    {
      m_parser->ProcessAssign($ALIAS_SYMBOLS->getText(), $ctx->i->handle);
    }
  ;

sentence_assign_contour
  : a=idtf_system '=' i=contourWithJoin[$ctx->a->handle]
    {
      m_parser->ProcessContourEndWithJoin($ctx->a->handle);
    }
  ;
    
idtf_lvl1_preffix returns [std::string text]
  : type=('sc_node'
  | 'sc_link'
  | 'sc_edge_dcommon'
  | 'sc_edge_ucommon'
  | 'sc_edge_main'
  | 'sc_edge_access'
  
  // backward compatibility
  | 'sc_arc_common'
  | 'sc_edge'
  | 'sc_arc_main'
  | 'sc_arc_access')

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
  : idtf_lvl1_value { $ctx->handle = $idtf_lvl1_value.handle; }
  // | LINK
  ;

idtf_edge returns [ElementHandle handle]
  : '(' src=idtf_atomic
        c=connector attrs=attr_list?
        trg=idtf_atomic
    ')'

    {
      ElementHandle const edge = m_parser->ProcessConnector($ctx->c->text);
      m_parser->ProcessTriple($ctx->src->handle, edge, $ctx->trg->handle);

      // append attributes
      if ($ctx->attrs != nullptr)
      {
        APPEND_ATTRS($attrs.items, edge);
      }

      $ctx->handle = edge;
    }
  ;
  
idtf_set returns [ElementHandle handle]
  : '{' 
      { 
        std::string const setIdtf = "..set_" + std::to_string($ctx->start->getLine()) + "_" + std::to_string($ctx->start->getCharPositionInLine());
        $ctx->handle = m_parser->ProcessIdentifier(setIdtf);
        ElementHandle const typeEdge = m_parser->ProcessConnector("->");
        ElementHandle const typeClass = m_parser->ProcessIdentifier("sc_node_tuple");

        m_parser->ProcessTriple(typeClass, typeEdge, $ctx->handle);
      }
    a1=attr_list? i1=idtf_common 
      {
        ElementHandle const edge = m_parser->ProcessConnector("->");
        m_parser->ProcessTriple($ctx->handle, edge, $ctx->i1->handle);
        
        if ($ctx->a1 != nullptr)
        {
          APPEND_ATTRS($ctx->a1->items, edge);
        }
      }
      internal_sentence_list[$ctx->i1->handle]?

      (';'
      a2=attr_list? i2=idtf_common
        {
          ElementHandle const edge = m_parser->ProcessConnector("->");
          m_parser->ProcessTriple($ctx->handle, edge, $ctx->i2->handle);
          
          if ($ctx->a2 != nullptr)
          {
            APPEND_ATTRS($ctx->a2->items, edge);
          }
        }
        internal_sentence_list[$ctx->i2->handle]?
      )*
    '}'
  ;

idtf_atomic returns [ElementHandle handle]
  : a=idtf_alias { $ctx->handle = $ctx->a->handle; }
  | is=idtf_system { $ctx->handle = $ctx->is->handle; }
  ;

idtf_url returns [ElementHandle handle]
  : LINK
    {
      std::string const value = $LINK->getText();
      SC_ASSERT(value.size() > 1, ());
      $ctx->handle = m_parser->ProcessFileURL(value.substr(1, value.size() - 2));
    }
  ;

idtf_common returns [ElementHandle handle]
  : a=idtf_atomic { $ctx->handle = $ctx->a->handle; }
  | ie=idtf_edge { $ctx->handle = $ctx->ie->handle; }
  | iset=idtf_set { $ctx->handle = $ctx->iset->handle; }
  | ct=contour { $ctx->handle = $ctx->ct->handle; }
  | cn=content { $ctx->handle = $ctx->cn->handle; }
  | u=idtf_url { $ctx->handle = $ctx->u->handle; }
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
        ElementHandle const edge = m_parser->ProcessConnector($c.text);
        m_parser->ProcessTriple(source, edge, trg);
        
        if ($ctx->attrs != nullptr)
        {
          APPEND_ATTRS($ctx->attrs->items, edge);
        }
      }
    }
    | internal_sentence_list[source]
  ;

internal_sentence_list [ElementHandle source]
  : '(*' (internal_sentence[source] ';;')+ '*)'
  ;

sentence_lvl1
   : src=idtf_lvl1 '|' edge=idtf_lvl1 '|' trg=idtf_lvl1
    {
      m_parser->ProcessTriple($ctx->src->handle, $ctx->edge->handle, $ctx->trg->handle);
    }
  ;

sentence_lvl_4_list_item [ElementHandle source]
  : c=connector attrs=attr_list? targets=idtf_list
    { 
      for (auto const & t : $ctx->targets->items)
      {
          ElementHandle const edge = m_parser->ProcessConnector($c.text);
          m_parser->ProcessTriple($source, edge, t);
          
          if ($ctx->attrs != nullptr)
          {
            APPEND_ATTRS($ctx->attrs->items, edge);
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
      ID_SYSTEM
      EDGE_ATTR
      {
        $ctx->items.emplace_back(m_parser->ProcessIdentifier($ID_SYSTEM->getText()),
                                 scs::TypeResolver::IsEdgeAttrConst($EDGE_ATTR->getText()));
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

CONTENT_BODY
  : '[]'
  | '[' CONTENT_SYBMOL_FIRST_END CONTENT_SYBMOL* ']'
  ;

LINK
  : '"' (~('"') | '\\"' )* '"'
  ;
  
EDGE_ATTR
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

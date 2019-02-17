
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



// Generated from scs.g4 by ANTLR 4.7.1

#pragma once


#include "antlr4-runtime.h"


namespace scs {


class  scsParser : public antlr4::Parser {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, T__8 = 9, T__9 = 10, T__10 = 11, T__11 = 12, T__12 = 13, T__13 = 14, 
    T__14 = 15, T__15 = 16, T__16 = 17, T__17 = 18, T__18 = 19, T__19 = 20, 
    T__20 = 21, T__21 = 22, T__22 = 23, T__23 = 24, T__24 = 25, T__25 = 26, 
    T__26 = 27, T__27 = 28, T__28 = 29, T__29 = 30, T__30 = 31, T__31 = 32, 
    T__32 = 33, T__33 = 34, T__34 = 35, T__35 = 36, T__36 = 37, T__37 = 38, 
    T__38 = 39, T__39 = 40, T__40 = 41, T__41 = 42, T__42 = 43, T__43 = 44, 
    T__44 = 45, T__45 = 46, T__46 = 47, T__47 = 48, T__48 = 49, T__49 = 50, 
    T__50 = 51, T__51 = 52, T__52 = 53, T__53 = 54, T__54 = 55, T__55 = 56, 
    T__56 = 57, T__57 = 58, T__58 = 59, T__59 = 60, T__60 = 61, T__61 = 62, 
    T__62 = 63, ID_SYSTEM = 64, ALIAS_SYMBOLS = 65, CONTOUR_BEGIN = 66, 
    CONTOUR_END = 67, CONTENT_BODY = 68, LINK = 69, EDGE_ATTR = 70, LINE_TERMINATOR = 71, 
    LINE_COMMENT = 72, MULTINE_COMMENT = 73, WS = 74
  };

  enum {
    RuleContent = 0, RuleContour = 1, RuleConnector = 2, RuleSyntax = 3, 
    RuleSentence_wrap = 4, RuleSentence = 5, RuleAlias = 6, RuleIdtf_system = 7, 
    RuleSentence_assign = 8, RuleIdtf_lvl1_preffix = 9, RuleIdtf_lvl1_value = 10, 
    RuleIdtf_lvl1 = 11, RuleIdtf_edge = 12, RuleIdtf_set = 13, RuleIdtf_common = 14, 
    RuleIdtf_list = 15, RuleInternal_sentence = 16, RuleInternal_sentence_list = 17, 
    RuleSentence_lvl1 = 18, RuleSentence_lvl_4_list_item = 19, RuleSentence_lvl_common = 20, 
    RuleAttr_list = 21
  };

  scsParser(antlr4::TokenStream *input);
  ~scsParser();

  virtual std::string getGrammarFileName() const override;
  virtual const antlr4::atn::ATN& getATN() const override { return _atn; };
  virtual const std::vector<std::string>& getTokenNames() const override { return _tokenNames; }; // deprecated: use vocabulary instead.
  virtual const std::vector<std::string>& getRuleNames() const override;
  virtual antlr4::dfa::Vocabulary& getVocabulary() const override;



  public:
    void setParser(scs::Parser * parser)
    {
      SC_ASSERT(parser, ());
      m_parser = parser;
    }

  //  static bool IsEmptyContour(std::string const & val)
  //  {
  //    if (!utils::StringUtils::StartsWith(val, "[*") || !utils::StringUtils::EndsWith(val, "*]"))
  //      return false;
  //
  //    SC_ASSERT(val.size() >= 4, ());
  //
  //    for (size_t i = 2; i < val.size() - 2; ++i)
  //    {
  //      if (val[i] != ' ')
  //        return false;
  //    }
  //
  //    return true;
  //  }


  private:
    scs::Parser * m_parser;

  public:



  class ContentContext;
  class ContourContext;
  class ConnectorContext;
  class SyntaxContext;
  class Sentence_wrapContext;
  class SentenceContext;
  class AliasContext;
  class Idtf_systemContext;
  class Sentence_assignContext;
  class Idtf_lvl1_preffixContext;
  class Idtf_lvl1_valueContext;
  class Idtf_lvl1Context;
  class Idtf_edgeContext;
  class Idtf_setContext;
  class Idtf_commonContext;
  class Idtf_listContext;
  class Internal_sentenceContext;
  class Internal_sentence_listContext;
  class Sentence_lvl1Context;
  class Sentence_lvl_4_list_itemContext;
  class Sentence_lvl_commonContext;
  class Attr_listContext; 

  class  ContentContext : public antlr4::ParserRuleContext {
  public:
    ElementHandle handle;
    bool isVar = false;;
    antlr4::Token *c = nullptr;;
    ContentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CONTENT_BODY();

   
  };

  ContentContext* content();

  class  ContourContext : public antlr4::ParserRuleContext {
  public:
    ElementHandle handle;
    int count = 0;;
    ContourContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CONTOUR_BEGIN();
    antlr4::tree::TerminalNode *CONTOUR_END();
    std::vector<Sentence_wrapContext *> sentence_wrap();
    Sentence_wrapContext* sentence_wrap(size_t i);

   
  };

  ContourContext* contour();

  class  ConnectorContext : public antlr4::ParserRuleContext {
  public:
    std::string text;
    antlr4::Token *c = nullptr;;
    ConnectorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;

   
  };

  ConnectorContext* connector();

  class  SyntaxContext : public antlr4::ParserRuleContext {
  public:
    SyntaxContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EOF();
    std::vector<Sentence_wrapContext *> sentence_wrap();
    Sentence_wrapContext* sentence_wrap(size_t i);

   
  };

  SyntaxContext* syntax();

  class  Sentence_wrapContext : public antlr4::ParserRuleContext {
  public:
    Sentence_wrapContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    SentenceContext *sentence();

   
  };

  Sentence_wrapContext* sentence_wrap();

  class  SentenceContext : public antlr4::ParserRuleContext {
  public:
    SentenceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Sentence_lvl1Context *sentence_lvl1();
    Sentence_assignContext *sentence_assign();
    Sentence_lvl_commonContext *sentence_lvl_common();

   
  };

  SentenceContext* sentence();

  class  AliasContext : public antlr4::ParserRuleContext {
  public:
    AliasContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ALIAS_SYMBOLS();

   
  };

  AliasContext* alias();

  class  Idtf_systemContext : public antlr4::ParserRuleContext {
  public:
    ElementHandle handle;
    antlr4::Token *id_systemToken = nullptr;;
    Idtf_systemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ID_SYSTEM();

   
  };

  Idtf_systemContext* idtf_system();

  class  Sentence_assignContext : public antlr4::ParserRuleContext {
  public:
    scsParser::AliasContext *a = nullptr;;
    scsParser::Idtf_commonContext *i = nullptr;;
    Sentence_assignContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    AliasContext *alias();
    Idtf_commonContext *idtf_common();

   
  };

  Sentence_assignContext* sentence_assign();

  class  Idtf_lvl1_preffixContext : public antlr4::ParserRuleContext {
  public:
    std::string text;
    antlr4::Token *type = nullptr;;
    Idtf_lvl1_preffixContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;

   
  };

  Idtf_lvl1_preffixContext* idtf_lvl1_preffix();

  class  Idtf_lvl1_valueContext : public antlr4::ParserRuleContext {
  public:
    ElementHandle handle;
    scsParser::Idtf_lvl1_preffixContext *type = nullptr;;
    antlr4::Token *i = nullptr;;
    Idtf_lvl1_valueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Idtf_lvl1_preffixContext *idtf_lvl1_preffix();
    antlr4::tree::TerminalNode *ID_SYSTEM();

   
  };

  Idtf_lvl1_valueContext* idtf_lvl1_value();

  class  Idtf_lvl1Context : public antlr4::ParserRuleContext {
  public:
    ElementHandle handle;
    scsParser::Idtf_lvl1_valueContext *idtf_lvl1_valueContext = nullptr;;
    Idtf_lvl1Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Idtf_lvl1_valueContext *idtf_lvl1_value();

   
  };

  Idtf_lvl1Context* idtf_lvl1();

  class  Idtf_edgeContext : public antlr4::ParserRuleContext {
  public:
    ElementHandle handle;
    scsParser::Idtf_systemContext *src = nullptr;;
    scsParser::ConnectorContext *c = nullptr;;
    scsParser::Attr_listContext *attrs = nullptr;;
    scsParser::Idtf_systemContext *trg = nullptr;;
    Idtf_edgeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Idtf_systemContext *> idtf_system();
    Idtf_systemContext* idtf_system(size_t i);
    ConnectorContext *connector();
    Attr_listContext *attr_list();

   
  };

  Idtf_edgeContext* idtf_edge();

  class  Idtf_setContext : public antlr4::ParserRuleContext {
  public:
    ElementHandle handle;
    scsParser::Attr_listContext *a1 = nullptr;;
    scsParser::Idtf_commonContext *i1 = nullptr;;
    scsParser::Attr_listContext *a2 = nullptr;;
    scsParser::Idtf_commonContext *i2 = nullptr;;
    Idtf_setContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Idtf_commonContext *> idtf_common();
    Idtf_commonContext* idtf_common(size_t i);
    std::vector<Attr_listContext *> attr_list();
    Attr_listContext* attr_list(size_t i);

   
  };

  Idtf_setContext* idtf_set();

  class  Idtf_commonContext : public antlr4::ParserRuleContext {
  public:
    ElementHandle handle;
    scsParser::AliasContext *a = nullptr;;
    scsParser::Idtf_systemContext *is = nullptr;;
    scsParser::Idtf_edgeContext *ie = nullptr;;
    scsParser::Idtf_setContext *iset = nullptr;;
    scsParser::ContourContext *ct = nullptr;;
    scsParser::ContentContext *cn = nullptr;;
    antlr4::Token *linkToken = nullptr;;
    Idtf_commonContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    AliasContext *alias();
    Idtf_systemContext *idtf_system();
    Idtf_edgeContext *idtf_edge();
    Idtf_setContext *idtf_set();
    ContourContext *contour();
    ContentContext *content();
    antlr4::tree::TerminalNode *LINK();

   
  };

  Idtf_commonContext* idtf_common();

  class  Idtf_listContext : public antlr4::ParserRuleContext {
  public:
    std::vector<ElementHandle> items;
    scsParser::Idtf_commonContext *i1 = nullptr;;
    scsParser::Idtf_commonContext *i2 = nullptr;;
    Idtf_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Idtf_commonContext *> idtf_common();
    Idtf_commonContext* idtf_common(size_t i);
    std::vector<Internal_sentence_listContext *> internal_sentence_list();
    Internal_sentence_listContext* internal_sentence_list(size_t i);

   
  };

  Idtf_listContext* idtf_list();

  class  Internal_sentenceContext : public antlr4::ParserRuleContext {
  public:
    ElementHandle source;
    scsParser::ConnectorContext *c = nullptr;;
    scsParser::Attr_listContext *attrs = nullptr;;
    scsParser::Idtf_listContext *targets = nullptr;;
    Internal_sentenceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    Internal_sentenceContext(antlr4::ParserRuleContext *parent, size_t invokingState, ElementHandle source);
    virtual size_t getRuleIndex() const override;
    ConnectorContext *connector();
    Idtf_listContext *idtf_list();
    Attr_listContext *attr_list();

   
  };

  Internal_sentenceContext* internal_sentence(ElementHandle source);

  class  Internal_sentence_listContext : public antlr4::ParserRuleContext {
  public:
    ElementHandle source;
    Internal_sentence_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    Internal_sentence_listContext(antlr4::ParserRuleContext *parent, size_t invokingState, ElementHandle source);
    virtual size_t getRuleIndex() const override;
    std::vector<Internal_sentenceContext *> internal_sentence();
    Internal_sentenceContext* internal_sentence(size_t i);

   
  };

  Internal_sentence_listContext* internal_sentence_list(ElementHandle source);

  class  Sentence_lvl1Context : public antlr4::ParserRuleContext {
  public:
    scsParser::Idtf_lvl1Context *src = nullptr;;
    scsParser::Idtf_lvl1Context *edge = nullptr;;
    scsParser::Idtf_lvl1Context *trg = nullptr;;
    Sentence_lvl1Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Idtf_lvl1Context *> idtf_lvl1();
    Idtf_lvl1Context* idtf_lvl1(size_t i);

   
  };

  Sentence_lvl1Context* sentence_lvl1();

  class  Sentence_lvl_4_list_itemContext : public antlr4::ParserRuleContext {
  public:
    ElementHandle source;
    scsParser::ConnectorContext *c = nullptr;;
    scsParser::Attr_listContext *attrs = nullptr;;
    scsParser::Idtf_listContext *targets = nullptr;;
    Sentence_lvl_4_list_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    Sentence_lvl_4_list_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState, ElementHandle source);
    virtual size_t getRuleIndex() const override;
    ConnectorContext *connector();
    Idtf_listContext *idtf_list();
    Attr_listContext *attr_list();

   
  };

  Sentence_lvl_4_list_itemContext* sentence_lvl_4_list_item(ElementHandle source);

  class  Sentence_lvl_commonContext : public antlr4::ParserRuleContext {
  public:
    scsParser::Idtf_commonContext *src = nullptr;;
    Sentence_lvl_commonContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Sentence_lvl_4_list_itemContext *> sentence_lvl_4_list_item();
    Sentence_lvl_4_list_itemContext* sentence_lvl_4_list_item(size_t i);
    Idtf_commonContext *idtf_common();

   
  };

  Sentence_lvl_commonContext* sentence_lvl_common();

  class  Attr_listContext : public antlr4::ParserRuleContext {
  public:
    std::vector<std::pair<ElementHandle, bool>> items;
    antlr4::Token *id_systemToken = nullptr;;
    antlr4::Token *edge_attrToken = nullptr;;
    Attr_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> ID_SYSTEM();
    antlr4::tree::TerminalNode* ID_SYSTEM(size_t i);
    std::vector<antlr4::tree::TerminalNode *> EDGE_ATTR();
    antlr4::tree::TerminalNode* EDGE_ATTR(size_t i);

   
  };

  Attr_listContext* attr_list();


  virtual bool sempred(antlr4::RuleContext *_localctx, size_t ruleIndex, size_t predicateIndex) override;
  bool contourSempred(ContourContext *_localctx, size_t predicateIndex);

private:
  static std::vector<antlr4::dfa::DFA> _decisionToDFA;
  static antlr4::atn::PredictionContextCache _sharedContextCache;
  static std::vector<std::string> _ruleNames;
  static std::vector<std::string> _tokenNames;

  static std::vector<std::string> _literalNames;
  static std::vector<std::string> _symbolicNames;
  static antlr4::dfa::Vocabulary _vocabulary;
  static antlr4::atn::ATN _atn;
  static std::vector<uint16_t> _serializedATN;


  struct Initializer {
    Initializer();
  };
  static Initializer _init;
};

}  // namespace scs

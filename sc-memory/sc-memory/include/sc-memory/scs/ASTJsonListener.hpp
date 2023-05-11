#pragma once

#include "antlr4-runtime.h"

#include <cstdio>
#include <nlohmann/json.hpp>

using ScJson = nlohmann::json;

namespace scs
{

class ASTErrorListener : public antlr4::ANTLRErrorListener
{
public:
  ScJson getErrors()
  {
    return m_errors;
  }

protected:
  ScJson m_errors = ScJson::array();

  void syntaxError(
      antlr4::Recognizer *,
      antlr4::Token * token,
      size_t line,
      size_t charPositionInLine,
      std::string const & msg,
      std::exception_ptr) override;

  void reportAmbiguity(
      antlr4::Parser *,
      antlr4::dfa::DFA const & dfa,
      size_t startIndex,
      size_t stopIndex,
      bool,
      antlrcpp::BitSet const &,
      antlr4::atn::ATNConfigSet *) override;

  void reportAttemptingFullContext(
      antlr4::Parser *,
      antlr4::dfa::DFA const &,
      size_t,
      size_t,
      antlrcpp::BitSet const &,
      antlr4::atn::ATNConfigSet *) override;

  void reportContextSensitivity(
      antlr4::Parser *,
      antlr4::dfa::DFA const &,
      size_t,
      size_t,
      size_t,
      antlr4::atn::ATNConfigSet *) override;
};

struct ASTNode
{
  ScJson info;
  std::set<ASTNode *> children;
  ASTNode * parentNode;
};

/**
 * This class provides an empty implementation of ASTJsonListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class ASTJsonListener : public antlr4::tree::ParseTreeListener  {
public:
  explicit ASTJsonListener(antlr4::Parser & parser)
    : m_parser(parser), m_ruleNames(m_parser.getRuleNames())
  {}

  void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override;
  void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override;
  void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override;
  void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override;

  void buildAST(ScJson & astJson);

  ~ASTJsonListener() override;

protected:
  antlr4::Parser & m_parser;
  std::vector<std::string> const & m_ruleNames;

  ASTNode * m_root = nullptr;
  ASTNode * m_currentNode = nullptr;
  ASTNode * m_parentNode = nullptr;

  void buildTokenStartInfo(antlr4::ParserRuleContext * ctx, ScJson & nodeInfo);

  void buildTokenStopInfo(antlr4::ParserRuleContext * ctx, ScJson & nodeInfo);
};

}  // namespace scs

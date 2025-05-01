/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <limits>

#include <nlohmann/json.hpp>

#include "antlr4-runtime.h"

using ScJson = nlohmann::json;

namespace scs
{

class SCsASTErrorListener : public antlr4::ANTLRErrorListener
{
public:
  ScJson GetErrors() const;

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

/*!
 * This class provides an empty implementation of SCsASTJsonListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class SCsASTJsonListener : public antlr4::tree::ParseTreeListener 
{
public:
  explicit SCsASTJsonListener(antlr4::Parser & parser);

  void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override;
  void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override;
  void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override;
  void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override;

  void BuildAST(ScJson & astJson);

  ~SCsASTJsonListener() override;

protected:
  antlr4::Parser & m_parser;
  std::vector<std::string> const & m_ruleNames;

  ASTNode * m_root = nullptr;
  ASTNode * m_currentNode = nullptr;
  ASTNode * m_parentNode = nullptr;

  void BuildTokenStartInfo(antlr4::ParserRuleContext * ctx, ScJson & nodeInfo);

  void BuildTokenStopInfo(antlr4::ParserRuleContext * ctx, ScJson & nodeInfo);
};

}  // namespace scs

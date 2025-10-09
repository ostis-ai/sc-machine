/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/scs/scs_ast_json_listener.hpp"

using namespace scs;

ScJson SCsASTErrorListener::GetErrors() const
{
  return m_errors;
}

void SCsASTErrorListener::syntaxError(
    antlr4::Recognizer *,
    antlr4::Token * token,
    size_t line,
    size_t charPositionInLine,
    std::string const & msg,
    std::exception_ptr)
{
  ScJson errorInfoJson;
  errorInfoJson["token"] = token ? ScJson(token->getText()) : ScJson();
  errorInfoJson["msg"] = msg;

  ScJson positionJson;
  positionJson["beginLine"] = token == nullptr ? line : token->getLine();
  positionJson["beginIndex"] = charPositionInLine;
  positionJson["endLine"] = token == nullptr ? positionJson["beginLine"].get<size_t>() : token->getLine();
  positionJson["endIndex"]
      = token == nullptr ? positionJson["beginIndex"].get<size_t>() : token->getText().size() + charPositionInLine;

  errorInfoJson["position"] = positionJson;

  m_errors.push_back(errorInfoJson);
}

void SCsASTErrorListener::reportAmbiguity(
    antlr4::Parser *,
    antlr4::dfa::DFA const & dfa,
    size_t startIndex,
    size_t stopIndex,
    bool,
    antlrcpp::BitSet const &,
    antlr4::atn::ATNConfigSet *)
{
  ScJson errorInfoJson;
  errorInfoJson["msg"] = "reportAmbiguity";

  m_errors.push_back(errorInfoJson);
}

void SCsASTErrorListener::reportAttemptingFullContext(
    antlr4::Parser *,
    antlr4::dfa::DFA const &,
    size_t,
    size_t,
    antlrcpp::BitSet const &,
    antlr4::atn::ATNConfigSet *)
{
  ScJson errorInfoJson;
  errorInfoJson["msg"] = "reportAttemptingFullContext";

  m_errors.push_back(errorInfoJson);
}

void SCsASTErrorListener::reportContextSensitivity(
    antlr4::Parser *,
    antlr4::dfa::DFA const &,
    size_t,
    size_t,
    size_t,
    antlr4::atn::ATNConfigSet *)
{
  ScJson errorInfoJson;
  errorInfoJson["msg"] = "reportContextSensitivity";

  m_errors.push_back(errorInfoJson);
}

SCsASTJsonListener::SCsASTJsonListener(antlr4::Parser & parser)
  : m_parser(parser), m_ruleNames(m_parser.getRuleNames())
{}

void SCsASTJsonListener::BuildTokenStartInfo(antlr4::ParserRuleContext * ctx, ScJson & nodeInfo)
{
  ScJson positionJson;
  positionJson["beginLine"] = ctx->getStart()->getLine();
  positionJson["beginIndex"] = ctx->getStart()->getCharPositionInLine();

  nodeInfo["ruleType"] = m_ruleNames[ctx->getRuleIndex()];
  nodeInfo["position"] = positionJson;
  nodeInfo["children"] = ScJson::array();
}

void SCsASTJsonListener::BuildTokenStopInfo(antlr4::ParserRuleContext * ctx, ScJson & nodeInfo)
{
  if (ctx->getStop() == nullptr)
    return;

  nodeInfo["position"]["endLine"] = ctx->getStop()->getLine();
  nodeInfo["position"]["endIndex"] = ctx->getStop()->getCharPositionInLine() + ctx->getStop()->getText().size();
}

void SCsASTJsonListener::enterEveryRule(antlr4::ParserRuleContext * ctx)
{
  auto * node = new ASTNode();

  if (m_root == nullptr)
  {
    m_currentNode = node;
    m_root = node;
    m_currentNode->parentNode = nullptr;
  }
  else
  {
    m_currentNode->children.insert(node);
    m_parentNode = m_currentNode;
    m_currentNode = node;
    m_currentNode->parentNode = m_parentNode;
  }

  BuildTokenStartInfo(ctx, m_currentNode->info);
}

void SCsASTJsonListener::exitEveryRule(antlr4::ParserRuleContext * ctx)
{
  BuildTokenStopInfo(ctx, m_currentNode->info);

  m_currentNode = m_parentNode;
  if (m_parentNode != nullptr)
    m_parentNode = m_currentNode->parentNode;
}

void SCsASTJsonListener::visitTerminal(antlr4::tree::TerminalNode * node)
{
  m_currentNode->info["token"] = node->getText();
}

void SCsASTJsonListener::visitErrorNode(antlr4::tree::ErrorNode * node)
{
}

void visitAllASTNodes(ASTNode * node, ScJson & json)
{
  for (ASTNode * child : node->children)
  {
    json.push_back(child->info);

    visitAllASTNodes(child, json[json.size() - 1]["children"]);
  }
}

void SCsASTJsonListener::BuildAST(ScJson & astJson)
{
  astJson = m_root->info;
  visitAllASTNodes(m_root, astJson["children"]);
}

void removeAllASTNodes(ASTNode * node)
{
  for (ASTNode * child : node->children)
  {
    removeAllASTNodes(child);
    delete child;
  }
}

SCsASTJsonListener::~SCsASTJsonListener()
{
  removeAllASTNodes(m_root);
  delete m_root;
  m_root = nullptr;
  m_currentNode = nullptr;
  m_parentNode = nullptr;
}

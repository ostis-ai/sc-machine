#include "ASTJsonListener.hpp"

using namespace scs;

void ASTErrorListener::syntaxError(
    antlr4::Recognizer *,
    antlr4::Token * token,
    size_t line,
    size_t charPositionInLine,
    std::string const & msg,
    std::exception_ptr)
{
  ScJson errorInfoJson;
  errorInfoJson["token"] = token ? token->getText() : "Invalid token";
  errorInfoJson["line"] = line;
  errorInfoJson["charPositionInLine"] = charPositionInLine;
  errorInfoJson["msg"] = msg;

  m_errors.push_back(errorInfoJson);
}

void ASTErrorListener::reportAmbiguity(
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

void ASTErrorListener::reportAttemptingFullContext(
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

void ASTErrorListener::reportContextSensitivity(
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

void ASTJsonListener::buildTokenStartInfo(antlr4::ParserRuleContext * ctx, ScJson & nodeInfo)
{
  ScJson positionJson;
  positionJson["beginLine"] = ctx->getStart()->getLine();
  positionJson["beginIndex"] = ctx->getStart()->getTokenIndex();

  nodeInfo["ruleType"] = m_ruleNames[ctx->getRuleIndex()];
  nodeInfo["position"] = positionJson;
  nodeInfo["children"] = ScJson::array();
}

void ASTJsonListener::buildTokenStopInfo(antlr4::ParserRuleContext * ctx, ScJson & nodeInfo)
{
  if (ctx->getStop() == nullptr)
    return;

  nodeInfo["position"]["endLine"] = ctx->getStop()->getLine();
  nodeInfo["position"]["endIndex"] = ctx->getStop()->getTokenIndex();
}

void ASTJsonListener::enterEveryRule(antlr4::ParserRuleContext * ctx)
{
  auto * node = new ASTNode();

  if (m_ast == nullptr)
  {
    m_currentNode = node;
    m_ast = node;
    m_currentNode->parentNode = nullptr;
  }
  else
  {
    m_currentNode->children.insert(node);
    m_parentNode = m_currentNode;
    m_currentNode = node;
    m_currentNode->parentNode = m_parentNode;
  }

  buildTokenStartInfo(ctx, m_currentNode->info);
}

void ASTJsonListener::exitEveryRule(antlr4::ParserRuleContext * ctx)
{
  buildTokenStopInfo(ctx, m_currentNode->info);

  m_currentNode = m_parentNode;
  if (m_parentNode != nullptr)
    m_parentNode = m_currentNode->parentNode;
}

void ASTJsonListener::visitTerminal(antlr4::tree::TerminalNode * node)
{
  m_currentNode->info["token"] = node->getText();
}

void ASTJsonListener::visitErrorNode(antlr4::tree::ErrorNode * node)
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

void ASTJsonListener::buildAST(ScJson & astJson)
{
  astJson = ScJson();
  astJson["ruleType"] = "syntax";

  visitAllASTNodes(m_ast, astJson["children"]);
}

void removeAllASTNodes(ASTNode * node)
{
  for (ASTNode * child : node->children)
  {
    removeAllASTNodes(child);
    delete child;
  }
}

ASTJsonListener::~ASTJsonListener()
{
  removeAllASTNodes(m_ast);
  delete m_ast;
  m_ast = nullptr;
  m_currentNode = nullptr;
  m_parentNode = nullptr;
}

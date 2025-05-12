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
  positionJson["beginIndex"] = ctx->getStart()->getCharPositionInLine();

  nodeInfo["ruleType"] = m_ruleNames[ctx->getRuleIndex()];
  nodeInfo["position"] = positionJson;
  nodeInfo["children"] = ScJson::array();
}

void ASTJsonListener::buildTokenStopInfo(antlr4::ParserRuleContext * ctx, ScJson & nodeInfo)
{
  if (ctx->getStop() == nullptr)
    return;

  nodeInfo["position"]["endLine"] = ctx->getStop()->getLine();
  nodeInfo["position"]["endIndex"] = ctx->getStop()->getCharPositionInLine() + ctx->getStop()->getText().size();
}

void ASTJsonListener::enterEveryRule(antlr4::ParserRuleContext * ctx)
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

  buildTokenStartInfo(ctx, m_currentNode->info);
}

void ASTJsonListener::exitEveryRule(antlr4::ParserRuleContext * ctx)
{
  buildTokenStopInfo(ctx, m_currentNode->info);

  if (m_ruleNames[ctx->getRuleIndex()] == "idtf_system" && m_currentNode->info.contains("token")) {
    ScJson symbolInfo;
    symbolInfo["name"] = m_currentNode->info["token"];
    symbolInfo["range"] = m_currentNode->info["position"];
    m_symbols.push_back(symbolInfo);
  }

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
  astJson = m_root->info;
  visitAllASTNodes(m_root, astJson["children"]);
  astJson["symbols"] = m_symbols;
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
  removeAllASTNodes(m_root);
  delete m_root;
  m_root = nullptr;
  m_currentNode = nullptr;
  m_parentNode = nullptr;
}

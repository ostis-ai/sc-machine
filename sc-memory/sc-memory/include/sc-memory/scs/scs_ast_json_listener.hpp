/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <limits>
#include <set>
#include <vector>

#include <nlohmann/json.hpp>

#include "antlr4-runtime.h"

using ScJson = nlohmann::json;

namespace scs
{

/*!
 * @brief Custom ANTLR error listener that collects parsing errors into a JSON array.
 *
 * This class overrides the standard ANTLR error reporting methods to capture
 * detailed error information, including token, message, and position,
 * and formats it as a JSON object within an array.
 */
class SCsASTErrorListener : public antlr4::ANTLRErrorListener
{
public:
  /*!
   * @brief Retrieves the collected parsing errors as a JSON array.
   * @return ScJson A JSON array where each element is a JSON object representing a parsing error.
   */
  ScJson GetErrors() const;

protected:
  /*!
   * @brief Stores the collected errors as a JSON array. Each element represents one error.
   */
  ScJson m_errors = ScJson::array();

  /*!
   * @brief Handles syntax errors detected by the parser.
   * Adds a JSON object describing the error (token, message, position) to m_errors.
   * Overrides antlr4::ANTLRErrorListener::syntaxError.
   * @param recognizer The recognizer that detected the error (unused).
   * @param token The offending token where the error occurred.
   * @param line The line number where the error occurred.
   * @param charPositionInLine The character position within the line where the error occurred.
   * @param msg The error message provided by ANTLR.
   * @param e The exception pointer, if any (unused).
   */
  void syntaxError(
      antlr4::Recognizer * /*recognizer*/,
      antlr4::Token * token,
      size_t line,
      size_t charPositionInLine,
      std::string const & msg,
      std::exception_ptr /*e*/) override;

  /*!
   * @brief Reports an ambiguity detected during parsing.
   * Currently adds a generic "reportAmbiguity" message to the errors JSON array.
   * Overrides antlr4::ANTLRErrorListener::reportAmbiguity.
   * @param parser The parser instance (unused).
   * @param dfa The DFA associated with the ambiguity.
   * @param startIndex Start index in the input stream where the ambiguity was detected.
   * @param stopIndex Stop index in the input stream where the ambiguity was detected.
   * @param exact Whether the ambiguity is exact (unused).
   * @param ambigAlts The set of ambiguous alternatives (unused).
   * @param configs The ATN configuration set (unused).
   */
  void reportAmbiguity(
      antlr4::Parser * /*parser*/,
      antlr4::dfa::DFA const & dfa,
      size_t startIndex,
      size_t stopIndex,
      bool /*exact*/,
      antlrcpp::BitSet const & /*ambigAlts*/,
      antlr4::atn::ATNConfigSet * /*configs*/) override;

  /*!
   * @brief Reports an attempt to use the full context during parsing.
   * Currently adds a generic "reportAttemptingFullContext" message to the errors JSON array.
   * Overrides antlr4::ANTLRErrorListener::reportAttemptingFullContext.
   * @param parser The parser instance (unused).
   * @param dfa The DFA associated with the context attempt (unused).
   * @param startIndex Start index in the input stream (unused).
   * @param stopIndex Stop index in the input stream (unused).
   * @param conflictingAlts The set of conflicting alternatives (unused).
   * @param configs The ATN configuration set (unused).
   */
  void reportAttemptingFullContext(
      antlr4::Parser * /*parser*/,
      antlr4::dfa::DFA const & /*dfa*/,
      size_t /*startIndex*/,
      size_t /*stopIndex*/,
      antlrcpp::BitSet const & /*conflictingAlts*/,
      antlr4::atn::ATNConfigSet * /*configs*/) override;

  /*!
   * @brief Reports context sensitivity detected during parsing.
   * Currently adds a generic "reportContextSensitivity" message to the errors JSON array.
   * Overrides antlr4::ANTLRErrorListener::reportContextSensitivity.
   * @param parser The parser instance (unused).
   * @param dfa The DFA associated with the context sensitivity (unused).
   * @param startIndex Start index in the input stream (unused).
   * @param stopIndex Stop index in the input stream (unused).
   * @param prediction The prediction number (unused).
   * @param configs The ATN configuration set (unused).
   */
  void reportContextSensitivity(
      antlr4::Parser * /*parser*/,
      antlr4::dfa::DFA const & /*dfa*/,
      size_t /*startIndex*/,
      size_t /*stopIndex*/,
      size_t /*prediction*/,
      antlr4::atn::ATNConfigSet * /*configs*/) override;
};

/*!
 * @brief Represents a node within the Abstract Syntax Tree (AST).
 *
 * Each node holds JSON information about the corresponding grammar rule or token,
 * references to its children, and a pointer to its parent node. This structure
 * is used internally by SCsASTJsonListener to build the tree.
 */
struct ASTNode
{
  /*! @brief JSON object containing information about the node (e.g., rule type, position, token text). */
  ScJson info;
  /*! @brief Set of pointers to the child nodes of this node. */
  std::set<ASTNode *> children;
  /*! @brief Pointer to the parent node in the AST. Null for the root node. */
  ASTNode * parentNode = nullptr; // Initialize to nullptr
};

/*!
 * @brief An ANTLR ParseTreeListener that constructs an Abstract Syntax Tree (AST)
 *        from parsed input and provides a method to serialize it into JSON format.
 *
 * This listener walks the parse tree generated by ANTLR using the enter/exit rule
 * methods. It builds an internal tree structure using ASTNode instances. It captures
 * rule information, token positions, and token text. The final AST, representing
 * the parsed structure, can be obtained as a single JSON object using BuildAST().
 *
 * This class provides a basic implementation based on antlr4::tree::ParseTreeListener.
 * It can be extended to create listeners that only need to handle a subset
 * of the available methods or add custom logic during the tree walk.
 */
class SCsASTJsonListener : public antlr4::tree::ParseTreeListener 
{
public:
  /*!
   * @brief Constructs the listener.
   * @param parser The ANTLR parser instance used for parsing. Required to access rule names.
   */
  explicit SCsASTJsonListener(antlr4::Parser & parser);

  /*!
   * @brief Called automatically by the ANTLR ParseTreeWalker when entering a parser rule context.
   * Creates a new ASTNode, populates its starting info (rule type, start position),
   * and links it into the tree structure (either as root or as a child of the current node).
   * Updates the current node and parent node pointers.
   * Overrides antlr4::tree::ParseTreeListener::enterEveryRule.
   * @param ctx The context for the rule being entered.
   */
  void enterEveryRule(antlr4::ParserRuleContext * ctx) override;

  /*!
   * @brief Called automatically by the ANTLR ParseTreeWalker when exiting a parser rule context.
   * Populates the ending position info for the current ASTNode.
   * Moves the current node pointer back up to its parent.
   * Overrides antlr4::tree::ParseTreeListener::exitEveryRule.
   * @param ctx The context for the rule being exited.
   */
  void exitEveryRule(antlr4::ParserRuleContext * ctx) override;

  /*!
   * @brief Called automatically by the ANTLR ParseTreeWalker when visiting a terminal node (token).
   * Adds the token's text to the 'token' field of the current ASTNode's info JSON object.
   * Overrides antlr4::tree::ParseTreeListener::visitTerminal.
   * @param node The terminal node being visited.
   */
  void visitTerminal(antlr4::tree::TerminalNode * node) override;

  /*!
   * @brief Called automatically by the ANTLR ParseTreeWalker when visiting an error node.
   * This implementation currently does nothing when an error node is encountered during the walk.
   * Overrides antlr4::tree::ParseTreeListener::visitErrorNode.
   * @param node The error node being visited (unused).
   */
  void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override;

  /*!
   * @brief Serializes the constructed internal AST (rooted at m_root) into the provided JSON object.
   * Traverses the internal ASTNode tree and builds a corresponding JSON structure.
   * @param astJson Output parameter: A JSON object that will be populated with the AST structure.
   *                The structure mirrors the ASTNode tree, with 'children' arrays.
   */
  void BuildAST(ScJson & astJson);

  /*!
   * @brief Destructor.
   * Responsible for cleaning up the dynamically allocated ASTNode objects created during the parse tree walk.
   * Calls a recursive helper function (`removeAllASTNodes` in the .cpp file) to delete the tree.
   */
  ~SCsASTJsonListener() override;

protected:
  /*! @brief Reference to the ANTLR parser instance provided during construction. */
  antlr4::Parser & m_parser;
  /*! @brief Reference to the vector of rule names obtained from the parser. */
  std::vector<std::string> const & m_ruleNames;

  /*! @brief Pointer to the root node of the constructed AST. Null initially. */
  ASTNode * m_root = nullptr;
  /*! @brief Pointer to the current ASTNode being processed during the tree walk. */
  ASTNode * m_currentNode = nullptr;
  /*! @brief Pointer to the parent of the current node, used when exiting rules. */
  ASTNode * m_parentNode = nullptr;

  /*!
   * @brief Helper method to populate the starting information (rule type, start position)
   *        into a JSON object representing an AST node.
   * @param ctx The parser rule context providing the start token information.
   * @param nodeInfo The JSON object (part of an ASTNode's info) to populate.
   */
  void BuildTokenStartInfo(antlr4::ParserRuleContext * ctx, ScJson & nodeInfo);

  /*!
   * @brief Helper method to populate the ending position information into a JSON object
   *        representing an AST node.
   * @param ctx The parser rule context providing the stop token information.
   * @param nodeInfo The JSON object (part of an ASTNode's info) to populate.
   */
  void BuildTokenStopInfo(antlr4::ParserRuleContext * ctx, ScJson & nodeInfo);
};

} // namespace scs

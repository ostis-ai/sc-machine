/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/
#pragma once

#include "nlKeynodes.hpp"

#include <sc-memory/cpp/sc_stream.hpp>
#include <sc-memory/cpp/sc_types.hpp>

#include <stack>

namespace nl
{
namespace util
{

class Token
{
  friend class TokensRender;

public:
  enum class Type : uint8_t
  {
    String,
    MainIdtf
  };

protected:
  virtual std::string Render(ScMemoryContext & ctx) const = 0;
};

// Doesn't change input data
class StringToken : public Token
{
public:
  explicit StringToken(std::string const & value) : m_value(value)
  {
  }

private:
  virtual std::string Render(ScMemoryContext &) const override
  {
    return m_value;
  };

private:
  std::string m_value;
};

// Resolve main idtf of element
class MainIdtfToken : public Token
{
public:
  MainIdtfToken(std::string const & value, ScAddr const & langAddr)
    : m_sysIdtf(value)
    , m_lang(langAddr)
  {
  }

  virtual std::string Render(ScMemoryContext & ctx) const override
  {
    ScAddr addr;
    if (ctx.HelperFindBySystemIdtf(m_sysIdtf, addr))
    {
      ScTemplate templ;

      templ.TripleWithRelation(
        addr,
        ScType::EdgeDCommonVar,
        ScType::Link >> "_link",
        ScType::EdgeAccessVarPosPerm,
        Keynodes::ms_nrelMainIdtf);

      templ.Triple(
        m_lang,
        ScType::EdgeAccessVarPosPerm,
        "_link");

      ScTemplateSearchResult res;
      if (!ctx.HelperSearchTemplate(templ, res))
        return m_sysIdtf;

      SC_ASSERT(res.Size() == 1, ("Invalid state of knowledge base"));
      
      // get content of a link and return it
      ScStream stream;
      if (!ctx.GetLinkContent(res[0]["_link"], stream))
        return m_sysIdtf;

      std::string result;
      if (!ScStreamConverter::StreamToString(stream, result))
        return m_sysIdtf;

      return result;
    }

    return m_sysIdtf;
  }

private:
  std::string m_sysIdtf;
  ScAddr m_lang;
};

using TokenList = std::vector<Token *>;

// --------------------------------
class Tokenizer
{
public:
  explicit Tokenizer(ScAddr const & lang)
    : m_lang(lang)
  {
  }

  ~Tokenizer()
  {
    for (Token * t : m_tokens)
      delete t;
  }

  sc_result operator() (std::string const & inputString)
  {
    auto CleanTokenInternal = [](std::string val)
    {
      // TODO: fix during utf-8 support
      val = val.substr(2, val.size() - 4); // remove {%%}
      utils::StringUtils::Trim(val);
      return val;
    };

    // parse string with simple stack
    std::stack<size_t> openStack;

    // TODO: support utf-8 (check)
    size_t lastString = 0;

    size_t prev = 0, curr = 1;
    while (curr < inputString.size())
    {
      // TODO: support brackects hierarchy

      // open
      if (inputString[prev] == '{' && inputString[curr] == '%')
      {
        if (openStack.empty())
        {
          SC_ASSERT(curr - lastString > 0, ());
          std::string const tokenInternal = inputString.substr(lastString, curr - lastString - 1);
          m_tokens.push_back(new StringToken(tokenInternal));
        }

        openStack.push(prev);
      }
      else if (inputString[prev] == '%' && inputString[curr] == '}')
      {
        if (openStack.empty())
          return SC_RESULT_ERROR_INVALID_PARAMS;

        size_t const openPos = openStack.top();
        openStack.pop();

        std::string const tokenInternal = CleanTokenInternal(inputString.substr(openPos, curr - openPos));
        Token * token = ResolveToken(tokenInternal);

        // TODO: provide error for unknown token
        SC_ASSERT(token != nullptr, ());
        m_tokens.push_back(token);

        lastString = curr + 1;
      }
      
      ++curr;
      ++prev;
    }

    // check if parsed correctly
    if (!openStack.empty())
      return SC_RESULT_ERROR_INVALID_PARAMS;

    return SC_RESULT_OK;
  }

  TokenList const & GetTokens() const
  {
    return m_tokens;
  }

private:
  Token * ResolveToken(std::string const & tokenBody) const
  {
    if (utils::StringUtils::StartsWith(tokenBody, "$", false))
      return new MainIdtfToken(tokenBody.substr(1), m_lang);

    return nullptr;
  }

private:
  TokenList m_tokens;
  
  ScAddr m_lang;
};

// --------------------------------
class TokensRender
{
public:
  std::string operator() (TokenList const & tokens, ScMemoryContext & ctx)
  {
    std::string result;
    for (Token const * t : tokens)
      result += t->Render(ctx);
    return result;
  }
};

} // namespace util
} // namespace nl

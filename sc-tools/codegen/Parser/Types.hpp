#pragma once

#include <cassert>
#include <utility>
#include <vector>
#include <list>
#include <set>
#include <sstream>

#include "ReservedTypes.hpp"

using Namespace = std::vector<std::string>;

class Exception final : public std::exception
{
public:
  Exception(std::string descr)
    : m_description(std::move(descr))
  {
  }

  std::string const & GetDescription() const
  {
    return m_description;
  }

private:
  std::string m_description;
};

#ifndef EMIT_ERROR
#  define EMIT_ERROR(__desc) \
    ({ \
      std::stringstream ss; \
      ss << __desc; \
      throw Exception(ss.str()); \
    })
#endif

typedef std::list<std::string> tStringList;

#pragma once

#include "Types.hpp"

class Cursor;

class Invokable
{
public:
  using Signature = std::vector<std::string>;

  Invokable(Cursor const & cursor);

protected:
  std::string m_returnType;

  Signature m_signature;
};

const auto kReturnTypeVoid = "void";

#pragma once

#include "Cursor.hpp"
#include "Namespace.hpp"

class Invokable
{
public:
    typedef std::vector<std::string> Signature;

    Invokable(const Cursor &cursor);

protected:
    std::string m_returnType;

    Signature m_signature;
};

const auto kReturnTypeVoid = "void";
#pragma once

#include "LanguageType.hpp"
#include "Invokable.hpp"

class Function 
    : public LanguageType
    , public Invokable
{
public:
    Function(const Cursor & cursor, const Namespace & currentNamespace, class Class * parent = nullptr);

    virtual ~Function(void);

private:
    class Class * m_parent;

    std::string m_name;
};

#pragma once

#include "LanguageType.hpp"
#include "Invokable.hpp"

class Method : public LanguageType, public Invokable
{
public:
    Method(const Cursor & cursor, const Namespace & currentNamespace, class Class * parent = nullptr);

    virtual ~Method(void);
    
private:
    class Class * m_parent;
    std::string m_name;
};

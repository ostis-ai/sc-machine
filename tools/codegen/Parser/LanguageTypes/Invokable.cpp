#include "Precompiled.hpp"

#include "LanguageTypes/Invokable.hpp"

#include <boost/algorithm/string/join.hpp>

Invokable::Invokable(const Cursor &cursor)
    : m_returnType(cursor.GetReturnType().GetDisplayName())
{
    auto type = cursor.GetType();
    unsigned count = type.GetArgumentCount();

    m_signature.clear();

    for (unsigned i = 0; i < count; ++i)
    {
        auto argument = type.GetArgument(i);

        // we need to make sure we have the qualified namespace
        if (argument.GetKind() == CXType_Typedef)
        {
            auto declaration = argument.GetDeclaration();

            auto parent = declaration.GetLexicalParent();

            Namespace parentNamespace;

            // walk up to the root namespace
            while (parent.GetKind() == CXCursor_Namespace)
            {
                parentNamespace.emplace_back(parent.GetDisplayName());

                parent = parent.GetLexicalParent();
            }

            // add the display name as the end of the namespace
            parentNamespace.emplace_back(
                argument.GetDisplayName()
           );

            auto qualifiedName = boost::algorithm::join(parentNamespace, "::");

            m_signature.emplace_back(qualifiedName);
        }
        // it should already be qualified
        else
        {
            m_signature.emplace_back(
                argument.GetDisplayName()
           );
        }
    }
}

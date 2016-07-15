/* ----------------------------------------------------------------------------
** © 201x Austin Brunkhorst, All Rights Reserved.
**
** MetaUtils.h
** --------------------------------------------------------------------------*/

#pragma once

#include "Cursor.hpp"
#include "Namespace.hpp"


namespace utils
{
    void ToString(const CXString &str, std::string &output);

    std::string GetQualifiedName(
    	const std::string &displayName, 
    	const Namespace &currentNamespace
	);

    std::string GetQualifiedName(
    	const Cursor &cursor, 
    	const Namespace &currentNamespace
	);

    void FatalError(const std::string &error);
}

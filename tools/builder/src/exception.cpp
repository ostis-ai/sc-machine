/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "exception.h"

Exception::Exception(Exception::eCodes code, const String &typeName, const String &descr, const String &fileName, int line)
    : mCode(code)
    , mTypeName(typeName)
    , mDescription(descr)
    , mFileName(fileName)
    , mLine(line)
{
}

const String& Exception::getTypeName() const
{
    return mTypeName;
}

const String& Exception::getDescription() const
{
    return mDescription;
}

const String& Exception::getFileName() const
{
    return mFileName;
}

int Exception::getLineNumber() const
{
    return mLine;
}

Exception::eCodes Exception::getCode() const
{
    return mCode;
}

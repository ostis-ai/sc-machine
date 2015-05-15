/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _exception_h_
#define _exception_h_

#include "types.h"

class Exception : public std::exception
{
public:
    enum eCodes
    {
        ERR_PARSE,
        ERR_INTERNAL_ERROR,
        ERR_FILE_NOT_FOUND,
        ERR_ITEM_NOT_FOUND,
        ERR_DUPLICATE_ITEM,
        ERR_INVALID_STATE,
        ERR_INVALID_PARAMS,
        ERR_NOT_IMPLEMENTED
    };

    explicit Exception(eCodes code, const String &typeName, const String &descr, const String &fileName, int line);
    ~Exception() throw() {}

    //! Returns type name of exception
    const String& getTypeName() const;
    //! Returns exception description
    const String& getDescription() const;
    //! Returns file name
    const String& getFileName() const;
    //! Returns line number
    int getLineNumber() const;
    //! Returns error code
    eCodes getCode() const;

private:
    //! Error code
    eCodes mCode;
    //! Exception type name
    String mTypeName;
    //! Error decription
    String mDescription;
    //! File where error occurs
    String mFileName;
    //! Line number, where error occurs
    int mLine;
};

template <int num>
struct ExceptionCodeType
{
    enum { number = num };
};

class ParseException : public Exception
{
public:
    ParseException(eCodes code, const String &descr, const String &fileName, int line)
        : Exception(code, "ParseError", descr, fileName, line)
    {
    }
};

class InternalErrorException : public Exception
{
public:
    InternalErrorException(eCodes code, const String &descr, const String &fileName, int line)
        : Exception(code, "InternalError", descr, fileName, line)
    {
    }
};

class FileNotFoundException : public Exception
{
public:
    FileNotFoundException(eCodes code, const String &descr, const String &fileName, int line)
        : Exception(code, "FileNotFound", descr, fileName, line)
    {
    }
};

class ItemNotFoundException : public Exception
{
public:
    ItemNotFoundException(eCodes code, const String &descr, const String &fileName, int line)
        : Exception(code, "ItemNotFound", descr, fileName, line)
    {
    }
};

class DuplicateItemException : public Exception
{
public:
    DuplicateItemException(eCodes code, const String &descr, const String &fileName, int line)
        : Exception(code, "DuplicateItem", descr, fileName, line)
    {
    }
};

class InvalidStateException : public Exception
{
public:
    InvalidStateException(eCodes code, const String &descr, const String &fileName, int line)
        : Exception(code, "InvalidState", descr, fileName, line)
    {
    }
};

class InvalidParamsException : public Exception
{
public:
    InvalidParamsException(eCodes code, const String &descr, const String &fileName, int line)
        : Exception(code, "InvalidParams", descr, fileName, line)
    {
    }
};

class NotImplementedException : public Exception
{
public:
    NotImplementedException(eCodes code, const String &descr, const String &fileName, int line)
        : Exception(code, "NotImplemented", descr, fileName, line)
    {
    }
};

class ExceptionFactory
{
private:
    /// Private constructor, no construction
    ExceptionFactory() {}
public:
    static ParseException create(
                ExceptionCodeType<Exception::ERR_PARSE> /*code*/,
                const String& desc,
                const String& file,
                long line)
            {
                return ParseException(Exception::ERR_PARSE, desc, file, line);
            }

    static InternalErrorException create(
                ExceptionCodeType<Exception::ERR_INTERNAL_ERROR> /*code*/,
                const String& desc,
                const String& file,
                long line)
            {
                return InternalErrorException(Exception::ERR_INTERNAL_ERROR, desc, file, line);
            }

    static FileNotFoundException create(
                ExceptionCodeType<Exception::ERR_FILE_NOT_FOUND> /*code*/,
                const String& desc,
                const String& file,
                long line)
            {
                return FileNotFoundException(Exception::ERR_FILE_NOT_FOUND, desc, file, line);
            }

    static ItemNotFoundException create(
                ExceptionCodeType<Exception::ERR_ITEM_NOT_FOUND> /*code*/,
                const String& desc,
                const String& file,
                long line)
            {
                return ItemNotFoundException(Exception::ERR_ITEM_NOT_FOUND, desc, file, line);
            }

    static DuplicateItemException create(
                ExceptionCodeType<Exception::ERR_DUPLICATE_ITEM> /*code*/,
                const String& desc,
                const String& file,
                long line)
            {
                return DuplicateItemException(Exception::ERR_DUPLICATE_ITEM, desc, file, line);
            }

    static InvalidStateException create(
                ExceptionCodeType<Exception::ERR_INVALID_STATE> /*code*/,
                const String& desc,
                const String& file,
                long line)
            {
                return InvalidStateException(Exception::ERR_INVALID_STATE, desc, file, line);
            }

    static InvalidParamsException create(
                ExceptionCodeType<Exception::ERR_INVALID_PARAMS> /*code*/,
                const String& desc,
                const String& file,
                long line)
            {
                return InvalidParamsException(Exception::ERR_INVALID_PARAMS, desc, file, line);
            }

    static NotImplementedException create(
                ExceptionCodeType<Exception::ERR_NOT_IMPLEMENTED> /*code*/,
                const String& desc,
                const String& file,
                long line)
            {
                return NotImplementedException(Exception::ERR_NOT_IMPLEMENTED, desc, file, line);
            }
};

#ifndef THROW_EXCEPT
#define THROW_EXCEPT(__num, __desc, __file, __line) \
    {StringStream __ss; __ss << __desc; throw ExceptionFactory::create(ExceptionCodeType<__num>(), __ss.str(), __file, __line);}
#endif

#endif

/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "utils/sc_log.hpp"
#include "utils/sc_message.hpp"
#include "utils/sc_console.hpp"

namespace utils
{

/// -----------------------

class ScException : public std::exception
{
public:
  _SC_EXTERN ScException(std::string const & description);
  _SC_EXTERN virtual ~ScException() throw();

  _SC_EXTERN const char* Message() const throw();

private:
  std::string m_description;
};

class ExceptionAssert final : public ScException
{
public:
  explicit ExceptionAssert(std::string const & msg) : ScException("Assert: " + msg) {}
};

class ExceptionCritical final : public ScException
{
public:
  explicit ExceptionCritical(std::string const & msg) : ScException("Critical: " + msg) {}
};

class ExceptionInvalidParams final : public ScException
{
public:
  explicit ExceptionInvalidParams(std::string const & msg) : ScException("InvalidParams: " + msg) {}
};

class ExceptionInvalidState final : public ScException
{
public:
  explicit ExceptionInvalidState(std::string const & msg) : ScException("InvalidState: " + msg) {}
};

class ExceptionItemNotFound final : public ScException
{
public:
  explicit ExceptionItemNotFound(std::string const & msg) : ScException("ItemNotFound: " + msg) {}
};

class ExceptionParseError final : public ScException
{
public:
  explicit ExceptionParseError(std::string const & msg) : ScException("ParseError: " + msg) {}
};

#define error(__str) { throw ScException(__str); }
#define error_invalid_params(__str) { throw ScExceptionInvalidParams(__str); }

// Asserts
#define THROW_EXCEPTION(_exception_class, _msg, _file, _line) \
{ \
  std::stringstream _str_message; \
  _str_message << _msg; \
  _str_message << std::endl << "File: " << _file << std::endl << "Line:" << _line << std::endl; \
  throw _exception_class(_str_message.str()); \
}

#define SC_THROW_EXCEPTION(_exception_class, _msg) THROW_EXCEPTION(_exception_class, _msg, __FILE__, __LINE__)

#define _ASSERT_IMPL(_expr, _msg, _file, _line) \
{ \
  if (!(_expr)) \
{ \
  std::string _message = ::utils::impl::Message("SC_ASSERT("#_expr")", ::utils::impl::Message _msg); \
  THROW_EXCEPTION(::utils::ExceptionAssert, _message, _file, _line); \
} \
}

#if SC_DEBUG_MODE
#	define SC_ASSERT(_expr, _msg) { _ASSERT_IMPL( _expr, _msg, __FILE__, __LINE__ ); }
// will be removed use SC_ASSERT instead
#	define ASSERT(_expr, _msg) { _ASSERT_IMPL(_expr, _msg, __FILE__, __LINE__); }
#else
#	define SC_ASSERT(_expr, _msg)
// will be removed use SC_ASSERT instead
#	define ASSERT(_expr, _msg)
#endif

#define _CHECK_IMPL(_expr, _msg, _name) \
  do \
{ \
  if (_expr) {} else \
{ \
  std::string _message = ::utils::impl::Message(std::string(_name) + "("#_expr")", ::utils::impl::Message _msg); \
  THROW_EXCEPTION(::utils::ExceptionAssert, _message, __FILE__, __LINE__); \
} \
} while (false);	// stop runtime

#define SC_CHECK(_expr, _msg) _CHECK_IMPL(_expr, _msg, "CHECK")
#define SC_CHECK_NOT(_expr, _msg) _CHECK_IMPL(!_expr, _msg, "CHECK_NOT")
#define SC_CHECK_EQUAL(_a, _b, _msg) \
  do \
{ \
  if ((_a) == (_b)) {} else \
{ \
  std::string _message = ::utils::impl::Message(std::string("SC_CHECK_EQUAL") + "("#_a" == "#_b")", ::utils::impl::Message _msg); \
  THROW_EXCEPTION(::utils::ExceptionAssert, _message, __FILE__, __LINE__); \
} \
} while (false);	// stop runtime

#define SC_CHECK_NOT_EQUAL(_a, _b, _msg) \
  do \
{ \
  if ((_a) != (_b)) {} else \
{ \
  std::string _message = ::utils::impl::Message(std::string("SC_CHECK_NOT_EQUAL") + "("#_a" != "#_b")", ::utils::impl::Message _msg); \
  THROW_EXCEPTION(::utils::ExceptionAssert, _message, __FILE__, __LINE__); \
} \
} while (false);	// stop runtime

#define SC_CHECK_GREAT(_a, _b, _msg) \
  do \
{ \
  if ((_a) > (_b)) {} else \
{ \
  std::string _message = ::utils::impl::Message(std::string("SC_CHECK_GREAT") + "("#_a" > "#_b")", ::utils::impl::Message _msg); \
  THROW_EXCEPTION(::utils::ExceptionAssert, _message, __FILE__, __LINE__); \
} \
} while (false);	// stop runtime

#define SC_CHECK_GREAT_EQ(_a, _b, _msg) \
  do \
{ \
  if ((_a) >= (_b)) {} else \
{ \
  std::string _message = ::utils::impl::Message(std::string("SC_CHECK_GREAT_EQ") + "("#_a" >= "#_b")", ::utils::impl::Message _msg); \
  THROW_EXCEPTION(::utils::ExceptionAssert, _message, __FILE__, __LINE__); \
} \
} while (false);	// stop runtime

#define SC_CHECK_LESS(_a, _b, _msg) \
  do \
{ \
  if ((_a) < (_b)) {} else \
{ \
  std::string _message = ::utils::impl::Message(std::string("SC_CHECK_LESS") + "("#_a" < "#_b")", ::utils::impl::Message _msg); \
  THROW_EXCEPTION(::utils::ExceptionAssert, _message, __FILE__, __LINE__); \
} \
} while (false);	// stop runtime

#define SC_CHECK_LESS_EQ(_a, _b, _msg) \
  do \
{ \
  if ((_a) <= (_b)) {} else \
{ \
  std::string _message = ::utils::impl::Message(std::string("SC_CHECK_LESS_EQ") + "("#_a" <= "#_b")", ::utils::impl::Message _msg); \
  THROW_EXCEPTION(::utils::ExceptionAssert, _message, __FILE__, __LINE__); \
} \
} while (false);	// stop runtime


} // namespace utils

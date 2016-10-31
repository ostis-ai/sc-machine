#pragma once

#include "utils/sc_log.hpp"
#include "utils/sc_message.hpp"

#include "sc_utils.hpp"

namespace utils
{

/// -----------------------

class AssertException final : public ScException
{
public:
	explicit AssertException(std::string const & msg) : ScException("Assert: " + msg) {}
};

class CriticalException final : public ScException 
{
public:
	explicit CriticalException(std::string const & msg) : ScException("Critical: " + msg) {}
};


// Asserts
#define THROW_EXCEPTION(_exception_class, _msg, _file, _line) \
{ \
	std::stringstream _str_message; \
	_str_message << _msg; \
	_str_message << std::endl << "File: " << _file << std::endl << "Line:" << _line << std::endl; \
	throw _exception_class(_str_message.str()); \
}

#define _ASSERT_IMPL(_expr, _msg, _file, _line) { if (!(_expr)) { THROW_EXCEPTION(::utils::AssertException, _msg, _file, _line); } }

#if DNA_DEBUG_MODE
#	define ASSERT(_expr, _msg) { bool v = (_expr); _ASSERT_IMPL(_expr, _msg, __FILE__, __LINE__) }
#else
#	define ASSERT(_expr, _msg)
#endif

#define _CHECK_IMPL(_expr, _msg, _name) \
	do \
	{ \
		if (_expr) {} else \
		{ \
			std::string _message = ::utils::impl::Message(std::string(_name) + "("#_expr")", ::utils::impl::Message _msg); \
			THROW_EXCEPTION(::utils::AssertException, _message, __FILE__, __LINE__); \
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
			THROW_EXCEPTION(::utils::AssertException, _message, __FILE__, __LINE__); \
		} \
	} while (false);	// stop runtime

#define SC_CHECK_NOT_EQUAL(_a, _b, _msg) \
	do \
	{ \
		if ((_a) != (_b)) {} else \
		{ \
			std::string _message = ::utils::impl::Message(std::string("SC_CHECK_NOT_EQUAL") + "("#_a" != "#_b")", ::utils::impl::Message _msg); \
			THROW_EXCEPTION(::utils::AssertException, _message, __FILE__, __LINE__); \
		} \
	} while (false);	// stop runtime

#define SC_CHECK_GREAT(_a, _b, _msg) \
	do \
	{ \
		if ((_a) > (_b)) {} else \
		{ \
			std::string _message = ::utils::impl::Message(std::string("SC_CHECK_GREAT") + "("#_a" > "#_b")", ::utils::impl::Message _msg); \
			THROW_EXCEPTION(::utils::AssertException, _message, __FILE__, __LINE__); \
		} \
	} while (false);	// stop runtime

#define SC_CHECK_GREAT_EQ(_a, _b, _msg) \
	do \
	{ \
		if ((_a) >= (_b)) {} else \
		{ \
			std::string _message = ::utils::impl::Message(std::string("SC_CHECK_GREAT_EQ") + "("#_a" >= "#_b")", ::utils::impl::Message _msg); \
			THROW_EXCEPTION(::utils::AssertException, _message, __FILE__, __LINE__); \
		} \
	} while (false);	// stop runtime

#define SC_CHECK_LESS(_a, _b, _msg) \
	do \
	{ \
		if ((_a) < (_b)) {} else \
		{ \
			std::string _message = ::utils::impl::Message(std::string("SC_CHECK_LESS") + "("#_a" < "#_b")", ::utils::impl::Message _msg); \
			THROW_EXCEPTION(::utils::AssertException, _message, __FILE__, __LINE__); \
		} \
	} while (false);	// stop runtime

#define SC_CHECK_LESS_EQ(_a, _b, _msg) \
	do \
	{ \
		if ((_a) <= (_b)) {} else \
		{ \
			std::string _message = ::utils::impl::Message(std::string("SC_CHECK_LESS_EQ") + "("#_a" <= "#_b")", ::utils::impl::Message _msg); \
			THROW_EXCEPTION(::utils::AssertException, _message, __FILE__, __LINE__); \
		} \
	} while (false);	// stop runtime




} // namespace utils

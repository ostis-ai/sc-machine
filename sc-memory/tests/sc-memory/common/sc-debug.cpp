#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"

TEST(ScException, ExceptionAssert)
{
  try
  {
    SC_THROW_EXCEPTION(utils::ExceptionAssert, "Test exception assert");
  }
  catch (utils::ScException const & e)
  {
    SC_LOG_ERROR(e.Message());
    SC_LOG_ERROR(e.Description());
  }
}

TEST(ScException, ExceptionCritical)
{
  try
  {
    SC_THROW_EXCEPTION(utils::ExceptionCritical, "Test exception critical");
  }
  catch (utils::ScException const & e)
  {
    SC_LOG_ERROR(e.Message());
    SC_LOG_ERROR(e.Description());
  }
}

TEST(ScException, ExceptionInvalidParams)
{
  try
  {
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Test exception invalid params");
  }
  catch (utils::ScException const & e)
  {
    SC_LOG_ERROR(e.Message());
    SC_LOG_ERROR(e.Description());
  }
}

TEST(ScException, ExceptionInvalidState)
{
  try
  {
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Test exception invalid state");
  }
  catch (utils::ScException const & e)
  {
    SC_LOG_ERROR(e.Message());
    SC_LOG_ERROR(e.Description());
  }
}

TEST(ScException, ExceptionInvalidType)
{
  try
  {
    SC_THROW_EXCEPTION(utils::ExceptionInvalidType, "Test exception invalid type");
  }
  catch (utils::ScException const & e)
  {
    SC_LOG_ERROR(e.Message());
    SC_LOG_ERROR(e.Description());
  }
}

TEST(ScException, ExceptionItemNotFound)
{
  try
  {
    SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "Test exception item not found");
  }
  catch (utils::ScException const & e)
  {
    SC_LOG_ERROR(e.Message());
    SC_LOG_ERROR(e.Description());
  }
}

TEST(ScException, ExceptionNotImplemented)
{
  try
  {
    SC_THROW_EXCEPTION(utils::ExceptionNotImplemented, "Test exception not implemented");
  }
  catch (utils::ScException const & e)
  {
    SC_LOG_ERROR(e.Message());
    SC_LOG_ERROR(e.Description());
  }
}

TEST(ScException, ExceptionParseError)
{
  try
  {
    SC_THROW_EXCEPTION(utils::ExceptionParseError, "Test exception parse error");
  }
  catch (utils::ScException const & e)
  {
    SC_LOG_ERROR(e.Message());
    SC_LOG_ERROR(e.Description());
  }
}

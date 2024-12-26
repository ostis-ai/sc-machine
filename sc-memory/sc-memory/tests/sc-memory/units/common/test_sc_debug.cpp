/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <sc-memory/test/sc_test.hpp>

#include <sc-memory/sc_memory.hpp>

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

TEST(ScLogLevelTest, DefaultLogLevelIsUnknown)
{
  utils::ScLogLevel logLevel;
  EXPECT_EQ(logLevel.ToString(), "Unknown");
}

TEST(ScLogLevelTest, FromStringConvertsCorrectly)
{
  utils::ScLogLevel logLevel;
  EXPECT_EQ(logLevel.FromString("Error").ToString(), "Error");
  EXPECT_EQ(logLevel.FromString("Warning").ToString(), "Warning");
  EXPECT_EQ(logLevel.FromString("Info").ToString(), "Info");
  EXPECT_EQ(logLevel.FromString("Debug").ToString(), "Debug");
  EXPECT_EQ(logLevel.FromString("Invalid").ToString(), "Unknown");
}

TEST(ScLogLevelTest, LogLevelComparison)
{
  utils::ScLogLevel errorLevel(utils::ScLogLevel::Error);
  utils::ScLogLevel warningLevel(utils::ScLogLevel::Warning);
  utils::ScLogLevel infoLevel(utils::ScLogLevel::Info);
  utils::ScLogLevel debugLevel(utils::ScLogLevel::Debug);

  EXPECT_TRUE(errorLevel <= warningLevel);
  EXPECT_TRUE(warningLevel <= infoLevel);
  EXPECT_TRUE(infoLevel <= debugLevel);
  EXPECT_FALSE(debugLevel <= errorLevel);
}

TEST(ScLogLevelTest, LogLevelAssignment)
{
  utils::ScLogLevel logLevel;
  logLevel.FromString("Warning");
  EXPECT_EQ(logLevel.ToString(), "Warning");

  logLevel = utils::ScLogLevel(utils::ScLogLevel::Error);
  EXPECT_EQ(logLevel.ToString(), "Error");
}

class ScLoggerTest : public testing::Test
{
protected:
  virtual void SetUp()
  {
    m_savedBuffer = std::cout.rdbuf();
    std::cout.rdbuf(m_loggerResult.rdbuf());
    m_logger = utils::ScLogger(utils::ScLogger::ScLogType::Console, "", utils::ScLogLevel::Level::Debug);
  }

  virtual void TearDown()
  {
    std::cout.rdbuf(m_savedBuffer);
  }

protected:
  utils::ScLogger m_logger;
  std::stringstream m_loggerResult;
  std::streambuf * m_savedBuffer;
};

TEST_F(ScLoggerTest, ErrorPrint)
{
  m_logger.Error("it", " is", " error", " message");

  std::string const & result = m_loggerResult.str();

  std::stringstream expectedStream;
  expectedStream << "[Error]: " << impl::GetANSIColor(ScConsole::Color::Red) << "it is error message";
  EXPECT_NE(result.find(expectedStream.str()), std::string::npos);
}

TEST_F(ScLoggerTest, WarningPrint)
{
  m_logger.Warning("it", " is", " warning", " message");

  std::string const & result = m_loggerResult.str();

  std::stringstream expectedStream;
  expectedStream << "[Warning]: " << impl::GetANSIColor(ScConsole::Color::Yellow) << "it is warning message";
  EXPECT_NE(result.find(expectedStream.str()), std::string::npos);
}

TEST_F(ScLoggerTest, InfoPrint)
{
  m_logger.Info("it", " is", " info", " message");

  std::string const & result = m_loggerResult.str();

  std::stringstream expectedStream;
  expectedStream << "[Info]: " << impl::GetANSIColor(ScConsole::Color::Grey) << "it is info message";
  EXPECT_NE(result.find(expectedStream.str()), std::string::npos);
}

TEST_F(ScLoggerTest, DebugPrint)
{
  m_logger.Debug("it", " is", " debug", " message");

  std::string const & result = m_loggerResult.str();

  std::stringstream expectedStream;
  expectedStream << "[Debug]: " << impl::GetANSIColor(ScConsole::Color::LightBlue) << "it is debug message";
  EXPECT_NE(result.find(expectedStream.str()), std::string::npos);
}

TEST_F(ScLoggerTest, ChangeSeverityLogLevel)
{
  m_logger.SetLogLevel(utils::ScLogLevel::Error);

  m_logger.Info("This message should not be logged.");

  EXPECT_TRUE(m_loggerResult.str().empty());
}

TEST_F(ScLoggerTest, MutePreventsLogging)
{
  m_logger.Mute();

  m_logger.Info("This message should not be logged.");

  EXPECT_TRUE(m_loggerResult.str().empty());

  m_logger.Unmute();
}

TEST_F(ScLoggerTest, MuteAndUnmuteLogging)
{
  m_logger.Mute();

  m_logger.Info("This message should not be logged.");

  EXPECT_TRUE(m_loggerResult.str().empty());

  m_logger.Unmute();

  m_logger.Info("This message should be logged.");

  EXPECT_NE(m_loggerResult.str().find("This message should be logged."), std::string::npos);
}

TEST_F(ScLoggerTest, PrefixForMessages)
{
  m_logger.SetPrefix("Prefix: ");

  m_logger.Info("This message should be logged.");

  EXPECT_NE(m_loggerResult.str().find("Prefix: This message should be logged."), std::string::npos);
}

TEST_F(ScLoggerTest, FileLoggingWritesCorrectMessage)
{
  std::string const testFile = "test_log.txt";

  std::ofstream ofs(testFile);
  ofs.close();

  m_logger.SetLogFile(testFile);
  m_logger.Info("This is a file log message.");

  std::ifstream ifs(testFile);
  std::string line;

  std::getline(ifs, line);

  ifs.close();

  EXPECT_NE(line.find("This is a file log message."), std::string::npos);
  std::remove(testFile.c_str());
}

TEST_F(ScLoggerTest, InvalidLogFilePath)
{
  std::string const testFile = "/invalid/path/to/log.txt";

  m_logger.SetLogFile(testFile);
  EXPECT_NO_THROW(m_logger.Info("This should not crash."));
}

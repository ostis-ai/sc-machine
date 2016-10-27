#include "sc_log.hpp"
#include "../sc_debug.hpp"

#include <ctime>
#include <iomanip>
#include <iostream>

namespace utils
{

ScLog * ScLog::ms_instance = nullptr;

ScLog::ScLog()
	: m_mode(Debug)
{
	ASSERT(!ms_instance, ());
	ms_instance = this;
}

ScLog::~ScLog()
{
	ms_instance = nullptr;
}

bool ScLog::Initialize(std::string const & file_name, eType mode /*= Info*/)
{
	m_mode = mode;
	m_file_stream.open(file_name, std::ofstream::out | std::ofstream::trunc);
	return m_file_stream.is_open();
}

void ScLog::Shutdown()
{
	m_file_stream.flush();
	m_file_stream.close();
}

void ScLog::Message(ScLog::eType type, std::string const & msg)
{
	if (m_mode <= type)
	{
		// get time
		std::time_t t = std::time(nullptr);
		std::tm tm = *std::localtime(&t);

		std::stringstream ss;
		ss << "[" << std::setw(2) << std::setfill('0') << tm.tm_hour
		   << ":" << std::setw(2) << std::setfill('0') << tm.tm_min
		   << ":" << std::setw(2) << std::setfill('0') << tm.tm_sec << "]: "
		   << msg << std::endl;

		std::cout << ss.str();
		m_file_stream << ss.str();
		m_file_stream.flush();
	}
}

} // namespace utils

#pragma once

#include "../sc_types.hpp"

#include <sstream>

namespace utils
{

class ScLog final
{
protected:
	_SC_EXTERN ScLog();
	_SC_EXTERN ~ScLog();

public:
	typedef enum
	{
		Debug = 0,
		Info,
		Warning,
		Error
	} eType;

	_SC_EXTERN bool Initialize(std::string const & file_name, eType mode = Info);
	_SC_EXTERN void Shutdown();

	/// TODO: thread safe
	_SC_EXTERN void Message(eType type, std::string const & msg);

	_SC_EXTERN static ScLog * GetInstance()
	{
		if (!ms_instance)
			return new ScLog();

		return ms_instance;
	}

private:
	std::ofstream m_file_stream;
	eType m_mode;

	static ScLog * ms_instance;
};


#define SC_LOG(__type, __msg) \
	{ std::stringstream ss; ss << __msg; ::utils::ScLog::GetInstance()->Message(__type, ss.str()); }

#define SC_LOG_DEBUG(__msg) SC_LOG(::utils::ScLog::Debug, __msg)
#define SC_LOG_INFO(__msg) SC_LOG(::utils::ScLog::Info, __msg)
#define SC_LOG_WARNING(__msg) SC_LOG(::utils::ScLog::Warning, __msg)
#define SC_LOG_ERROR(__msg) SC_LOG(::utils::ScLog::Error, __msg)

#define SC_LOG_INIT(__msg) SC_LOG_INFO("[init] " << __msg)
#define SC_LOG_SHUTDOWN(__msg) SC_LOG_INFO("[shutdown] " << __msg)
#define SC_LOG_LOAD(__msg) SC_LOG_INFO("[load] " << __msg)
#define SC_LOG_UNLOAD(__msg) SC_LOG_INFO("[unload] " << __msg)


} // namesapce utils

#ifndef _2DE_LOG_H_
#define _2DE_LOG_H_

#include "2de_Core.h"

namespace Deku2d
{

	/**
	* CLog - a class for logging.
	*/

	class CLog : public CTSingleton<CLog>
	{
	public:
		enum ELogMode
		{
			LOG_MODE_STDOUT,
			LOG_MODE_STDERR,
			LOG_MODE_FILE,
		};

		enum ELogFileWriteMode
		{
			LOG_FILE_WRITE_MODE_TRUNCATE,
			LOG_FILE_WRITE_MODE_APPEND,
		};

		~CLog();

		void WriteToLog(const char *Event, const char *Format, ...);

		bool isEnabled() const;
		void Toggle(bool AEnabled);

		ELogMode GetLogMode() const;
		void SetLogMode(ELogMode ALogMode);

		ELogFileWriteMode GetLogFileWriteMode() const;
		void SetLogFileWriteMode(ELogFileWriteMode ALogFileWriteMode);

		bool GetDatedLogFileNames() const;
		void SetDatedLogFileNames(bool ADatedLogFileNames);

		string GetLogFilePath() const;
		void SetLogFilePath(const string &ALogFilePath);

		string GetLogName() const;
		void SetLogName(const string &ALogName);

		int GetLogLevel() const;
		void SetLogLevel(int ALogLevel);

		int GetEventPriority(const string &AEvent) const;
		void SetEventPriority(const string &AEvent, int APriority);

		static const int MAX_LOG_STR_LENGTH = 1024;

	protected:
		CLog();
		friend class CTSingleton<CLog>;

	private:
		typedef map<string, int> EventsPrioritiesContainer;
		typedef EventsPrioritiesContainer::const_iterator EventsPrioritiesIterator;

		void SetStandardEventsPriorities();

		bool Enabled;
		ELogMode LogMode;
		ELogFileWriteMode LogFileWriteMode;
		bool DatedLogFileNames;
		ostream *Stream;
		string LogFilePath;
		string LogName;
		EventsPrioritiesContainer EventsPriorities;
		int LogLevel;

		char LogStr[MAX_LOG_STR_LENGTH];
	};

	// #define SIMPLIFIED_LOG // to use simple logging to std-out, instead of singleton-helled CLog... sometimes it's useful for debugging..
	#ifdef SIMPLIFIED_LOG
		#define Log Environment::LogToStdOut
	#else
		#define Log CLog::Instance()->WriteToLog
	#endif // SIMPLIFIED_LOG

}	//	namespace Deku2d

#endif	// _2DE_LOG_H_

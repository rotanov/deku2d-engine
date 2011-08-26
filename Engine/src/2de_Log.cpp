#include "2de_Log.h"
#include "2de_Engine.h"	// bad

namespace Deku2d
{
	//////////////////////////////////////////////////////////////////////////
	// CLog

	CLog::CLog()
	{
		SetName("Log");
		Enabled = true;
		LogMode = LOG_MODE_FILE;	// well, usual default behavior for user-space (non-system) programs is to log on a console,
						// but for now i left the behavior, some of us used to... may be changed any time by calling
						// CLog::SetLogMode(CLog::ELogMode ALogMode);

		LogFileWriteMode = LOG_FILE_WRITE_MODE_TRUNCATE;
		DatedLogFileNames = true;
		Stream = NULL;
		LogFilePath = "";
		LogName = "System";
		LogLevel = 0;

		SetStandardEventsPriorities();
	}

	CLog::~CLog()
	{
		if (Stream != NULL)
		{
			Log("INFO", "Log finished\n");

			if (LogMode == LOG_MODE_FILE)
			{
				dynamic_cast<ofstream*>(Stream)->close();
				delete Stream;
			}
		}
	}

	void CLog::WriteToLog(const char *Event, const char *Format, ...)
	{
		if (!Enabled)
			return;

		if (GetEventPriority(Event) < LogLevel)
			return;

		if (Stream == NULL)
		{
			SetLogMode(LogMode);
			if (Stream == NULL)
				return;
		}

		va_list args;
		va_start(args, Format);

		vsnprintf(LogStr, MAX_LOG_STR_LENGTH, Format, args);
		LogStr[MAX_LOG_STR_LENGTH - 1] = 0;

		va_end(args);

		string message = "[";
	#ifdef LOG_TIME_TICK
		message += to_string(SDL_GetTicks()) + "]\t\t[";
	#else
		message += Environment::DateTime::GetFormattedTime(Environment::DateTime::GetLocalTimeAndDate(), "%c") + "] [";
	#endif // LOG_TIME_TICK
		message += string(Event) + "] " + LogStr;

		/**Stream << "[";
	#ifdef LOG_TIME_TICK
		*Stream << SDL_GetTicks() << "]\t\t[";
	#else
		*Stream << CEnvironment::DateTime::GetFormattedTime(CEnvironment::DateTime::GetLocalTimeAndDate(), "%c") << "] [";
	#endif // LOG_TIME_TICK

		*Stream << Event << "] " << buffer << endl;*/

		*Stream << message << endl;

		if (CEngine::Instance()->isRunning())
		{
			CEvent *logOutputEvent = new CEvent("LogOutput", this);
			logOutputEvent->SetData("Text", message);
			CEventManager::Instance()->TriggerEvent(logOutputEvent);
		}
	}

	bool CLog::isEnabled() const
	{
		return Enabled;
	}

	void CLog::Toggle(bool AEnabled)
	{
		Enabled = AEnabled;
	}

	CLog::ELogMode CLog::GetLogMode() const 
	{
		return LogMode;
	}

	void CLog::SetLogMode(CLog::ELogMode ALogMode)
	{
		if (Stream != NULL)
		{
			Log("INFO", "Log finished\n");

			if (LogMode == LOG_MODE_FILE)
			{
				dynamic_cast<ofstream*>(Stream)->close();
				delete Stream;
			}
		}

		switch (ALogMode)
		{
		case LOG_MODE_STDOUT:
			Stream = &cout;
			Log("INFO", "Log to stdout started");
			break;
		case LOG_MODE_STDERR:
			Stream = &cerr;
			Log("INFO", "Log to stderr started");
			break;
		case LOG_MODE_FILE:
			ios_base::openmode om;

			switch (LogFileWriteMode)
			{
			case LOG_FILE_WRITE_MODE_TRUNCATE:
				om = ios_base::trunc;
				break;
			case LOG_FILE_WRITE_MODE_APPEND:
				om = ios_base::app;
				break;
			}

			string NewLogFileName = LogFilePath + LogName;

			if (DatedLogFileNames)
			{
				tm* TimeStruct = Environment::DateTime::GetLocalTimeAndDate();
				NewLogFileName += "-" + Environment::DateTime::GetFormattedTime(TimeStruct, "%y%m%d-%H%M%S");
			}
			NewLogFileName += ".log";

			Stream = new ofstream(NewLogFileName.c_str(), ios_base::out | om);
			if (Stream == NULL || !dynamic_cast<ofstream*>(Stream)->is_open())
			{
				cerr << "Can't open log file '" << NewLogFileName << "'." << endl;
				cerr << "Falling back to log to stdout..." << endl;
				SetLogMode(LOG_MODE_STDOUT);
				return;
			}
			Log("INFO", "Log to file '%s' started", NewLogFileName.c_str());
			break;
		}
		LogMode = ALogMode;
	}

	CLog::ELogFileWriteMode CLog::GetLogFileWriteMode() const
	{
		return LogFileWriteMode;
	}

	void CLog::SetLogFileWriteMode(CLog::ELogFileWriteMode ALogFileWriteMode)
	{
		LogFileWriteMode = ALogFileWriteMode;
	}

	bool CLog::GetDatedLogFileNames() const
	{
		return DatedLogFileNames;
	}

	void CLog::SetDatedLogFileNames(bool ADatedLogFileNames)
	{
		DatedLogFileNames = ADatedLogFileNames;
	}

	string CLog::GetLogFilePath() const
	{
		return LogFilePath;
	}

	void CLog::SetLogFilePath(const string &ALogFilePath)
	{
		LogFilePath = ALogFilePath;
	}

	string CLog::GetLogName() const
	{
		return LogName;
	}

	void CLog::SetLogName(const string &ALogName)
	{
		LogName = ALogName;
	}

	int CLog::GetLogLevel() const
	{
		return LogLevel;
	}

	void CLog::SetLogLevel(int ALogLevel)
	{
		LogLevel = ALogLevel;
	}

	int CLog::GetEventPriority(const string &AEvent) const
	{
		EventsPrioritiesIterator it = EventsPriorities.find(AEvent);
		if (it == EventsPriorities.end())
			return numeric_limits<int>::max();

		return it->second;
	}

	void CLog::SetEventPriority(const string &AEvent, int APriority)
	{
		EventsPriorities[AEvent] = APriority;
	}

	void CLog::SetStandardEventsPriorities()
	{
		SetEventPriority("DEBUG", 0);
		SetEventPriority("INFO", 1);
		SetEventPriority("WARNING", 2);
		SetEventPriority("ERROR", 3);
	}

}	//	namespace Deku2d

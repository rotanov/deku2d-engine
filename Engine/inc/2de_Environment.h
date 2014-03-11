#ifndef _2DE_ENVIRONMENT_H_
#define _2DE_ENVIRONMENT_H_

#include "2de_Core.h"

namespace Deku2D
{
	/**
	* Environment - contains functions and probably some constants that are related to environment.
	*
	* Ideally in future all (or not all) platform-dependent code should be here.
	*/

	namespace Environment
	{
		namespace DateTime
		{
			tm* GetLocalTimeAndDate();
			string GetFormattedTime(tm *TimeStruct, const char *Format);
		}	//	namespace DateTime

		namespace Paths
		{
			string GetExecutablePath();

			string GetWorkingDirectory();
			void SetWorkingDirectory(const string &AWorkingDirectory);

			string GetConfigPath();
			void SetConfigPath(const string &AConfigPath);

			string GetLogPath();
			void SetLogPath(const string &ALogPath);

			string GetUniversalDirectory();
		}	//	namespace Paths

		namespace Variables
		{
			string Get(const string &AName);
			void Set(const string &AName, const string &AValue);
		}	//	namespace Variables

		void LogToStdOut(const char *Event, const char *Format, ...);
		string GetLineTerminator();
	}	//	namespace Environment

}

#endif // _2DE_ENVIRONMENT_H_

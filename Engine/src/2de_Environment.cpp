#include "2de_Environment.h"

#include "2de_Engine.h"

namespace Deku2D
{
	//////////////////////////////////////////////////////////////////////////
	// Environment

	namespace Environment
	{

		namespace DateTime
		{

			tm* GetLocalTimeAndDate()
			{
				time_t RawTime;
				time(&RawTime);
				return localtime(&RawTime);
			}

			string GetFormattedTime(tm *TimeStruct, const char *Format)
			{
				int AllocSize = 128;
				bool allocated = false;

				char *buffer = NULL;

				while (!allocated)
				{
					delete[] buffer;
					buffer = new char[AllocSize];
					if (!buffer)
						return string();

					allocated = (strftime(buffer, AllocSize - 1, Format, TimeStruct) != 0);
					AllocSize *= 2;
				}
				
				string result = buffer;
				SAFE_DELETE_ARRAY(buffer);
				return result;
			}

		}

		namespace Paths
		{

			namespace
			{
				string ConfigPath = "Config/";
				string LogPath = "Logs/";
			};

			void DelFNameFromFPath(char *src)
			{
				int i = strlen(src)-1;
				while (src[i] != '/' && src[i] != '\\' && src[i] != ':')
					i--;
				src[i+1] = 0;
			}

			string GetExecutablePath()
			{
				string result;

			#ifdef _WIN32
				char *MainDir = new char[MAX_PATH];
				GetModuleFileName(GetModuleHandle(0), MainDir, MAX_PATH);
				DelFNameFromFPath(MainDir);

				result = MainDir;

				delete [] MainDir;
			#else
				// executable path is useless on *nix-like systems, so return empty string
			#endif // _WIN32

				return result;
			}

			string GetWorkingDirectory()
			{
				char dir[MAX_PATH];

			#ifdef _WIN32
				GetCurrentDirectory(MAX_PATH, dir);
			#else
				getcwd(dir, MAX_PATH);
			#endif //_WIN32

				return string(dir);
			}

			void SetWorkingDirectory(const string &AWorkingDirectory)
			{
			#ifdef _WIN32
				SetCurrentDirectory(AWorkingDirectory.c_str());
			#else
				chdir(AWorkingDirectory.c_str());
			#endif // _WIN32
			}

			string GetConfigPath()
			{
				return ConfigPath;
			}

			void SetConfigPath(const string &AConfigPath)
			{
				ConfigPath = AConfigPath;
			}

			string GetLogPath()
			{
				return LogPath;
			}

			void SetLogPath(const string &ALogPath)
			{
				LogPath = ALogPath;
			}

			/**
			* Environment::Paths::GetUniversalDirectory - возвращает стандартную для системы директорию для помещения всех изменяемых файлов программы, таких как конфиг, лог, возможно сэйвы и т. п.
			*
			* Название Universal здесь не очень удачное, но ничего лучше я придумать не смог...
			*/

			string GetUniversalDirectory()
			{
				string result;
			#ifdef _WIN32
				result = Environment::Variables::Get("APPDATA");
				if (!result.empty())
					result += "\\" + CEngine::Instance()->GetProgramName() + "\\";
			#else
				string HomeDir = Environment::Variables::Get("HOME");
				if (HomeDir.empty())
				{
					string UserName = Environment::Variables::Get("USER");
					if (UserName.empty())
						UserName = Environment::Variables::Get("USERNAME");

					if (!UserName.empty())
						HomeDir = "/home/" + UserName;
				}

				if (!HomeDir.empty())
					result = HomeDir + "/." + CEngine::Instance()->GetProgramName() + "/";
			#endif // _WIN32

				return result;
			}

		}

		namespace Variables
		{

			string Get(const string &AName)
			{
				char *var = getenv(AName.c_str());
				return ((var == NULL) ? "" : string(var));
			}

			void Set(const string &AName, const string &AValue)
			{
				static string PutEnvString;
				PutEnvString = AName + "=" + AValue;
				SDL_putenv(const_cast<char *>(PutEnvString.c_str()));
			}

		}

		/**
		* Environment::LogToStdOut - simplified log to the standard output, the way around singleton hell of CLog.
		*
		* This function is used by Log macro if SIMPLIFIED_LOG is defined (this is false by default). Also can be used manually.
		* Generally, it's useful in cases of debugging, and when the engine is not started, but you want to log something.
		*/

		void LogToStdOut(const char *Event, const char *Format, ...)
		{
			va_list args;
			va_start(args, Format);

			static char logStr[CLog::MAX_LOG_STR_LENGTH];

			vsnprintf(logStr, CLog::MAX_LOG_STR_LENGTH, Format, args);
			logStr[CLog::MAX_LOG_STR_LENGTH - 1] = 0;

			va_end(args);

			cout << "[" << Environment::DateTime::GetFormattedTime(Environment::DateTime::GetLocalTimeAndDate(), "%c") << "] ["
				<< Event << "] " << logStr << endl;
		}

		string GetLineTerminator()
		{
		#ifdef _WIN32
			static string LineTerminator = "\r\n";
		#else
			static string LineTerminator = "\n";
		#endif // _WIN32

			return LineTerminator;
		}

	}
}

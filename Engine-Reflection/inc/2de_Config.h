#ifndef _2DE_CONFIG_H_
#define _2DE_CONFIG_H_

#include "2de_Core.h"
#include "2de_Xml.h"

namespace Deku2D
{
	/**
	* CConfigStorage - a class that represents some abstract configuration storage. It's used as the base class for all concrete storage types.
	*/

	class CConfigStorage
	{
	public:
		CConfigStorage(const string &ARootNodeName);

		virtual string Get(const string &ASection, const string &AParameter);
		virtual void Set(const string &ASection, const string &AParameter, const string &AValue);
		virtual bool Exists(const string &ASection, const string &AParameter);
		virtual void Delete(const string &ASection, const string &AParameter);

		// TODO: or is it really needed? ok, ON_DEMAND_TODO
		//virtual void AddSection(const string &ASection);
		//virtual void RemoveSection(const string &ASection);

	protected:
		CXMLNode* GetRoot();
		void CreateInitialStructure();

		CXML Data;
		string RootNodeName;

	};

	/**
	* CConfigCommandLineStorage - a class that represents a configuration storage, that is obtained by conversion from command line arguments.
	*/

	class CConfigCommandLineStorage : public CConfigStorage
	{
	public:
		CConfigCommandLineStorage(const string &ARootNodeName);

	};

	/**
	* CConfigFileStorage - a class that represents a file configuration storage.
	*/

	class CConfigFileStorage : public CConfigStorage
	{
	public:
		CConfigFileStorage(const string &AFilename, const string &ARootNodeName);
		~CConfigFileStorage();

		void Set(const string &ASection, const string &AParameter, const string &AValue);
		void Delete(const string &ASection, const string &AParameter);

	private:
		void Save();

		string Filename;

	};

	/**
	* CConfigDefaultsStorage - a class that represents a default configuration storage.
	*/

	class CConfigDefaultsStorage : public CConfigStorage
	{
	public:
		CConfigDefaultsStorage(const string &ARootNodeName);

	};


	/**
	* CAbstractConfig - an abstract class that defines an interface for setting and getting configuration values, and provides convinience classes and functions.
	*/

	class CAbstractConfig
	{
	public:
		class CConfigParameter
		{
		public:
			CConfigParameter(CAbstractConfig *AConfig, const string &ASection, const string &AParameter);

			template<typename T>
			CConfigParameter& operator=(const T &ASource)
			{
				Config->Set(Section, Name, to_string(ASource));
				return *this;
			}

			template<typename T>
			operator T()
			{
				return from_string<T>(Config->Get(Section, Name));
			}

			template<typename T>
			T To()
			{
				return from_string<T>(Config->Get(Section, Name));
			}

		private:
			CAbstractConfig *Config;
			string Section;
			string Name;
		};

		class CConfigSection
		{
		public:
			CConfigSection(CAbstractConfig *AConfig, const string &ASection);

			CConfigParameter operator[](const string &AParameter);

		private:
			CAbstractConfig *Config;
			string Name;

		};

		virtual string Get(const string &ASection, const string &AParameter) = 0;
		virtual void Set(const string &ASection, const string &AParameter, const string &AValue) = 0;

		CConfigSection Section(const string &ASection);
	};


	/**
	* CConfig - a class that represents main engine configuration.
	*/

	class CConfig : public CAbstractConfig, public CObject
	{
	public:
		string Get(const string &ASection, const string &AParameter);
		void Set(const string &ASection, const string &AParameter, const string &AValue);


		static void AddDefault(const string &ASection, const string &AParameter, const string &AValue);

		// ON_DEMAND_TODO: void RestoreDefaults(); - copy settings from Defaults to FileConfig..
		// ON_DEMAND_TODO: maybe functions to access storages, for example for deleting parameters, adding sections, etc.

	private:
		CConfig();
		void FillDefaults();

		static const char *MAIN_CONFIG_ROOT_NODE_NAME;

		CConfigFileStorage FileConfig;
		CConfigCommandLineStorage CommandLineConfig;
		static CConfigDefaultsStorage Defaults;

		friend class CTSingleton<CConfig>;
	};

	static CTSingleton<CConfig> Config;

	enum ECommandLineArgumentType
	{
		COMMAND_LINE_ARGUMENT_TYPE_SHORT_OPTION,
		COMMAND_LINE_ARGUMENT_TYPE_LONG_OPTION,
		COMMAND_LINE_ARGUMENT_TYPE_OPTION_VALUE,
		COMMAND_LINE_ARGUMENT_TYPE_PLAIN,
	};

	/**
	* CCommandLineArgument - a structure that represents single command line argument as a token.
	*/

	struct CCommandLineArgument
	{
		CCommandLineArgument(ECommandLineArgumentType AType, const string &AContent);

		ECommandLineArgumentType Type;
		string Content;
	};

	/**
	* CCommandLineArgumentsManager - command line arguments manager. It performs lexical analysis of arguments and provides access to them.
	*/

	class CCommandLineArgumentsManager : public CObject
	{
	public:
		typedef list<CCommandLineArgument> ArgumentsStorage;

		bool Initialize(int argc, char *argv[]);

		string GetPlain(int Index);

		string GetOption(const string &LongName);
		string GetOption(char ShortName);
		string GetOption(const string &LongName, char ShortName);

		bool GetFlag(const string &LongName);
		bool GetFlag(char ShortName);
		bool GetFlag(const string &LongName, char ShortName);

		bool IsOptionExists(const string &LongName);
		bool IsOptionExists(char ShortName);
		bool IsOptionExists(const string &LongName, char ShortName);

		bool RegisterOption(const string &LongName);
		bool RegisterOption(char ShortName);
		bool RegisterOption(const string &LongName, char ShortName);

		string GetError() const;
		bool GetErrorState() const;

	private:
		CCommandLineArgumentsManager();

		bool Lex(int argc, char *argv[]);

		void SetErrorState(const string &AError);

		ArgumentsStorage Arguments;
		string Error;
		bool ErrorState;

		friend class CTSingleton<CCommandLineArgumentsManager>;
	};

	static CTSingleton<CCommandLineArgumentsManager> CommandLineArgumentsManager;

	struct CArgumentsConfigMapping
	{
		CArgumentsConfigMapping(const string &AArgumentLongName, char AArgumentShortName, const string &ASection, const string &AParameter, bool AOption);

		string ArgumentLongName;
		char ArgumentShortName;

		string Section;
		string Parameter;

		bool Option;
	};

	/**
	* CArgumentsConfigMappingsManager - a manager tht maps command line arguments to configuration parameters.
	*/

	class CArgumentsConfigMappingsManager : public CObject
	{
	public:
		typedef list<CArgumentsConfigMapping> MappingsStorage;
		typedef MappingsStorage::iterator MappingsIterator;

		void Add(CArgumentsConfigMapping AMapping);

		MappingsIterator Begin();
		MappingsIterator End();

	private:
		CArgumentsConfigMappingsManager();

		MappingsStorage Mappings;

		friend class CTSingleton<CArgumentsConfigMappingsManager>;
	};

	static CTSingleton<CArgumentsConfigMappingsManager> ArgumentsConfigMappingsManager;

}	//	namespace Deku2D

#endif // _2DE_CONFIG_H_

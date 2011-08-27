#ifndef _2DE_CONFIG_H_
#define _2DE_CONFIG_H_

#include "2de_Core.h"
#include "2de_Xml.h"

namespace Deku2d
{
	/**
	* CConfigStorage - класс, представляющий собой некое абстрактное хранилище конфигурации. Используется как базовый класс для всех конкретных видов хранилищ.
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
	* CConfigCommandLineStorage - класс, представляющий собой хранилище конфигурации, получемой преобразованием из аргументов командой строки.
	*/

	class CConfigCommandLineStorage : public CConfigStorage
	{
	public:
		CConfigCommandLineStorage(const string &ARootNodeName);

	};

	/**
	* CConfigFileStorage - класс, представляющий собой файловое хранилище конфигурации.
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
	* CConfigDefaultsStorage - класс, представляющий собой хранилище конфигурации по-умолчанию. Необходимо наследоваться от него для создания умолчальных значений для конкретной конфигурации.
	*/

	class CConfigDefaultsStorage : public CConfigStorage
	{
	public:
		CConfigDefaultsStorage(const string &ARootNodeName);

	};

	/**
	* CMainConfigDefaults - класс, представляющий собой хранилище конфигурации по-умолчанию для главной конфигурации движка.
	*/

	class CMainConfigDefaults : public CConfigDefaultsStorage
	{
	public:
		CMainConfigDefaults(const string &ARootNodeName);

	};

	// ON_DEMAND_TODO:
	// class CAbstractConfig to make configs apart of main config.. should just describe interface..

	/**
	* CConfig - класс, представляющий собой главную конфигурацию движка.
	*/

	class CConfig : public CObject
	{
	public:
		class CConfigParameter
		{
		public:
			CConfigParameter(CConfig *AConfig, const string &ASection, const string &AParameter);

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
			CConfig *Config;
			string Section;
			string Name;
		};

		class CConfigSection
		{
		public:
			CConfigSection(CConfig *AConfig, const string &ASection);

			CConfigParameter operator[](const string &AParameter);

		private:
			CConfig *Config;
			string Name;

		};

		string Get(const string &ASection, const string &AParameter);
		void Set(const string &ASection, const string &AParameter, const string &AValue);

		CConfigSection Section(const string &ASection);

		void AddDefault(const string &ASection, const string &AParameter, const string &AValue);

		// ON_DEMAND_TODO: void RestoreDefaults(); - copy settings from Defaults to FileConfig..
		// ON_DEMAND_TODO: maybe functions to access storages, for example for deleting parameters, adding sections, etc.

	private:
		CConfig();
		~CConfig();

		void Initialize();

		const char *MAIN_CONFIG_ROOT_NODE_NAME;

		bool Initialized;
		CConfigFileStorage *FileConfig;
		CConfigCommandLineStorage *CommandLineConfig;
		CMainConfigDefaults Defaults;


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
	* CCommandLineArgument - структура, представляющая собой отдельный аргумент как лексему.
	*/

	struct CCommandLineArgument
	{
		CCommandLineArgument(ECommandLineArgumentType AType, const string &AContent);

		ECommandLineArgumentType Type;
		string Content;
	};

	/**
	* CCommandLineArgumentsManager - менеджер аргументов коммандной строки. Разбивает аргументы на лексемы и предоставляет к ним доступ.
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
	* CArgumentsConfigMappingsManager - менеджер отображений из аргументов командной строки в конфигурационные парамеры.
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

}	//	namespace Deku2d

#endif // _2DE_CONFIG_H_

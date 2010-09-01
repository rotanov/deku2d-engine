#ifndef _2DE_CONFIG_H_
#define _2DE_CONFIG_H_

#include "2de_Core.h"
#include "2de_Xml.h"

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

// TODO:
/*class CConfigCommandLineStorage : public CConfigStorage
{
public:
	// constructor will take two args - vector/list/object with arguments and ARootNodeName

};*/

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

class CConfigDefaultsStorage : public CConfigStorage
{
public:
	CConfigDefaultsStorage(const string &ARootNodeName);

};

class CMainConfigDefaults : public CConfigDefaultsStorage
{
public:
	CMainConfigDefaults(const string &ARootNodeName);

};

// ON_DEMAND_TODO:
// class CAbstractConfig to make configs apart of main config.. should just describe interface..

class CConfig : public CTSingleton<CConfig>
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
	//CConfigCommandLineStorage *CommandLineConfig;
	CMainConfigDefaults Defaults;


	friend class CTSingleton<CConfig>;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
* Command line parser classes - not done yet - commented out.
*/

/*class CCommandLineArgument
{
public:
	enum ECommandLineArgumentType
	{
		COMMAND_LINE_ARGUMENT_TYPE_INVALID,
		COMMAND_LINE_ARGUMENT_TYPE_FLAG,
		COMMAND_LINE_ARGUMENT_TYPE_PARAMETER,
	};

	CCommandLineArgument(ECommandLineArgumentType AType, const string &ALongName, char AShortName);

	bool IsValid();

	ECommandLineArgumentType Type;

	string LongName;
	char ShortName;
};

class CCommandLineArgumentValue
{
public:
	CCommandLineArgumentValue();
	CCommandLineArgumentValue(bool AFlag);
	CCommandLineArgumentValue(const string &AValue);

	union
	{
		bool Flag;
		string Value;

	};
};

class CCommandLineArgumentsManager : public CTSingleton<CCommandLineArgumentsManager>
{
public:
	void Add(const CCommandLineArgument &AArgument);

	CCommandLineArgument GetByName(char AShortName);
	CCommandLineArgument GetByName(const string &ALongName);

protected:
	friend class CTSingleton<CCommandLineArgumentsManager>;
	CCommandLineArgumentsManager() { }

private:
	list<CCommandLineArgument> ArgumentsPrototypes;

};

class CCommandLineArguments
{
public:
	CCommandLineArguments(int argc, char *argv[]);

	void Parse();

	CVariant Get(char AShortName);
	CVariant Get(const string &ALongName);

private:
	vector<string> Argv;
	map<CCommandLineArgument, CCommandLineArgumentValue> Arguments;
};*/

#endif // _2DE_CONFIG_H_

#include "2de_Config.h"

#include "2de_Engine.h"

namespace Deku2D
{
	//////////////////////////////////////////////////////////////////////////
	// CConfigStorage

	CConfigStorage::CConfigStorage(const string &ARootNodeName) : RootNodeName(ARootNodeName)
	{
	}

	string CConfigStorage::Get(const string &ASection, const string &AParameter)
	{
		CXMLNode *Temp = GetRoot();

		if (Temp->IsErroneous())
		{
			Log("WARNING", "Root node '%s' has not been found in config storage", RootNodeName.c_str());
			return string();
		}

		Temp = Temp->Children.First(ASection);

		if (Temp->IsErroneous())
		{
			Log("WARNING", "Section '%s' has not been found in config storage", ASection.c_str());
			return string();
		}

		Temp = Temp->Children.First(AParameter);

		if (Temp->IsErroneous())
		{
			Log("WARNING", "Parameter '%s' from section '%s' has not been found in config storage", AParameter.c_str(), ASection.c_str());
			return string();
		}

		if (!Temp->HasAttribute("value"))
		{
			Log("WARNING", "Parameter '%s' from section '%s' doesn't have value", AParameter.c_str(), ASection.c_str());
			return string();
		}

		return Temp->GetAttribute("value");
	}

	void CConfigStorage::Set(const string &ASection, const string &AParameter, const string &AValue)
	{
		CXMLNode *Parent = GetRoot();

		if (Parent->IsErroneous())
		{
			Log("WARNING", "Root node '%s' has not been found in config storage", RootNodeName.c_str());
			return;
		}

		CXMLNode *Temp = Parent->Children.First(ASection);

		if (Temp->IsErroneous())
		{
			Parent->Children.AddLast(new CXMLNormalNode(ASection));
			Temp = Parent->Children.First(ASection);
		}

		Parent = Temp;
		Temp = Temp->Children.First(AParameter);

		if (Temp->IsErroneous())
		{
			Parent->Children.AddLast(new CXMLNormalNode(AParameter));
			Temp = Parent->Children.First(AParameter);
		}

		Temp->SetAttribute("value", AValue);
	}

	bool CConfigStorage::Exists(const string &ASection, const string &AParameter)
	{
		CXMLNode *Node = GetRoot()->Children.First(ASection)->Children.First(AParameter);
		return (!Node->IsErroneous() && Node->HasAttribute("value"));
	}

	void CConfigStorage::Delete(const string &ASection, const string &AParameter)
	{
		CXMLNode *SectionNode = GetRoot()->Children.First(ASection);

		if (SectionNode->IsErroneous())
			return;

		CXMLNode *ParamNode = SectionNode->Children.First(AParameter);

		if (ParamNode->IsErroneous())
			return;

		SectionNode->Children.Remove(ParamNode);

		delete ParamNode;
	}

	CXMLNode* CConfigStorage::GetRoot()
	{
		return Data.Root.First(RootNodeName);
	}

	void CConfigStorage::CreateInitialStructure()
	{
		Data.Root.DeleteAll();
		Data.Root.AddLast(new CXMLPrologNode);
		Data.Root.AddLast(new CXMLNormalNode(RootNodeName));
	}

	//////////////////////////////////////////////////////////////////////////
	// CConfigCommandLineStorage

	CConfigCommandLineStorage::CConfigCommandLineStorage(const string &ARootNodeName) : CConfigStorage(ARootNodeName)
	{
		CreateInitialStructure();
		CArgumentsConfigMappingsManager *ArgConfigMap = ArgumentsConfigMappingsManager.Instance();
		CCommandLineArgumentsManager *ArgMan = CommandLineArgumentsManager.Instance();

		for (CArgumentsConfigMappingsManager::MappingsIterator it = ArgConfigMap->Begin(); it != ArgConfigMap->End(); ++it)
		{
			if (ArgMan->IsOptionExists(it->ArgumentLongName, it->ArgumentShortName)) 
				Set(it->Section, it->Parameter, ArgMan->GetOption(it->ArgumentLongName, it->ArgumentShortName)); 
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// CConfigFileStorage

	CConfigFileStorage::CConfigFileStorage(const string &AFilename, const string &ARootNodeName) : CConfigStorage(ARootNodeName), Filename(AFilename)
	{
		Data.LoadFromFile(AFilename);

		if (Data.Root.First(RootNodeName)->IsErroneous())
			Log("WARNING", "Can't load configuration from '%s'", Filename.c_str());
		else
			Log("INFO", "Configuration from '%s' loaded successfully", Filename.c_str());
	}

	CConfigFileStorage::~CConfigFileStorage()
	{
		Save();
	}

	void CConfigFileStorage::Set(const string &ASection, const string &AParameter, const string &AValue)
	{
		CConfigStorage::Set(ASection, AParameter, AValue);
		Save();
	}

	void CConfigFileStorage::Delete(const string &ASection, const string &AParameter)
	{
		CConfigStorage::Delete(ASection, AParameter);
		Save();
	}

	void CConfigFileStorage::Save()
	{
		Data.SaveToFile(Filename);
	}

	//////////////////////////////////////////////////////////////////////////
	// CConfigDefaultsStorage

	CConfigDefaultsStorage::CConfigDefaultsStorage(const string &ARootNodeName) : CConfigStorage(ARootNodeName)
	{
		CreateInitialStructure();
	}

	//////////////////////////////////////////////////////////////////////////
	// CAbstractConfig::CConfigParameter

	CAbstractConfig::CConfigParameter::CConfigParameter(CAbstractConfig *AConfig, const string &ASection, const string &AParameter) : Config(AConfig), Section(ASection), Name(AParameter)
	{
	}

	//////////////////////////////////////////////////////////////////////////
	// CAbstractConfig::CConfigSection

	CAbstractConfig::CConfigSection::CConfigSection(CAbstractConfig *AConfig, const string &ASection) : Config(AConfig), Name(ASection)
	{
	}

	CAbstractConfig::CConfigParameter CAbstractConfig::CConfigSection::operator[](const string &AParameter)
	{
		return CConfigParameter(Config, Name, AParameter);
	}

	//////////////////////////////////////////////////////////////////////////
	// CAbstractConfig

	CAbstractConfig::CConfigSection CAbstractConfig::Section(const string &ASection)
	{
		return CConfigSection(this, ASection);
	}

	//////////////////////////////////////////////////////////////////////////
	// CConfig

	const char *CConfig::MAIN_CONFIG_ROOT_NODE_NAME = "Configuration";

	CConfigDefaultsStorage CConfig::Defaults(MAIN_CONFIG_ROOT_NODE_NAME);

	string CConfig::Get(const string &ASection, const string &AParameter)
	{
		if (CommandLineConfig.Exists(ASection, AParameter))
			return CommandLineConfig.Get(ASection, AParameter);

		if (FileConfig.Exists(ASection, AParameter))
			return FileConfig.Get(ASection, AParameter);

		if (Defaults.Exists(ASection, AParameter))
			return Defaults.Get(ASection, AParameter);

		Log("WARNING", "No parameter named '%s' has been found in '%s' section of config", AParameter.c_str(), ASection.c_str());
		return string();
	}

	void CConfig::Set(const string &ASection, const string &AParameter, const string &AValue)
	{
		FileConfig.Set(ASection, AParameter, AValue);
	}

	void CConfig::AddDefault(const string &ASection, const string &AParameter, const string &AValue)
	{
		Defaults.Set(ASection, AParameter, AValue);
	}

	CConfig::CConfig() : FileConfig(Environment::Paths::GetConfigPath() + CEngine::Instance()->GetProgramName() + ".xml", MAIN_CONFIG_ROOT_NODE_NAME), CommandLineConfig(MAIN_CONFIG_ROOT_NODE_NAME)
	{
		SetName("Config");
		FillDefaults();
	}

	void CConfig::FillDefaults()
	{
		Defaults.Set("Video", "Fullscreen", "false");
		Defaults.Set("Video", "FpsLimit", "60");
		Defaults.Set("Data", "LogLevel", "0");
		// TODO: add more
	}

	//////////////////////////////////////////////////////////////////////////
	// CCommandLineArgument

	CCommandLineArgument::CCommandLineArgument(ECommandLineArgumentType AType, const string &AContent) : Type(AType), Content(AContent)
	{
	}

	//////////////////////////////////////////////////////////////////////////
	// CCommandLineArgumentsManager

	bool CCommandLineArgumentsManager::Initialize(int argc, char *argv[])
	{
		return Lex(argc, argv);
	}

	string CCommandLineArgumentsManager::GetPlain(int Index)
	{
		int CurIndex = -1;
		for (ArgumentsStorage::iterator it = Arguments.begin(); it != Arguments.end(); ++it)
		{
			if (it->Type == COMMAND_LINE_ARGUMENT_TYPE_PLAIN)
			{
				CurIndex++;
				if (Index == CurIndex)
				{
					return it->Content; 
				}
			}
		}

		return "";
	}

	string CCommandLineArgumentsManager::GetOption(const string &LongName)
	{
		return GetOption(LongName, 0);
	}

	string CCommandLineArgumentsManager::GetOption(char ShortName)
	{
		return GetOption("", ShortName);
	}

	string CCommandLineArgumentsManager::GetOption(const string &LongName, char ShortName)
	{
		for (ArgumentsStorage::iterator it = Arguments.begin(); it != Arguments.end(); ++it)
		{
			if ((it->Type == COMMAND_LINE_ARGUMENT_TYPE_SHORT_OPTION && it->Content == string(1, ShortName)) ||
				(it->Type == COMMAND_LINE_ARGUMENT_TYPE_LONG_OPTION && it->Content == LongName))
			{
				++it;
				if (it->Type == COMMAND_LINE_ARGUMENT_TYPE_OPTION_VALUE)
				{
					return it->Content;
				}
				else
				{
					return "true";
				}
			}
		}

		return "";
	}

	bool CCommandLineArgumentsManager::GetFlag(const string &LongName)
	{
		return GetFlag(LongName, 0);
	}

	bool CCommandLineArgumentsManager::GetFlag(char ShortName)
	{
		return GetFlag("", ShortName);
	}

	bool CCommandLineArgumentsManager::GetFlag(const string &LongName, char ShortName)
	{
		for (ArgumentsStorage::iterator it = Arguments.begin(); it != Arguments.end(); ++it)
		{
			if ((it->Type == COMMAND_LINE_ARGUMENT_TYPE_SHORT_OPTION && it->Content == string(1, ShortName)) ||
				(it->Type == COMMAND_LINE_ARGUMENT_TYPE_LONG_OPTION && it->Content == LongName))
			{
				return true;
			}
		}

		return false;
	}

	bool CCommandLineArgumentsManager::IsOptionExists(const string &LongName)
	{
		return IsOptionExists(LongName, 0);
	}

	bool CCommandLineArgumentsManager::IsOptionExists(char ShortName)
	{
		return IsOptionExists("", ShortName);
	}

	bool CCommandLineArgumentsManager::IsOptionExists(const string &LongName, char ShortName)
	{
		for (ArgumentsStorage::iterator it = Arguments.begin(); it != Arguments.end(); ++it)
		{
			if ((it->Type == COMMAND_LINE_ARGUMENT_TYPE_SHORT_OPTION && it->Content == string(1, ShortName)) ||
				(it->Type == COMMAND_LINE_ARGUMENT_TYPE_LONG_OPTION && it->Content == LongName))
			{
				return true;
			}
		}

		return false;
	}

	bool CCommandLineArgumentsManager::RegisterOption(const string &LongName)
	{
		return RegisterOption(LongName, 0);
	}

	bool CCommandLineArgumentsManager::RegisterOption(char ShortName)
	{
		return RegisterOption("", ShortName);
	}

	bool CCommandLineArgumentsManager::RegisterOption(const string &LongName, char ShortName)
	{
		for (ArgumentsStorage::iterator it = Arguments.begin(); it != Arguments.end(); ++it)
		{
			if ((it->Type == COMMAND_LINE_ARGUMENT_TYPE_SHORT_OPTION && it->Content == string(1, ShortName)) ||
				(it->Type == COMMAND_LINE_ARGUMENT_TYPE_LONG_OPTION && it->Content == LongName))
			{
				++it;
				if (it == Arguments.end() || it->Type != COMMAND_LINE_ARGUMENT_TYPE_PLAIN)
				{
					SetErrorState("option '" + (--it)->Content + "' must have value");
					return false;
				}

				it->Type = COMMAND_LINE_ARGUMENT_TYPE_OPTION_VALUE; 

				break;
			}
		}

		return true;
	}

	string CCommandLineArgumentsManager::GetError() const
	{
		return Error;
	}

	bool CCommandLineArgumentsManager::GetErrorState() const
	{
		return ErrorState;
	}

	CCommandLineArgumentsManager::CCommandLineArgumentsManager() : ErrorState(false)
	{
		SetName("CommandLineArgumentsManager");
	}

	bool CCommandLineArgumentsManager::Lex(int argc, char *argv[])
	{
		string Current;
		bool EndOfOptions = false;

		for (int i = 1; i < argc; i++)
		{
			Current = argv[i];
			if (EndOfOptions || Current[0] != '-' || Current == "-")
			{
				Arguments.push_back(CCommandLineArgument(COMMAND_LINE_ARGUMENT_TYPE_PLAIN, Current));
			}
			else
			{
				if (Current == "--")
				{
					EndOfOptions = true;
				}
				else
				{
					if (Current.substr(0, 2) == "--")
					{
						Arguments.push_back(CCommandLineArgument(COMMAND_LINE_ARGUMENT_TYPE_LONG_OPTION, Current.substr(2)));
					}
					else
					{
						for (int j = 1; j < Current.length(); j++)
						{
							if (isalpha(Current[j]))
							{
								Arguments.push_back(CCommandLineArgument(COMMAND_LINE_ARGUMENT_TYPE_SHORT_OPTION, string(1, Current[j])));
							}
							else
							{
								SetErrorState("invalid option: " + string(1, Current[j]));
								return false;
							}
						}
					}
				}
			}
		}

		return true;
	}

	void CCommandLineArgumentsManager::SetErrorState(const string &AError)
	{
		if (ErrorState)
			return;	// report only first error

		Error = AError;
		ErrorState = true;
	}

	//////////////////////////////////////////////////////////////////////////
	// CArgumentsConfigMapping

	CArgumentsConfigMapping::CArgumentsConfigMapping(const string &AArgumentLongName, char AArgumentShortName, const string &ASection, const string &AParameter, bool AOption) :
		ArgumentLongName(AArgumentLongName), ArgumentShortName(AArgumentShortName), Section(ASection), Parameter(AParameter), Option(AOption)
	{
	}

	//////////////////////////////////////////////////////////////////////////
	// CArgumentsConfigMappingsManager

	void CArgumentsConfigMappingsManager::Add(CArgumentsConfigMapping AMapping)
	{
		Mappings.push_back(AMapping);
	}

	CArgumentsConfigMappingsManager::MappingsIterator CArgumentsConfigMappingsManager::Begin()
	{
		return Mappings.begin();
	}

	CArgumentsConfigMappingsManager::MappingsIterator CArgumentsConfigMappingsManager::End()
	{
		return Mappings.end();
	}

	CArgumentsConfigMappingsManager::CArgumentsConfigMappingsManager()
	{
		SetName("ArgumentsConfigMappingsManager");

		Add(CArgumentsConfigMapping("data-path", 0, "Data", "DataPath", true));
		Add(CArgumentsConfigMapping("window-width", 0, "Video", "WindowWidth", true));
		Add(CArgumentsConfigMapping("window-height", 0, "Video", "WindowHeight", true));
		Add(CArgumentsConfigMapping("clear-resources-cache", 0, "Data", "ForceReindex", false));
		Add(CArgumentsConfigMapping("log-level", 0, "Data", "LogLevel", true));
		// TODO: add more mappings
	}

}	//	namespace Deku2D

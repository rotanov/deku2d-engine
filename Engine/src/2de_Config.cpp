#include "2de_Config.h"

#include "2de_Engine.h"

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
// CMainConfigDefaults

CMainConfigDefaults::CMainConfigDefaults(const string &ARootNodeName) : CConfigDefaultsStorage(ARootNodeName)
{
	// fill Data with defaults
	Set("Video", "Fullscreen", "false");
}

//////////////////////////////////////////////////////////////////////////
// CConfig::CConfigParameter

CConfig::CConfigParameter::CConfigParameter(CConfig *AConfig, const string &ASection, const string &AParameter) : Config(AConfig), Section(ASection), Name(AParameter)
{
}

//////////////////////////////////////////////////////////////////////////
// CConfig::CConfigSection

CConfig::CConfigSection::CConfigSection(CConfig *AConfig, const string &ASection) : Config(AConfig), Name(ASection)
{
}

CConfig::CConfigParameter CConfig::CConfigSection::operator[](const string &AParameter)
{
	return CConfigParameter(Config, Name, AParameter);
}

//////////////////////////////////////////////////////////////////////////
// CConfig

string CConfig::Get(const string &ASection, const string &AParameter)
{
	if (!Initialized)
		Initialize();

	//if (CommandLineConfig->Exists(ASection, AParameter))
		//return CommandLineConfig->Get(ASection, AParameter);

	if (FileConfig->Exists(ASection, AParameter))
		return FileConfig->Get(ASection, AParameter);

	if (Defaults.Exists(ASection, AParameter))
		return Defaults.Get(ASection, AParameter);

	Log("WARNING", "No parameter named '%s' has been found in '%s' section of config", AParameter.c_str(), ASection.c_str());
	return string();
}

void CConfig::Set(const string &ASection, const string &AParameter, const string &AValue)
{
	if (!Initialized)
		Initialize();

	FileConfig->Set(ASection, AParameter, AValue);
}

CConfig::CConfigSection CConfig::Section(const string &ASection)
{
	return CConfigSection(this, ASection);
}

void CConfig::AddDefault(const string &ASection, const string &AParameter, const string &AValue)
{
	Defaults.Set(ASection, AParameter, AValue);
}

CConfig::CConfig() : MAIN_CONFIG_ROOT_NODE_NAME("Configuration"), Initialized(false), FileConfig(NULL), /*CommandLineConfig(NULL),*/ Defaults(MAIN_CONFIG_ROOT_NODE_NAME)
{
}

CConfig::~CConfig()
{
	delete FileConfig;
	//delete CommandLineConfig;
}

void CConfig::Initialize()
{
	FileConfig = new CConfigFileStorage(CEnvironment::Paths::GetConfigPath() + CEngine::Instance()->GetProgramName() + ".xml", MAIN_CONFIG_ROOT_NODE_NAME);
	//CommandLineConfig = new CConfigCommandLineStorage(CCommandLineArguments::Instance()->Get(), MAIN_CONFIG_ROOT_NODE_NAME);

	Initialized = true;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Command line parser classes -  not done yet - commented out.

/*
//////////////////////////////////////////////////////////////////////////
// CCommandLineArgument

CCommandLineArgument::CCommandLineArgument(ECommandLineArgumentType AType, const string &ALongName, char AShortName) : Type(AType), LongName(ALongName), ShortName(AShortName)
{
}

bool CCommandLineArgument::IsValid()
{
	return (Type != COMMAND_LINE_ARGUMENT_TYPE_INVALID);
}

//////////////////////////////////////////////////////////////////////////
// CCommandLineArgumentValue

CCommandLineArgumentValue()
{
}

CCommandLineArgumentValue(bool AFlag) : Flag(AFlag)
{
}

CCommandLineArgumentValue(const string &AValue) : Value(AValue)
{
}

//////////////////////////////////////////////////////////////////////////
// CCommandLineArgumentsManager

void CCommandLineArgumentsManager::Add(const CCommandLineArgument &AArgument)
{
	// TODO: find some effective way to check arguments conflicts (same short or long name)..
	ArgumentsPrototypes.push_back(AArgument);
}

CCommandLineArgument CCommandLineArgumentsManager::GetByName(char AShortName)
{
	for (list<CCommandLineArgument>::iterator it = ArgumentsPrototypes.begin(); it != ArgumentsPrototypes.end(); ++it)
	{
		if ((*it).ShortName == AShortName)
		{
			return *it;
		}
	}

	return CCommandLineArgument(CCommandLineArgument::COMMAND_LINE_ARGUMENT_TYPE_INVALID, "", 0);
}

CCommandLineArgument CCommandLineArgumentsManager::GetByName(const string &ALongName)
{
	for (list<CCommandLineArgument>::iterator it = ArgumentsPrototypes.begin(); it != ArgumentsPrototypes.end(); ++it)
	{
		if ((*it).LongName == ALongName)
		{
			return *it;
		}
	}

	return CCommandLineArgument(CCommandLineArgument::COMMAND_LINE_ARGUMENT_TYPE_INVALID, "", 0);
}

//////////////////////////////////////////////////////////////////////////
// CCommandLineArguments

CCommandLineArguments::CCommandLineArguments(int argc, char *argv[])
{
	// start with 1 as we don't need program executable name..
	for (int i = 1; i < argc; i++)
	{
		Argv.push_back(argv[i]);
	}

	Parse();
}

void CCommandLineArguments::Parse()
{
	// parse 'em all..
	
	string current;
	CCommandLineArgument arg;
	CCommandLineArgumentValue val;
	bool AwaitingValue = false;
	bool OptionsEnd = false;
	
	for (vector<string>::iterator it = Argv.begin(); it != Argv.end(); ++it)
	{
		current = *it;
		if (current.substr(0, 2) == "--")
		{
			if (AwaitingValue && !OptionsEnd)
			{
				Log("WARNING", "No value for command line parameter");
				// oh damn.. need redesign...
			}
			arg = CCommandLineArgumentsManager::Instance()->GetByName(current.substr(2));
			switch (arg.Type)
			{
			case CCommandLineArgument::COMMAND_LINE_ARGUMENT_TYPE_INVALID:
				Log("WARNING", "Unparsed command line argument: '%s'", current.c_str());
				break;
			case CCommandLineArgument::COMMAND_LINE_ARGUMENT_TYPE_FLAG:
				val.Flag = true;
				Arguments[arg] = val;
				break;
			case CCommandLineArgument::COMMAND_LINE_ARGUMENT_TYPE_PARAMETER:
				++it;
				val.Value = *it;
				break;
			}
		}
		else if (current[0] == '-')
		{

		}
		if ((*it).substr(0, 2) == "--") 
	}

}

CVariant CCommandLineArguments::Get(char AShortName)
{
	for (map<CCommandLineArgument, CCommandLineArgumentValue>::iterator it = Arguments.begin(); it != Arguments.end(); ++it)
	{
		if (it->first.ShortName == AShortName)
		{
			if (it->first.Type == CCommandLineArgument::COMMAND_LINE_ARGUMENT_TYPE_FLAG)
				return it->second.Flag;
			else if (it->first.Type == CCommandLineArgument::COMMAND_LINE_ARGUMENT_TYPE_PARAMETER)
				return it->second.Value;
		}
	}

	return false;
}

CVariant CCommandLineArguments::Get(const string &ALongName)
{
	for (map<CCommandLineArgument, CCommandLineArgumentValue>::iterator it = Arguments.begin(); it != Arguments.end(); ++it)
	{
		if (it->first.LongName == ALongName)
		{
			if (it->first.Type == CCommandLineArgument::COMMAND_LINE_ARGUMENT_TYPE_FLAG)
				return it->second.Flag;
			else if (it->second.Type == CCommandLineArgument::COMMAND_LINE_ARGUMENT_TYPE_PARAMETER)
				return it->second.Value;
		}
	}

	return false;
}*/

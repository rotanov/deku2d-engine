#include "2de_Xml.h"

//////////////////////////////////////////////////////////////////////////
// CXMLNode

CXMLNode::CXMLNode() : Parent(NULL)
{

}

CXMLNode::~CXMLNode()
{
}

const string& CXMLNode::GetName() const
{
	return Name;
}

void CXMLNode::SetName(const string &AName)
{
	Name = AName;
}

void CXMLNode::SetParent(CXMLNode *AParent)
{
	Parent = AParent;
}

//////////////////////////////////////////////////////////////////////////
// CXMLPrologNode

CXMLPrologNode::CXMLPrologNode()
{
	SetName("xml");
	Version = "1.0";
}

const string& CXMLPrologNode::GetVersion() const
{
	return Version;
}

void CXMLPrologNode::SetVersion(const string &AVersion)
{
	Version = AVersion;
}

string CXMLPrologNode::GetText()
{
	return "<?" + GetName() + " version=\"" + Version + "\"?>";
}

//////////////////////////////////////////////////////////////////////////
// CChildrenList::Iterator

CXMLChildrenList::Iterator::Iterator()
{

}
/*Iterator(CXMLNode &AXMLNode);*/
/*Iterator(const iterator &AIterator)
{
}*/

/*CXMLNormalNode::CChildrenList::Iterator& CXMLNormalNode::CChildrenList::Iterator::operator=(const CXMLNormalNode::CChildrenList::Iterator &AIterator)
{
	Backend = AIterator.Backend;
	return *this;
}*/

bool CXMLChildrenList::Iterator::operator==(const CXMLChildrenList::Iterator &AIterator) const
{
	return (Backend == AIterator.Backend);
}

bool CXMLChildrenList::Iterator::operator!=(const CXMLChildrenList::Iterator &AIterator) const
{
	return (Backend != AIterator.Backend);
}

CXMLChildrenList::Iterator& CXMLChildrenList::Iterator::operator++()
{
	++Backend;
	return *this;
}

CXMLChildrenList::Iterator CXMLChildrenList::Iterator::operator++(int)
{
	Iterator result = *this;
	++Backend;
	return result;
}

CXMLChildrenList::Iterator& CXMLChildrenList::Iterator::operator--()
{
	--Backend;
	return *this;
}

CXMLChildrenList::Iterator CXMLChildrenList::Iterator::operator--(int)
{
	Iterator result = *this;
	--Backend;
	return result;
}

CXMLNode* CXMLChildrenList::Iterator::operator*()
{
	return *Backend;
}

//////////////////////////////////////////////////////////////////////////
// CChildrenList

CXMLChildrenList::CXMLChildrenList(CXMLNode *ANode) : Node(ANode)
{
}

CXMLChildrenList::~CXMLChildrenList()
{
}

void CXMLChildrenList::AddFirst(CXMLNode *ANode)
{
	ANode->SetParent(Node);
	Backend.push_front(ANode);
}

void CXMLChildrenList::AddLast(CXMLNode *ANode)
{
	ANode->SetParent(Node);
	Backend.push_back(ANode);
}

void CXMLChildrenList::AddAfter(const Iterator &AIterator, CXMLNode *ANode)
{
	ANode->SetParent(Node);
	list<CXMLNode *>::iterator iter = AIterator.Backend;
	Backend.insert(++iter, ANode);
}

void CXMLChildrenList::AddBefore(const Iterator &AIterator, CXMLNode *ANode)
{
	ANode->SetParent(Node);
	Backend.insert(AIterator.Backend, ANode);
}

CXMLNode* CXMLChildrenList::Remove(Iterator &AIterator)
{
	CXMLNode *result = *AIterator;
	Backend.erase(AIterator.Backend);

	// return iterator following right after deleted iterator
	// is iterator check required or not?
	// 	we don't need to do anything, we just wrap this shit around standard erase

	result->SetParent(NULL);
	return result;
}

void CXMLChildrenList::DeleteAll()
{
	for (Iterator it = Begin(); it != End(); ++it)
	{
		delete *it;
	}
	Clear();
}

void CXMLChildrenList::Clear()
{
	Backend.clear();
}

CXMLChildrenList::Iterator CXMLChildrenList::Begin()
{
	Iterator result;
	result.Backend = Backend.begin();
	return result;
}

CXMLChildrenList::Iterator CXMLChildrenList::End()
{
	Iterator result;
	result.Backend = Backend.end();
	return result;
}

bool CXMLChildrenList::IsEmpty() const
{
	return Backend.empty();
}

bool CXMLChildrenList::GetSize() const
{
	return Backend.size();
}

//////////////////////////////////////////////////////////////////////////
// CXMLNormalNode

CXMLNormalNode::CXMLNormalNode(const string &AName) : Children(this)
{
	SetName(AName);
}

CXMLNormalNode::~CXMLNormalNode()
{
	Children.DeleteAll();
}

string CXMLNormalNode::GetAttribute(const string &AName) const
{
	map<string, string>::const_iterator iter = Attributes.find(AName);
	if (iter == Attributes.end())
	{
		Log("WARNING", "Attribute '%s' not found in XML node", AName.c_str());
		return "";
	}

	return iter->second;
}

void CXMLNormalNode::SetAttribute(const string &AName, const string &AValue)
{
	Attributes[AName] = AValue;
}

void CXMLNormalNode::DeleteAttribute(const string &AName)
{
	if (!Attributes.erase(AName))
		Log("WARNING", "Attribute '%s' not found in XML node", AName.c_str());
}

string CXMLNormalNode::GetText()
{
	string res = "<" + GetName();

	if (!Attributes.empty())
	{
		for (map<string, string>::iterator it = Attributes.begin(); it != Attributes.end(); ++it)
		{
			res += " " + it->first + "=\"" + it->second + "\"";
		}
	}

	if (Children.IsEmpty())
		res += " /";
	else
	{
		res += ">\n";
		for (ChildrenIterator it = Children.Begin(); it != Children.End(); ++it)
			res += (*it)->GetText();
		res += "</" + GetName();
	}

	res += ">\n";

	return res;
}

//////////////////////////////////////////////////////////////////////////
// CXMLSingleValueNode

CXMLSingleValueNode::CXMLSingleValueNode(const string &AValue /*= ""*/)
{
	Value = AValue;
}
const string& CXMLSingleValueNode::GetValue() const
{
	return Value;
}

void CXMLSingleValueNode::SetValue(const string &AValue)
{
	Value = AValue;
}

//////////////////////////////////////////////////////////////////////////
// CXMLCommentNode

CXMLCommentNode::CXMLCommentNode(const string &AValue /*= ""*/) : CXMLSingleValueNode(AValue)
{
	SetName("!--");
}

string CXMLCommentNode::GetText()
{
	return "<" + GetName() + GetValue() + "-->\n";
}

//////////////////////////////////////////////////////////////////////////
// CXMLTextNode

CXMLTextNode::CXMLTextNode(const string &AValue /*= ""*/) : CXMLSingleValueNode(AValue)
{
}

string CXMLTextNode::GetText()
{
	return GetValue() + "\n"; 
	// according to current agreement all whitespace following after text
	// ignored
}

//////////////////////////////////////////////////////////////////////////
// CXML

CXML::CXML(const string &AFilename /*= " "*/) : Root(NULL)
{
	if (!AFilename.empty())
		LoadFromFile(AFilename);
}

CXML::~CXML()
{
	Root.DeleteAll();
}

#include <fstream>

void CXML::LoadFromFile(const string &AFilename)
{
	//CFile f(AFilename, CFile::OPEN_MODE_READ);

	// damn, CFile is just useless shit..
	// temporary (i hope) using fstream...

	ifstream temp(AFilename.c_str());

	string tmpstr;
	string AllContent;

	while (temp.good())
	{
		getline(temp, tmpstr);
		AllContent += tmpstr;
	}
	temp.close();

	Root.DeleteAll();

	CXMLParser parser(AllContent);

	Root = parser.Parse();

	// string AllContent = // Get all file content in string somehow..
	// Root.DeleteAll();
	// CXMLParser parser(AllContent);
	// Root = parser.Parse();

	//f.Close();
}

void CXML::SaveToFile(const string &AFilename)
{
	CFile f(AFilename, CFile::OPEN_MODE_WRITE);
	for (CXMLNormalNode::ChildrenIterator it = Root.Begin(); it != Root.End(); ++it)
	{
		f.WriteLine((*it)->GetText());
	}
	f.Close();
}

//////////////////////////////////////////////////////////////////////////
// CXMLParser

CXMLParser::CXMLParser(const string &AText) : Result(NULL), Text(AText), Current(0)
{
}

CXMLChildrenList CXMLParser::Parse()
{
	Result.Clear();

	while (Good())
	{
		CXMLNode *node = ParseNode();
		if (node)
			Result.AddLast(node);
	}

	Text = "";//why? // to prevent parsing the same text again at subsequent calls to Parse

	return Result;
}

void CXMLParser::SetText(const string &AText)
{
	Text = AText;
}

bool CXMLParser::Good()
{
	return Current < Text.length();
}

void CXMLParser::SkipWhiteSpace()
{
	while (Good() && isWhiteSpace(Text[Current]))
		Current++;
}

bool CXMLParser::isWhiteSpace(char c)
{
	return (c == ' ') || (c == '\n') || (c == '\t') || (c == '\r');
}

bool CXMLParser::isValidNameChar(char c) 
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

bool CXMLParser::isAnotherTag()
{
	return Text[Current] == '<';
}

CXMLNode* CXMLParser::ParseNode()
{
	SkipWhiteSpace();
	if (!Good())
		return NULL;

	// определяем тип ноды
	if (Text[Current] == '<')
	{
		Current++;
		if (Text.substr(Current, 3) == "!--")
		{
			return ParseComment();
		}
		else if (Text.substr(Current, 4) == "?xml")
		{
			return ParseProlog();
		}
		else if (Text[Current + 1] == '/')
		{
			// Current += 2;
			// string EndName = ParseNodeEndTagName(); // (pseudo-function)
			// if (CurrentLevel && EndName != CurrentLevel->GetName()) // CXMLNode *CurrentLevel - class member - points to current level parent or NULL for root level
			//
			// {
			// 	ReportError("Incorrect nesting: end tag doesn't match start tag at", Current);
			// }
			// else if (!CurrentLevel)
			// {
			// 	ReportError("Unexpected end tag without corresponding start tag at root level at", Current);
			// }
			//
			// return NULL;	// always return NULL, because it's not real node
		}
		else
		{
			// return ParseNormal();
		}
	}
	else
	{
		// Text node
		return ParseText();
	}
}

CXMLCommentNode* CXMLParser::ParseComment()
{
	Current += 3;

	int start = Current;
	int EndMatch = 0;

	while (Good())
	{
		if (Text[Current] == '-')
			EndMatch++;
		else if (EndMatch == 2 && Text[Current] == '>')
		{
			Current++;
			return new CXMLCommentNode(Text.substr(start, Current - start - 3));
		}
		else
			EndMatch = 0;
	}

	ReportError("Unterminated comment found at", Current);

	return NULL;
}

CXMLPrologNode* CXMLParser::ParseProlog()
{
	Current += 4;
	if (!isWhiteSpace(Text[Current]))
		return NULL;

	CXMLPrologNode *result = new CXMLPrologNode;

	int AttrStart;
	int EndMatch = 0;

	while (Good() && !isAnotherTag())
	{
		Current++;
		if (EndMatch == 0 && Text[Current] == '?')
			EndMatch++;
		else if (EndMatch == 1 && Text[Current] == '>')
		{
			Current++;
			return result;
		}
		else
		{
			EndMatch = 0;
			AttrStart = Current; // there may be whitespace, so it's not fully correct...
			pair<string, string> attr = ParseAttribute();

			// error handling
			if (attr.first == "")
				ReportError("Failed to read attribute at", AttrStart);

			if (attr.first == "version")
				result->SetVersion(attr.second);

			//else if (attr.first == "encoding")
			//result->SetEncoding(attr.second);
			// else ignore..
		}
	}

	ReportError("Unterminated prolog found at", Current);

	delete result;
	return NULL;
}

CXMLTextNode* CXMLParser::ParseText()
{
	SkipWhiteSpace();

	int start = Current;
	string buffer;

	while (Good() && !isAnotherTag())
	{
		if (Text[Current] == '&')
		{
			buffer += Text.substr(start, Current - start);
			start = Current;
			buffer += ParseEntity();
		}

		Current++;
	}
	//	Some error handling // nothing needed, text nodes may end anywhere...

	buffer += Text.substr(start, Current - start);

	// need to right trim whitespace, btw.. if someone didn't get it, "right trim" means delete any whitespace from tail..
	return new CXMLTextNode(buffer);
}

pair<string, string> CXMLParser::ParseAttribute()
{
	pair<string, string> result;

	SkipWhiteSpace();

	result.first = ParseName();

	SkipWhiteSpace();//

	if (Text[Current] != '=')
	{
		result.first = "";
		return result;
	}
	Current++;

	result.second = ParseValue();
	// error handling here if needed // can't do it properly.. value can be "", so we can't determine, whether it's parsed correctly..
	return result;
}

string CXMLParser::ParseName()
{
	int start = Current;

	while (Good() && isValidNameChar(Text[Current]))
		Current++;

	if (!Good())
	{
		// no need to report error here - it's being handled in higher level.. am i right?..
	}

	return Text.substr(start, Current - start);
}

string CXMLParser::ParseValue()
{
	if (Text[Current] != '"')
		return "";

	int start = ++Current;

	while (Good() && Text[Current] != '"')
		Current++;

	if (!Good())
	{
		ReportError("Unterminated quoted string at", start - 1);
		return "";
	}

	return Text.substr(start, Current - start);
}

string CXMLParser::ParseEntity()
{
	int start = ++Current;

	while (Good() && !isWhiteSpace(Text[Current]))
	{
		if (Text[Current] == ';')
		{
			string entity = Text.substr(start, Current - start);
			string result;	// maybe char, but i'm not sure, can it be more than one char?.. in most cases - no, but who knows...
			if (entity == "amp")
				result = "&";
			else if (entity == "lt")
				result = "<";
			else if (entity == "gt")
				result = ">";
			else if (entity == "quot")
				result = "\"";
			// there are many more entities.. these are basic ones..
			
			return result;
		}
	}

	ReportError("Unterminated entity at", start - 1);

	return "";
}

void CXMLParser::ReportError(const string &Message, int Position)
{
	// temporary
	// add severity support.. sometimes it's warning, sometimes - error..
	//Log("ERROR", string(Message + " %d.").c_str(), Position [> here may be an xml file name... or not... <]);
	// CLog crashes everything when engine's not running... so we will temporary use cout
	cout << Message << " " << Position << endl;
}

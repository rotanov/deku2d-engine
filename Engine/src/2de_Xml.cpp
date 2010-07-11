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

void CXML::LoadFromFile(const string &AFilename)
{
	CFile f(AFilename, CFile::OPEN_MODE_READ);

	// string AllContent = // Get all file content in string somehow..
	// Root.DeleteAll();
	// CXMLParser parser(AllContent);
	// Root = parser.Parse();

	f.Close();
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

bool CXMLParser::isWhiteSpace(const char &c)
{
	return (c == ' ') || (c == '\n') || (c == '\t') || (c == '\r');
}

bool CXMLParser::isAnotherTag()
{
	return Text[Current] == '<';
}


bool CXMLParser::isValidNameChar(const char &c) 
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

string CXMLParser::ParseName()
{
	int start = Current;
	while (Good() && isValidNameChar(Text[Current]))
		Current;
	return Text.substr(start, Current - start);
}

string CXMLParser::ParseValue()
{
	if (Text[Current] != '"')
		return "";
	int start = ++Current;
	while (Good() && Text[Current++] != '"');
	//if (!Good())
	//	ReportError("Unterminated quoted string at", start);
	return Text.substr(start, Current - start);
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
	// error handling here if needed
	return result;
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
		else
		{
			// Normal
		}
	}
	else
	{
		// Text node
		return ParseText();
	}
}

CXMLNode *CXMLParser::ParseComment()
{
	Current += 3;

	int Start = Current;
	int EndMatch = 0;

	while (Good())
	{
		if (Text[Current] == '-')
			EndMatch++;
		else if (EndMatch == 2 && Text[Current] == '>')
			return new CXMLCommentNode(Text.substr(Start, Current - Start - 3));
		else
			EndMatch = 0;
	}

	ReportError("Unterminated comment found at", Current);

	return NULL;
}

CXMLNode *CXMLParser::ParseProlog()
{
	Current += 4;
	if (!isWhiteSpace(Text[Current]))
		return NULL;

	CXMLPrologNode *result = new CXMLPrologNode;

	int Start = Current;
	int EndMatch = 0;

	while (Good() && !isAnotherTag())
	{
		Current++;
		if (EndMatch == 0 && Text[Current] == '?')
			EndMatch++;
		else if (EndMatch == 1 && Text[Current] == '>')
			return result;
		else
			EndMatch = 0;
		pair<string, string> attr = ParseAttribute();
		// error handling
		//if (attr.first == "")
		//	ReportError("Failed to read attribute at", Current/*wrong*/);
		if (attr.first == "version")
			result->SetVersion(attr.second);
		//else if (attr.first == "encoding")
			//result->SetEncoding(attr.second);
		// else ignore..
	}

	ReportError("Unterminated prolog found at", Current);

	delete result;
	return NULL;
}

CXMLNode* CXMLParser::ParseText()
{
	SkipWhiteSpace();
	int start = Current;
	while (Good() && Text[Current++] != '<');
	//if (!Good())
	//	Some error handling
	// TODO: deal with entity parsing

	return new CXMLTextNode(ParseEntity(Text.substr(start, Current - start)));
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

	Text = "";//why?

	return Result;
}

void CXMLParser::ReportError( const string &Message, const int &Position )
{
	// temporary
	Log("Parse Error", Message + " %d.", Position /* here may be an xml file name... or not... */);
}

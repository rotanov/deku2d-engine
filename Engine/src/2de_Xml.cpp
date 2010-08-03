#include "2de_Xml.h"

//////////////////////////////////////////////////////////////////////////
// CChildrenList::ConstIterator

bool CXMLChildrenList::ConstIterator::operator==(const CXMLChildrenList::ConstIterator &AIterator) const
{
	return (Backend == AIterator.Backend);
}

bool CXMLChildrenList::ConstIterator::operator!=(const CXMLChildrenList::ConstIterator &AIterator) const
{
	return (Backend != AIterator.Backend);
}

CXMLChildrenList::ConstIterator& CXMLChildrenList::ConstIterator::operator++()
{
	++Backend;
	return *this;
}

CXMLChildrenList::ConstIterator CXMLChildrenList::ConstIterator::operator++(int)
{
	ConstIterator result = *this;
	++Backend;
	return result;
}

CXMLChildrenList::ConstIterator& CXMLChildrenList::ConstIterator::operator--()
{
	--Backend;
	return *this;
}

CXMLChildrenList::ConstIterator CXMLChildrenList::ConstIterator::operator--(int)
{
	ConstIterator result = *this;
	--Backend;
	return result;
}

CXMLNode* CXMLChildrenList::ConstIterator::operator*()
{
	return *Backend;
}

//////////////////////////////////////////////////////////////////////////
// CChildrenList::Iterator

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
	CXMLNode *result = *Backend;
	return (result == NULL) ? &ErroneousNode : result; // not really sure 'bout this..
}

//////////////////////////////////////////////////////////////////////////
// CChildrenList

CXMLChildrenList::CXMLChildrenList(CXMLNode *ANode /*= NULL*/) : Node(ANode)
{
}

CXMLChildrenList::~CXMLChildrenList()
{
}

CXMLNode* CXMLChildrenList::First(const string &AValue /*= ""*/)
{
	if (IsEmpty())
		return &ErroneousNode;
	
	if (AValue.empty())
		return Backend.front();

	for (StorageType::const_iterator it = Backend.begin(); it != Backend.end(); ++it)
	{
		if ((*it)->GetValue() == AValue)
			return *it;
	}

	return &ErroneousNode;
}

CXMLNode* CXMLChildrenList::Last(const string &AValue /*= ""*/)
{
	if (IsEmpty())
		return &ErroneousNode;

	if (AValue.empty())
		return Backend.back();

	for (StorageType::const_reverse_iterator it = Backend.rbegin(); it != Backend.rend(); ++it)
	{
		if ((*it)->GetValue() == AValue)
			return *it;
	}
	
	return &ErroneousNode;
}

CXMLNode* CXMLChildrenList::operator[](size_t AID)
{
	if (IsEmpty())
		return &ErroneousNode;
	
	Iterator it = Begin();

	for (size_t i = 0; i <= AID; i++)
	{
		++it;
		if (it == End())
			return &ErroneousNode;
	}

	return *it;
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
	StorageType::iterator iter = AIterator.Backend;
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

CXMLChildrenList::ConstIterator CXMLChildrenList::Begin() const
{
	ConstIterator result;
	result.Backend = Backend.begin();
	return result;
}

CXMLChildrenList::ConstIterator CXMLChildrenList::End() const
{
	ConstIterator result;
	result.Backend = Backend.end();
	return result;
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
// CXMLNode

CXMLNode::CXMLNode() : Children(this), Parent(NULL)
{

}

CXMLNode::~CXMLNode()
{
}

string CXMLNode::GetValue() const
{
	return "";
}

void CXMLNode::SetValue(const string &AValue)
{
	Log("WARNING", "This type of XML node doesn't support changing its value directly");
}

string CXMLNode::GetName() const
{
	return Name;
}

void CXMLNode::SetName(const string &AName)
{
	Log("WARNING", "This type of XML node doesn't support changing its name");
}

string CXMLNode::GetText() const
{
	return GetStartingSequence() + GetName() + GetContent() + GetEndingSequence() + "\n";
}

void CXMLNode::SetInnerText(const string &AText)
{
	Log("WARNING", "This type of XML node doesn't support setting inner text");
}

CXMLNode* CXMLNode::GetParent()
{
	return (Parent == NULL) ? &ErroneousNode : Parent;
}

void CXMLNode::SetParent(CXMLNode *AParent)
{
	Parent = AParent;
}

string CXMLNode::GetAttribute(const string &AName) const
{
	Log("WARNING", "This type of XML node can't have any attributes");
	return "";
}

void CXMLNode::SetAttribute(const string &AName, const string &AValue)
{
	Log("WARNING", "This type of XML node can't have any attributes");
}

void CXMLNode::DeleteAttribute(const string &AName)
{
	Log("WARNING", "This type of XML node can't have any attributes");
}

bool CXMLNode::IsErroneous() const
{
	return false;
}

CXMLNode* CXMLNode::PreviousSibling(const string &AValue /*= ""*/) const
{
	if (!Parent)
		return &ErroneousNode;

	CXMLChildrenList::StorageType::const_reverse_iterator it = find(Parent->Children.Backend.rbegin(), Parent->Children.Backend.rend(), this);

	if (it == Parent->Children.Backend.rend())
		return &ErroneousNode;
	
	++it;

	if (it == Parent->Children.Backend.rend())
		return &ErroneousNode;

	if (AValue.empty())
		return  *it;

	for (; it != Parent->Children.Backend.rend(); ++it)
	{
		if ((*it)->GetValue() == AValue)
			return *it;
	}

	return &ErroneousNode;
}

CXMLNode* CXMLNode::NextSibling(const string &AValue /*= ""*/) const
{
	if (!Parent)
		return &ErroneousNode;

	CXMLChildrenList::StorageType::const_iterator it = find(Parent->Children.Backend.begin(), Parent->Children.Backend.end(), this);

	if (it == Parent->Children.Backend.end())
		return &ErroneousNode;
	
	++it;

	if (it == Parent->Children.Backend.end())
		return &ErroneousNode;

	if (AValue.empty())
		return  *it;

	for (; it != Parent->Children.Backend.end(); ++it)
	{
		if ((*it)->GetValue() == AValue)
			return *it;
	}

	return &ErroneousNode;
}

CXMLNode* CXMLNode::PreviousSimilarSibling() const
{
	return PreviousSibling(GetValue());
}

CXMLNode* CXMLNode::NextSimilarSibling() const
{
	return NextSibling(GetValue());
}

string CXMLNode::GetStartingSequence() const
{
	return "";
}

string CXMLNode::GetEndingSequence() const
{
	return "";
}

string CXMLNode::GetContent() const
{
	return GetValue();
}

//////////////////////////////////////////////////////////////////////////
// CXMLErroneousNode

CXMLErroneousNode* CXMLErroneousNode::Copy()
{
	Log("WARNING", "You're trying to copy erronoues XML node");
	return this;
}

string CXMLErroneousNode::GetValue() const
{
	Log("WARNING", "You're trying to get value of erronoues XML node");
	return "";
}

void CXMLErroneousNode::SetValue(const string &AValue)
{
	Log("WARNING", "You're trying to set value of erronoues XML node");
}

string CXMLErroneousNode::GetName() const
{
	Log("WARNING", "You're trying to get name of erronoues XML node");
	return "";
}

void CXMLErroneousNode::SetName(const string &AName)
{
	Log("WARNING", "You're trying to set name of erronoues XML node");
}

string CXMLErroneousNode::GetText() const
{
	Log("WARNING", "You're trying to get text of erroneous node");
	return "";
}

void CXMLErroneousNode::SetInnerText(const string &AText)
{
	Log("WARNING", "You're trying to set inner text of erronoues XML node");
}

CXMLNode* CXMLErroneousNode::GetParent()
{
	Log("WARNING", "You're trying to get parent of erronoues XML node");
	return this;
}

void CXMLErroneousNode::SetParent(CXMLNode *AParent)
{
	Log("WARNING", "You're trying to set parent of erronoues XML node");
}

string CXMLErroneousNode::GetAttribute(const string &AName) const
{
	Log("WARNING", "You're trying to get attribute of erronoues XML node");
	return "";
}

void CXMLErroneousNode::SetAttribute(const string &AName, const string &AValue)
{
	Log("WARNING", "You're trying to set attribute of erronoues XML node");
}

void CXMLErroneousNode::DeleteAttribute(const string &AName)
{
	Log("WARNING", "You're trying to delete attribute of erronoues XML node");
}

bool CXMLErroneousNode::IsErroneous() const
{
	return true;
}

//////////////////////////////////////////////////////////////////////////
// CXMLNodeWithAttributes

string CXMLNodeWithAttributes::GetAttribute(const string &AName) const
{
	map<string, string>::const_iterator iter = Attributes.find(AName);
	if (iter == Attributes.end())
	{
		Log("WARNING", "Attribute '%s' has not been found in XML node", AName.c_str());
		return "";
	}

	return iter->second;
}

void CXMLNodeWithAttributes::SetAttribute(const string &AName, const string &AValue)
{
	Attributes[AName] = AValue;
}

void CXMLNodeWithAttributes::DeleteAttribute(const string &AName)
{
	if (!Attributes.erase(AName))
		Log("WARNING", "Attribute '%s' has not been found in XML node", AName.c_str());
}

string CXMLNodeWithAttributes::GetValue() const
{
	return Name;
}

string CXMLNodeWithAttributes::GetContent() const
{
	string result;

	if (!Attributes.empty())
	{
		for (map<string, string>::const_iterator it = Attributes.begin(); it != Attributes.end(); ++it)
		{
			result += " " + it->first + "=\"" + CXMLHelper::Instance()->EntitiesEncode(it->second) + "\"";
		}
	}

	return result;
}

//////////////////////////////////////////////////////////////////////////
// CXMLPrologNode

CXMLPrologNode::CXMLPrologNode()
{
	Name = "xml";
	SetVersion("1.0");
}

CXMLPrologNode* CXMLPrologNode::Copy()
{
	CXMLPrologNode *result = new CXMLPrologNode;
	*result = *this;
	return result;
}

string CXMLPrologNode::GetVersion() const
{
	return GetAttribute("version");
}

void CXMLPrologNode::SetVersion(const string &AVersion)
{
	SetAttribute("version", AVersion);
}

string CXMLPrologNode::GetStartingSequence() const
{
	return "<?";
}

string CXMLPrologNode::GetEndingSequence() const
{
	return "?>";
}

//////////////////////////////////////////////////////////////////////////
// CXMLNormalNode

CXMLNormalNode::CXMLNormalNode(const string &AName)
{
	SetName(AName);
}

CXMLNormalNode::~CXMLNormalNode()
{
	Children.DeleteAll();
}

CXMLNormalNode* CXMLNormalNode::Copy()
{
	CXMLNormalNode *result = new CXMLNormalNode(Name);
	result->Attributes = Attributes;

	for (ChildrenIterator it = Children.Begin(); it != Children.End(); ++it)
		result->Children.AddLast((*it)->Copy());

	return result;
}

void CXMLNormalNode::SetName(const string &AName)
{
	Name = AName;
}

void CXMLNormalNode::SetInnerText(const string &AText)
{
	Children.DeleteAll();

	CXMLParser parser(AText);
	Children = parser.Parse();
}

string CXMLNormalNode::GetStartingSequence() const
{
	return "<";
}

string CXMLNormalNode::GetEndingSequence() const
{
	return ">";
}

string CXMLNormalNode::GetContent() const
{
	string result = CXMLNodeWithAttributes::GetContent();

	if (Children.IsEmpty())
		result += " /";
	else
	{
		result += ">\n";
		for (CXMLChildrenList::ConstIterator it = Children.Begin(); it != Children.End(); ++it)
			result += (*it)->GetText();
		result += "</" + GetName();
	}

	return result;
}

//////////////////////////////////////////////////////////////////////////
// CXMLSingleValueNode

CXMLSingleValueNode::CXMLSingleValueNode(const string &AValue) : Value(AValue)
{
}

string CXMLSingleValueNode::GetValue() const
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
}

CXMLCommentNode* CXMLCommentNode::Copy()
{
	CXMLCommentNode *result = new CXMLCommentNode;
	*result = *this;
	return result;
}

string CXMLCommentNode::GetStartingSequence() const
{
	return "<!--";
}

string CXMLCommentNode::GetEndingSequence() const
{
	return "-->";
}

//////////////////////////////////////////////////////////////////////////
// CXMLTextNode

CXMLTextNode::CXMLTextNode(const string &AValue /*= ""*/) : CXMLSingleValueNode(AValue)
{
}

CXMLTextNode* CXMLTextNode::Copy()
{
	CXMLTextNode *result = new CXMLTextNode;
	*result = *this;
	return result;
}

string CXMLTextNode::GetText() const
{
	return CXMLHelper::Instance()->EntitiesEncode(GetValue()) + "\n"; 
}

//////////////////////////////////////////////////////////////////////////
// CXML

CXML::CXML(const string &AFilename /*= " "*/) : Root(NULL)
{
	if (!AFilename.empty())
		LoadFromFile(AFilename);
}

CXML::CXML(const CXML &ASource) : Root(NULL)
{
	for (CXMLChildrenList::ConstIterator it = ASource.Root.Begin(); it != ASource.Root.End(); ++it)
		Root.AddLast((*it)->Copy());
}

CXML::~CXML()
{
	Root.DeleteAll();
}

CXML& CXML::operator=(const CXML &ASource)
{
	for (CXMLChildrenList::ConstIterator it = ASource.Root.Begin(); it != ASource.Root.End(); ++it)
		Root.AddLast((*it)->Copy()); 
	
	return *this;
}

void CXML::LoadFromFile(const string &AFilename)
{
	CFile file(AFilename, CFile::OPEN_MODE_READ);
	string AllContent = file.GetContent();
	file.Close();

	Root.DeleteAll();

	CXMLParser parser(AllContent); 
	Root = parser.Parse();
}

void CXML::SaveToFile(const string &AFilename)
{
	CFile file(AFilename, CFile::OPEN_MODE_WRITE);

	for (CXMLNormalNode::ChildrenIterator it = Root.Begin(); it != Root.End(); ++it)
		file.WriteText((*it)->GetText());

	file.Close();
}

//////////////////////////////////////////////////////////////////////////
// CXMLParser

CXMLParser::CXMLParser(const string &AText) : Result(NULL), Text(AText), Current(0), CurrentLevel(NULL)
{
}

CXMLChildrenList CXMLParser::Parse()
{
	ParseInside(&Result);

 	Text = "";
 
 	return Result;
}

void CXMLParser::ParseInside(CXMLChildrenList *List)
{
	List->Clear();

	CXMLNode *Level = CurrentLevel;

	while (Good() && Level == CurrentLevel)
	{
		CXMLNode *node = ParseNode();
		if (node)
			List->AddLast(node);
	}
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

bool CXMLParser::isValidNameChar(char c, bool Initial /*= false*/)
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == ':' ||
		(!Initial && ((c >= '0' && c <= '9') || c == '-' || c == '.')));
}

bool CXMLParser::isAnotherTag()
{
	return Text[Current] == '<';
}

bool CXMLParser::hasAttribute()
{
	SkipWhiteSpace();
	return Good() && isValidNameChar(Text[Current], true);
}

CXMLNode* CXMLParser::ParseNode()
{
	SkipWhiteSpace();
	if (!Good())
		return NULL;
	
	int start = Current;

	// определяем тип ноды
	if (Text[Current] == '<')
	{
		Current++;
		if (!Good())
		{
			ReportError("ERROR", "Unterminated tag encountered at", start);
			return NULL;
		}

		if (Text.substr(Current, 3) == "!--")
		{
			return ParseComment();
		}
		else if (Text.substr(Current, 4) == "?xml")
		{
			return ParseProlog();
		}
		else if (Text[Current] == '/')
		{
			Current++;
			string EndName = ParseTagName();

			if (CurrentLevel && EndName != CurrentLevel->GetName()) 
				ReportError("ERROR", "Incorrect nesting: end tag doesn't match start tag at", start);
			else if (!CurrentLevel)
			 	ReportError("ERROR", "Unexpected end tag without corresponding start tag at root level at", start);

			if (!Good())
			{
				ReportError("ERROR", "Unterminated tag encountered at", start);
				return NULL;
			}

			if (Text[Current] != '>')
				ReportError("ERROR", "Unexpected symbols in end tag at", Current);
			
			while (Good() && Text[Current] != '>')
				Current++;

			if (!Good())
			{
				ReportError("ERROR", "Unterminated tag encountered at", start);
				return NULL;
			}

			Current++;

			if (CurrentLevel)
				CurrentLevel = CurrentLevel->GetParent();
			
			return NULL;	// always return NULL, because it's not real node
		}
		else
		{
			return ParseNormal();
		}
	}
	else
	{
		// Text node
		return ParseText();
	}
	return NULL;
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
		Current++;
	}

	ReportError("ERROR", "Unterminated comment found at", start - 4);

	return NULL;
}

CXMLNormalNode* CXMLParser::ParseNormal()
{
	int start = Current - 1;
	if (!Good() || isWhiteSpace(Text[Current]))
	{
		ReportError("ERROR", "Incorrect tag at", start);
		return NULL;
	}

	string TagName = ParseTagName();
	if (TagName.empty())
	{
		ReportError("ERROR", "Incorrect tag at", start);
		return NULL;
	}

	CXMLNormalNode *result = new CXMLNormalNode(TagName);
	int AttrStart = 0;

	while (Good() && Text[Current] != '>')
	{
		if (hasAttribute())
		{
			AttrStart = Current;
			pair<string, string> attr = ParseAttribute();

			if (attr.first == "")
				ReportError("ERROR", "Failed to read attribute at", AttrStart);
			else
				result->SetAttribute(attr.first, attr.second);
		}
		//some error handling if following symbol is something veryyy baaaaad // what? o_O

		Current++;

	}

	if (!Good())
	{
		ReportError("ERROR", "Unexpected end of tag at", Current);
		return NULL;
	}

	Current++;
	if (Text[Current - 1] == '>' && Text[Current - 2] == '/')
	{
		//no child coming
		return result;
	}

	result->SetParent(CurrentLevel);//shit
	CurrentLevel = result;
	// reading a childlist...
	ParseInside(&result->Children);
	return result;
}

CXMLPrologNode* CXMLParser::ParseProlog()
{
	int start = Current - 1;
	Current += 4;
	if (!Good() || !isWhiteSpace(Text[Current]))
		return NULL;

	CXMLPrologNode *result = new CXMLPrologNode;

	int AttrStart;
	int EndMatch = 0;

	while (Good() && !isAnotherTag())
	{
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
			if (hasAttribute())
			{
				AttrStart = Current;
				pair<string, string> attr = ParseAttribute();

				if (attr.first == "")
					ReportError("ERROR", "Failed to read attribute at", AttrStart);

				if (attr.first == "version")
					result->SetVersion(attr.second);
			}

			//else if (attr.first == "encoding")
			//result->SetEncoding(attr.second);
			// else ignore..
		}

		Current++;
	}

	ReportError("ERROR", "Unterminated prolog found at", start);

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
			buffer += ParseEntity();
			start = Current + 1;
		}

		if (isWhiteSpace(Text[Current]) && !isWhiteSpace(Text[Current - 1]))
		{
			buffer += Text.substr(start, Current - start);
			start = Current;
		}

		Current++;
	}

	return new CXMLTextNode(buffer);
}

pair<string, string> CXMLParser::ParseAttribute()
{
	pair<string, string> result;

	SkipWhiteSpace();

	result.first = ParseAttributeName();

	if (result.first.empty())
		return result;

	SkipWhiteSpace();

	if (!Good() || Text[Current] != '=')
	{
		result.first = "";
		return result;
	}

	Current++;
	SkipWhiteSpace();

	result.second = ParseAttributeValue();

	// error handling here if needed // can't do it properly.. value can be "", so we can't determine, whether it's parsed correctly..
	return result;
}

string CXMLParser::ParseAttributeName()
{
	int start = Current;

	if (!Good() || !isValidNameChar(Text[Current], true))
		return "";
	
	Current++;

	while (Good() && isValidNameChar(Text[Current]))
		Current++;

	if (!Good())
		return "";

	return Text.substr(start, Current - start);
}

string CXMLParser::ParseAttributeValue()
{
	// SPECS: attribute values should be normalized.. but we're not-validating parser, so just count all attributes as CDATA

	if (!Good() || Text[Current] != '"')
		return "";

	int start = ++Current;
	string buffer;
	
	while (Good() && Text[Current] != '"')
	{
		if (Text[Current] == '&')
		{
			buffer += Text.substr(start, Current - start);
			buffer += ParseEntity();
			start = Current + 1;
		}

		Current++;
	}

	if (!Good())
	{
		ReportError("ERROR", "Unterminated quoted string at", start - 1);
		return "";
	}

	buffer += Text.substr(start, Current - start);

	return buffer;
}

string CXMLParser::ParseEntity()
{
	int start = ++Current;

	while (Good() && !isWhiteSpace(Text[Current]))
	{
		if (Text[Current] == ';')
		{
			string entity = Text.substr(start, Current - start);
			char result = CXMLHelper::Instance()->GetCharByEntity(entity);

			if (result == 0)
			{
				ReportError("ERROR", "Unknown entity at", start - 1);
				return '&' + entity + ';';
			}

			return string(1, result);
		}

		Current++;
	}

	ReportError("ERROR", "Unterminated entity at", start - 1);

	return "";
}

string CXMLParser::ParseTagName()
{
	int start = Current;

	// SPECS: Disallowed initial characters for Names include digits, diacritics, the full stop and the hyphen. -- handle this

	while (Good() && isValidNameChar(Text[Current]))
		Current++;

	return Text.substr(start, Current - start);
}

void CXMLParser::ReportError(const string &Severity, const string &Message, int Position)
{
	Log(Severity.c_str(), string("XML parser: " + Message + " %d.").c_str(), Position);

	// TODO: may be add something like "file", "line" and "column", if it's possible..
}

//////////////////////////////////////////////////////////////////////////
// CXMLHelper

CXMLHelper* CXMLHelper::Instance()
{
	return &_instance;
}

string CXMLHelper::EntitiesEncode(const string &AText)
{
	string result;
	int current = 0;
	int start = 0;

	while (current < AText.length())
	{
		if (Entities.count(AText[current]))
		{
			result += AText.substr(start, current - start);
			start = current + 1;
			result += '&' + Entities[AText[current]] + ';';
		}
		current++;
	}

	result += AText.substr(start, current - start);

	return result;
}

string CXMLHelper::EntitiesDecode(const string &AText)
{
	// STUB, or maybe not needed.
	return "";
}

char CXMLHelper::GetCharByEntity(const string &AEntity)
{
	// SPECS: add support for character references: &#a and &#xb, where a is decimal and b is hexadecimal
	// 	but i think, they require unicode...

	for (EntityMap::iterator it = Entities.begin(); it != Entities.end(); ++it)
		if (it->second == AEntity)
			return it->first;

	return 0;
}

CXMLHelper::CXMLHelper()
{
	Entities['&'] = "amp";
	Entities['"'] = "quot";
	Entities['\''] = "apos";
	Entities['<'] = "lt";
	Entities['>'] = "gt";
}

CXMLHelper CXMLHelper::_instance;

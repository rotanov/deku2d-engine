#include "2de_Xml.h"

//////////////////////////////////////////////////////////////////////////
// CXMLNode

CXMLNode::CXMLNode() : Parent(NULL)
{

}

CXMLNode::~CXMLNode()
{
	//mb kill all children
	// 	sure.. kill 'em all.. .. or not... i don't know, need to think about memory management and ownership...
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
// CXMLNormalNode::CChildrenList::Iterator

CXMLNormalNode::CChildrenList::Iterator::Iterator()
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

bool CXMLNormalNode::CChildrenList::Iterator::operator==(const CXMLNormalNode::CChildrenList::Iterator &AIterator) const
{
	return (Backend == AIterator.Backend);
}

bool CXMLNormalNode::CChildrenList::Iterator::operator!=(const CXMLNormalNode::CChildrenList::Iterator &AIterator) const
{
	return (Backend != AIterator.Backend);
}

CXMLNormalNode::CChildrenList::Iterator& CXMLNormalNode::CChildrenList::Iterator::operator++()
{
	++Backend;
	return *this;
}

CXMLNormalNode::CChildrenList::Iterator CXMLNormalNode::CChildrenList::Iterator::operator++(int)
{
	Iterator result = *this;
	++Backend;
	return result;
}

CXMLNormalNode::CChildrenList::Iterator& CXMLNormalNode::CChildrenList::Iterator::operator--()
{
	--Backend;
	return *this;
}

CXMLNormalNode::CChildrenList::Iterator CXMLNormalNode::CChildrenList::Iterator::operator--(int)
{
	Iterator result = *this;
	--Backend;
	return result;
}

CXMLNode* CXMLNormalNode::CChildrenList::Iterator::operator*()
{
	return *Backend;
}

/*void DeleteElement(); 		// i think iterator should not do this shit.. list should..
void InsertBefore(CXMLNode &ANode);
void InsertAfter(CXMLNode &ANode);*/

//~Iterator(); // for what?

//////////////////////////////////////////////////////////////////////////
// CXMLNormalNode::CChildrenList

CXMLNormalNode::CChildrenList::CChildrenList(CXMLNode *ANode) : Node(ANode)
{
}

CXMLNormalNode::CChildrenList::~CChildrenList()
{
}

void CXMLNormalNode::CChildrenList::AddFirst(CXMLNode *ANode)
{
	ANode->SetParent(Node);
	Backend.push_front(ANode);
}

void CXMLNormalNode::CChildrenList::AddLast(CXMLNode *ANode)
{
	ANode->SetParent(Node);
	Backend.push_back(ANode);
}

void CXMLNormalNode::CChildrenList::AddAfter(const Iterator &AIterator, CXMLNode *ANode)
{
	ANode->SetParent(Node);
	list<CXMLNode *>::iterator iter = AIterator.Backend;
	Backend.insert(++iter, ANode);
}

void CXMLNormalNode::CChildrenList::AddBefore(const Iterator &AIterator, CXMLNode *ANode)
{
	ANode->SetParent(Node);
	Backend.insert(AIterator.Backend, ANode);
}

CXMLNode* CXMLNormalNode::CChildrenList::Remove(Iterator &AIterator)
{
	CXMLNode *result = *AIterator;
	Backend.erase(AIterator.Backend);

	// return iterator following right after deleted iterator
	// is iterator check required or not?
	// 	we don't need to do anything, we just wrap this shit around standard erase

	result->SetParent(NULL);
	return result;
}

CXMLNormalNode::CChildrenList::Iterator CXMLNormalNode::CChildrenList::Begin()
{
	Iterator result;
	result.Backend = Backend.begin();
	return result;
}
CXMLNormalNode::CChildrenList::Iterator CXMLNormalNode::CChildrenList::End()
{
	Iterator result;
	result.Backend = Backend.end();
	return result;
}

bool CXMLNormalNode::CChildrenList::IsEmpty() const
{
	return Backend.empty();
}

bool CXMLNormalNode::CChildrenList::GetSize() const
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
	ChildrenIterator del;
	while (!Children.IsEmpty())
	{
		del = Children.Begin();
		delete *del;
		Children.Remove(del);
	}
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
	CXMLNormalNode::ChildrenIterator del;
	while (!Root.IsEmpty())
	{
		del = Root.Begin();
		delete *del;
		Root.Remove(del);
	}
}

void CXML::LoadFromFile(const string &AFilename)
{
	CFile f(AFilename, CFile::OPEN_MODE_READ);

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

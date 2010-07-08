#include "2de_Xml.h"

//////////////////////////////////////////////////////////////////////////
// CXMLNode

CXMLNode::CXMLNode()
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

void CXMLNormalNode::CChildrenList::AddFirst(CXMLNode *ANode)
{
	Backend.push_front(ANode);
}

void CXMLNormalNode::CChildrenList::AddLast(CXMLNode *ANode)
{
	Backend.push_back(ANode);
}

void CXMLNormalNode::CChildrenList::AddAfter(const Iterator &AIterator, CXMLNode *ANode)
{
	list<CXMLNode *>::iterator iter = AIterator.Backend;
	Backend.insert(++iter, ANode);
}

void CXMLNormalNode::CChildrenList::AddBefore(const Iterator &AIterator, CXMLNode *ANode)
{
	Backend.insert(AIterator.Backend, ANode);
}

CXMLNode* CXMLNormalNode::CChildrenList::Remove(Iterator &AIterator)
{
	CXMLNode *result = *AIterator; // WTF?!!! AIterator is link // WTF?! you know nothing about operator*, don't you?
	Backend.erase(AIterator.Backend); // may be so? // yes
	// return iterator following right after deleted iterator
	// is iterator check required or not?
	// 	we don't need to do anything, we just wrap this shit around standard erase

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

CXMLNormalNode::CXMLNormalNode(const string &AName)
{
	SetName(AName);
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
		res += "/";
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
// CXMLCommentNode

CXMLCommentNode::CXMLCommentNode()
{
	SetName("!--");
}

CXMLCommentNode::CXMLCommentNode( const string &AValue )
{
	SetName("!--");
	Value = AValue;
}
const string& CXMLCommentNode::GetValue() const
{
	return Value;
}

void CXMLCommentNode::SetValue(const string &AValue)
{
	Value = AValue;
}

string CXMLCommentNode::GetText()
{
	return "<" + GetName() + GetValue() + "-->\n";
}

//////////////////////////////////////////////////////////////////////////
// CXML

CXML::CXML(const string &AFilename /*= " "*/): Root("xml")
{
	if (!AFilename.empty())
		LoadFromFile(AFilename);
}

CXML::~CXML()
{

}

void CXML::LoadFromFile(const string &AFilename)
{

}

void CXML::SaveToFile(const string &AFilename)
{
	CFile f(AFilename, CFile::OPEN_MODE_WRITE);
	f.WriteString(Root.GetText());
	// damn shit with binary file
	// in the end of the string it writes \0 - that's not the deal...
	f.Close();
}
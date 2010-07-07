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

Iterator& CXMLNormalNode::CChildrenList::Iterator::operator=(const Iterator &AIterator)
{
	Backend = AIterator.Backend;
	return *this;
}

bool CXMLNormalNode::CChildrenList::Iterator::operator==(const Iterator &AIterator) const
{
	return (Backend == AIterator.Backend);
}

bool CXMLNormalNode::CChildrenList::Iterator::operator!=(const Iterator &AIterator) const
{
	return (Backend != AIterator.Backend);
}

Iterator& CXMLNormalNode::CChildrenList::Iterator::operator++()
{
	++Backend;
	return *this;
}

Iterator CXMLNormalNode::CChildrenList::Iterator::operator++(int)
{
	Iterator result = *this;
	++Backend;
	return result;
}

Iterator& CXMLNormalNode::CChildrenList::Iterator::operator--()
{
	--Backend;
	return *this;
}

Iterator CXMLNormalNode::CChildrenList::Iterator::operator--(int)
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

CXMLNode* CXMLNormalNode::CChildrenList::Remove(const Iterator &AIterator)
{
	CXMLNode *result = *AIterator;
	Backend.erase(AIterator);

	return result;
}

Iterator CXMLNormalNode::CChildrenList::Begin()
{
	Iterator result;
	result.Backend = Backend.begin();
	return result;
}
Iterator CXMLNormalNode::CChildrenList::End()
{
	Iterator result;
	result.Backend = Backend.end():
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

const string& CXMLNormalNode::GetAttribute(const string &AName) const
{
	map<string, string>::iterator iter = Attributes.find(AName);
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

void DeleteAttribute(const string &AName)
{
	if (!Attributes.erase(AName))
		Log("WARNING", "Attribute '%s' not found in XML node", AName.c_str());
}

//////////////////////////////////////////////////////////////////////////
// CXMLCommentNode

CXMLCommentNode::CXMLCommentNode()
{
	Name = "!--"
}

const string& CXMLCommentNode::GetValue() const
{
	return Value;
}

void CXMLCommentNode::SetValue(const string &AValue)
{
	Value = AValue;
}

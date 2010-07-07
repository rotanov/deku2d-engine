#include "2de_Xml.h"

//////////////////////////////////////////////////////////////////////////
// CXMLNode

CXMLNode::CXMLNode()
{

}

CXMLNode::~CXMLNode()
{
	//mb kill all children
}

const string& CXMLNode::GetName() const
{
	return Name;
}

void CXMLNode::SetName( const string &AName )
{
	Name = AName;
}

//////////////////////////////////////////////////////////////////////////
// CXMLNormalNode

//////////////////////////////////////////////////////////////////////////
// CXMLCommentNode

CXMLCommentNode::CXMLCommentNode()
{

}

const string& CXMLCommentNode::GetValue() const
{
	return Value;
}

void CXMLCommentNode::SetValue( const string &AValue )
{
	Value = AValue;
}

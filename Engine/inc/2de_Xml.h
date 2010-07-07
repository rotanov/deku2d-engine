#ifndef _2DE_XML_H_
#define _2DE_XML_H_

#include "2de_Core.h"

class CXMLNode
{
public:
	CXMLNode();
	~CMLNode();

	void SetName(const string &AName);
	const string& GetName() const;
protected:
	CXMLNode *Parent;
private:
	string Name;

};

// TODO: class for "prolog", "specification" or whatever node..

class CXMLNormalNode : public CXMLNode
{
public:
	void AddChild(CXMLNode &ANode);
	void RemoveChild(CXMLNode &ANode);
	// TODO: interface for iterating through children..

	// TODO: interface for attributes..

protected:		// do we really need protected here? maybe no..
	list<CXMLNode *> Children;
	map<string, string> Attributes;
private:

};

class CXMLCommentNode : public CXMLNode
{
public:
	CXMLCommentNode();	// set Name to "!--" in constructor

	void SetValue(const string &AValue);
	const string& GetValue() const;
private:
	string Value;

};

class CXML
{
public:
	CXML();
	CXML(const CXML &ASource); // don't know, if it's really required...
	~CXML();
	CXML& operator=(const CXML &ASource);

	void LoadFromFile(const string &AFilename);
	void SaveToFile(const string &AFilename);

	// TODO: add some interface to root.. here we have 2 possibilities:
	// 	1. CXMLNormalNode& GetRoot();
	// 	2. AddChild, RemoveChild like in normal node
	// 	I like the second way more..

private:
	CXMLNormalNode *Root;

};


#endif // _2DE_XML_H_

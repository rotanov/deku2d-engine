#ifndef _2DE_XML_H_
#define _2DE_XML_H_

#include "2de_Core.h"

class CXMLNode
{
public:
	CXMLNode();
	~CMLNode();
	virtual bool HaveChildren();//return 0 by default;

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
	void HaveChildren(); // return 1
	// TODO: interface for iterating through children..

	// TODO: interface for attributes..

protected:		// do we really need protected here? maybe no..//of course we don't
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

class Iterator
{
public:
	Iterator();
	Iterator(CXMLNode *AXMLNode); // damn... mb add constructor by id later
	Iterator(const iterator &AIterator);

	Iterator &operator=(const Iterator &AIterator);

	bool operator==(const Iterator &AIterator) const;
	bool operator!=(const Iterator &AIterator) const;

	Iterator &operator++();
	Iterator operator++(int);
	Iterator &operator--();
	Iterator operator--(int);

	void DeleteElement();
	void InsertBefore(CXMLNode *elem);
	void InsertAfter(CXMLNode *elem);

	~Iterator(); // for what?
private:
	list<CXMLNode *>::iterator Backend;
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

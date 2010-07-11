#ifndef _2DE_XML_H_
#define _2DE_XML_H_

#include "2de_Core.h"

class CXMLNode
{
public:
	CXMLNode();
	virtual ~CXMLNode();
	//virtual bool HaveChildren(); // 0 by default

	const string& GetName() const;
	void SetName(const string &AName);
	virtual string GetText() = 0;

	void SetParent(CXMLNode *AParent); // i don't like, that it's in public, but it doesn't work otherwise..

protected:
	CXMLNode *Parent;
private:
	string Name;
};

class CXMLPrologNode : public CXMLNode
{
public:
	CXMLPrologNode();

	const string& GetVersion() const;
	void SetVersion(const string &AVersion);

	string GetText();
private:
	string Version;
};

class CXMLChildrenList
{
public:
	class Iterator
	{
	public:
		Iterator();
		/*Iterator(CXMLNode &AXMLNode);
		  Iterator(const iterator &AIterator);*/

		//Iterator& operator=(const Iterator &AIterator); // standard (assigning corresponding members) is sufficient
		// deal with operator= and delete or uncomment it

		bool operator==(const Iterator &AIterator) const;
		bool operator!=(const Iterator &AIterator) const;

		Iterator& operator++();
		Iterator operator++(int);
		Iterator& operator--();
		Iterator operator--(int);

		CXMLNode* operator*();

	private:
		list<CXMLNode *>::iterator Backend;

		friend class CXMLChildrenList;
	};

	CXMLChildrenList(CXMLNode *ANode);
	~CXMLChildrenList();

	void AddFirst(CXMLNode *ANode);
	void AddLast(CXMLNode *ANode);
	void AddAfter(const Iterator &AIterator, CXMLNode *ANode);
	void AddBefore(const Iterator &AIterator, CXMLNode *ANode);
	CXMLNode* Remove(Iterator &AIterator);
	void DeleteAll();
	void Clear();

	Iterator Begin();
	Iterator End();

	bool IsEmpty() const;
	bool GetSize() const;

private:
	list<CXMLNode *> Backend;
	CXMLNode *Node;
};

class CXMLNormalNode : public CXMLNode
{
public:
	typedef CXMLChildrenList::Iterator ChildrenIterator;

	CXMLNormalNode(const string &AName);
	~CXMLNormalNode();

	//bool HaveChildren(); // return 1

	// TODO: maybe something like CXMLChildrenList GetElementsByName(const string &AName); (like in JavaScript: document.getElementsByTagName)

	string GetAttribute(const string &AName) const;
	void SetAttribute(const string &AName, const string &AValue);
	void DeleteAttribute(const string &AName);

	string GetText();
	
	CXMLChildrenList Children;

private:
	map<string, string> Attributes;
};

class CXMLSingleValueNode : public CXMLNode
{
public:
	CXMLSingleValueNode(const string &AValue = "");

	const string& GetValue() const;
	void SetValue(const string &AValue);

	virtual string GetText() = 0;
private:
	string Value;
};

class CXMLCommentNode : public CXMLSingleValueNode
{
public:
	CXMLCommentNode(const string &AValue = "");

	string GetText();

};

class CXMLTextNode : public CXMLSingleValueNode
{
public:
	CXMLTextNode(const string &AValue = "");

	string GetText();
};

class CXML
{
public:
	CXML(const string &AFilename = "");
	CXML(const CXML &ASource); // don't know, if it's really required...
	~CXML();
	CXML& operator=(const CXML &ASource);

	void LoadFromFile(const string &AFilename);
	void SaveToFile(const string &AFilename);

	CXMLChildrenList Root;

private:

};

class CXMLParser
{
public:
	CXMLParser(const string &AText);
	CXMLChildrenList Parse();

private:
	void SetText(const string &AText);

	bool Good();

	void SkipWhiteSpace();
	bool isWhiteSpace(char c);
	bool isValidNameChar(char c) ;

	bool isAnotherTag();

	CXMLNode* ParseNode();
	CXMLCommentNode* ParseComment();
	CXMLPrologNode* ParseProlog();
	CXMLTextNode* ParseText();
	//CXMLNormalNode* ParseNormal(); // need to handle nesting, setting parent, etc....
	pair<string, string> ParseAttribute();
	string ParseName();
	string ParseValue();
	string ParseEntity();

	void ReportError(const string &Message, int Position);

	CXMLChildrenList Result;
	int Current;
	string Text;
};

#endif // _2DE_XML_H_

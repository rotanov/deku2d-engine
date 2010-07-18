#ifndef _2DE_XML_H_
#define _2DE_XML_H_

#include "2de_Core.h"

class CXMLNode
{
public:
	CXMLNode();
	virtual ~CXMLNode();

	virtual CXMLNode* Copy() = 0;

	const string& GetName() const;
	void SetName(const string &AName);
	virtual string GetText() = 0;

	CXMLNode* GetParent();
	void SetParent(CXMLNode *AParent);

protected:
	CXMLNode *Parent;
	string Name;
};

class CXMLPrologNode : public CXMLNode
{
public:
	CXMLPrologNode();

	CXMLPrologNode* Copy();

	const string& GetVersion() const;
	void SetVersion(const string &AVersion);

	string GetText();
private:
	string Version;
};

class CXMLChildrenList
{
public:
	class ConstIterator
	{
	public:
		bool operator==(const ConstIterator &AIterator) const;
		bool operator!=(const ConstIterator &AIterator) const;

		ConstIterator& operator++();
		ConstIterator operator++(int);
		ConstIterator& operator--();
		ConstIterator operator--(int);

		CXMLNode* operator*();

	private:
		list<CXMLNode *>::const_iterator Backend;

		friend class CXMLChildrenList;
	};

	class Iterator : public ConstIterator
	{
	public:
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

	ConstIterator Begin() const;
	ConstIterator End() const;
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

	CXMLNormalNode* Copy();

	// TODO: maybe something like CXMLChildrenList GetElementsByName(const string &AName); (like in JavaScript: document.getElementsByTagName)
	// TODO: add support for special "ID" attribute (see specs).. its values are unique for entire document.. add GetElementByID, that should return node by its ID

	string GetAttribute(const string &AName) const;
	void SetAttribute(const string &AName, const string &AValue);
	void DeleteAttribute(const string &AName);

	string GetText();
	void SetInnerText(const string &AText);
	
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

	CXMLCommentNode* Copy();

	string GetText();

};

class CXMLTextNode : public CXMLSingleValueNode
{
public:
	CXMLTextNode(const string &AValue = "");

	CXMLTextNode* Copy();

	string GetText();
};

class CXML
{
public:
	CXML(const string &AFilename = "");
	CXML(const CXML &ASource);
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
	bool isValidNameChar(char c, bool Initial = false);

	bool isAnotherTag();
	bool hasAttribute();

	void ParseInside(CXMLChildrenList *List);

	CXMLNode* ParseNode();
	CXMLCommentNode* ParseComment();
	CXMLNormalNode* ParseNormal();
	CXMLPrologNode* ParseProlog();
	CXMLTextNode* ParseText();
	pair<string, string> ParseAttribute();
	string ParseAttributeName();
	string ParseAttributeValue();
	string ParseEntity();
	string ParseTagName();

	void ReportError(const string &Severity, const string &Message, int Position);
	CXMLNode *CurrentLevel;
	CXMLChildrenList Result;
	int Current;
	string Text;
};

class CXMLHelper
{
public:
	static CXMLHelper* Instance();

	string EntitiesEncode(const string &AText);
	string EntitiesDecode(const string &AText);

	char GetCharByEntity(const string &AEntity);
private:
	typedef map<char, string> EntityMap;

	CXMLHelper();

	EntityMap Entities;

	static CXMLHelper _instance;
};

#endif // _2DE_XML_H_

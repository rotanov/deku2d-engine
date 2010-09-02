#ifndef _2DE_XML_H_
#define _2DE_XML_H_

#include "2de_Core.h"

class CXMLNode; // forward declaration

/**
* CXMLChildrenList - класс, представляющий собой список детей XML-узла.
*
* Является обёрткой над std::list<CXMLNode *>, расширяющей и дополняющей его специализированными функциями.
*/

class CXMLChildrenList
{
public:
	typedef list<CXMLNode *> StorageType;

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
		StorageType::const_iterator Backend;

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
		StorageType::iterator Backend;

		friend class CXMLChildrenList;
	};

	CXMLChildrenList(CXMLNode *ANode = NULL);
	~CXMLChildrenList();

	CXMLNode* First(const string &AValue = "");
	CXMLNode* Last(const string &AValue = "");
	CXMLNode* operator[](size_t AID);

	void AddFirst(CXMLNode *ANode);
	void AddLast(CXMLNode *ANode);
	void AddAfter(const Iterator &AIterator, CXMLNode *ANode);
	void AddBefore(const Iterator &AIterator, CXMLNode *ANode);
	CXMLNode* Remove(Iterator &AIterator);
	CXMLNode* Remove(CXMLNode *ANode);
	void DeleteAll();
	void Clear();

	ConstIterator Begin() const;
	ConstIterator End() const;
	Iterator Begin();
	Iterator End();

	bool IsEmpty() const;
	bool GetSize() const;

private:
	StorageType Backend;
	CXMLNode *Node;

	friend class CXMLNode;
};

/**
* CXML - основной класс для работы с XML, представляющий объектную модель XML-документа как целое.
*/

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
};

/**
* CXMLNode - базовый класс XML-узла. Обеспечивает полиморфизм.
*/

class CXMLNode
{
public:
	typedef CXMLChildrenList::Iterator ChildrenIterator;

	CXMLNode();
	virtual ~CXMLNode();

	virtual CXMLNode* Copy() = 0;

	virtual string GetValue() const;
	virtual void SetValue(const string &AValue);

	virtual string GetName() const;
	virtual void SetName(const string &AName);

	virtual string GetText() const;
	virtual void SetInnerText(const string &AText);

	virtual CXMLNode* GetParent();
	virtual void SetParent(CXMLNode *AParent);

	virtual bool HasAttribute(const string &AName) const;
	virtual string GetAttribute(const string &AName) const;
	virtual void SetAttribute(const string &AName, const string &AValue);
	virtual void DeleteAttribute(const string &AName);

	virtual bool IsErroneous() const;

	CXMLNode* PreviousSibling(const string &AValue = "") const;
	CXMLNode* NextSibling(const string &AValue = "") const;
	CXMLNode* PreviousSimilarSibling() const;
	CXMLNode* NextSimilarSibling() const;

	virtual void UpdateDepth();

	CXMLChildrenList Children;

protected:
	virtual string GetStartingSequence() const;
	virtual string GetEndingSequence() const;
	virtual string GetContent() const;
	string GetTabulation() const;

	CXMLNode *Parent;
	string Name;
	unsigned int Depth;

	map<string, string, CCaseInsensetiveComparison> Attributes;
};

/**
* CXMLErroneousNode - класс ошибочного XML-узла. Статический объект данного класса возвращается в тех случаях, где по смыслу возвращался бы NULL.
*
* С помощью статического объекта этого класса достигаются две цели:
* 	1. Ошибочные действия пользователя над XML-деревом не вызывают падение из-за NULL-dereferencing'а.
* 	2. Пользователь надёжно уведомляется о возникших ошибках посредством лога.
*/

class CXMLErroneousNode : public CXMLNode
{
public:
	CXMLErroneousNode* Copy();

	string GetValue() const;
	void SetValue(const string &AValue);

	string GetName() const;
	void SetName(const string &AName);

	string GetText() const;
	void SetInnerText(const string &AText);

	CXMLNode* GetParent();
	void SetParent(CXMLNode *AParent);

	string GetAttribute(const string &AName) const;
	void SetAttribute(const string &AName, const string &AValue);
	void DeleteAttribute(const string &AName);

	bool IsErroneous() const;
};

static CXMLErroneousNode ErroneousNode;

/**
* CXMLNodeWithAttributes - класс XML-узла, имеющего аттрибуты. Используется как базовый класс для CXMLNormalNode и CXMLPrologNode.
*/

class CXMLNodeWithAttributes : public CXMLNode
{
public:
	bool HasAttribute(const string &AName) const;
	string GetAttribute(const string &AName) const;
	void SetAttribute(const string &AName, const string &AValue);
	void DeleteAttribute(const string &AName);

	string GetValue() const;

protected:
	string GetContent() const;
};

/**
* CXMLPrologNode - класс XML-узла - пролога.
*/

class CXMLPrologNode : public CXMLNodeWithAttributes
{
public:
	CXMLPrologNode();

	CXMLPrologNode* Copy();

	string GetVersion() const;
	void SetVersion(const string &AVersion);

	//string GetText() const;

protected:
	string GetStartingSequence() const;
	string GetEndingSequence() const;
};

/**
* CXMLNormalNode - класс XML-узла - элемента, то есть узла, который может иметь детей.
*/

class CXMLNormalNode : public CXMLNodeWithAttributes
{
public:
	CXMLNormalNode(const string &AName);
	~CXMLNormalNode();

	CXMLNormalNode* Copy();

	void SetName(const string &AName);

	// TODO: maybe something like CXMLChildrenList GetElementsByName(const string &AName); (like in JavaScript: document.getElementsByTagName)
	// TODO: add support for special "ID" attribute (see specs).. its values are unique for entire document.. add GetElementByID, that should return node by its ID

	void SetInnerText(const string &AText);

	void UpdateDepth();

protected:
	string GetStartingSequence() const;
	string GetEndingSequence() const;
	string GetContent() const;
};

/**
* CXMLSingleValueNode - класс XML-узла с одиночным текстовым значением.
*
* Является базовым классом для комментариев и текстовых узлов.
*/

class CXMLSingleValueNode : public CXMLNode
{
public:
	CXMLSingleValueNode(const string &AValue);

	string GetValue() const;
	void SetValue(const string &AValue);

protected:
	string Value;
};

/**
* CXMLCommentNode - класс XML-узла - комментария.
*/

class CXMLCommentNode : public CXMLSingleValueNode
{
public:
	CXMLCommentNode(const string &AValue = "");

	CXMLCommentNode* Copy();

protected:
	string GetStartingSequence() const;
	string GetEndingSequence() const;
};

/**
* CXMLTextNode - класс текстового узла XML.
*/

class CXMLTextNode : public CXMLSingleValueNode
{
public:
	CXMLTextNode(const string &AValue = "");

	CXMLTextNode* Copy();

	string GetText() const;
};

/**
* CXMLParser - парсер XML.
*/

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
	string Text;
	int Current;
};

/**
* CXMLHelper - класс, содержащий некоторые вспомогательные функции для XML, в частности для парсера.
*/

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

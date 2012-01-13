#ifndef _2DE_XML_H_
#define _2DE_XML_H_

#include "2de_Core.h"

namespace Deku2D
{
	class CXMLNode; // forward declaration

	/**
	* CXMLChildrenList - a class that represents XML-node children list.
	*
	* This class is a wrapper around std::list<CXMLNode *>, that extends it with special functions.
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
		CXMLNode *OwnerNode;	// OwnerNode, not just Owner because it's now clear that it's Node, Not ChildrenList or something. Rename if want.

		friend class CXMLNode;
	};

	/**
	* CXML - the main class for working with XML that provides the object model of an XML-document as a whole.
	*/

	class CXML
	{
	public:
		CXML(const string &AFilename = "");
		CXML(const CXML &ASource);
		~CXML();
		CXML& operator=(const CXML &ASource);

		void LoadFromFile(const string &AFilename);
		void LoadFromMemory(byte *AData, size_t ALength);

		void SaveToFile(const string &AFilename);

		CXMLChildrenList Root;

	private:
		void LoadFromStorage(CStorage &AStorage);
	};

	/**
	* CXMLNode - an XML-node base class for polymorphism.
	*/

	class CXMLNode
	{
	public:
		enum EXMLNodeType
		{
			XML_NODE_TYPE_UNDEFINED,
			XML_NODE_TYPE_NORMAL,
			XML_NODE_TYPE_PROLOG,
			XML_NODE_TYPE_COMMENT,
			XML_NODE_TYPE_TEXT,
			XML_NODE_TYPE_ERRONEOUS,
		};

		typedef CXMLChildrenList::Iterator ChildrenIterator;
		typedef map<string, string, CCaseInsensitiveComparison> AttributesContainer;
		typedef AttributesContainer::const_iterator AttributesIterator;

		CXMLNode();
		virtual ~CXMLNode();

		virtual CXMLNode* Copy() = 0;

		EXMLNodeType GetType() const;

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
		unsigned Depth;
		EXMLNodeType Type;

		AttributesContainer Attributes;
	};

	/**
	* CXMLErroneousNode - an erroneous XML-node class. Static instance of this class is returned in cases, where otherwise NULL would be returned.
	*
	* Two goals are achieved using the static instance of this class:
	* 	1. Erroneous actions of a user on an XML-tree don't cause crashes due to NULL-dereferencings.
	* 	2. The user is properly notified about the occured errors by means of the log.
	*/

	class CXMLErroneousNode : public CXMLNode
	{
	public:
		CXMLErroneousNode();

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
	* CXMLNodeWithAttributes - a class for an XML-node that has attributes. Used as a base class for CXMLNormalNode and CXMLPrologNode.
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
	* CXMLPrologNode - an XML prolog node class.
	*/

	class CXMLPrologNode : public CXMLNodeWithAttributes
	{
	public:
		CXMLPrologNode();

		CXMLPrologNode* Copy();

		string GetVersion() const;
		void SetVersion(const string &AVersion);

	protected:
		string GetStartingSequence() const;
		string GetEndingSequence() const;
	};

	/**
	* CXMLNormalNode - an XML element class, i. e. a node that can have children.
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
	* CXMLSingleValueNode - a class for a XML-node with single text value.
	*
	* It is a base class for comment and text nodes.
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
	* CXMLCommentNode - an XML comment node class.
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
	* CXMLTextNode - an XML text node class.
	*/

	class CXMLTextNode : public CXMLSingleValueNode
	{
	public:
		CXMLTextNode(const string &AValue = "");

		CXMLTextNode* Copy();

		string GetText() const;
	};

	/**
	* CXMLParser - an XML parser.
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
	* CXMLHelper - a class that contains some helper-functions for XML, particularly for the parser.
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

}	//	namespace Deku2D

#endif // _2DE_XML_H_

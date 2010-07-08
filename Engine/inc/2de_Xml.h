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

class CXMLNormalNode : public CXMLNode
{
public:
	class CChildrenList
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

			/*void DeleteElement(); 		// i think iterator should not do this shit.. list should..
			void InsertBefore(CXMLNode &ANode);
			void InsertAfter(CXMLNode &ANode);*/

			//~Iterator(); // for what?
		private:
			list<CXMLNode *>::iterator Backend;

			friend class CXMLNormalNode::CChildrenList;
		};

		CChildrenList(CXMLNode *ANode);
		~CChildrenList();

		void AddFirst(CXMLNode *ANode);
		void AddLast(CXMLNode *ANode);
		void AddAfter(const Iterator &AIterator, CXMLNode *ANode);
		void AddBefore(const Iterator &AIterator, CXMLNode *ANode);
		CXMLNode* Remove(Iterator &AIterator);

		Iterator Begin();
		Iterator End();

		bool IsEmpty() const;
		bool GetSize() const;

	private:
		list<CXMLNode *> Backend;
		CXMLNode *Node;
	};

	typedef CChildrenList::Iterator ChildrenIterator;	// maybe not required..

	CXMLNormalNode(const string &AName);
	~CXMLNormalNode();

	//bool HaveChildren(); // return 1

	// TODO: maybe something like CChildrenList GetElementsByName(const string &AName); (like in JavaScript: document.getElementsByTagName)

	string GetAttribute(const string &AName) const;
	void SetAttribute(const string &AName, const string &AValue);
	void DeleteAttribute(const string &AName);

	string GetText();
	
	CChildrenList Children;

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

	CXMLNormalNode::CChildrenList Root;

private:

};


#endif // _2DE_XML_H_

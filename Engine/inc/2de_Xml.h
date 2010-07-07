#ifndef _2DE_XML_H_
#define _2DE_XML_H_

#include "2de_Core.h"

class CXMLNode
{
public:
	CXMLNode();
	~CXMLNode();
	virtual bool HaveChildren(); // 0 by default

	const string& GetName() const;
	void SetName(const string &AName);

protected:
	CXMLNode *Parent;
private:
	string Name;

};

// TODO: class for "prolog", "specification" or whatever node..

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
			Iterator(CXMLNode &AXMLNode);
			Iterator(const iterator &AIterator);

			Iterator& operator=(const Iterator &AIterator);

			bool operator==(const Iterator &AIterator) const;
			bool operator!=(const Iterator &AIterator) const;

			Iterator& operator++();
			Iterator operator++(int);
			Iterator& operator--();
			Iterator operator--(int);

			void DeleteElement(); 		// i think iterator should not do this shit.. list should..
			void InsertBefore(CXMLNode &ANode);
			void InsertAfter(CXMLNode &ANode);

			~Iterator(); // for what?
		private:
			list<CXMLNode *>::iterator Backend;
		};

		void AddFirst(CXMLNode &ANode);
		void AddLast(CXMLNode &ANode);
		void AddAfter(const Iterator &AIterator, CXMLNode &ANode);
		void AddBefore(const Iterator &AIterator, CXMLNode &ANode);
		void Remove(const Iterator &AIterator);

		Iterator Begin();
		Iterator End();

		bool IsEmpty() const;
		bool GetLength() const;

	private:
		list<CXMLNode *> Backend;

	};

	typedef CChildrenList::Iterator ChildrenIterator;	// maybe not required..

	bool HaveChildren(); // return 1
	// TODO: interface for iterating through children..

	// TODO: interface for attributes..
	const string& GetAttribute(const string &AName) const;
	void SetAttribute(const string &AName, const string &AValue);
	void DeleteAttribute(const string &AName);

	CChildrenList Children;

protected:		// do we really need protected here? maybe no..//of course we don't
	map<string, string> Attributes;
private:

};

class CXMLCommentNode : public CXMLNode
{
public:
	CXMLCommentNode();	// set Name to "!--" in constructor

	const string& GetValue() const;
	void SetValue(const string &AValue);
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

	CXMLNormalNode::CChildrenList Root;	// <-- i found a way to store root children list..

private:

};


#endif // _2DE_XML_H_

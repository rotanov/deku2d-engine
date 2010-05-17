#ifndef _2DE_XML_H_
#define _2DE_XML_H_
/*
<-----------------Xml-Engine----------------->
*	Developer : DekuTeam
*	List based Xml Engine
*	2008 year
*/

#include "2de_Core.h"

#define XML_DYNAMIC
#ifndef XML_DYNAMIC
#define MAX_NODES					0xffff
#endif

#define XML_NODE_BEGIN				0x0001
#define XML_NODE_ATTRIBUTE			0x0002
#define XML_NODE_END				0x0003
#define XML_EMPTY_STRING			0x0004
#define XML_ERROR_STRING			0x000f

#define XML_ERROR

#ifdef XML_ERROR
#define XML_ERROR0					"Unexpected attributes list!"
#define XML_ERROR1					"Unexpected end of file!"
#define XML_ERROR2					"Syntaxis error!"
#define XML_ERROR3					"Limit of string characters expired!"
#define XML_ERROR4					"Cannot set attributes for node which doesn't exist!"
#define XML_ERROR5					"Parse error!"
#define XML_ERROR6					"Fatal error! Memory damaged!"
#define XML_ERROR7					"Fatal error! Unexpected node end!"
#define XML_ERROR8					"Fatal error! Cannot write to specified file!"
#endif

#define XML_NODE					0x0
#define XML_ATTRIBUTE				0x1

//note if string is erroneous type parser will return XML_EMPTY_STRING!
//but may cause log error

//Lexem structure
struct Lexem
{
	int						Start;
	int						Len;
	int						Type;
};

//Lexem types
#define XMLLEXEM_BEGIN				0x0
#define XMLLEXEM_SLASH				0x1
#define XMLLEXEM_END				0x2
#define XMLLEXEM_EQUAL				0x3
#define XMLLEXEM_COMMENT			0x4
#define XMLLEXEM_FREE				0x5
//-------------------------------------
#define XMLLEXEM_STRING				0xf

//XML enumeration
#define XMLENUM_ATTRSONLY			0x0
//XML enumeration results
#define XMLENUM_END					0x00
#define XMLENUM_ATTRIBUTE			0x01

//note : using only DataBase on XML Engine!!!!
class CXMLNode
{
public:
	CXMLNode();
	string					Name;
	string					Value;
	CXMLNode				*Child;
	CXMLNode				*Parent;
	CXMLNode				*Next;
	CXMLNode				*Previous;
	CXMLNode				*Last;
	byte					Type;
	unsigned int			count;
	int						depth;
	CXMLNode				*Add(string name, string value);
	void					Clear();
	CXMLNode*				Get(const char* Str);
	char					*GetValue(){return (char*)Value.data();}
	void					Write(CFile &f, int depth);
	bool					GetPair(string &Key, string &Value);
	void					ResetEnum(int Kind){enumPtr = Child;enumKind = Kind;_last = 0;}
	int						Enum(string &outKey, string &outValue, int &res);
	void					ResetPointer();
private:
	int						enumKind;
	bool					_last;
	CXMLNode				*Pointer;
	CXMLNode				*enumPtr;
};

typedef CXMLNode *XMLNode;
//Parse functions
unsigned int XMLParse(string Str, string &Buff1, string &Buff2);

class CXMLTable
{
public :
	CXMLTable();
	~CXMLTable(){First->Clear();}
	bool					LoadFromFile(const char *fname);
	bool					SaveToFile(const char *fname);
	XMLNode					First;
private:
};

#endif // _2DE_XML_H_

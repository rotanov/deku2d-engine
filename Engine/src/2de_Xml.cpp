#include "2de_Xml.h"
//<------------------XMLNode------------------->
//<-XMLNode->
/*
*	Constructor for XMLNode
*	Setting up all attributes to default
*/
CXMLNode::CXMLNode()
{
	Value = "";
	Child = NULL;
	Parent = NULL;
	Next = NULL;
	count = 0;
	Type = XML_ATTRIBUTE;
	Last = NULL;
	Previous = NULL;
	Name = "";
	enumPtr = NULL;
	enumKind = 0;
	Pointer = NULL;
	_last = 0;
}
/*
*	_XMLNode *_XMLNode::Get(PCHAR Str);
------------------------------------------------
*	Searches for list element with name Str in
*	children of this
------------------------------------------------
*	Returns pointer if success, NULL if failed.
*/
CXMLNode *CXMLNode::Get(const char* Str)
{
	if (Str==NULL){
		Log("ERROR", "Cannot get XML node for because string pointer has NULL value", Str);
		return NULL;
	}
	CXMLNode *result = NULL;
	CXMLNode *ptr = this->Child;
	if (ptr==NULL){
		Log("ERROR", "Cannot get XML node for \"%s\"", Str);
		return NULL;
	}
	while (ptr != NULL && result == NULL)
	{
		if (ptr->Name == Str)
		{
			result = ptr;
		}
		ptr = ptr->Next;
	}
	return result;
}
/*
*	_XMLNode *_XMLNode::Add(char *name, char *value);
------------------------------------------------
*	Adds children to current list.
------------------------------------------------
*	Returns pointer to it.
*/
CXMLNode *CXMLNode::Add(string name, string value)
{
/*	if (name==NULL || value==NULL)
	{
		Log("XMLERROR", XML_ERROR6);
		return NULL;
	}*/
	if (Child == NULL)
	{
		Child = new CXMLNode; // @todo: Memory leak here.
		Last = Child;
		Last->Name = name;
		Last->Value = value;
		Type = XML_NODE;
		Last->depth = depth + 1;
		count ++;
		return Last;
	}
	else{
		if (Last != NULL)
		{
			Last->Next = new CXMLNode;
			XMLNode TempLast = Last;
			Last = Last->Next;
			Last->Name = name;
			Last->Value = value;
			Type = XML_NODE;
			Last->depth = depth + 1;
			Last->Previous = TempLast;
			count ++;
			return Last;
		}
		else
			Log("XMLERROR", XML_ERROR6);
	}
	return NULL;
}
/*
*	void _XMLNode::Clear();
------------------------------------------------
*	Clears all node data.
------------------------------------------------
*/
void CXMLNode::Clear()
{
	Name="";
	Value="";
	if (Type == XML_ATTRIBUTE){
		return;
	}
	CXMLNode *ptr = Child;
	while (ptr!=NULL)
	{
		CXMLNode *tmp=ptr;
		ptr = ptr->Next;
		tmp->Clear();
		delete tmp;
	}
}
/*
*	bool _XMLNode::GetPair(string Key, string Value);
------------------------------------------------
*	Gets pair
------------------------------------------------
*/
bool CXMLNode::GetPair(string &Key, string &Value)
{
	if (Pointer == NULL)
		return false;
	Key = Pointer->Name;
	Value = Pointer->Value;
	Pointer = Pointer->Next;
	return true;
}
/*
*	void _XMLNode::ResetPointer();
------------------------------------------------
*	Resets pointer to the start position
------------------------------------------------
*/
void CXMLNode::ResetPointer()
{
	Pointer = Child;
}

/*
*	int _XMLNode::Enum(string &outKey, string &outValue, int &res);
------------------------------------------------
*	Begins enumeration of children
------------------------------------------------
*/
int CXMLNode::Enum(string &outKey, string &outValue, int &res)
{
	res = XMLENUM_END;
	/*if (enumPtr == NULL)
		ResetEnum(XMLENUM_ATTRSONLY);*/
	switch (enumKind)
	{
	case XMLENUM_ATTRSONLY:
		{

			if (enumPtr == NULL/*&&!_last*/)
			{
				_last = 0;
				return XMLENUM_END;
			}
			_last = 0;
			while (enumPtr->Type == XML_NODE)
			{
				if (enumPtr->Next == NULL)
					return XMLENUM_END;
				enumPtr = enumPtr->Next;
			}
			outKey = enumPtr->Name;
			outValue = enumPtr->Value;
//			res = XMLENUM_ATTRIBUTE;
			enumPtr = enumPtr->Next;
			return XMLENUM_ATTRIBUTE;
			break;
		}
	}
	return XMLENUM_END;
}

/*
*	void _XMLNode::Write(CFile f, int depth);
------------------------------------------------
*	Writes node data to file
------------------------------------------------
*/
void CXMLNode::Write(CFile &f, int depth)
{
	char b=9;
	for (int i=0;i<depth;i++)
		f.Write(&b, 1);
	if (Type==XML_ATTRIBUTE){
		f.WriteLine(Name + " = \"" + Value + "\"");
		if (Next!=NULL)
			Next->Write(f, depth);
	}
	else
	{
		f.WriteLine("<" + Name + ">");

		if (Child)
			Child->Write(f, depth + 1);
		
		for (int i=0;i<depth;i++)
			f.Write(&b, 1);
		f.WriteLine("</" + Name + ">");
		if (Next!=NULL)
			Next->Write(f, depth);
	}
}

bool isEmpty(char c)
{
	return (c==' ' || c == '	')?true:false;
}

void SkipWhite(string s, int &pos)
{
	int len = s.length();
	while (isEmpty((char)(s.c_str()[pos])))
	{
		if (pos == len - 1)
			return;
		pos++;
	}
}

bool isAllowed(byte c)
{
	if (((c <= 'z' && c >= 'a') || (c <= 'Z' && c >= 'A')) || ((c <= '9' && c >= '0') || (c <= 46 && c >= 32))|| c == 9)
		return 1;
	return 0;
}

bool isAllowedEx(byte c)
{
	if (((c <= 'z' && c >= 'a') || (c <= 'Z' && c >= 'A')) || ((c <= '9' && c >= '0') || (c <= 46 && c >= 32))|| c == 9|| c =='\\' || c == '/' || c == '_' /* hello there it's me */ || c == ':')
		return 1;
	return 0;
}
void RemWhite(string &s)
{
	int pos = s.length() - 1;
	if (pos < 0)
		return;
	while (s.c_str()[pos] == ' ' || s.c_str()[pos] == '	')
	{
		s.resize(pos, 0);
		pos = s.length() - 1;
		if (pos <= 0)
			return;
	}
}

int GetString(string s, int ps, string &res, char c, bool remlast, bool firstwhite, bool schar)
{
	if (!firstwhite && (s.c_str()[ps] == ' ' || s.c_str()[ps] == '	'))	
		return -1;
	int pos = ps;
	res = "";
//	if (firstwhite)
//		SkipWhite(s, pos);
	while (s.c_str()[pos] != c)
	{
		//
		if (pos == s.length()-1)
			return -1;
		if (schar)
		{
			if (!isAllowedEx(s.c_str()[pos]))
				return -1;
		}
		else
		{
			if (!isAllowed(s.c_str()[pos]))
				return -1;
		}
		res += s.c_str()[pos];
		pos ++;
	}
	if (remlast) 
		RemWhite(res);
	if (res.length() == 0)
		return -1;
	return pos;
}
/*
------------------------------------------------
*	unsigned int XMLParse(char *Str)
------------------------------------------------
*	Analyses specified string and returning type
*	of it.
------------------------------------------------
*	Returns XML_NODE_BEGIN if string is
*	beginning of xml node.
*	Returns XML_NODE_ATTRIBUTE if string is
*	attribute of current node.
*	Returns XML_NODE_END if string is
*	ending of xml node.
*	Returns XML_EMPTY_STRING if string is empty
*	or it is comment.
*/
unsigned int XMLParse(string Str, string &Buff1, string &Buff2){
	//analysing
	//first dividing to lexems
	//but now setting up char table
	int pos = 0;
	SkipWhite(Str, pos);
	if (Str == "" || pos - 1 == Str.length())
		return XML_EMPTY_STRING;
	if (Str.c_str()[pos] == '<')
	{
		//here new category
		int pos1 = pos + 1;
		SkipWhite(Str, pos1);
		int res = 0;
		if (Str.c_str()[pos1] == '/')
		{
			//End of category
			res = XML_NODE_END;
			pos1++;
		}
		else
		{
			//new category
			res = XML_NODE_BEGIN;
		}
		int ind = GetString(Str, pos1, Buff1, '>', 1, 0, 0);
		if (ind == -1)
		{
			Log("XMLERROR", XML_ERROR2);
			return XML_ERROR_STRING;
		}
		pos1 = ind;
		SkipWhite(Str, pos1);
		if (!isEmpty(Str.c_str()[pos1])&&pos1 + 1 != Str.length())
			res = XML_ERROR_STRING;

		return res;
	}
	else
	{
		if (Str.c_str()[pos] == '#')
		{
			return XML_EMPTY_STRING;
		}
		else
		{
			int ind = GetString(Str, pos, Buff1, '=', true, false, /* hello it's me again. there was 0 befor me*/ 1);
			if (ind == -1)
			{
				Log("XMLERROR", XML_ERROR2);
				return XML_ERROR_STRING;
			}
			ind++;
			SkipWhite(Str, ind);
			if (Str.c_str()[ind] != '"')
			{
				Log("XMLERROR", XML_ERROR2);
				return XML_ERROR_STRING;
			}
			ind = GetString(Str, ind+1, Buff2, '"', false, false, 1);
			if (ind == -1)
			{
				Log("XMLERROR", XML_ERROR2);
				return XML_ERROR_STRING;
			}
			return XML_NODE_ATTRIBUTE;
		}
	}
}
//<------------------XMLTable------------------>

//*Currently only reading (will be fixed soon)

/*
*	Constructor for XMLTable
*	Setting up all attributes to default
*/
CXMLTable::CXMLTable()
{
	First = NULL;
	First = new CXMLNode; // @todo: Memory leak here.
}
/*
------------------------------------------------
*	bool XMLTable::LoadFromFile(char *fname)
------------------------------------------------
*	Reading trunk structure with attributes from
*	specified file.
*	Returns true if succeeded, and false on failed.
------------------------------------------------
*	Can be failed only if file doesn't exist or
*	it blocked from reading and also can be failed
*	on parse error.
*/
bool CXMLTable::LoadFromFile(const char *fname)
{
	CFile f;
	if (!f.Open(fname, CFile::OPEN_MODE_READ))
	{
		Log("XMLERROR", "Cannot load xml file \"%s\".", fname);
		return false;
	}
	string Buffer;//max allowed string - 4096
	string Key;//max allowed string - 4096
	string Value;//max allowed string - 4096
//	memset(Buffer, 0, 4096);
	//Log("XML", "Preparing");
	// int strstart = 0; // unused variable
	First->Clear();
	XMLNode _tmp = new CXMLNode;
	XMLNode ptr = _tmp;
	int cnt=0;
	while (!f.Eof())
	{
		Buffer="";
		Key="";
		Value="";
		//now reading string
		byte b = 0;
		//Log("XML","Reading");
		while (b != '\n' && !f.Eof())
		{			
			if (f.Read(&b, 1) && !f.Eof())
				if (b != 13 && b != 10){
					Buffer+= b;
				}
		}
		//limit
#ifdef XML_LIMIT
		if (Buffer.length >= 4096)
		{
			//error
			Log("XMLERROR", XML_ERROR3);
			f.Close();
			return 0;
		}
#endif
		//now parsing
		//Log("XML","Parsing");
		int ty = XMLParse(Buffer, Key, Value);
		//Log("XML","Analysing %s %d", Buffer, Buffer[strlen(Buffer) - 1]);
		//checking type
		switch (ty)
		{
		case XML_NODE_BEGIN:
			{
				XMLNode tmp = ptr->Add(Key, Value);
				tmp->Parent = ptr;
				ptr = tmp;
				cnt++;
				break;
			}
		case XML_NODE_ATTRIBUTE:
			{
				if (cnt!=0){
					XMLNode tmp = ptr->Add(Key, Value);
					tmp->Parent = ptr;
				}
				else
				{
					//error
					Log("XMLERROR", XML_ERROR0);
					f.Close();
					return 0;
				}
				break;
			}
		case XML_NODE_END:
			{
				if (cnt==0)
				{
					Log("XMLERROR", XML_ERROR1);
					f.Close();
					return false;
				}
				if (ptr->Name!= Key)
				{
					Log("XMLERROR", XML_ERROR7);
					f.Close();
					return false;
				}
				ptr = ptr->Parent;
				cnt--;
				break;
			}
		case XML_ERROR_STRING:
			{
				f.Close();
				return false;
			}
		default:
			break;
		}
	}
	First = _tmp->Child;
	delete _tmp;
	return true;
}
/**
------------------------------------------------
*	bool XMLTable::SaveToFile(char *fname)
------------------------------------------------
*	Writes trunk structure with attributes to
*	specifed file.
*	Returns true if succeded, and false on failed.
------------------------------------------------
*	Can be failed only if file blocked from 
*	writing.
*/
bool CXMLTable::SaveToFile(const char *fname)
{
	CFile f;
	if (!f.Open(fname, CFile::OPEN_MODE_WRITE)){
		Log("XMLERROR",XML_ERROR8);
		return 0;
	}
	First->Write(f,0);
	return f.Close();
}

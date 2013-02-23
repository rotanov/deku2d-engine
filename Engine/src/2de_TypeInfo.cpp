#include "2de_TypeInfo.h"

map<string, TypeInfo*>& TypeInfo::_getTypeInfos()
{
	static map<string, TypeInfo*> _typeInfos;
	return _typeInfos;
}



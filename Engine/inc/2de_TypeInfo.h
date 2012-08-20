#pragma once

#include "2de_PropertyInfo.h"
#include <map>
#include <string>
#include "rapidjson/document.h"
using std::map;
using std::string;

#define TEMPLATED_CALL( TYPEINFO, FUNCTION, A0 )	\
	TYPEINFO->CastedCall??????

class TypeInfo
{
public:
	virtual char* Name() const = 0;
	virtual void* New() const = 0;
	virtual PropertyInfo* GetProperty(const string &name) const = 0;
	virtual map<string, PropertyInfo*>& Properties() = 0;
	virtual TypeInfo* BaseInfo() = 0;
	virtual string GetString(void *instance) = 0;
	virtual void SetString(void *instance, const string &value) = 0;
	PropertyInfo *FindProperty(const string &name)
	{
		TypeInfo *typeInfo = this;
		PropertyInfo *prop = 0;
		while (typeInfo && !prop)
		{
			if (typeInfo->Properties().find(name) != typeInfo->Properties().end())
				prop = typeInfo->GetProperty(name);
			typeInfo = typeInfo->BaseInfo();
		}
		return prop;
	}
	static TypeInfo* GetTypeInfo(const string &typeName)
	{
		if (_typeInfos.find(typeName) == _typeInfos.end())
		{
			char errMsg[128];
			sprintf(errMsg, "Type info for type: \"%s\" doesn't exist.", typeName.c_str());
			throw(errMsg);
		}
		else
			return _typeInfos[typeName];
	}
	virtual void FillJSONValue( rapidjson::Value*, void* instance)
	{
		throw "FillJSONValue unimplemented for this type";
	}



protected:
	static map<string, TypeInfo*> _typeInfos;
};

#define _D2D_BEGIN_TYPE_INFO_DECL(TYPE)	\
	class TypeInfo##TYPE : public TypeInfo	\
	{	\
	public:	\

#define _D2D_TYPE_INFO_DECL_BASE(TYPE)	\
		TypeInfo##TYPE() { _typeInfos[#TYPE] = &_instance; }	\
		void* New() const { return Make<TYPE>::New(); }	\
		char* Name() const { return #TYPE; }	\
		virtual void SetString(void *instance, const string &value)	\
		{	\
			TYPE *typedInstance = static_cast<TYPE*>(instance);	\
			*typedInstance = Convert<TYPE>::FromString(value);	\
		}	\
		string GetString(void *instance)	\
		{	\
			TYPE *typedInstance = static_cast<TYPE*>(instance);	\
			return Convert<TYPE>::ToString(*typedInstance);	\
		}	\
		PropertyInfo* GetProperty(const string &name) const { return _props[name]; }	\
		map<string, PropertyInfo*>& Properties() { return _props; }	\

#define _D2D_END_TYPE_INFO_DECL(TYPE)	\
	static TypeInfo##TYPE _instance;	\
	static map<string, PropertyInfo*> _props;	\
};	\
TypeInfo##TYPE TypeInfo##TYPE::_instance;	\
map<string, PropertyInfo*> TypeInfo##TYPE::_props;	\

#define D2D_DECL_TYPE_INFO(TYPE)	\
	_D2D_BEGIN_TYPE_INFO_DECL(TYPE)	\
	_D2D_TYPE_INFO_DECL_BASE(TYPE)	\
		TypeInfo* BaseInfo() { return 0; }	\
	_D2D_END_TYPE_INFO_DECL(TYPE)	\

#define D2D_DECL_DERIVED_TYPE_INFO(TYPE_DERIVED, TYPE_BASE)	\
	_D2D_BEGIN_TYPE_INFO_DECL(TYPE_DERIVED)	\
	_D2D_TYPE_INFO_DECL_BASE(TYPE_DERIVED)	\
		TypeInfo* BaseInfo() { return _typeInfos[#TYPE_BASE]; }	\
	_D2D_END_TYPE_INFO_DECL(TYPE_DERIVED)	\

#define D2D_INJECT_TYPE_INFO(TYPE)	\
	public:	\
	virtual TypeInfo* GetTypeInfo() const { return TypeInfo::GetTypeInfo(#TYPE); }	\


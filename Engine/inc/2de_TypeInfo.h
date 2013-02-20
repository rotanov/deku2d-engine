#ifndef _2DE_TYPEINFO_H_
#define _2DE_TYPEINFO_H_

#include <map>
#include <string>
#include "rapidjson/document.h"

#include "2de_PropertyInfo.h"

using std::map;
using std::string;

class TypeInfo
{
public:
	TypeInfo() : _hasDerived( false ) {}
	bool HasDerived() const { return _hasDerived; }
	virtual const char* Name() const = 0;
	virtual void* New() const = 0;
	virtual PropertyInfo* GetProperty(const string &name) const = 0;
	virtual map<string, PropertyInfo*>& Properties() = 0;
	virtual TypeInfo* BaseInfo() = 0;
	virtual string GetString(void *instance) = 0;
	virtual void SetString(void *instance, const string &value) = 0;
	virtual bool IsIntegral() const = 0;
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
	virtual TypeInfo* GetRunTimeTypeInfo(void* instance) const { throw "Not implemented."; }
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

	static void Initialize() // Run time initialization and checks
	{
		for (map<string, TypeInfo*>::iterator i = _typeInfos.begin(); i != _typeInfos.end(); ++i)
		{
			if (i->second->BaseInfo())
			{
				i->second->BaseInfo()->SetHasDerived( true );
			}
		}
	}



protected:
	static map<string, TypeInfo*> _typeInfos;

	void SetHasDerived( bool hasDerived ) { _hasDerived = hasDerived; }

private:
	bool _hasDerived;
};

template <typename Type, int Implemented = Deku2D::IsConvertible<string, Type>::result>
class TypeInfoStringToTypeHelper
{
public:
	static void StringToType(const string& s, Type* instance)
	{
		throw "pizdec conversion not implemented";
	}
};

template <typename Type>
class TypeInfoStringToTypeHelper<Type, 1>
{
public:
	static void StringToType( const string& s, Type* instance )
	{
		*instance = Deku2D::Convert<Type>::FromString(s);
	}
};

template <typename Type, int Implemented = Deku2D::IsConvertible<Type, string>::result>
class TypeInfoTypeToStringHelper
{
public:
	static string TypeToString(Type* instance)
	{
		throw "pizdec conversion not implemented";
	}
};

template <typename Type>
class TypeInfoTypeToStringHelper<Type, 1>
{
public:
	static string TypeToString(Type* instance)
	{
		return Deku2D::Convert<Type>::ToString(*instance);
	}
};

#define _D2D_BEGIN_TYPE_INFO_DECL(TYPE)	\
	class TypeInfo##TYPE : public TypeInfo	\
	{	\
	public:	\

#define _D2D_TYPE_INFO_DECL_BASE(TYPE)	\
		TypeInfo##TYPE() { _typeInfos[#TYPE] = &_instance; }	\
		void* New() const { return Make<TYPE>::New(); }	\
		const char* Name() const { return #TYPE; }	\
		virtual bool IsIntegral() const { return Deku2D::IsIntegral<TYPE>::result; }	\
		virtual void SetString(void *instance, const string &value)	\
		{	\
			TypeInfoStringToTypeHelper<TYPE>::StringToType(value, static_cast<TYPE*>(instance));	\
		}	\
		string GetString(void *instance)	\
		{	\
			return TypeInfoTypeToStringHelper<TYPE>::TypeToString(static_cast<TYPE*>(instance));	\
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
		virtual TypeInfo* GetRunTimeTypeInfo(void* instance) const { return static_cast<TYPE_DERIVED*>(instance)->GetTypeInfo(); }	\
	_D2D_END_TYPE_INFO_DECL(TYPE_DERIVED)	\

#define D2D_INJECT_TYPE_INFO(TYPE)	\
	public:	\
	virtual TypeInfo* GetTypeInfo() const { return TypeInfo::GetTypeInfo(#TYPE); }


#endif // _2DE_TYPEINFO_H_


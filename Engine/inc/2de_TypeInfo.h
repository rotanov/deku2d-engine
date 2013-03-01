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

	virtual const char* Name() const = 0;
	virtual void* New() const = 0;
	virtual PropertyInfo* GetProperty(const string &name) const = 0;
	virtual map<string, PropertyInfo*>& Properties() = 0;
	virtual TypeInfo* BaseInfo() = 0;
	virtual string GetString(void *instance) = 0;
	virtual void SetString(void *instance, const string &value) = 0;
	virtual bool IsIntegral() const = 0;

	bool HasDerived() const
	{
		return _hasDerived;
	}

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

	virtual TypeInfo* GetRunTimeTypeInfo(const void*) const
	{
		throw std::runtime_error("Not implemented.");
	}

	static TypeInfo* GetTypeInfo(const string &typeName)
	{
		if (_getTypeInfos().find(typeName) == _getTypeInfos().end())
		{
			char errMsg[128];
			sprintf(errMsg, "Type info for type: \"%s\" doesn't exist.", typeName.c_str());
			throw(errMsg);
		}
		else
			return _getTypeInfos()[typeName];
	}

	virtual void FillJSONValue(rapidjson::Value*, void*)
	{
		throw "FillJSONValue unimplemented for this type";
	}

	static void Initialize() // Run time initialization and checks
	{
		for (map<string, TypeInfo*>::iterator i = _getTypeInfos().begin(); i != _getTypeInfos().end(); ++i)
		{
			if (i->second->BaseInfo())
			{
				i->second->BaseInfo()->SetHasDerived( true );
			}
		}
	}

	// returns string with human-readable type summary
	static string GetTypeDescriptionString(const string& typeName, unsigned depth = 0)
	{
		if (_getTypeInfos()[typeName] == 0)
			return "Type info record for type \"" + typeName + "\" doesn't exist.\n";

		TypeInfo& typeInfo= *_getTypeInfos()[typeName];
		string identStr(depth, '\t');
		string identStr1 = identStr + "\t";
		string identStr2 = identStr + "\t\t";
		string identStr3 = identStr + "\t\t\t";
		string eolStr = "\n";
		string r;

		r = identStr + "Description of type\"" + typeName + "\":" + eolStr + eolStr +
			identStr1 + "name: " + typeInfo.Name() + eolStr + eolStr +
			identStr1 + "Properties:" + eolStr;

		for (map<string, PropertyInfo*>::iterator i = typeInfo.Properties().begin(); i != typeInfo.Properties().end(); ++i)
		{
			PropertyInfo& p = *(i->second);
			r += identStr2 + "key: \"" + (i->first) + "\"" + eolStr +
				 identStr3 + "name: \"" + p.Name() + "\"" + eolStr +
				 identStr3 + "type name: \"" + p.TypeName() + "\"" + eolStr +
				 identStr3 + "owner name: \"" + p.OwnerName() + "\"" + eolStr +
				 identStr3 + "valid flags: ";

			if (p.IsArray())
				r += "array, ";
			if (p.IsPointer())
				r += "pointer, ";
			if (p.Integral())
				r += "integral, ";
			r += eolStr;
		}

		if (typeInfo.BaseInfo() != NULL)
		{
			r += eolStr;
			string baseName = typeInfo.BaseInfo()->Name();
			r += identStr1 + "Derived from type: \"" + baseName + "\"" + eolStr;
			r += TypeInfo::GetTypeDescriptionString(baseName, depth + 1);
		}

		return r;
	}

protected:
	//static map<string, TypeInfo*> _typeInfos;
	static map<string, TypeInfo*>& _getTypeInfos();

	void SetHasDerived( bool hasDerived ) { _hasDerived = hasDerived; }

private:
	bool _hasDerived;
};

template <typename Type, int Implemented = Deku2D::IsConvertible<string, Type>::result>
class TypeInfoStringToTypeHelper
{
public:
	static void StringToType(const string&, Type*)
	{
		throw std::runtime_error("StringToType conversion not implemented.");
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
	static string TypeToString(Type*)
	{
		throw std::runtime_error("TypeToString conversion not implemented.");
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

/////////////////////////////////////////////////
#define _D2D_TYPE_INFO_DECLARATION_BEGIN(TYPE)	\
	class TypeInfo##TYPE : public TypeInfo		\
	{											\
	public:										\

/////////////////////////////////////////////////////////////////////////////////////////////////////
#define _D2D_TYPE_INFO_DECLARATION_BASE(TYPE)														\
		TypeInfo##TYPE()																			\
		{																							\
			_getTypeInfos()[#TYPE] = &_instance;													\
		}																							\
																									\
		void* New() const																			\
		{																							\
			return Deku2D::Make<TYPE>::New();														\
		}																							\
																									\
		const char* Name() const																	\
		{																							\
			return #TYPE;																			\
		}																							\
																									\
		virtual bool IsIntegral() const																\
		{																							\
			return Deku2D::IsIntegral<TYPE>::result;												\
		}																							\
																									\
		virtual void SetString(void *instance, const string &value)									\
		{																							\
			TypeInfoStringToTypeHelper<TYPE>::StringToType(value, static_cast<TYPE*>(instance));	\
		}																							\
																									\
		string GetString(void *instance)															\
		{																							\
			return TypeInfoTypeToStringHelper<TYPE>::TypeToString(static_cast<TYPE*>(instance));	\
		}																							\
																									\
		PropertyInfo* GetProperty(const string &name) const											\
		{																							\
			return _props[name];																	\
		}																							\
																									\
		map<string, PropertyInfo*>& Properties()													\
		{																							\
			return _props;																			\
		}																							\

/////////////////////////////////////////////////////
#define _D2D_TYPE_INFO_DECLARATION_END(TYPE)		\
	static TypeInfo##TYPE _instance;				\
	static map<string, PropertyInfo*> _props;		\
};													\
													\
TypeInfo##TYPE TypeInfo##TYPE::_instance;			\
map<string, PropertyInfo*> TypeInfo##TYPE::_props;	\

/////////////////////////////////////////////
#define D2D_TYPE_INFO_DECLARE(TYPE)			\
	_D2D_TYPE_INFO_DECLARATION_BEGIN(TYPE)	\
	_D2D_TYPE_INFO_DECLARATION_BASE(TYPE)	\
											\
		TypeInfo* BaseInfo()				\
		{									\
			return 0;						\
		}									\
											\
	_D2D_TYPE_INFO_DECLARATION_END(TYPE)	\

/////////////////////////////////////////////////////////////////////////////
#define D2D_TYPE_INFO_DECLARE_DERIVED(TYPE_DERIVED, TYPE_BASE)				\
	_D2D_TYPE_INFO_DECLARATION_BEGIN(TYPE_DERIVED)							\
	_D2D_TYPE_INFO_DECLARATION_BASE(TYPE_DERIVED)							\
																			\
		TypeInfo* BaseInfo()												\
		{																	\
			return _getTypeInfos()[#TYPE_BASE];								\
		}																	\
																			\
		virtual TypeInfo* GetRunTimeTypeInfo(const void* instance) const	\
		{																	\
			return static_cast<const TYPE_DERIVED*>(instance)->GetTypeInfo();		\
		}																	\
																			\
	_D2D_TYPE_INFO_DECLARATION_END(TYPE_DERIVED)							\

/////////////////////////////////////////////////////
#define D2D_TYPE_INFO_INJECT(TYPE)					\
	public:											\
		virtual TypeInfo* GetTypeInfo() const		\
		{											\
			return TypeInfo::GetTypeInfo(#TYPE);	\
		}											\

#endif // _2DE_TYPEINFO_H_


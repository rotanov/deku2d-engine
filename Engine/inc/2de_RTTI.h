#ifndef _2DE_RTTI_H_
#define _2DE_RTTI_H_

#include <typeinfo>
#include <string>

class CObject;

// I hope we won't ever use this sand-witch approach.
// Hope clReflect soon will be usable

class CRTTI
{
public:
	CRTTI(const std::string &AClassName, CRTTI *ABaseClassRTTI);

private:
	std::string ClassName;
	CRTTI *BaseRTTI;
};

class CNullRTTIClass
{
public:
	static inline CRTTI* GetClassRTTI() { return &RTTI; }
	virtual CRTTI* GetRTTI() { return &RTTI; }

protected:
	static CRTTI RTTI;

};

template <typename Heir, typename BaseClass>
class CRTTIClass: public BaseClass
{
public:
	CRTTIClass();
	typedef BaseClass Base;

	static Heir* Create();
	static void RegisterReflection();
	static inline CRTTI* GetClassRTTI() { return &RTTI; }
	virtual CRTTI* GetRTTI() { return &RTTI; }

protected:
	static CRTTI RTTI;

};

template<typename Heir, typename BaseClass>
CRTTI CRTTIClass<Heir, BaseClass>::RTTI( typeid(Heir).name, BaseClass::GetClassRTTI());

template<typename Heir, typename BaseClass>
inline CRTTIClass<Heir, BaseClass>::CRTTIClass()
{

}

template<typename Heir, typename BaseClass>
Heir* CRTTIClass<Heir, BaseClass>::Create()
{
	return new Heir();
}

template<typename Heir, typename BaseClass>
void CRTTIClass<Heir, BaseClass>::RegisterReflection()
{
	
}

namespace EPropertyType
{
	enum E
	{
		UNKNOWN,
		INT,
		UINT,
		FLOAT,
		DOUBLE,
		STRING,
		CHAR,
		PTR,
		LAST,
	};
}

template < typename T >
class CPropertyType
{
public:
	static EPropertyType::E Type;
};

template<typename T> EPropertyType::E CPropertyType<T>::Type = EPropertyType::UNKNOWN;
template<> EPropertyType::E CPropertyType<unsigned int>::Type = EPropertyType::UINT;
template<> EPropertyType::E CPropertyType<int>::Type = EPropertyType::FLOAT;

class CAbstractProperty
{
public:
	virtual EPropertyType::E GetType() const = 0;

protected:
	const char *name;
};

template<typename T>
class CTypedProperty : public CAbstractProperty
{
public:
	virtual EPropertyType::E GetType() const;
	virtual T GetValue( CObject* pObject ) = 0;
	virtual void SetValue( CObject* pObject, T value ) = 0;
};

template<typename OwnerType, typename T>
class CProperty : public CTypedProperty<T>
{
public:
	typedef T (OwnerType::*GetterType)();
	typedef void (OwnerType::*SetterType)( T Value );
	virtual T GetValue( CObject * pObject);
	virtual void SetValue( CObject* pObject, T Value);
protected:
	GetterType Getter;
	SetterType Setter;
};

// RegisterProperty<uint>( "Mah property" ) for example

#endif	// _2DE_RTTI_H_

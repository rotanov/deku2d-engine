#ifndef _2DE_PROPERTY_H_
#define _2DE_PROPERTY_H_

//#include "2de_Core.h"
#include "2de_Serialization.h"
#include <string>
#include <vector>

namespace Deku2D
{
	using std::string;
	using std::vector;
	class CNullClass;

	template <typename T>
	bool IsRTTIzed()
	{
		return false;
	}

	class CProperty
	{
	public:
		CProperty(const string& aName) : name(aName){}
		const string& GetName() const;
		void SetName(const string& aName);
		virtual string GetStringValue(const CNullClass* owner) const = 0;
		//virtual void Serialize(ostream &out, CStreamSerializer* serializer, CNullClass* owner) = 0;
		//virtual void Deserialize(istream &out, CStreamSerializer* serializer, CNullClass* owner) = 0;
		virtual bool IfRTTIzed() const = 0;
		virtual const void* GetRaw(const CNullClass* owner) const = 0;

	protected:
		string name;
	};

	typedef vector<CProperty*> TPropertyVector;

	template<typename T>
	class CTypedProperty : public CProperty
	{
	public:
		CTypedProperty(const string& aName) : CProperty(aName){}
		//virtual void Serialize(ostream &out, CStreamSerializer* serializer, CNullClass* owner) = 0;
		//virtual void Deserialize(istream &out, CStreamSerializer* serializer, CNullClass* owner) = 0;
		virtual const void* GetRaw(const CNullClass* owner) const = 0;
		virtual bool IfRTTIzed() const = 0;
		virtual string GetStringValue(const CNullClass* owner) const = 0;
	};

	template<typename T, typename TOwner>
	class COwnedProperty : public CTypedProperty<T>
	{
	public:
		typedef const T& (TOwner::*TGetter)()const;
		typedef void (TOwner::*TSetter)(const T& aValue);
		typedef T Type;
		typedef TOwner TOwner;
		COwnedProperty(const string& aName, TGetter aGetter, TSetter aSetter) : CTypedProperty<T>(aName), getter(aGetter), setter(aSetter)
		{
		
		}
		/*
		virtual void Serialize(ostream &out, CStreamSerializer* serializer,  CNullClass* owner)
		{
			serializer;	// eliminates warning for now
			serializer->Serialize<T>(out, name, GetValue( owner ));
		}
		virtual void Deserialize(istream &out, CStreamSerializer* serializer,  CNullClass* owner)
		{
			serializer;	// eliminates warning for now
			T value = GetValue( owner );
			serializer->Deserialize<T>(out, name, value);
			SetValue( owner, value );
		}
		*/
		void SetValue(CNullClass* owner, const T& value)
		{
			(static_cast<TOwner*>(owner)->*setter)(value);
		}
		const T& GetValue(const CNullClass* owner) const
		{
			return (static_cast<const TOwner*>(owner)->*getter)();
		}
		virtual string GetStringValue(const CNullClass* owner) const
		{
			return Deku2D::to_string(GetValue(owner));
		}
		virtual bool IfRTTIzed() const
		{
			return IsRTTIzed<Type>();
		}
		virtual const void* GetRaw(const CNullClass* owner) const
		{
			return reinterpret_cast<const void*>(&GetValue(owner));
		}
	private:
		TGetter	getter;
		TSetter	setter;
	};

	#define DECLARE_TRIVIAL_PROPERTY(TYPE, NAME)	\
		private:	\
			TYPE NAME;	\
		public:	\
			const TYPE& Get##NAME() const { return NAME; }	\
			void Set##NAME(const TYPE& a##NAME) { NAME = a##NAME; }

	#define REGISTER_PROPERTY(TYPE, NAME, CLASS)	\
		CLASS::RegisterProperty<TYPE>(#NAME, &CLASS::Get##NAME, &CLASS::Set##NAME);

};	// namespace Deku2D

#endif //	_2DE_PROPERTY_H_
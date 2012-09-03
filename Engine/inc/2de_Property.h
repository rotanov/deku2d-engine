#ifndef _2DE_PROPERTY_H_
#define _2DE_PROPERTY_H_

#include "2de_Serialization.h"
#include <string>
#include <vector>

using namespace std;

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

	template <typename T>
	bool IsCollection()
	{
		return false;
	}

	template <typename T>
	bool IsCompound()
	{
		return false;
	}

	template <typename T>
	bool IsSimple()
	{
		return false;
	}

	class CProperty
	{
	public:
		CProperty(const string& aName) : name(aName){}

		const string& GetName() const;
		void SetName(const string& aName);

		virtual const string& GetTypeName() const = 0;
		virtual void SetTypeName(const string& aName) = 0;

		virtual unsigned GetSize() const = 0;
		virtual void SetSize(unsigned aSize) = 0;

		virtual string GetStringValue(const CNullClass* owner) const = 0;
		virtual const void* GetRaw(const CNullClass* owner) const = 0;
		virtual bool IfRTTIzed() const = 0;

	protected:
		string name;
	};

	typedef vector<CProperty*> TPropertyVector;

	template<typename T>
	class CTypedProperty : public CProperty
	{
	public:
		CTypedProperty(const string& aName, const string& aTypeName) : CProperty(aName), typeName(aTypeName), size(sizeof(T)){}

		virtual const string& GetTypeName() const { return typeName; }
		virtual void SetTypeName(const string& aName) { throw "pizdec nelzya"; }

		virtual unsigned GetSize() const { return size; }
		virtual void SetSize(unsigned aSize) { throw "pizec net"; }

		virtual string GetStringValue(const CNullClass* owner) const = 0;
		virtual const void* GetRaw(const CNullClass* owner) const = 0;
		virtual bool IfRTTIzed() const = 0;

		virtual void SetValue(CNullClass* owner, const T& value) = 0;
		virtual const T GetValue(const CNullClass* owner) const = 0;

	private:
		string typeName;
		unsigned size;
	};

	/*
	template<>
	class CTypedProperty<string> : public CProperty
	{
	public:
		CTypedProperty(const string& aName, const string& aTypeName) : CProperty(aName), typeName(aTypeName), size(sizeof(string)){}

		virtual const string& GetTypeName() const { return typeName; }
		virtual void SetTypeName(const string& aName) { throw "pizdec nelzya"; }

		virtual unsigned GetSize() const { return size; }
		virtual void SetSize(unsigned aSize) { throw "pizec net"; }

		virtual string GetStringValue(const CNullClass* owner) const = 0;
		virtual const void* GetRaw(const CNullClass* owner)
		{
			return GetValue(owner).c_str();
		}
		virtual bool IfRTTIzed() const = 0;

		virtual void SetValue(CNullClass* owner, const string& value) = 0;
		virtual const string GetValue(const CNullClass* owner) const = 0;

	private:
		string typeName;
		unsigned size;
	};
	*/

	template<typename T, typename Owner>
	class COwnedProperty : public CTypedProperty<T>
	{
	public:
		typedef T (Owner::*TGetter)()const;
		typedef void (Owner::*TSetter)(const T& aValue);
		typedef T Type;
		typedef Owner TOwner;

		COwnedProperty(const string& aName, const string& aTypeName, TGetter aGetter, TSetter aSetter) : CTypedProperty<T>(aName, aTypeName), getter(aGetter), setter(aSetter) { }

		void SetValue(CNullClass* owner, const T& value)
		{
			(static_cast<TOwner*>(owner)->*setter)(value);
		}

		const T GetValue(const CNullClass* owner) const
		{
			return (static_cast<const TOwner*>(owner)->*getter)();
		}

		virtual string GetStringValue(const CNullClass* owner) const
		{
			const T value = GetValue(owner);
			return to_string(value);
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

	#define D2D_PROPERTY(TYPE, NAME)	\
		private:	\
			TYPE NAME;	\
		public:	\
			const TYPE& Get##NAME() const { return NAME; }	\
			void Set##NAME(const TYPE& a##NAME) { NAME = a##NAME; }

	#define D2D_REGISTER_PROPERTY(TYPE, NAME, CLASS)	\
		CLASS::RegisterProperty<TYPE>(#NAME, #TYPE, &CLASS::Get##NAME, &CLASS::Set##NAME);

};	// namespace Deku2D

#endif //	_2DE_PROPERTY_H_

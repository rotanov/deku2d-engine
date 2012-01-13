#ifndef _2DE_RTTI_H_
#define _2DE_RTTI_H_

#define _SECURE_SCL 0
#include <typeinfo>
#include <string>
#include <vector>
#include <map>
#include <cassert>
#include "2de_Property.h"

namespace Deku2D
{
	using std::string;
	using std::map;
	using std::vector;

	class CRTTI
	{
	public:

		class PropertyIterator
		{
		public:
			PropertyIterator(CRTTI* aRTTI) : idx(-1), RTTI(aRTTI)
			{
				assert(aRTTI != NULL);
				(*this)++;
			}

			CProperty* operator ->() const
			{
				assert(Ok());
				return RTTI->GetProperties()[idx];
			}

			CProperty& operator *() const
			{
				assert(Ok());
				return *(RTTI->GetProperties()[idx]);
			}

			PropertyIterator& operator ++();

			bool Ok() const { return idx != -1; }

		private:
			const CRTTI* RTTI;
			int idx;
		};

		typedef vector<const CRTTI*> TRTTIVector;
		typedef map<string, CRTTI*> TRTTIMap;

		CRTTI(const string aName);

		bool IsKindOf(const CRTTI* RTTI) const;
		void RegisterProperty(CProperty* aProperty);
		const string& GetName() const;
		const TPropertyVector& GetProperties() const;
		virtual CNullClass* MakeNew() const = 0;
		static TRTTIVector& GetClasses();
		static void FinalizeDeclaration();
		static CRTTI* GetRTTIByName(const std::string& aName);
		virtual const CRTTI* GetBaseClassInfo() const = 0;
		virtual void InvokePropertyRegistration() const = 0;

	private:
		string name;
		unsigned id;
		TPropertyVector properties;
		static bool* isFirstKindOfSecond;
		TRTTIVector derived;
		static unsigned count;

		bool _IsKindOf(const CRTTI* RTTI) const
		{
			const CRTTI* baseRTTI = GetBaseClassInfo();
			if (RTTI == this)
				return true;
			if (this == baseRTTI)
				return false;
			if (RTTI == baseRTTI)
				return true;
			return baseRTTI->_IsKindOf(RTTI);		
		}
		static TRTTIMap& GetClassesMap();
		CRTTI();
		CRTTI(const CRTTI& aRTTI);
		CRTTI& operator =(const CRTTI& rhs);
	};

	template<typename Heir, typename Base>
	class CTypedRTTI : public CRTTI
	{
	public:
		typedef Heir THeir;
		typedef Base TBase;
		typedef CTypedRTTI<THeir, TBase> TClassInfo;
		typedef CTypedRTTI<TBase, typename TBase::TBase> TBaseClassInfo;

		CTypedRTTI(const string aName) : CRTTI(aName), baseRTTI(TBase::GetRTTIStatic()) {}
		virtual const CRTTI* GetBaseClassInfo() const { return static_cast<CRTTI*>(baseRTTI); }
		virtual void InvokePropertyRegistration() const
		{
			Heir::RegisterReflection();
		}
	private:
		TBaseClassInfo *baseRTTI;
		THeir* MakeNew() const	{ return new THeir; }
		virtual const CRTTI* GetBase() const	{return static_cast<CRTTI*>(baseRTTI);}
	};

	//	Note: classInfo is not private because we want access in form TYPE::classInfo in some cases.
	//	However one should use GetRTTI with pointers to class instances.
	//	TODO: needed solution to hide classInfo from access through pointer to class instance.
	//	static TClassInfo* GetRTTI() { return &classInfo; }
	#define D2D_DECLARE_RTTI(TYPE_HEIR, TYPE_BASE)	\
		public:	\
			typedef TYPE_BASE TBase;	\
			typedef TYPE_HEIR THeir;	\
			typedef CTypedRTTI<THeir, TBase> TClassInfo;	\
			static TClassInfo classInfo##TYPE_HEIR;	\
			virtual CRTTI* GetRTTI() const { return &classInfo##TYPE_HEIR; }	\
			template<typename T>	\
			static void RegisterProperty(	const string &aName,	\
											typename COwnedProperty<T, TYPE_HEIR>::TGetter aGetter,	\
											typename COwnedProperty<T, TYPE_HEIR>::TSetter aSetter	\
										)	\
			{	\
				COwnedProperty<T, TYPE_HEIR>* property = new COwnedProperty<T, TYPE_HEIR>(aName, aGetter, aSetter);	\
				classInfo##TYPE_HEIR.RegisterProperty(property);	\
			}	\
			static TClassInfo* GetRTTIStatic() { return &classInfo##TYPE_HEIR; }	\
			static TYPE_HEIR* MakeInstance()	\
			{	\
				return new TYPE_HEIR();	\
			}

	#define D2D_DEFINE_RTTI(TYPE_HEIR, TYPE_BASE)	\
		TYPE_HEIR::TClassInfo TYPE_HEIR::classInfo##TYPE_HEIR(#TYPE_HEIR);	\
		template <>	\
		bool IsRTTIzed<TYPE_HEIR>()	\
		{	\
			return true;	\
		}

	class CNullClass
	{
	public:
		CNullClass(){}
		typedef CNullClass TBase;
		typedef CNullClass THeir;
		typedef CTypedRTTI<CNullClass, CNullClass> TClassInfo;
		static TClassInfo classInfoCNullClass;
		static void RegisterReflection(){}
		static TClassInfo*	GetRTTIStatic() { return &classInfoCNullClass; }
		virtual CRTTI* GetRTTI() const { return &classInfoCNullClass; }
	};

}	//	namespace Deku2D

#endif	// _2DE_RTTI_H_

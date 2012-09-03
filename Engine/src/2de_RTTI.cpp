#include "2de_RTTI.h"

#include <algorithm>

namespace Deku2D
{

	bool* CRTTI::isFirstKindOfSecond = NULL;
	unsigned CRTTI::count = 0;
	bool CRTTI::finalized = false;

	CNullClass::TClassInfo CNullClass::classInfoCNullClass("CNullClass");

	CRTTI::CRTTI(const string aName) : name(aName)
	{
		// MurmurHash3_x86_32(name.c_str(), name.size(), 0, &id);	//	WHY? (нахуя?)
		id = GetClasses().size();
		if (GetClassesMap()[aName])
		{
			throw("pizdec");
		}
		GetClassesMap()[aName] = this;
		if (find(GetClasses().begin(), GetClasses().end(), this) == GetClasses().end())
			GetClasses().push_back(this);
		else
			throw("pizdec");
	}

	bool CRTTI::IsKindOf(const CRTTI* RTTI) const
	{
		return isFirstKindOfSecond[id * count + RTTI->id];
	}

	void CRTTI::FinalizeDeclaration()	// Someone should invoke this surely once and after all.
	{
		assert(!finalized);
		finalized = true;
		count = GetClasses().size();
		isFirstKindOfSecond = new bool [count*count];
		for (unsigned i = 0; i < count; i++)
		{
		//	GetClasses()[i]->GetBaseClassInfo()->derived.push_back(GetClasses()[i]);	// Here, because order of construction is undefined.
			// But putting it here makes me use not const reference to base class RTTI which is sad.
			//	Surely we could detect if order of construction call is incorrect at runtime. But correcting it may result
			//	in shitload of pain if many classes total. 
			for( unsigned j = 0; j < count; j++)
				isFirstKindOfSecond[i * count + j] = GetClasses()[i]->_IsKindOf(GetClasses()[j]);
		}

		for (unsigned i = 0; i < count; i++)
		{
			GetClasses()[i]->InvokePropertyRegistration();
		}
		finalized = true;
	}

	string CRTTI::DumpRTTIDebugInfo()
	{
		assert(finalized);
		string result;
		for (unsigned i = 0; i < count; i++)
		{
			result += GetClasses()[i]->GetName();
			result += "\r\n";

			for (CRTTI::PropertyIterator j(GetClasses()[i]); j.Ok(); ++j)
			{
				result += "\t";
				result += j->GetTypeName();
				result += " : ";
				result += j->GetName();				
				result += "\r\n";
			}
		}
		return result;
	}

	void CRTTI::RegisterProperty(CProperty* aProperty)
	{
		properties.push_back(aProperty);
	}

	const string& CRTTI::GetName() const
	{
		return name;
	}

	const TPropertyVector& CRTTI::GetProperties() const
	{
		return properties;
	}

	CRTTI::TRTTIVector& CRTTI::GetClasses()
	{
		static TRTTIVector classes;
		return classes;
	}

	CRTTI::TRTTIMap& CRTTI:: GetClassesMap()
	{
		static TRTTIMap classesMap;
		return classesMap;
	}

	CRTTI* CRTTI::GetRTTIByName( const std::string& aName )
	{
		return GetClassesMap()[aName];
	}

	CRTTI::PropertyIterator& CRTTI::PropertyIterator::operator ++()
	{
		int size = (RTTI->GetProperties().size() - 1);
		if (idx < size)
		{
			idx++;
			return *this;
		}
		idx = 0;
		do 
			RTTI = RTTI->GetBaseClassInfo();
		while (RTTI->GetProperties().size() == 0 && RTTI != CNullClass::GetRTTIStatic());
		if (RTTI->GetProperties().size() > 0)
			return *this;
		idx = -1;
		return *this;
	}

}	//	namespace Deku2d

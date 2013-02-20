#ifndef _2DE_COMMON_MANAGER_H_
#define _2DE_COMMON_MANAGER_H_

#include "2de_Core.h"
#include "2de_Object.h"

namespace Deku2D
{

	// Template class for some manager
	template <typename C>	// C - container type
	class CCommonManager : public virtual CObject // is it necessary?
	{
	public:
		typedef C ManagerContainer;
		typedef typename C::value_type T;
		ManagerContainer Objects;

	public:		
		typedef typename ManagerContainer::iterator ManagerIterator;
		typedef typename ManagerContainer::const_iterator ManagerConstIterator;

		T Get(const string &AName)	// I think, we're better to avoid search by object's name. Search by ID or address instead.
		{					// 	hashmap, anyone?...
			T result = NULL;

			for (ManagerIterator i = Objects.begin(); i != Objects.end(); ++i)
			{
				if ((*i)->isDestroyed())
				{
					continue;
				}
				if ((*i)->GetName() == AName)
				{
					result = *i;
					break;
				}
			}

			return result;
		}

		virtual void Add(const T &AObject)
		{
			Objects.push_back(AObject);
		}

		void Remove(const T &AObject)
		{
			for (ManagerIterator i = Objects.begin(); i != Objects.end(); ++i)
			{
				if (*i == AObject)
				{
					Objects.erase(i);
					return;
				}
			}
		}

		void Remove(size_t AID)
		{
			T temp = NULL;
			for (ManagerIterator i = Objects.begin(); i != Objects.end(); ++i)
				if ((*i)->GetID() == AID)			
				{
					temp = *i;
					break;
				}
			if (temp == NULL)
				return;
			Objects.remove(temp);
		}

		void Remove(const string &AName)
		{
			T temp = NULL;
			for (ManagerIterator i = Objects.begin(); i != Objects.end(); ++i)
				if ((*i)->GetName() == AName)
				{
					temp = *i;
					break;
				}
			if (temp == NULL)
				return;
			Objects.remove(temp);
		}

		CCommonManager()
		{
			ClassName = "CommonManager";
		}

		virtual ~CCommonManager()
		{
			Objects.clear();
		}
	};

}


#endif // _2DE_COMMON_MANAGER_H_

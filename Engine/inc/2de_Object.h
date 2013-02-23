#ifndef _2DE_OBJECT_H_
#define _2DE_OBJECT_H_

#include "2de_RTTI.h"
#include "2de_TypeInfo.h"

namespace Deku2D
{

	class CEvent;

	/**
	* CObject - base class for many classes.
	*/

	class CObject : public CNullClass
	{
		D2D_TYPE_INFO_INJECT(CObject)

	public:		
		CObject();
		virtual ~CObject();

		string GetName() const;
		virtual void SetName(const string &AObjectName);

		string GetStandardName() const;

		unsigned GetID() const;

		bool isDestroyed() const;
		void SetDestroyed();

		bool isManaged() const;

		virtual void ProcessEvent(const CEvent &AEvent);

	protected:
		CObject(const CObject &AObject);
		CObject& operator=(const CObject &AObject);

		bool Managed;
		bool Destroyed;
		unsigned ID;
		string ClassName;

	private:
		string Name;

		static unsigned CObjectCount;

		friend class CFactory;
	};


}	// namespace Deku2D

#endif // _2DE_OBJECT_H_

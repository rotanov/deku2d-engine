#include "2de_Object.h"
#include "2de_Engine.h"

namespace Deku2D
{
	//////////////////////////////////////////////////////////////////////////
	// CObject	
	unsigned CObject::CObjectCount = 0;

	CObject::CObject() : Managed(false), Destroyed(false), ID(++CObjectCount), Name("CObject" + itos(ID))
	{
		ClassName = "Object";
	}

	CObject::CObject(const CObject &AObject) : Managed(false), Destroyed(false), ID(++CObjectCount), Name("CObject" + itos(ID))
	{
		AObject;	// warning eliminator
					// also, why no info is taken from AObject? Looks suspicious.
		ClassName = "Object";
	}

	CObject& CObject::operator=(const CObject &AObject)
	{
		throw std::logic_error("Forbidden");

		if (this == &AObject)
			return *this;

		Destroyed = AObject.Destroyed;
		ID = ++CObjectCount;
		Name = AObject.GetName() + " copy";

		return *this;
	}

	CObject::~CObject()
	{
		if (CEngine::Instance()->isRunning())
			EventManager->UnsubscribeFromAll(this);
	}

	string CObject::GetName() const
	{
		return Name;
	}

	void CObject::SetName(const string &AObjectName)
	{
		if (Managed)
			Factory->Rename(Name, AObjectName);
		else
			Name = AObjectName;
	}

	string CObject::GetStandardName() const
	{
		return ClassName + itos(ID);
	}

	unsigned CObject::GetID() const
	{
		return ID;
	}

	/**
	* CObject::SetDestroyed - marks non-managed objects as destroyed, calls CFactory::Destroy for managed ones. Does nothing for objects already marked as destroyed.
	*/

	void CObject::SetDestroyed()
	{
		if (Destroyed)
			return;

		if (Managed)
			Factory->Destroy(this);
		else
			Destroyed = true;
	}

	bool CObject::isDestroyed() const
	{
		return Destroyed;
	}

	bool CObject::isManaged() const
	{
		return Managed;
	}

	void CObject::ProcessEvent(const CEvent &AEvent)
	{
		Log("INFO", "Unhandled event '%s' in object '%s'", AEvent.GetName().c_str(), Name.c_str());
	}

};	// namespace Deku2D

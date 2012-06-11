#include "2de_Event.h"

#include "2de_Engine.h"

namespace Deku2D
{
	//////////////////////////////////////////////////////////////////////////
	// CEvent

	CEvent::CEvent() : Sender(NULL)
	{
	}

	CEvent::CEvent(const string &AName, CObject *ASender) : Name(AName), Sender(ASender)
	{
	}

	bool CEvent::IsDataExists(const string &AName) const
	{
		return Data.count(AName);
	}

	CEvent::DataIterator CEvent::Begin() const
	{
		return Data.begin();
	}

	CEvent::DataIterator CEvent::End() const
	{
		return Data.end();
	}

	string CEvent::GetName() const
	{
		return Name;
	}

	void CEvent::SetName(const string &AName)
	{
		Name = AName;
	}

	CObject* CEvent::GetSender() const
	{
		return Sender;
	}

	void CEvent::SetSender(CObject *ASender)
	{
		Sender = ASender;
	}

	//////////////////////////////////////////////////////////////////////////
	// CEventManager

	CEventManager::CEventManager()
	{
		SetName("EventManager");
	}

	CEventManager::~CEventManager()
	{
	}

	void CEventManager::Subscribe(const string &AEventName, CObject *Subscriber)
	{
		if (IsSubscribed(AEventName, Subscriber))
			return;

		Subscribers.insert(pair<string, CObject *>(AEventName, Subscriber));
	}

	void CEventManager::Unsubscribe(const string &AEventName, CObject *Subscriber)
	{
		for (SubscribersContainer::iterator it = Subscribers.lower_bound(AEventName); it != Subscribers.upper_bound(AEventName); ++it)
		{
			if (it->second == Subscriber)
			{
				Subscribers.erase(it);
				return;
			}
		}
	}

	void CEventManager::UnsubscribeFromAll(CObject *Subscriber)
	{
		vector<SubscribersContainer::iterator> ToDelete;
		for (SubscribersContainer::iterator it = Subscribers.begin(); it != Subscribers.end(); ++it)
		{
			if (it->second == Subscriber)
				ToDelete.push_back(it);
		}
		for(vector<SubscribersContainer::iterator>::iterator i = ToDelete.begin(); i != ToDelete.end(); ++i)
			Subscribers.erase(*i);
	}

	bool CEventManager::IsSubscribed(const string &AEventName, CObject *Subscriber) const
	{
		for (SubscribersContainer::const_iterator it = Subscribers.lower_bound(AEventName); it != Subscribers.upper_bound(AEventName); ++it)
		{
			if (it->second == Subscriber)
			{
				return true;
			}
		}

		return false;
	}

	/**
	* CEventManager::TriggerEvent - sends a CEvent instance with the specified AEventName and ASender to all objects that are subscribed to this event.
	*/

	void CEventManager::TriggerEvent(const string &AEventName, CObject *ASender)
	{
		CEvent e(AEventName, ASender);
		TriggerEvent(e);
	}

	/**
	* CEventManager::TriggerEvent - sends AEvent to all objects that are subscribed to this event.
	*/

	void CEventManager::TriggerEvent(CEvent &AEvent)
	{
		if (!CEngine::Instance()->isRunning())
			return;

		string name = AEvent.GetName();

		for (SubscribersContainer::iterator it = Subscribers.lower_bound(name); it != Subscribers.upper_bound(name); ++it)
			it->second->ProcessEvent(AEvent);
	}

}	//	namespace Deku2D

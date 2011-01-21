#include "2de_Event.h"

//////////////////////////////////////////////////////////////////////////
// CEventManager

CEventManager::CEventManager()
{
	SetName("CEventManager");
}

CEventManager::~CEventManager()
{
}

void CEventManager::Subscribe(const string &AEventName, CObject *Subscriber)
{
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
	for (SubscribersContainer::iterator it = Subscribers.begin(); it != Subscribers.end(); ++it)
	{
		if (it->second == Subscriber)
			Subscribers.erase(it);
	}
}

void CEventManager::TriggerEvent(const string &AEventName, CObject *ASender)
{
	TriggerEvent(new CEvent(AEventName, ASender));	// Why the fuck the "new" is here and "delete" is below
}

void CEventManager::TriggerEvent(CEvent *AEvent)
{
	for (SubscribersContainer::iterator it = Subscribers.lower_bound(AEvent->GetName()); it != Subscribers.upper_bound(AEvent->GetName()); ++it)
	{
		it->second->ProcessEvent(*AEvent); 
	}

	delete AEvent;	// I mean why delete is here? Dis is bad bad bad bad bad i think. What sort of convention is it?
					// Such convention if fine if it lies within a private methods of class, but these are public.
}

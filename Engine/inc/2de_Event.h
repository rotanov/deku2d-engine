#ifndef _2DE_EVENT_H_
#define _2DE_EVENT_H_

#include "2de_Core.h"

class CEventManager : public CTSingleton<CEventManager>
{
public:
	~CEventManager();

	void Subscribe(const string &AEventName, CObject *Subscriber);
	void Unsubscribe(const string &AEventName, CObject *Subscriber);

	void TriggerEvent(const string &AEventName, CObject *ASender);
	void TriggerEvent(CEvent *AEvent);
	
	bool ProcessEvents();

private:
	typedef queue<CEvent *> LoopContainer;
	typedef multimap<string, CObject *> SubscribersContainer;

	LoopContainer Loop;
	SubscribersContainer Subscribers;

	friend class CTSingleton<CEventManager>;
};

#endif // _2DE_EVENT_H_

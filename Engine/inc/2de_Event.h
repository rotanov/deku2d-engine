#ifndef _2DE_EVENT_H_
#define _2DE_EVENT_H_

#include "2de_Core.h"

class CEventManager : public CTSingleton<CEventManager>
{
public:
	void Subscribe(const string &AEventName, CObject *Subscriber);
	void Unsubscribe(const string &AEventName, CObject *Subscriber);
	void UnsubscribeFromAll(CObject *Subscriber);

	void TriggerEvent(const string &AEventName, CObject *ASender);
	void TriggerEvent(CEvent *AEvent);
	
protected:
	CEventManager();
	~CEventManager();
private:
	typedef multimap<string, CObject *> SubscribersContainer;

	SubscribersContainer Subscribers;

	friend class CTSingleton<CEventManager>;
};

#endif // _2DE_EVENT_H_

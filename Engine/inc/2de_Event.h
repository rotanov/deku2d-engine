#ifndef _2DE_EVENT_H_
#define _2DE_EVENT_H_

#include "2de_Core.h"

namespace Deku2d
{
	class CEventManager : public CTSingleton<CEventManager>
	{
	public:
		void Subscribe(const string &AEventName, CObject *Subscriber);
		void Unsubscribe(const string &AEventName, CObject *Subscriber);
		void UnsubscribeFromAll(CObject *Subscriber);

		bool IsSubscribed(const string &AEventName, CObject *Subscriber) const;

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
}	//	namespace Deku2d

#endif // _2DE_EVENT_H_

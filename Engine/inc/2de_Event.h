#ifndef _2DE_EVENT_H_
#define _2DE_EVENT_H_

#include "2de_Core.h"
#include "2de_Log.h"

namespace Deku2d
{
	class CEvent
	{
	public:
		typedef map<string, string> DataContainer;
		typedef DataContainer::const_iterator DataIterator;

		CEvent();
		CEvent(const string &AName, CObject *ASender);

		template<typename T>
		T GetData(const string &AName) const;

		template<typename T>
		void SetData(const string &AName, const T &AValue);

		bool IsDataExists(const string &AName) const;

		DataIterator Begin() const;
		DataIterator End() const;

		string GetName() const;
		void SetName(const string &AName);

		CObject* GetSender() const;
		void SetSender(CObject *ASender);

	private:
		string Name;
		CObject *Sender;
		map<string, string> Data;
	};


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


	template<typename T>
	T CEvent::GetData(const string &AName) const
	{
		map<string, string>::const_iterator it = Data.find(AName);
		if (it == Data.end())
		{
			Log("ERROR", "Event '%s' doesn't have data field named '%s'", Name.c_str(), AName.c_str());
			return from_string<T>("");
		}

		return from_string<T>(it->second);
	}

	template<typename T>
	void CEvent::SetData(const string &AName, const T &AValue)
	{
		Data[AName] = to_string(AValue);
	}

}	//	namespace Deku2d

#endif // _2DE_EVENT_H_

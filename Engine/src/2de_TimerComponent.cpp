#include "2de_TimerComponent.h"

#include "2de_Engine.h"

namespace Deku2D
{
	//////////////////////////////////////////////////////////////////////////
	// CTimerComponent

	CTimerComponent::CTimerComponent() : Accumulated(0.0f)
	{
		ClassName = "TimerComponent";
		SetEnabled(false);
	}

	CTimerComponent* CTimerComponent::Clone(const string &ACloneName /*= ""*/) const
	{
		return CloneHelper(new CTimerComponent(*this), ACloneName);
	}

	void CTimerComponent::ProcessEvent(const CEvent &AEvent)
	{
		if (AEvent.GetName() == "EveryFrame")
		{
			Accumulated += CEngine::Instance()->GetDeltaTime();
			if (Accumulated >= Interval)
			{
				CEvent *TimerTickEvent = new CEvent("TimerTick", this);
				TimerTickEvent->SetData("Name", GetName());
				EventManager->TriggerEvent(TimerTickEvent);
				Accumulated = 0.0f;
			}
		}
	}

	void CTimerComponent::SetEnabled(bool AEnabled)
	{
		CGameObject::SetEnabled(AEnabled);

		if (Created)
		{
			if (IsEnabled())
				EventManager->Subscribe("EveryFrame", this);
			else
				EventManager->Unsubscribe("EveryFrame", this);
		}

		Accumulated = 0.0f;
	}

	float CTimerComponent::GetInterval() const
	{
		return Interval;
	}

	void CTimerComponent::SetInterval(float AInterval)
	{
		Interval = AInterval;
		Accumulated = 0.0f;	// is it needed?.. or make separate Reset(), SetElapsed() or whatever?..
	}

	void CTimerComponent::Deserialize(CXMLNode *AXML)
	{
		CGameObject::Deserialize(AXML);

		if (AXML->HasAttribute("Interval"))
		{
			SetInterval(from_string<float>(AXML->GetAttribute("Interval")));
		}
	}

	void CTimerComponent::FinalizeCreation()
	{
		CGameObject::FinalizeCreation();

		if (IsEnabled())
			EventManager->Subscribe("EveryFrame", this);
	}

}	// namespace Deku2D

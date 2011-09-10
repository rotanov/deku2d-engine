#ifndef _2DE_TIMER_COMPONENT_H_
#define _2DE_TIMER_COMPONENT_H_

#include "2de_GameObject.h"

namespace Deku2D
{
	/**
	* CTimerComponent - component that represents a timer.
	*/

	class CTimerComponent : public CGameObject
	{
	public:
		D2D_DECLARE_VISITABLE()

		CTimerComponent(float AInterval = 0.0f);

		CTimerComponent* Clone(const string &ACloneName = "") const;

		void ProcessEvent(const CEvent &AEvent);

		bool isEnabled() const;
		void SetEnabled(bool AEnabled);

		float GetInterval() const;
		void SetInterval(float AInterval);

		void Deserialize(CXMLNode *AXML);

	private:
		bool Enabled;
		float Interval;
		float Accumulated;

	};

}	//	namespace Deku2D

#endif	//	_2DE_TIMER_COMPONENT_H_

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

		CTimerComponent();

		CTimerComponent* Clone(const string &ACloneName = "") const;

		void ProcessEvent(const CEvent &AEvent);

		bool isEnabled() const;
		void SetEnabled(bool AEnabled);

		float GetInterval() const;
		void SetInterval(float AInterval);

		void Deserialize(CXMLNode *AXML);
		void FinalizeCreation();

	private:
		float Interval;
		float Accumulated;

	};

}	//	namespace Deku2D

#endif	//	_2DE_TIMER_COMPONENT_H_

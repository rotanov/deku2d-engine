#include "2de_Property.h"

namespace Deku2D
{

	const string& CProperty::GetName() const
	{
		return name;
	}
	void CProperty::SetName(const string& aName)
	{
		name = aName;
	}

};	// namespace Deku2D

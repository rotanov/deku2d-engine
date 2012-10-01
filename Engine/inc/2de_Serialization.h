#ifndef _2DE_SERIALIZATION_H_
#define _2DE_SERIALIZATION_H_

#include <string>

namespace Deku2D
{
	class CObject;
	void SerializeObjectToJSON(CObject* o, const std::string& name);

}; // namespace Deku2D

#endif //	_2DE_SERIALIZATION_H_

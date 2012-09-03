#ifndef _2DE_SERIALIZATION_H_
#define _2DE_SERIALIZATION_H_

#include <string>

namespace Deku2D
{

	class CNullClass;
	void SerializeObjectToXml(CNullClass* object, const std::string& name);
	void SerializeObjectToJSON(CNullClass* object, const std::string& name);
	class CGameObject;
	CGameObject* DeserializeObjectFromJSON(const std::string &name);

}; // namespace Deku2D

#endif //	_2DE_SERIALIZATION_H_

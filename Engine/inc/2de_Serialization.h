#ifndef _2DE_SERIALIZATION_H_
#define _2DE_SERIALIZATION_H_

#include <string>

namespace Deku2D
{
	namespace Serialization
	{
		// @todo: Serialize to (and from) abstract stream, not file.
		void ToJSON(const void* instance, const std::string& typeName, const std::string& filename);
		void FromJSON(void* instance, const std::string& typeName, const std::string& filename);

	}	// namespace Serialization

}	// namespace Deku2D

#endif //	_2DE_SERIALIZATION_H_

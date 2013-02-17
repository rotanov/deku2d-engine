#ifndef _2DE_SERIALIZATION_H_
#define _2DE_SERIALIZATION_H_

#include <string>

namespace Deku2D
{
	namespace Serialization
	{
		// @todo: Serialize to abstract stream, not file.
		void ToJSON(void* o, const std::string& className, const std::string& filename);
	} // namespace Serialization

}; // namespace Deku2D

#endif //	_2DE_SERIALIZATION_H_

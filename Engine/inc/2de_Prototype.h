#ifndef _2DE_PROTOTYPE_H_
#define _2DE_PROTOTYPE_H_

#include "2de_Resource.h"

namespace Deku2D
{
	/**
	* CPrototype - resource that represents XML prototype - hierarchy of components.
	*/

	class CPrototype : public CResource
	{
	public:
		CPrototype();
		~CPrototype();

		bool Load();
		void Unload();

		CXMLNode* GetRootNode();

	private:
		CXML XML;

	};

}	//	namespace Deku2D

#endif	//	_2DE_PROTOTYPE_H_

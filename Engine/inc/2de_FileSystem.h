#ifndef _2DE_FILESYSTEM_H_
#define _2DE_FILESYSTEM_H_

#include "2de_Core.h"

namespace Deku2D
{
	/**
	* FileSystem - a namespace that contains functions for working with file system in a cross-platform way.
	*/

	namespace FileSystem
	{
		bool Exists(const string &APath);
		
		// TODO: extend, add more functions..
	}

}

#endif // _2DE_FILESYSTEM_H_

#include "2de_FileSystem.h"

namespace Deku2D
{
	//////////////////////////////////////////////////////////////////////////
	// FileSystem

	namespace FileSystem
	{
		bool Exists(const string &APath)
		{
			struct stat FileStat;
			return (stat(APath.c_str(), &FileStat) == 0);
		}
	}


}

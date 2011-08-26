#ifndef _2DE_IMAGE_UTILS_H_
#define _2DE_IMAGE_UTILS_H_

#include "2de_RGBA.h"

namespace Deku2d
{
	typedef unsigned int ILuint;

	/**
	*	Since I found it useful to use this module separated from Deku2d I did that:
	*	#define _2DE_IMAGE_UTILS_SEPARATED_ - you can define it somewhere before header appers
	*	and it should work fine.
	*/

	class CImageData
	{
	public:
		unsigned char *Data;
		unsigned Height;
		unsigned Width;
		unsigned char BPP;

		CImageData();
		~CImageData();
		bool LoadFromFile(const std::string &Filename);
		bool LoadFromMemory(void *Ptr, ILuint Size);
		RGBAub& operator [](unsigned Index);
		RGBAub& operator ()(unsigned col, unsigned row);

	private:
		void InitImageData();
	};

}	//	namespace Deku2d

#endif // _2DE_IMAGE_UTILS_H_

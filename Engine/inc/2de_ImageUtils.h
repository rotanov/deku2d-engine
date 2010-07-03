#ifndef _2DE_IMAGE_UTILS_H_
#define _2DE_IMAGE_UTILS_H_

#include "2de_Core.h"

class CImageData
{
public:
	byte			*Data;
	size_t			Height;
	size_t			Width;
	byte			BPP;
	bool			doCleanData;

	CImageData();
	~CImageData();
	bool LoadFromFile(const string &Filename);
};

#endif // _2DE_IMAGE_UTILS_H_

#ifndef _2DE_IMAGE_UTILS_H_
#define _2DE_IMAGE_UTILS_H_

#include "2de_Core.h"

class CImageData
{
public:
	byte			*data;
	size_t			height;
	size_t			width;
	byte			bpp;

	CImageData();
	~CImageData();
	bool	LoadFromFile(const string &Filename);
};

#endif // _2DE_IMAGE_UTILS_H_

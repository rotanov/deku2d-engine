#ifndef _2DE_IMAGE_UTILS_H_
#define _2DE_IMAGE_UTILS_H_

#include <stdlib.h>
#include "2de_Core.h"

class CImageData
{
public:
	byte* data;
	unsigned int height;
	unsigned int width;
	byte bpp;

	CImageData();
	~CImageData();
	/**
	*	Checks for color key within whole image
	*/
	bool	MakeRGBA();
	bool	LoadFromFile(const string &Filename);
};

#endif // _2DE_IMAGE_UTILS_H_
#ifndef _2DE_IMAGE_UTILS_H_
#define _2DE_IMAGE_UTILS_H_

#include "2de_Core.h"

class CImageData
{
public:
	byte			*data;
	unsigned int	height;
	unsigned int	width;
	byte			bpp;
	RGBAub			Colorkey;

	CImageData();
	~CImageData();
	bool	ProcessColorkey();
	bool	LoadFromFile(const string &Filename);
};

#endif // _2DE_IMAGE_UTILS_H_
#ifndef _2DE_IMAGE_UTILS_H_
#define _2DE_IMAGE_UTILS_H_

#include "2de_Core.h"

extern const unsigned int IMAGE_SHROOM_TITLE_WIDTH;
extern const unsigned int IMAGE_SHROOM_TITLE_HEIGHT;
extern char IMAGE_SHROOM_TITLE_DATA[];

extern const unsigned int IMAGE_DEFAULT_FONT_WIDTH;
extern const unsigned int IMAGE_DEFAULT_FONT_HEIGHT;
extern char IMAGE_DEFAULT_FONT_DATA[];


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

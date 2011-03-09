#ifndef _2DE_IMAGE_UTILS_H_
#define _2DE_IMAGE_UTILS_H_

#include "2de_Core.h"
#include "2de_Math.h"

extern unsigned int  IMAGE_SHROOM_TITLE_SIZE;
extern byte IMAGE_SHROOM_TITLE_DATA[];

// extern const unsigned int IMAGE_DEFAULT_FONT_WIDTH;
// extern const unsigned int IMAGE_DEFAULT_FONT_HEIGHT;
// extern char IMAGE_DEFAULT_FONT_DATA[];


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
	bool LoadFromMemory(void *Ptr, ILuint Size);

	RGBAub& operator [](unsigned Index)
	{
		assert(Index >= 0 && Index < Width * Height);
		return (reinterpret_cast<RGBAub *>(Data))[Index];
	}

	RGBAub& operator ()(unsigned col, unsigned row)
	{
		assert(col * row >= 0 && col * row < Width * Height);
		return (reinterpret_cast<RGBAub *>(Data))[ row * Width + col ];
		return RGBAub();
	}

private:
	void InitImageData();
};

#endif // _2DE_IMAGE_UTILS_H_

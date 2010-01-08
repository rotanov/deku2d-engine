#ifndef _2DE_IMAGE_UTILS_H
#define _2DE_IMAGE_UTILS_H

#include <stdlib.h>
#include <png.h>

#include "2de_Core.h"

#define PNG_SIG_BYTES 8

#ifndef _WIN32

// я убрал все эти дополнительные проверки.. GCC полностью поддерживает pragma pack, а остальные non-windows компиляторы мы пока не поддерживаем
#pragma pack(push,2)

typedef struct tagBITMAPFILEHEADER {
	unsigned short	bfType;
	unsigned long	bfSize;
	unsigned short	bfReserved1;
	unsigned short	bfReserved2;
	unsigned long	bfOffBits;
} BITMAPFILEHEADER, FAR *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;

#pragma pack(pop)

struct RGBQUAD
{
	byte	rgbBlue;
	byte	rgbGreen;
	byte	rgbRed;
	byte	rgbReserved;
};


struct BITMAPINFOHEADER
{
	unsigned long	biSize;
	long		biWidth;
	long		biHeight;
	unsigned short	biPlanes;
	unsigned short	biBitCount;
	unsigned long	biCompression;
	unsigned long	biSizeImage;
	long		biXPelsPerMeter;
	long		biYPelsPerMeter;
	unsigned long	biClrUsed;
	unsigned long	biClrImportant;
};

struct BITMAPINFO
{
	BITMAPINFOHEADER	bmiHeader;
	RGBQUAD				bmiColors[1];
};

#endif //_WIN32

class CImageData
{
public:
	byte* data;
	unsigned int height;
	unsigned int width;
	byte bpp;

	CImageData(): data(NULL), height(0), width(0), bpp(0) {}

	~CImageData()
	{
		if (data != NULL)
			delete [] data;
	}
	/**
	*	Loads an image data from BMP file.
	*/
	bool	LoadBMP(const char *filename);
	/**
	*	Loads an image data from PNG file.
	*/
	bool	LoadPNG(const char *filename);
	/**
	*	Converts Image from RGB to RGBA format in memory.
	*	Use only after LoadBMP() function call.
	*/
	bool	MakeRGBA();
	/**
	*	Get the RGBAub struct with color data of Pixel of image at (x, y) coords.
	*/
	RGBAub Pixel(int x, int y);
};

typedef CImageData* PImageData;

#endif // _2DE_IMAGE_UTILS_H

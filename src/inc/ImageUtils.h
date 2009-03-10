// TODO: i think there may be some problems with memory access and pointers while
// using GetPixel for 3 bytes images , and accessing to alpha byte...
// and goal to solve this problem currently it is necessary to call makergba
// routine for all loaded images to avoid this problem. 
// Also All most of ImageData members have not tested with PNG images.
// So don't use it.
// TODO! теперь надо перевести гигантский сумбурный комментарий сверху и подумать.

#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#include <stdlib.h>
#include <png.h>

#include "CoreUtils.h"

#define PNG_SIG_BYTES 8

#ifndef WIN32

#if ! (defined(lint) || defined(RC_INVOKED))
#if ( _MSC_VER >= 800 && !defined(_M_I86)) || defined(_PUSHPOP_SUPPORTED)
#pragma warning(disable:4103)
#if !(defined( MIDL_PASS )) || defined( __midl )
#pragma pack(push,2)
#else
#pragma pack(2)
#endif
#else
#pragma pack(2)
#endif
#endif // ! (defined(lint) || defined(RC_INVOKED))

typedef struct tagBITMAPFILEHEADER {
        WORD    bfType;
        DWORD   bfSize;
        WORD    bfReserved1;
        WORD    bfReserved2;
        DWORD   bfOffBits;
} BITMAPFILEHEADER, FAR *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;


#if ! (defined(lint) || defined(RC_INVOKED))
#if ( _MSC_VER >= 800 && !defined(_M_I86)) || defined(_PUSHPOP_SUPPORTED)
#pragma warning(disable:4103)
#if !(defined( MIDL_PASS )) || defined( __midl )
#pragma pack(pop)
#else
#pragma pack()
#endif
#else
#pragma pack()
#endif
#endif // ! (defined(lint) || defined(RC_INVOKED))


struct RGBQUAD
{
	BYTE	rgbBlue;
	BYTE	rgbGreen;
	BYTE	rgbRed;
	BYTE	rgbReserved;
};


struct BITMAPINFOHEADER
{
	DWORD	biSize;
	LONG	biWidth;
	LONG	biHeight;
	WORD	biPlanes;
	WORD	biBitCount;
	DWORD	biCompression;
	DWORD	biSizeImage;
	LONG	biXPelsPerMeter;
	LONG	biYPelsPerMeter;
	DWORD	biClrUsed;
	DWORD	biClrImportant;
};

struct BITMAPINFO
{
	BITMAPINFOHEADER	bmiHeader;
	RGBQUAD				bmiColors[1];
};

#endif

class CImageData
{
public:
	pbyte data;
	unsigned int height;
	unsigned int width;
	byte bpp;

	CImageData():height(0), width(0), bpp(0), data(NULL){}

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

#endif IMAGE_UTILS_H

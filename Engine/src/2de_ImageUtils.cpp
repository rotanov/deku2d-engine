#include "2de_ImageUtils.h"

#include <IL/il.h>

#ifndef _2DE_IMAGE_UTILS_SEPARATED_
	#include "2de_Core.h"
#endif

CImageData::CImageData() : Data(NULL), Height(0), Width(0), BPP(0)
{
}

CImageData::~CImageData()
{
	delete [] Data;
}

bool CImageData::LoadFromFile(const std::string &Filename)
{
	unsigned ILID = ilGenImage();
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	ilEnable(IL_ORIGIN_SET);
	ilBindImage(ILID);

	if (!ilLoadImage(Filename.c_str()))
		return false;

	InitImageData();

	ilDeleteImage(ILID);
	return true;
}

bool CImageData::LoadFromMemory(void *Ptr, ILuint Size)
{
	unsigned ILID = ilGenImage();
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	ilEnable(IL_ORIGIN_SET);
	ilBindImage(ILID);

	if (!ilLoadL(IL_TYPE_UNKNOWN, Ptr, Size))
		return false;

	InitImageData();

	ilDeleteImage(ILID);
	return true;
}

// well, i just was not able to cook some good name for this function...
void CImageData::InitImageData()
{
	Width = ilGetInteger(IL_IMAGE_WIDTH);
	Height = ilGetInteger(IL_IMAGE_HEIGHT);

	//bpp = ilGetInteger(IL_IMAGE_FORMAT);
	//bpp = ilGetInteger(IL_IMAGE_BPP);	// you meant IL_IMAGE_BYTES_PER_PIXEL, didn't you?
	BPP = 4;

	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	Data = new unsigned char[Width * Height * BPP];
	ilCopyPixels(0, 0, 0, Width, Height, 1, IL_RGBA, IL_UNSIGNED_BYTE, Data);

	// we can get an error message using ILU (iluGetErrorString(ilGetError())), but it's not worth to be dependent on it..
	if (ilGetError() != IL_NO_ERROR)
#ifndef _2DE_IMAGE_UTILS_SEPARATED_
		Log("ERROR", "IL failed");
#else
		fprintf(stderr, "IL failed");
#endif
}

RGBAub& CImageData::operator []( unsigned Index )
{
	assert(Index >= 0 && Index < Width * Height);
	return (reinterpret_cast<RGBAub *>(Data))[Index];
}

RGBAub& CImageData::operator ()( unsigned col, unsigned row )
{
	assert(col * row >= 0 && col * row < Width * Height);
	return (reinterpret_cast<RGBAub *>(Data))[ row * Width + col ];
	//return RGBAub();
}

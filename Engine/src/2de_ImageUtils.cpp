#include "2de_ImageUtils.h"

#include <IL/il.h>

CImageData::CImageData() : data(NULL), height(0), width(0), bpp(0){}

CImageData::~CImageData()
{
	if (data != NULL)
		delete [] data;
}

bool CImageData::LoadFromFile(const string &Filename)
{
	unsigned int ILID = ilGenImage();
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	ilEnable(IL_ORIGIN_SET);
	ilBindImage(ILID);
	if (!ilLoadImage(Filename.c_str()))
		return false;
	width = ilGetInteger(IL_IMAGE_WIDTH);
	height = ilGetInteger(IL_IMAGE_HEIGHT);

	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	data = new byte[width * height * 4];
	ilCopyPixels(0, 0, 0, width, height, 1, IL_RGBA, IL_UNSIGNED_BYTE, data);

	// хорошо бы писать в лог что-то поумнее, но для того, чтобы достать вменяемый текст ошибки, нужно подключать ILU: iluGetErrorString(ilGetError())
	// учитывая, что из него никаких больше функций не нужно, мне его подключать не охота..
	if (ilGetError() != IL_NO_ERROR)
		Log("ERROR", "IL failed.");

	//bpp = ilGetInteger(IL_IMAGE_FORMAT);
	//bpp = ilGetInteger(IL_IMAGE_BPP);	// если вдруг на будущее, то IL_IMAGE_BYTES_PER_PIXEL
	bpp = 4;
	ilDeleteImage(ILID);
	return true;
}

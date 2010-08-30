#include "2de_ImageUtils.h"

CImageData::CImageData() : Data(NULL), Height(0), Width(0), BPP(0), doCleanData(true)
{
}

CImageData::~CImageData()
{
	if (doCleanData && Data != NULL)
		delete [] Data;
}

bool CImageData::LoadFromFile(const string &Filename)
{
	unsigned int ILID = ilGenImage();
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	ilEnable(IL_ORIGIN_SET);
	ilBindImage(ILID);
	if (!ilLoadImage(Filename.c_str()))
		return false;
	Width = ilGetInteger(IL_IMAGE_WIDTH);
	Height = ilGetInteger(IL_IMAGE_HEIGHT);

	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	Data = new byte[Width * Height * 4];
	ilCopyPixels(0, 0, 0, Width, Height, 1, IL_RGBA, IL_UNSIGNED_BYTE, Data);

	// хорошо бы писать в лог что-то поумнее, но для того, чтобы достать вменяемый текст ошибки, нужно подключать ILU: iluGetErrorString(ilGetError())
	// учитывая, что из него никаких больше функций не нужно, мне его подключать не охота..
	if (ilGetError() != IL_NO_ERROR)
		Log("ERROR", "IL failed.");

	//bpp = ilGetInteger(IL_IMAGE_FORMAT);
	//bpp = ilGetInteger(IL_IMAGE_BPP);	// если вдруг на будущее, то IL_IMAGE_BYTES_PER_PIXEL
	BPP = 4;
	ilDeleteImage(ILID);
	return true;
}

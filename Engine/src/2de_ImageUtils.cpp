#include "2de_ImageUtils.h"
#include <assert.h>

CImageData::CImageData() : data(NULL), height(0), width(0), bpp(0)
{

}

CImageData::~CImageData()
{
	if (data != NULL)
		delete [] data;
}

bool  CImageData::MakeRGBA()
{
	if (data ==  NULL)
		return false;
	byte *Rs, *Gs, *Bs, *Ap, *Rp, *Gp, *Bp;
	for(int i = 0;i < height*width; i++)
	{
		Rs = data + i*bpp;
		Gs = data + i*bpp + 1;
		Bs = data + i*bpp + 2;
		Rp = data + i*bpp ;
		Gp = data + i*bpp + 1;
		Bp = data + i*bpp + 2;
		Ap = data + i*bpp + 3;
		if ( *Rs == 255 && *Bs == 255 && *Gs == 0 )
			*Ap = 0;
		*Rp =*Rs;
		*Gp =*Gs;
		*Bp =*Bs;
	}
    return true;
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

	data = new byte[width * height * 4];
	ilCopyPixels(0, 0, 0, width, height, 1, IL_RGBA, IL_UNSIGNED_BYTE, data);
	//bpp = ilGetInteger(IL_IMAGE_FORMAT);
	//bpp = ilGetInteger(IL_IMAGE_BPP);
	bpp = 4;
	ilDeleteImage(ILID);
	return true;
}
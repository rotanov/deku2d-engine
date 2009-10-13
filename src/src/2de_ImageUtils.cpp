#include "2de_ImageUtils.h"
#include <assert.h>

RGBAub CImageData::Pixel(int x, int y)
{
	return *((RGBAub*)(data + (y*width + x)*bpp));
}

bool CImageData::LoadBMP(const char * name)
{
	DWORD dwDIBSize;
	BITMAPFILEHEADER bfh ;
	BITMAPINFO bi;
	CFile file;
	pbyte R, G, B;
	byte temp;
	
	if (!file.Open(name, CFile::fomRead))
	{
		Log("ERROR", "Cant load bmp file: \"%s\" file doesnt exist or can't open", name);
		return false;
	}
  
	file.Read(&bfh, sizeof(BITMAPFILEHEADER));

	if (bfh.bfType != 0x4d42)
	{
		Log("ERROR", "Cant load bmp file: \"%s\" is not a BMP file", name);
		return false;
	}
	file.Read(&bi, sizeof(BITMAPINFOHEADER));
	if (bi.bmiHeader.biBitCount != 24)
	{
		Log("ERROR", "Cant load bmp file: \"%s\" only 24 bits per pixel supported", name);
		return false;
	}

	width	= bi.bmiHeader.biWidth;
	height	= bi.bmiHeader.biHeight;
	bpp		= bi.bmiHeader.biBitCount / 8;

	dwDIBSize = bpp*width*height;
	data = new byte[dwDIBSize];//(pbyte)VirtualAlloc(NULL, dwDIBSize, MEM_COMMIT, PAGE_READWRITE );

	file.Read(data, dwDIBSize);

	for( int i = 0; i < (height*width); i++)
	{
		B = data + i*3 ;
		R = data + i*3 + 2;
		G = data + i*3 + 1;
		temp	= *B;
		*B		= *R;
		*R		= temp;
	}

	file.Close();
	Log("INFO", "BMP image \"%s\" loaded", name); 
	return true;
}

bool  CImageData::MakeRGBA()
{
	if (data ==  NULL)
		return false;
	pbyte _data;
	_data = new byte [width*height*4];
	if (_data == NULL)
		return false;
	pbyte Rs,Gs,Bs,Ap,Rp,Gp,Bp;
	for(int i = 0;i < height*width; i++)
	{
		Rs = data + i*3;
		Gs = data + i*3 + 1;
		Bs = data + i*3 + 2;
		Rp = _data + i*4 ;
		Gp = _data + i*4 + 1;
		Bp = _data + i*4 + 2;
		Ap = _data + i*4 + 3;
		if ( *Rs == 255 && *Bs == 255 && *Gs == 0 ) //  Here is the color key check
			*Ap = 0;								// the color key is 255 0 255  
		else
			*Ap = 255;
		*Rp =*Rs;
		*Gp =*Gs;
		*Bp =*Bs;
	}
	delete [] data;
	data = _data;
	bpp = 4;
    return true;
}

bool CImageData::LoadPNG(const char * filename)
{
	FILE *png_file = fopen(filename, "rb");
	if (png_file == NULL)
	{
		Log("ERROR", "Can't Load PNG file: \"%s\" can't open or doesn't exist", filename);
		return false;
	}

	byte header[PNG_SIG_BYTES];

	fread(header, 1, PNG_SIG_BYTES, png_file);
	if (png_sig_cmp(header, 0, PNG_SIG_BYTES))
	{
		Log("ERROR", "Can't load PNG file: \"%s\" is not a PNG file.", filename);
		return false;
	}

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop info_ptr = png_create_info_struct(png_ptr);
	png_infop end_info = png_create_info_struct(png_ptr);

	if(setjmp(png_jmpbuf(png_ptr)))
	{
		Log("ERROR", "Can't load PNG file: \"%s\". Some error occured.", filename);
		return false;
	}
	png_init_io(png_ptr, png_file);
	png_set_sig_bytes(png_ptr, PNG_SIG_BYTES);
	png_read_info(png_ptr, info_ptr);

	width = png_get_image_width(png_ptr, info_ptr);
	height = png_get_image_height(png_ptr, info_ptr);

	png_uint_32 bit_depth, color_type;
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);
	color_type = png_get_color_type(png_ptr, info_ptr);
			
	if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_gray_1_2_4_to_8(png_ptr);

	if (bit_depth == 16)
			png_set_strip_16(png_ptr);
			
	if(color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png_ptr);
	else if(color_type == PNG_COLOR_TYPE_GRAY ||
			color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		{
			png_set_gray_to_rgb(png_ptr);
		}

	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr);
	else
		png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);

	png_read_update_info(png_ptr, info_ptr);

	png_uint_32 rowbytes = png_get_rowbytes(png_ptr, info_ptr);
	png_uint_32 numbytes = rowbytes*(height);
	png_byte* pixels = (png_byte *)malloc(numbytes);
	png_byte** row_ptrs = (png_byte **)malloc((height) * sizeof(png_byte*));

	int i;
	for (i=0; i<(height); i++)
	  row_ptrs[i] = pixels + ((height) - 1 - i)*rowbytes;

	png_read_image(png_ptr, row_ptrs);

	free(row_ptrs);
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	fclose(png_file);

	data = pixels;
	bpp = 4;

	Log("INFO", "PNG image \"%s\" loaded", filename); 
	return true;
}

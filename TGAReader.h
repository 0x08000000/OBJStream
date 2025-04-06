#ifndef TGAREADER_H
#define TGAREADER_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "TGAUtil.h"

#define DEBUG 0

//For check if file exist
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <io.h>
#define FILE_EXIST(file) (_access(file, F_OK) == 0)
#define HAS_READ_ACCESS(file) (_access(file, R_OK) == 0)
#else
#include <unistd.h>
#define FILE_EXIST(file) (access(file, F_OK) == 0)
#define HAS_READ_ACCESS(file) (access(file, R_OK) == 0)
#endif

#define CAST_PIXEL_DATA(TYPE, BUFFER) ((TYPE *)(BUFFER))
#define NOT_IMPLEMENTED TGA_Printf("%s: NOT IMPLEMENTED", __FUNCTION__);

void TGA_Printf(char *Format, ...);

/* From Wikipedia
        Image type 1 and 9: Depending on the Pixel Depth value, 
        image data representation is an 8, 15, or 16 bit index into a color map 
        that defines the color of the pixel. 
        
        Image type 2 and 10: The image data is a direct representation of the pixel color. 
        For a Pixel Depth of 15 and 16 bit, each pixel is stored with 5 bits per color.
        If the pixel depth is 16 bits, the topmost bit is reserved for transparency. 
        For a pixel depth of 24 bits, each pixel is stored with 8 bits per color.
        A 32-bit pixel depth defines an additional 8-bit alpha channel. 
        Image type 3 and 11: The image data is a direct representation of grayscale data. 
        he pixel depth is 8 bits for images of this type

*/
enum TGAImageType
{
	NO_DATA				= 0,
	UNCOMPRESSED_COLOR_MAPPED	= 1,
	UNCOMPRESSED_TRUE_COLOR		= 2,
	UNCOMPRESSED_GRAYSCALE		= 3,
	RUN_LENGTH_ENCODED_COLOR_MAPPED = 9,
	RUN_LENGTH_ENCODED_TRUE_COLOR	= 10,
	RUN_LENGTH_ENCODED_GRAYSCALE	= 11,
};

struct TGAColorMapSpecification
{
	uint16_t FirstEntryIndex;
	uint16_t ColorMapLength;
	uint8_t	 ColorMapEntrySize;
};

struct Pixel8
{
	uint8_t LUMA;
};

struct Pixel16
{
	uint8_t R : 5;
	uint8_t G : 5;
	uint8_t B : 5;
	uint8_t A : 1;
};

struct Pixel24
{
	uint8_t R;
	uint8_t G;
	uint8_t B;
};

struct Pixel32
{
	uint8_t R;
	uint8_t G;
	uint8_t B;
	uint8_t A;
};

struct TGAImageSpecification
{
	uint16_t XOrigin[2];
	uint16_t YOrigin[2];
	uint16_t ImageWidth;
	uint16_t ImageHeight;
	uint8_t	 PixelDepth; //bits per pixel
	uint8_t	 ImageDescriptor;
};

struct TGAHeader
{
	uint8_t IDLength; // 0 - 255
	uint8_t ColorMapType; // 0 no color map, 1 if present, other values not to be used.
	uint8_t ImageType; // bits 3–0 give the alpha channel depth, bits 5–4 give pixel ordering

	union
	{
		struct TGAColorMapSpecification ColorMapSpec;
		uint8_t				ColorMapSpecification[5];
	};

	union
	{
		struct TGAImageSpecification ImageTypeSpec;
		uint8_t			     ImageTypeSpecification[5];
	};
};

struct TGAFooter
{
	uint32_t ExtensionAreaOffset;
	uint32_t DeveloperDirectoryOffset;
	char	 Signature[18];
};

struct TGAFile
{
	char		*FilePath;

	struct TGAHeader Header;

	uint8_t		 ImageIDSize;
	uint8_t		*ImageIDData;

	uint8_t		 ColorMapSize; //size depends on color map specification
	union
	{
		struct Pixel8  *Pixel8ColorMap;
		struct Pixel16 *Pixel16ColorMap;
		struct Pixel24 *Pixel24ColorMap;
		struct Pixel32 *Pixel32ColorMap;
	};

	uint32_t PixelCount;
	union
	{
		struct Pixel8  *Pixel8Buffer;
		struct Pixel16 *Pixel16Buffer;
		struct Pixel24 *Pixel24Buffer;
		struct Pixel32 *Pixel32Buffer;
	};

	struct Pixel8	*LumaData;

	uint8_t		 HasFooter;
	struct TGAFooter Footer;

	uint8_t		 HasColorMap;
};

uint8_t TGA_LoadFile(struct TGAFile *File);
#endif
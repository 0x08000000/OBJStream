#include "TGAReader.h"
#include "TGAutil.h"
#include <stdio.h>

/**
 * Prints a message to the standard output preceded by the string "TGA: ".
 *
 * This function is used by the TGA library to print messages to the user. If
 * the message is a null pointer, it prints an error message to the standard
 * error output instead.
 *
 * @param Format The format string of the message to print.
 * @param ... The arguments to be formatted in the format string.
 */
void TGA_Printf(char *Format, ...)
{
#if DEBUG
	if (!Format) {
		fprintf(stderr,
			"TGA: Error, null pointer reference in TGA_Printf\n");
		return;
	}

	va_list args;
	va_start(args, Format);
	fprintf(stdout, "TGA: ");
	vfprintf(stdout, Format, args);
	fprintf(stdout, "\n\r");
	va_end(args);
#endif
}

void TGAPRIVATE_ReadCompressedPixel(struct TGAFile *File)
{
	NOT_IMPLEMENTED;
}

/**
 * Reads the TGA file and stores the important information in the struct
 * TGAFile.
 *
 * This function checks if the file exists, if it has write access, opens it
 * and reads the header to get the important information for parsing. It then
 * reads the image ID, color map and image data according to the header's
 * information. Finally, it reads the footer.
 *
 * @param File A pointer to the struct TGAFile where the information will be
 *             stored.
 *
 * @return 1 if the file was read successfully, 0 otherwise.
 */
uint8_t TGAPRIVATE_ReadFile(struct TGAFile *File)
{
	//Check for null pointer reference
	if (!File || !File->FilePath) {
		TGA_Printf("Error: File path is NULL");
		return 0;
	}

	//Check if the file exists
	if (!FILE_EXIST(File->FilePath)) {
		TGA_Printf("Error: File does not exist");
		return 0;
	}

	//Check if the file has write access
	if (!HAS_READ_ACCESS(File->FilePath)) {
		TGA_Printf("Error: File does not have write access");
		return 0;
	}

	//Open the file
	FILE *FP = fopen(File->FilePath, "rb");
	if (!FP) {
		TGA_Printf("Error: File could not be opened");
		return 0;
	}

	//Read header to get the important information for parsing.
	if (fread(&(File->Header), sizeof(struct TGAHeader), 1, FP) != 1) {
		TGA_Printf("Error: File could not be read");
		fclose(FP);
		return 0;
	}

	//Check if the header is valid
	if (File->Header.IDLength > 255 || File->Header.ColorMapSpec.ColorMapLength == 0) {
		TGA_Printf("Error: File header is invalid");
		fclose(FP);
		return 0;
	}

	//Get the image ID
	if (File->Header.IDLength > 0) {
		File->ImageIDData = malloc(File->Header.IDLength + 1);
		if (!File->ImageIDData) {
			TGA_Printf(
				"Error: File->ImageIDData could not be allocated");
			fclose(FP);
			return 0;
		}

		if (fread(File->ImageIDData, File->Header.IDLength, 1, FP) != 1) {
			TGA_Printf("Error: File could not be read");
			fclose(FP);
			return 0;
		}

		File->ImageIDData[File->Header.IDLength] = '\0';
	}

	//Get the color map
	File->ColorMapSize = 0;
	if (File->Header.ImageType == UNCOMPRESSED_GRAYSCALE) {
		File->ColorMapSize =
			(File->Header.ColorMapSpec.ColorMapEntrySize >> 3) *
			File->Header.ColorMapSpec.ColorMapLength;

		File->Pixel32ColorMap = malloc(File->ColorMapSize);
		if (!File->Pixel32ColorMap) {
			TGA_Printf(
				"Error: File->Pixel32ColorMap could not be allocated");
			fclose(FP);
			return 0;
		}

		if (fread(File->Pixel32ColorMap, File->ColorMapSize, 1, FP) != 1) {
			TGA_Printf("Error: File could not be read");
			fclose(FP);
			return 0;
		}
	}

	//Read Image Data
	File->PixelCount = File->Header.ImageTypeSpec.ImageHeight *
			   File->Header.ImageTypeSpec.ImageWidth;

	File->Pixel32Buffer =
		malloc((File->Header.ImageTypeSpec.PixelDepth >> 3) *
		       File->PixelCount);
	File->LumaData = malloc(sizeof(struct Pixel8) * File->PixelCount);

	if (!File->Pixel32Buffer || !File->LumaData) {
		TGA_Printf(
			"Error: File->Pixel32Buffer or File->LumaData could not be allocated");
		fclose(FP);
		return 0;
	}

	if (fread(File->Pixel32Buffer, File->Header.ImageTypeSpec.PixelDepth,
		  File->PixelCount, FP) != File->PixelCount) {
		TGA_Printf("Error: File could not be read");
		fclose(FP);
		return 0;
	}

	//Read the footer.
	fseek(FP, -26, SEEK_END);
	if (fread(&(File->Footer), 26, 1, FP) != 1) {
		TGA_Printf("Error: File could not be read");
		fclose(FP);
		return 0;
	}

	if (TGAUtil_GetHASH(File->Footer.Signature) == TGAUtil_GetHASH("TRUEVISION-XFILE.")) {
		File->HasFooter = 1;
        }
	fclose(FP);

	return 1;
}

/**
 * Prints the content of the TGAFile struct to the standard output.
 *
 * This function is useful for debugging purposes. It prints all the
 * information contained in the TGAFile struct in a formatted way.
 *
 * @param File A pointer to the struct TGAFile to be printed.
 */
void TGA_PrintFile(struct TGAFile *File)
{
	TGA_Printf("TGA Struct Content:");
	TGA_Printf("FilePath: %s", File->FilePath);
	TGA_Printf("Image Type: %d", File->Header.ImageType);
	TGA_Printf("Image Width: %d", File->Header.ImageTypeSpec.ImageWidth);
	TGA_Printf("Image Height: %d", File->Header.ImageTypeSpec.ImageHeight);
	TGA_Printf("Pixel Depth: %d", File->Header.ImageTypeSpec.PixelDepth);
	TGA_Printf("Image ID Length: %d", File->Header.IDLength);
	TGA_Printf("Image ID Data: %s", File->ImageIDData);
	TGA_Printf("Color Map Length: %d", File->Header.ColorMapSpec.ColorMapLength);
	TGA_Printf("Color Map Entry Size: %d", File->Header.ColorMapSpec.ColorMapEntrySize);
	TGA_Printf("Pixel Count: %d", File->PixelCount);
	TGA_Printf("Has Color Map: %d", File->HasColorMap);
	TGA_Printf("Has Footer: %d", File->HasFooter);
	TGA_Printf("Footer Signature: %s", File->Footer.Signature);
	TGA_Printf("Footer Extension Area Offset: %d",File->Footer.ExtensionAreaOffset);
	TGA_Printf("Footer Developer Directory Offset: %d", File->Footer.DeveloperDirectoryOffset);
	TGA_Printf("Footer Has Footer: %d", File->HasFooter);
}

/**
 * Loads a TGA file into a struct TGAFile.
 *
 * This function returns 1 if the file was loaded successfully or 0 if
 * there was an error.
 *
 * @param File A pointer to the struct TGAFile to be loaded.
 */
uint8_t TGA_Load(struct TGAFile *File)
{
        if(!File)       
        {
                TGA_Printf("Error: File is NULL");
                return 0;
        }
        
        if(!TGAPRIVATE_ReadFile(File))
        {
                TGA_Printf("Error: File could not be read");
                return 0;       
        }

	return 1;
}
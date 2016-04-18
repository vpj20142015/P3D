#include <gl\gl.h>
#include <gl\glut.h>

#include "jpeg.h"

void SwiftTextureJpeg(unsigned int tTexture[], LPSTR strFileName, int ID) {
	
	tImageJPG *pBitMap = SwiftLoadJpeg(strFileName);

	glGenTextures(1, &tTexture[ID]);
	glBindTexture(GL_TEXTURE_2D, tTexture[ID]);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, pBitMap->sizeX, pBitMap->sizeY, GL_RGB, GL_UNSIGNED_BYTE, pBitMap->data);
	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);

	free(pBitMap->data);			
	free(pBitMap);
}

tImageJPG *SwiftLoadJpeg(const char *strFileName)
{	
	FILE *pFile;
	struct jpeg_decompress_struct cInfo;
	tImageJPG *pImageData = NULL;
	jpeg_error_mgr jerr;

	if ((pFile = fopen(strFileName, "rb")) == NULL){
		return 0;
	}

	cInfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cInfo);
	jpeg_stdio_src(&cInfo, pFile);
	jpeg_read_header(&cInfo, true);
	jpeg_start_decompress(&cInfo);

	pImageData = (tImageJPG*)malloc(sizeof(tImageJPG));

	pImageData->rowSpan = cInfo.image_width * cInfo.num_components;
	pImageData->sizeX   = cInfo.image_width;
	pImageData->sizeY   = cInfo.image_height;
	
	pImageData->data = new unsigned char[pImageData->rowSpan * pImageData->sizeY];
	
	unsigned char** rowPtr = new unsigned char*[pImageData->sizeY];
	for (int i = 0; i < pImageData->sizeY; i++){
		rowPtr[i] = &(pImageData->data[i*pImageData->rowSpan]);
	}
	
	int rowsRead = cInfo.output_height-1;
	while (cInfo.output_scanline < cInfo.output_height) 
	{
		rowsRead -= jpeg_read_scanlines(&cInfo, &rowPtr[rowsRead], cInfo.output_height - rowsRead);
	}
	delete [] rowPtr;
	
	jpeg_finish_decompress(&cInfo);

	jpeg_destroy_decompress(&cInfo);
	
	fclose(pFile);
	
	return pImageData;
}

#include <jpeglib.h>
#pragma comment(lib, "jpeg.lib")

tImageJPG *SwiftLoadJpeg (const char *srFileName);

void SwiftTextureJpeg(unsigned int tTexture[], LPSTR strFileName, int ID);
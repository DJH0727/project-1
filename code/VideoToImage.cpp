#include "VideoToImage.h"
int VideotoImage(const char* ffmPath, const char* imagePath,
	const char* imageFormat, const char* videoPath)
{
	char systemCommand[256];
	snprintf(systemCommand, 256, "\"%s\"ffmpeg.exe -i %s -f image2  %s\\%%04d.%s"
	, ffmPath, videoPath, imagePath,imageFormat);
	return system(systemCommand);
}
/*int main()
{
	VideotoImage("", "test", "png", "out.mp4");
	return 0;
}*/
#include "ImageToVideo.h"
int ImagetoVideo(const char* ffmPath, unsigned int videofps, const char* imagePath,
	const char* imageFormat, const char* videoOutput, unsigned int bitRates)
{
	char systemCommand[256];
		snprintf(systemCommand, 256, "\"%s\"ffmpeg.exe -r %u -f image2 -i %s\\%%04d.%s -b:v %uK %s"
			, ffmPath, videofps, imagePath, imageFormat, bitRates, videoOutput);
	//std::cout << systemCommand<<std::endl;
	return system(systemCommand);
}
/*int main()
{
	char ffmPath[] = "";
	//std::cout << ffmPath << std::endl;
	ImagetoVideo(ffmPath,30,"test","png","out.mp4",30);
	return 0;
}*/
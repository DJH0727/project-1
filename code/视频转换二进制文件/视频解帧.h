#pragma once
#include <opencv2/opencv.hpp>
#include <cstdlib>
#include <cstring>
#include <cstdio>
//#include<Windows.h>
#include <fstream>
#include <iostream>

using namespace cv;
using namespace std;

int VideotoImage(const char* ffmPath, const char* imagePath,
	const char* imageFormat, const char* videoPath)
{
	char systemCommand[256];
	snprintf(systemCommand, 256, "%s\\ffmpeg.exe -i %s -f image2  %s\\%%04d.%s"
		, ffmPath, videoPath, imagePath, imageFormat);
	char CreatImagePath[20];
	snprintf(CreatImagePath, 20, "md %s", imagePath);
	system(CreatImagePath);

	return system(systemCommand);

	//char DeleteImagePath[20];
	// snprintf(CreatImagePath, 20, "rmdir /S /Q %s", imagePath);
	// system(DeleteImagePath);
}


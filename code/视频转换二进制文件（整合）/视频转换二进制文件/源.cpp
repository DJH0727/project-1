#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <time.h>
#include <set>
#include <iomanip> // 用于 std::setw 和 std::setfill  
#include <sstream> 
#include"二维码图像处理.h"
#include"视频解帧.h"
#include "二维码解码.h"
using namespace cv;
using namespace std;
#define _CRT_SECURE_NO_WARNINGS

// 函数将整数转换为具有指定宽度的字符数组，不足部分用零填充  
void intToCharArrayWithLeadingZeros(int number, char result[]) {
	std::ostringstream oss;
	oss << std::setw(4) << std::setfill('0') << number;
	std::string str = oss.str();
	std::copy(str.begin(), str.end(), result);
	result[str.size()] = '\0'; // 添加字符串结束符  
}
int main()
{
	int count = 0;
	char ffmPath[256]= "D:\\ffmpeg-2024-03-04-git-e30369bc1c-full_build\\bin", imagePath[256] = "C:\\Users\\Acer\\Documents\\源代码\\videotransform\\code", imageFormat[4]="jpg", videoPath[256];
	cin >> videoPath;
	count=VideotoImage(ffmPath, imagePath, imageFormat, videoPath);
	for (int i = 0; i < count; i++)
	{
		char imagename[5];
		intToCharArrayWithLeadingZeros(i, imagename);
        imageprcessing(imagename,imageFormat);
	}
	Imagetofile();
}
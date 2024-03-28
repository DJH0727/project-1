#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <time.h>
#include <set>
#include <iomanip> // ���� std::setw �� std::setfill  
#include <sstream> 
#include"��ά��ͼ����.h"
#include"��Ƶ��֡.h"
#include "��ά�����.h"
using namespace cv;
using namespace std;
#define _CRT_SECURE_NO_WARNINGS

// ����������ת��Ϊ����ָ����ȵ��ַ����飬���㲿���������  
void intToCharArrayWithLeadingZeros(int number, char result[]) {
	std::ostringstream oss;
	oss << std::setw(4) << std::setfill('0') << number;
	std::string str = oss.str();
	std::copy(str.begin(), str.end(), result);
	result[str.size()] = '\0'; // ����ַ���������  
}
int main()
{
	int count = 0;
	char ffmPath[256]= "D:\\ffmpeg-2024-03-04-git-e30369bc1c-full_build\\bin", imagePath[256] = "C:\\Users\\Acer\\Documents\\Դ����\\videotransform\\code", imageFormat[4]="jpg", videoPath[256];
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
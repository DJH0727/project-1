#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include <opencv2/opencv.hpp>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include<Windows.h>
#include <fstream>
#include <iostream>

using namespace cv;
using namespace std;



	void DrowRect(Mat& image);
	void OpenFile(vector<int>& data, const char* BinFileName);
	Mat ScaleMat(Mat& image);
	int DrowQRPoint(vector<int>& data, Mat& image, int cnt);
	void CreatQRcode(const char* BinFileName, int QRnum);




int PictureEncode(const char* imageName, const char* imageFormat, const char* BinFileName);

int BinFiletoVideo(const char* BinFileName, const char* videoOutput, int TimeLimit, unsigned int videofps = 15);

#pragma once
#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include <opencv2/opencv.hpp>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include<Windows.h>
#include <fstream>
#include <iostream>
#include"pic.h"
#include"Encode.h"
#include"2.h"
using namespace cv;
using namespace std;

int QRtoUchar(Mat& QRcode, std::vector<int>& data);

void GetQR(const char *VideoName, const char* OutBinName, const char* VoutBinName);

bool DataToFile(const vector<int>& data, const char* BinFileName, const char* VoutBinFileName);

bool areImagesEqual(const Mat& image1, const Mat& image2);
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

int VideotoImage(const char* ffmPath, const char* imagePath,
	const char* imageFormat, const char* videoPath);

void DrowRect(Mat& image);

void OpenFile(vector<char>& data, Mat& image);

Mat BigMat(Mat& image);

void DrowQRPoint(vector<char>& data, Mat& image);
void CreatQRcode();

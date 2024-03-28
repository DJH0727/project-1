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

using namespace cv;
using namespace std;

int QRtoUchar(Mat& QRcode, std::vector<int>& data);
#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>

#define QR_SIZE 136
#define BIG_SIZE 680
#define BLACK Scalar(0, 0, 0, 255)
#define WHITE Scalar(255, 255, 255, 255)
#define RED   Scalar(0,0,255,255)//≤‚ ‘”√
#define RECT_SIZE 18
using namespace cv;
using namespace std;
bool QRjudge(vector<Point>& contours, Mat& mat);
double MinCount(Point2f rectPoints[], int flag);
double MaxCount(Point2f rectPoints[], int flag);
double angle(Point pt1, Point pt2, Point pt0);
vector<Point2f> detectCorners(const Mat& image, const vector<Point>& contour);
bool isApproxSquare(const vector<Point>& contour);
bool QRjudge1(vector<Point>& contours, Mat& mat);
Mat QRFind(Mat& mat, int& flag);
Mat ImagePreProcessing(Mat& mat);
Mat OutPutQR(Mat& outputQR, Mat& originalImage);
int FuckingShit();
#pragma once

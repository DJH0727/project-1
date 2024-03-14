#define _CRT_SECURE_NO_WARNINGS
#include "VideoToImage.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>

#define QR_SIZE 136
#define BIG_SIZE 680
#define BLACK Scalar(0, 0, 0, 255)
#define WHITE Scalar(255, 255, 255, 255)
#define RED   Scalar(0,0,255,255)//测试用
#define RECT_SIZE 18
using namespace cv;
using namespace std;

bool QRjudge(vector<Point>& contours, Mat& mat);
Mat QRFind(Mat &mat)
{
	Mat outputQR= Mat::zeros(mat.size(), CV_8UC3);
	vector<Point> center;
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	int numOfRec=0;
	findContours(mat, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
	int ic = 0;
	int parentIdx = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		if (hierarchy[i][2] != -1 && ic == 0)
		{
			parentIdx = i;
			ic++;
		}
		else if (hierarchy[i][2] != -1)
		{
			ic++;
		}
		else if (hierarchy[i][2] == -1)
		{
			parentIdx = -1;
			ic = 0;
		}
		if (ic == 2)
		{	//通过图像处理进行深层次的判断
			if (QRjudge(contours[parentIdx], mat)) {
				RotatedRect rect = minAreaRect(Mat(contours[parentIdx]));

				// 画图部分
				Point2f points[4];
				rect.points(points);
				for (int j = 0; j < 4; j++) {
					line(mat, points[j], points[(j + 1) % 4], Scalar(0, 255, 0), 2);
				}
				drawContours(outputQR, contours, parentIdx, Scalar(0, 0, 255), -1);

				// 如果满足条件则存入
				center.push_back(rect.center);
				numOfRec++;
				imwrite("output_image.jpg", outputQR);
			}
			ic = 0;
			parentIdx = -1;
		}
	}
	return outputQR;
	
}
Mat ImagePreProcessing(Mat &mat)//图像预处理
{
	Mat grayImage;
	Mat	binaryImage;
	Mat tempImage;
	//转灰度图
	cvtColor(mat, grayImage, COLOR_BGR2GRAY);
	//imwrite("output_gray_image.jpg", grayImage);
	//降噪
	double BlurSize = mat.rows * mat.cols;
	blur(grayImage, tempImage, Size(3, 3));
	//提高对比度
	convertScaleAbs(tempImage, tempImage);
	//直方图计算
	//equalizeHist(tempImage, tempImage);
	//二值化，应该还要进行摩尔纹的处理?
	threshold(grayImage, binaryImage, 0, 255, THRESH_BINARY | THRESH_OTSU);

	cv::imwrite("output_binary_image.jpg", binaryImage);
	return binaryImage;
}
bool QRjudge(vector<Point>& contours, Mat& mat)
{
	double area = contourArea(contours);
	if (area < 600)//轮廓面积太小//在环境比较杂的情况下
	{
		return 0;
	}
	//更具体的似乎还可以直接在这里进行旋转操作，但先不进行
	return 1;
}
Mat OutPutQR(Mat &outputQR,Mat &originalImage)//在这个函数调用之前应该完成旋转等调整
{
	//Point2f rectPointsNew[3][4];
	vector<Point> QRcontours;
	vector<vector<Point>> contours;
	Mat tempImage;
	Mat outputImage;
	cvtColor(outputQR, tempImage, COLOR_BGR2GRAY);
	findContours(tempImage, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
	double Area = 0,xmax=0,ymax=0,height,width,xmaxi,ymaxi,x,y;
	//cout << contours.size() << endl;
	for (int i = 0; i < contours.size(); i++)
	{
		RotatedRect rectangle = minAreaRect(contours[i]);
		Point2f rectPoints[4];
		rectangle.points(rectPoints);
		/*for (int j = 0; j < 4; j++)
			line(originalImage, rectPoints[j], rectPoints[(j + 1) % 4], Scalar(0, 0, 255), 3);
		double area = contourArea(contours[i]);
		if (area > Area)
		{
			Area = area;
			QRcontours = contours[i];
		}*/
		if (rectPoints[0].x > xmax)
		{
			xmax = rectPoints[0].x;
			xmaxi = i;
		}
		if (rectPoints[0].y > ymax)
		{
			ymax = rectPoints[0].y;
			ymaxi = i;
		}
	}
	RotatedRect rectangle;
	Point2f rectPoints[4];
	rectangle = minAreaRect(contours[xmaxi]);
	rectangle.points(rectPoints);
	y = rectPoints[0].y;//左上角识别点的y坐标与最右边识别点左上角坐标的y相同
	xmax = rectPoints[1].x;//实际取哪个可能还要比较一下，目前来看似乎是以顺时针来取
	rectangle = minAreaRect(contours[ymaxi]);
	rectangle.points(rectPoints);
	x = rectPoints[0].x;//左上角识别点的x坐标与最下边识别点左上角坐标的x相同
	ymax = rectPoints[3].y;
	height = ymax - y;
	width = xmax - x;
	Rect2f roi_rect(x, y, width, height);
	Mat roi = originalImage(roi_rect);
	Mat new_image(roi.size(), roi.type(), cv::Scalar(255, 255, 255));
	roi.copyTo(new_image);
	imwrite("output_updated_image.jpg", roi);
	imwrite("output_new_updated_image.jpg", new_image);
	return new_image;
}
int main()
{
	Mat tempImage;
	Mat mat = imread("test_1.png", IMREAD_COLOR);
	tempImage=ImagePreProcessing(mat);
	tempImage=QRFind(tempImage);
	//equalizeHist(mat, mat);
	OutPutQR(tempImage, mat);

	return 0;
}

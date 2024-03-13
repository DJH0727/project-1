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
			//if (IsQrPoint(contours[parentIdx], mat)) {
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
			//}
			ic = 0;
			parentIdx = -1;
		}
	}
	return outputQR;
	
}
Mat ImagePreProcessing()//图像预处理
{
	Mat mat = imread("test_1.png", IMREAD_COLOR);
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
	equalizeHist(tempImage, tempImage);
	//二值化，应该还要进行摩尔纹的处理?
	threshold(grayImage, binaryImage, 0, 255, THRESH_BINARY | THRESH_OTSU);

	cv::imwrite("output_binary_image.jpg", binaryImage);
	return binaryImage;
}
Mat QRcodeFind(Mat &binaryImage)
{
	Mat QRImage;
	vector<std::vector<Point>> contours;
	findContours(binaryImage, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); //寻找轮廓
	for (const auto& contour : contours) {
		Rect boundingBox = boundingRect(contour); //获取边界矩形
		Mat qrcode = QRImage(boundingBox); //裁剪图像
	}
	//imwrite("qrcode.jpg", qrcode); //保存结果图像
	return QRImage;//写不出来desu
}
int main()
{
	Mat tempImage;
	tempImage=ImagePreProcessing();
	QRFind(tempImage);

	return 0;
}

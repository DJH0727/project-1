#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <time.h>
#include <set>
#include <fstream>
#include <iostream>
using namespace cv;
using namespace std;

bool QRjudge(vector<Point>& contours, Mat& mat);
double MinCount(Point2f rectPoints[], int flag)
{
	double xmin = 1000000, ymin = 1000000;
	for (int i = 0; i < 4; i++)
	{
		if (rectPoints[i].x < xmin)
			xmin = rectPoints[i].x;
		if (rectPoints[i].y < ymin)
			ymin = rectPoints[i].y;
	}
	if (flag == 1)
		return xmin;
	else
		return ymin;
}
double MaxCount(Point2f rectPoints[], int flag)
{
	double xmax = 0, ymax = 0;
	for (int i = 0; i < 4; i++)
	{
		if (rectPoints[i].x > xmax)
			xmax = rectPoints[i].x;
		if (rectPoints[i].y > ymax)
			ymax = rectPoints[i].y;
	}
	if (flag == 1)
		return xmax;
	else
		return ymax;
}
// 计算两个向量之间的夹角
double angle(Point pt1, Point pt2, Point pt0) {
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2));
}

vector<Point2f> detectCorners(const Mat& image, const vector<Point>& contour) {
	// 将轮廓转换为矩形
	Rect rect = boundingRect(contour);

	// 截取轮廓部分的图像
	Mat roi = image(rect);

	// 使用Harris角点检测算法检测角点
	Mat corners;
	cornerHarris(roi, corners, 2, 3, 0.04);

	// 选择角点
	vector<Point2f> cornerPoints;
	for (int i = 0; i < corners.rows; ++i) {
		for (int j = 0; j < corners.cols; ++j) {
			if (corners.at<float>(i, j) > 0.01) { // 调整此阈值以适应不同的情况
				cornerPoints.push_back(Point2f(rect.x + j, rect.y + i));
			}
		}
	}

	return cornerPoints;
}

// 形状过滤
bool isApproxSquare(const vector<Point>& contour) {
	// 对轮廓进行多边形逼近
	vector<Point> approx;
	double epsilon = 0.1;
	double perimeter = arcLength(contour, true);
	approxPolyDP(contour, approx, epsilon * perimeter, true);

	// 检查逼近的多边形是否是四边形
	if (approx.size() >= 4 && isContourConvex(approx)) {
		// 计算轮廓面积
		double area = fabs(contourArea(approx));

		// 计算周长
		double perimeter = arcLength(approx, true);

		// 计算多边形的圆度
		double circularity = 4 * CV_PI * area / (perimeter * perimeter);

		// 根据需要调整此处的阈值
		if (circularity >= 0.6 && circularity <= 1.4) {
			return true;
		}
	}
	return false;
}

bool QRjudge1(vector<Point>& contours, Mat& mat)
{
	double area = contourArea(contours);
	if (area < 600)//轮廓面积太小//在环境比较杂的情况下
	{
		return 0;
	}
	//更具体的似乎还可以直接在这里进行旋转操作，但先不进行
	if (isApproxSquare(contours))
	{
		vector<Point2f> cornerPoints = detectCorners(mat, contours);
		if (cornerPoints.size() >= 3)
		{
			return 1;
		}
	}
	else return 0;
}
Mat QRFind(Mat& mat, int& flag)
{
	Mat outputQR = Mat::zeros(mat.size(), CV_8UC3);
	vector<Point> center;
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	int numOfRec = 0;
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
			if (QRjudge1(contours[parentIdx], mat)) {
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
				flag = 1;
				numOfRec++;
				imwrite("output_image.jpg", outputQR);
			}
			ic = 0;
			parentIdx = -1;
		}
	}
	return outputQR;

}
Mat ImagePreProcessing(Mat& mat)//图像预处理
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

Mat OutPutQR(Mat& outputQR, Mat& originalImage)//在这个函数调用之前应该完成旋转等调整
{
	//Point2f rectPointsNew[3][4];
	vector<Point> QRcontours;
	vector<vector<Point>> contours;
	Mat tempImage;
	Mat outputImage;
	cvtColor(outputQR, tempImage, COLOR_BGR2GRAY);
	findContours(tempImage, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
	double Area = 0, xmax = 0, ymax = 0, height, width, xmaxi, ymaxi, x, y;
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
		double tempx, tempy;
		tempx = MinCount(rectPoints, 1);
		tempy = MinCount(rectPoints, 2);//可以和之后的进行一个小优化
		if (tempx > xmax)
		{
			xmax = tempx;
			xmaxi = i;
		}
		if (tempy > ymax)
		{
			ymax = tempy;
			ymaxi = i;
		}
		cout << tempx << endl << tempy << endl;
	}
	RotatedRect rectangle;
	Point2f rectPoints[4];
	rectangle = minAreaRect(contours[xmaxi]);
	rectangle.points(rectPoints);
	y = MinCount(rectPoints, 2);//左上角识别点的y坐标与最右边识别点左上角坐标的y相同
	xmax = MaxCount(rectPoints, 1);//实际取哪个可能还要比较一下，目前来看似乎是以顺时针来取
	rectangle = minAreaRect(contours[ymaxi]);
	rectangle.points(rectPoints);
	x = MinCount(rectPoints, 1);//左上角识别点的x坐标与最下边识别点左上角坐标的x相同
	ymax = MaxCount(rectPoints, 2);
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
int imageprcessing(char imageName[], char imageFormat[])
{
	int flag = 0;
	char image[256];
	char format[256];
	cin >> imageName;
	cin >> imageFormat;
	snprintf(image, 256, "%s.%s", imageName, imageFormat);
	snprintf(format, 256, ".%s", imageFormat);
	Mat tempImage;
	Mat mat = imread(image, IMREAD_COLOR);
	tempImage = ImagePreProcessing(mat);
	tempImage = QRFind(tempImage, flag);
	if (!flag)
		return 0;
	//equalizeHist(mat, mat);
	OutPutQR(tempImage, mat);

	return 1;
}
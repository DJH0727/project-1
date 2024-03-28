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
// ������������֮��ļн�
double angle(Point pt1, Point pt2, Point pt0) {
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2));
}

vector<Point2f> detectCorners(const Mat& image, const vector<Point>& contour) {
	// ������ת��Ϊ����
	Rect rect = boundingRect(contour);

	// ��ȡ�������ֵ�ͼ��
	Mat roi = image(rect);

	// ʹ��Harris�ǵ����㷨���ǵ�
	Mat corners;
	cornerHarris(roi, corners, 2, 3, 0.04);

	// ѡ��ǵ�
	vector<Point2f> cornerPoints;
	for (int i = 0; i < corners.rows; ++i) {
		for (int j = 0; j < corners.cols; ++j) {
			if (corners.at<float>(i, j) > 0.01) { // ��������ֵ����Ӧ��ͬ�����
				cornerPoints.push_back(Point2f(rect.x + j, rect.y + i));
			}
		}
	}

	return cornerPoints;
}

// ��״����
bool isApproxSquare(const vector<Point>& contour) {
	// ���������ж���αƽ�
	vector<Point> approx;
	double epsilon = 0.1;
	double perimeter = arcLength(contour, true);
	approxPolyDP(contour, approx, epsilon * perimeter, true);

	// ���ƽ��Ķ�����Ƿ����ı���
	if (approx.size() >= 4 && isContourConvex(approx)) {
		// �����������
		double area = fabs(contourArea(approx));

		// �����ܳ�
		double perimeter = arcLength(approx, true);

		// �������ε�Բ��
		double circularity = 4 * CV_PI * area / (perimeter * perimeter);

		// ������Ҫ�����˴�����ֵ
		if (circularity >= 0.6 && circularity <= 1.4) {
			return true;
		}
	}
	return false;
}

bool QRjudge1(vector<Point>& contours, Mat& mat)
{
	double area = contourArea(contours);
	if (area < 600)//�������̫С//�ڻ����Ƚ��ӵ������
	{
		return 0;
	}
	//��������ƺ�������ֱ�������������ת���������Ȳ�����
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
		{	//ͨ��ͼ����������ε��ж�
			if (QRjudge1(contours[parentIdx], mat)) {
				RotatedRect rect = minAreaRect(Mat(contours[parentIdx]));

				// ��ͼ����
				Point2f points[4];
				rect.points(points);
				for (int j = 0; j < 4; j++) {
					line(mat, points[j], points[(j + 1) % 4], Scalar(0, 255, 0), 2);
				}
				drawContours(outputQR, contours, parentIdx, Scalar(0, 0, 255), -1);

				// ����������������
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
Mat ImagePreProcessing(Mat& mat)//ͼ��Ԥ����
{
	Mat grayImage;
	Mat	binaryImage;
	Mat tempImage;
	//ת�Ҷ�ͼ
	cvtColor(mat, grayImage, COLOR_BGR2GRAY);
	//imwrite("output_gray_image.jpg", grayImage);
	//����
	double BlurSize = mat.rows * mat.cols;
	blur(grayImage, tempImage, Size(3, 3));
	//��߶Աȶ�
	convertScaleAbs(tempImage, tempImage);
	//ֱ��ͼ����
	//equalizeHist(tempImage, tempImage);
	//��ֵ����Ӧ�û�Ҫ����Ħ���ƵĴ���?
	threshold(grayImage, binaryImage, 0, 255, THRESH_BINARY | THRESH_OTSU);

	cv::imwrite("output_binary_image.jpg", binaryImage);
	return binaryImage;
}

Mat OutPutQR(Mat& outputQR, Mat& originalImage)//�������������֮ǰӦ�������ת�ȵ���
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
		tempy = MinCount(rectPoints, 2);//���Ժ�֮��Ľ���һ��С�Ż�
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
	y = MinCount(rectPoints, 2);//���Ͻ�ʶ����y���������ұ�ʶ������Ͻ������y��ͬ
	xmax = MaxCount(rectPoints, 1);//ʵ��ȡ�ĸ����ܻ�Ҫ�Ƚ�һ�£�Ŀǰ�����ƺ�����˳ʱ����ȡ
	rectangle = minAreaRect(contours[ymaxi]);
	rectangle.points(rectPoints);
	x = MinCount(rectPoints, 1);//���Ͻ�ʶ����x���������±�ʶ������Ͻ������x��ͬ
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
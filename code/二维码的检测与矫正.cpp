#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <time.h>
#include <set>
using namespace cv;
using namespace std;
int main(int argc, char** argv)
{
	String srcImagePath("D:\\test\\333.JPG"); // 原图路径
	if (argc > 1) {
		srcImagePath = argv[1];
	}

	Mat srcImage;
	srcImage = imread(srcImagePath, IMREAD_COLOR); // 载入初始图片
	resize(srcImage, srcImage, Size(500, 500));   //图片缩放为500*500进行后续计算
	imwrite("D:\\test\\111.JPG", srcImage);   //保存图片
	double alpha = 1.8;  //对比度因子
	int beta = -30;   //亮度因子
	for (int y = 0; y < srcImage.rows; y++) {
		for (int x = 0; x < srcImage.cols; x++) {
			for (int c = 0; c < 3; c++) {
				srcImage.at<Vec3b>(y, x)[c] =
					saturate_cast<uchar>(alpha * (srcImage.at<Vec3b>(y, x)[c]) + beta);
			}
		}
	}
	cvtColor(srcImage, srcImage, COLOR_BGR2GRAY);  //转化为灰度图
	imwrite("D:\\test\\111.JPG", srcImage);   //保存图片
	Mat filterImage;
	bilateralFilter(srcImage, filterImage, 13, 26, 6);  //双边滤波
	//medianBlur ( grayImage, filterImage, 3 );          //中值滤波
	imwrite("D:\\test\\111.JPG", srcImage);   //保存图片
	threshold(filterImage, srcImage, 210, 255, THRESH_BINARY_INV); //二值化
	imwrite("D:\\test\\111.JPG", srcImage);   //保存图片
	Mat erodeImage;
	erode(srcImage, erodeImage, Mat(), Point(-1, -1), 2);  //腐蚀化
	Mat dilateImage;
	dilate(erodeImage, dilateImage, Mat(), Point(-1, -1), 19); //膨胀化
	Mat cannyImage;
	Canny(dilateImage, cannyImage, 10, 100, 3, false); //canny边缘检测
	imwrite("D:\\test\\111.JPG", cannyImage);
	//********在canny图上画出所有hough拟合直线

	Mat allLinesImage = cannyImage.clone();
	vector<Vec2f> lines;
	HoughLines(allLinesImage, lines, 5, CV_PI / 180, 100);   //hough算子拟合直线
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0];
		float theta = lines[i][1];
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;
		Point pt1(cvRound(x0 + 1000 * (-b)),
			cvRound(y0 + 1000 * (a)));
		Point pt2(cvRound(x0 - 1000 * (-b)),
			cvRound(y0 - 1000 * (a)));
		line(allLinesImage, pt1, pt2, Scalar(255), 1, 8);
	}
	imwrite("D:\\test\\111.JPG", allLinesImage);
	//********删除相似直线直到只剩4条拟合直线
	double A = 50.0;
	double B = CV_PI / 180 * 20; //20度
	//Mat resLinesImage = cannyImage.clone();
	vector<Vec2f> resLines(lines);
	set<size_t> removeIndex;
	int countLess4 = 0;
	int countMore4 = 0;
	while (1) {
		for (size_t i = 0; i < resLines.size(); i++) {
			for (size_t j = i + 1; j < resLines.size(); j++) {
				float rho1 = resLines[i][0];
				float theta1 = resLines[i][1];
				float rho2 = resLines[j][0];
				float theta2 = resLines[j][1];

				//theta大于pi，减去进行统一
				if (theta1 > CV_PI) theta1 = theta1 - CV_PI;
				if (theta2 > CV_PI) theta2 = theta2 - CV_PI;

				//记录需要删除的lines
				bool thetaFlag = abs(theta1 - theta2) <= B ||
					(theta1 > CV_PI / 2 && theta2 < CV_PI / 2 && CV_PI - theta1 + theta2 < B) ||
					(theta2 > CV_PI / 2 && theta1 < CV_PI / 2 && CV_PI - theta2 + theta1 < B);

				if (abs(abs(rho1) - abs(rho2)) <= A && thetaFlag) {
					removeIndex.insert(j);
				}
			}
		}
		//删除多余的lines
		vector<Vec2f> res;
		for (int i = 0; i < resLines.size(); i++) {
			if (removeIndex.count(i) == 0) {
				res.push_back(resLines[i]);
			}
		}
		resLines = res;
		//直到删除只剩4条直线。
		if (resLines.size() > 4) {
			A = A + 4;
			B = B + 2 * CV_PI / 180;
			countMore4++;
			if (countMore4 % 50 == 0)
				cout << "countMore4：" << countMore4 << endl;

		}
		else if (resLines.size() < 4) {
			B = B - CV_PI / 180;
			countLess4++;
			if (countLess4 % 50 == 0)
				cout << "countLess4：" << countLess4 << endl;
		}
		else {
			cout << "删除后的剩余直线个数：" << resLines.size() << endl;
			break;
		}
	}
	//********在canny图上画出剩下的4条拟合直线
	Mat fourLinesImage = cannyImage.clone();
	vector<Vec2f> fourLines(resLines);
	for (size_t i = 0; i < fourLines.size(); i++)
	{
		float rho = fourLines[i][0];
		float theta = fourLines[i][1];
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;
		Point pt1(cvRound(x0 + 1000 * (-b)),
			cvRound(y0 + 1000 * (a)));
		Point pt2(cvRound(x0 - 1000 * (-b)),
			cvRound(y0 - 1000 * (a)));
		line(fourLinesImage, pt1, pt2, Scalar(255), 1, 8);
	}
	imwrite("D:\\test\\111.JPG", fourLinesImage);
	//********求出四条定位直线在图像界内的四个交点。
	double threshold = 0.2 * min(1.0 * srcImage.rows, 1.0 * srcImage.cols);
	vector<Point> points;
	for (int i = 0; i < fourLines.size(); i++) {
		for (int j = i + 1; j < fourLines.size(); j++) {
			double rho1 = fourLines[i][0];
			double theta1 = fourLines[i][1];
			double rho2 = fourLines[j][0];
			double theta2 = fourLines[j][1];
			//消除theta等于零导致斜率无法计算的情况
			if (theta1 == 0) theta1 = 0.01;
			if (theta2 == 0) theta2 = 0.01;

			double a1 = cos(theta1), a2 = cos(theta2);
			double b1 = sin(theta1), b2 = sin(theta2);

			double x = (rho2 * b1 - rho1 * b2) / (a2 * b1 - a1 * b2);  //直线交点公式
			double y = (rho1 - a1 * x) / b1;
			Point pt(cvRound(x), cvRound(y));

			if (pt.x <= srcImage.cols + threshold && pt.x >= 0 - threshold
				&& pt.y < srcImage.rows + threshold && pt.y >= 0 - threshold) {
				points.push_back(pt);
			}
		}
	}
	//********将获取到的交点按顺时针排序并保存在sortPoints中
	Point sortPoints[4];
	double min_x = 99999.9;
	int index = -1;
	for (int i = 0; i < points.size(); i++) {
		if (min_x > points[i].x) {
			min_x = points[i].x;
			index = i;
		}
	}
	Point left = points[index];
	points.erase(points.begin() + index);  //删除第index个元素
	sortPoints[0] = left;
	int autonum = 1;
	while (points.size() != 0) {
		double mingrad = 99999.9;
		int idx = -1;
		for (int i = 0; i < points.size(); i++) {
			double curgrad = (points[i].y - left.y) * 1.0 / (points[i].x - left.x);
			if (mingrad > curgrad) {
				mingrad = curgrad;
				idx = i;
			}
		}
		sortPoints[autonum++] = points[idx];
		points.erase(points.begin() + idx);
	}
	//********按照sortPoints四个点进行仿射变换
	int minSide = min(srcImage.rows, srcImage.cols);
	int center_x = srcImage.rows / 2;
	int center_y = srcImage.cols / 2;
	Point2f srcTri[4];
	Point2f dstTri[4];
	srcTri[0] = Point2f(sortPoints[0].x, sortPoints[0].y);
	srcTri[1] = Point2f(sortPoints[1].x, sortPoints[1].y);
	srcTri[2] = Point2f(sortPoints[2].x, sortPoints[2].y);
	srcTri[3] = Point2f(sortPoints[3].x, sortPoints[3].y);

	dstTri[0] = Point2f(center_x - 0.45 * minSide, center_y - 0.45 * minSide);
	dstTri[1] = Point2f(center_x + 0.45 * minSide, center_y - 0.45 * minSide);
	dstTri[2] = Point2f(center_x + 0.45 * minSide, center_y + 0.45 * minSide);
	dstTri[3] = Point2f(center_x - 0.45 * minSide, center_y + 0.45 * minSide);

	Mat perspImage = Mat::zeros(srcImage.rows, srcImage.cols, srcImage.type());
	// 提取图像映射
	Mat transmtx = getPerspectiveTransform(srcTri, dstTri);

	Mat binImage;
	bitwise_not(srcImage, binImage); //二值图像反色
	warpPerspective(binImage, perspImage, transmtx, perspImage.size());
	imwrite("D:\\test\\111.JPG", perspImage);
	return 0;
}
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

#define QR_SIZE 64  // 定义二维码的大小
#define BLACK cv::Vec3b(0, 0, 0)  // 定义颜色黑色
#define WHITE cv::Vec3b(255, 255, 255)  // 定义颜色白色
#define RECT_SIZE 12  // 定义定位图案的大小

int OpenFile(const std::string& filePath, std::vector<int>& data);
void DrowRect(cv::Mat& image);
void DrawQRPoint(const std::vector<int>& data, cv::Mat& image);
cv::Mat ScaleMat(const cv::Mat& image);
void CreatQRcode(const std::string& filePath);
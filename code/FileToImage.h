#ifndef QR_CODE_GENERATOR_H
#define QR_CODE_GENERATOR_H

#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

#define QR_SIZE 128
#define BLACK cv::Scalar(0, 0, 0)
#define WHITE cv::Scalar(255, 255, 255)
#define RECT_SIZE 24

int OpenFile(const std::string& filePath, std::vector<int>& data);
void DrowRect(cv::Mat& image);
void DrawQRPoint(const std::vector<int>& data, cv::Mat& image);
cv::Mat BigMat(const cv::Mat& image);
void CreatQRcode(const std::string& filePath);

#endif // QR_CODE_GENERATOR_H

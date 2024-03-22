#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>


#define QR_SIZE 64*10  // 二维码的大小
#define RECT_SIZE 12*10  // 定位图案的大小
#define COLOR_THRESHOLD 128  // 颜色阈值

// 从二维码图片中读取数据并转换为二进制串
void ExtractQRCode(const std::string& filePath, std::vector<int>& data)
{
    cv::Mat image = cv::imread(filePath); // 读取二维码图片

    
    cv::resize(image, image, cv::Size(QR_SIZE, QR_SIZE));// opencv扩大图片尺寸函数

    for (int i = 0; i < QR_SIZE; i=i+10) {
        for (int j = 0; j < QR_SIZE; j=j+10) {
            cv::Vec3b pixel = image.at<cv::Vec3b>(i, j);//这里之后会增加一个平均数识别

            if ((i < RECT_SIZE && j < RECT_SIZE) ||
                (i < RECT_SIZE && j >= QR_SIZE - RECT_SIZE) ||
                (i >= QR_SIZE - RECT_SIZE && j < RECT_SIZE)) {
                continue; // 跳过定位点
            }
            int grayscale = (pixel[0] + pixel[1] + pixel[2]) / 3; // 灰度值为RGB分量的平均值
            if (grayscale > COLOR_THRESHOLD) {
                data.push_back(1); // 黑色像素对应二进制中的1
            }
            else {
                data.push_back(0); // 白色像素对应二进制中的0
            }
        }
    }
}

// 输出二进制串到屏幕
void PrintBinaryData(const std::vector<int>& data)
{
    std::cout << "二维码的二进制串：" << std::endl;
    for (int bit : data) {
        std::cout << bit;
    }
    std::cout << std::endl;
}

// 将二进制数据写入文件
void WriteBinaryDataToFile(const std::string& filePath, const std::vector<int>& data)
{
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file for writing." << std::endl;
        return;
    }

    for (int bit : data) {
        file << bit;
    }

    file.close();
}

int main()
{
    std::string filePath;
    std::cout << "请输入二维码图片的路径：";
    std::cin >> filePath;

    std::vector<int> binaryData;
    ExtractQRCode(filePath, binaryData); // 从二维码图片中提取数据

    PrintBinaryData(binaryData); // 输出二进制串到屏幕

    std::string outputFilePath = "output.txt"; // 文件输出路径
    WriteBinaryDataToFile(outputFilePath, binaryData); // 将二进制数据写入文件

    std::cout << "已成功将二维码数据写入文件：output.txt" <<std::endl;
    return 0;
}

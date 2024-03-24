//增加10x10像素块平均识别，提高准确率，加入了在文件末尾写入的功能
#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>

#define QR_SIZE 64*10  // 二维码的大小
#define RECT_SIZE 12*10  // 定位图案的大小
#define COLOR_THRESHOLD 128  // 颜色阈值
#define BLOCK_SIZE 10 // 每个块的大小

// 计算一个10x10像素块的平均灰度值
int CalculateBlockAverage(cv::Mat& image, int start_i, int start_j)
{
    int total_grayscale = 0;

    for (int i = start_i; i < start_i + BLOCK_SIZE; ++i) {
        for (int j = start_j; j < start_j + BLOCK_SIZE; ++j) {
            cv::Vec3b pixel = image.at<cv::Vec3b>(i, j);
            total_grayscale += (pixel[0] + pixel[1] + pixel[2]) / 3;
        }
    }

    return total_grayscale / (BLOCK_SIZE * BLOCK_SIZE);
}

// 从二维码图片中读取数据并转换为二进制串
void ExtractQRCode(const std::string& filePath, std::vector<int>& data)
{
    cv::Mat image = cv::imread(filePath); // 读取二维码图片

    for (int i = 0; i < QR_SIZE; i += BLOCK_SIZE) {
        for (int j = 0; j < QR_SIZE; j += BLOCK_SIZE) {
            int block_grayscale = CalculateBlockAverage(image, i, j);

            if ((i < RECT_SIZE && j < RECT_SIZE) ||
                (i < RECT_SIZE && j >= QR_SIZE - RECT_SIZE) ||
                (i >= QR_SIZE - RECT_SIZE && j < RECT_SIZE)) {
                continue; // 跳过定位点
            }

            if (block_grayscale > COLOR_THRESHOLD) {
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

// 将二进制数据写入文件（在文件末尾继续写入）
void WriteBinaryDataToFile(const std::string& filePath, const std::vector<int>& data)
{
    std::ofstream file(filePath, std::ios::app); // 打开文件，追加模式

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
    WriteBinaryDataToFile(outputFilePath, binaryData); // 将二进制数据写入文件（在文件末尾继续写入）

    std::cout << "已成功将二维码数据写入文件：output.txt" << std::endl;
    return 0;
}

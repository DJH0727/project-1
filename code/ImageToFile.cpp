//更改功能，读取code文件夹中所有二维码，写入output.txt,但解码部分对不上
#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>

#define QR_SIZE 64  // 二维码的大小
#define RECT_SIZE 12  // 定位图案的大小
#define COLOR_THRESHOLD 128  // 颜色阈值

// 从二维码图片中读取数据并转换为二进制串
void ExtractQRCode(const std::string& filePath, std::vector<int>& data)
{
    cv::Mat image = cv::imread(filePath, cv::IMREAD_GRAYSCALE); // 读取二维码图片并转换为灰度图像

    cv::resize(image, image, cv::Size(QR_SIZE, QR_SIZE)); // 调整图像尺寸为二维码大小
    int borderSize = RECT_SIZE / 9;

    for (int i = borderSize - 1; i <= QR_SIZE - borderSize; i++) {
        for (int j = borderSize - 1; j <= QR_SIZE - borderSize; j++) {
            // 跳过定位图案区域
            if ((i < RECT_SIZE + borderSize && j < RECT_SIZE + borderSize) ||
                (i < RECT_SIZE + borderSize && j >= QR_SIZE - RECT_SIZE + borderSize) ||
                (i >= QR_SIZE - RECT_SIZE + borderSize && j < RECT_SIZE + borderSize)) {
                continue;
            }

            uchar pixel = image.at<uchar>(i, j); // 获取灰度图像中的像素值

            if (pixel > COLOR_THRESHOLD) {
                data.push_back(1); // 黑色像素对应二进制中的1
            }
            else {
                data.push_back(0); // 白色像素对应二进制中的0
            }
        }
    }
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
    // 遍历code文件夹中的所有文件
    std::string folderPath = "code/";
    std::vector<cv::String> fileNames;
    cv::glob(folderPath, fileNames);

    // 遍历每个文件并解码
    for (const auto& fileName : fileNames) {
        std::vector<int> binaryData;
        ExtractQRCode(fileName, binaryData); // 从二维码图片中提取数据

        // 写入解码数据到 output.txt
        WriteBinaryDataToFile(folderPath + "output.txt", binaryData);
        
    }
    std::cout << "已成功将解码的二维码数据写入文件：output.txt" << std::endl;
    return 0;
}

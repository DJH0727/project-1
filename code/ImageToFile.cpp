#include <opencv2/opencv.hpp>
#include <fstream>
#include <vector>
#include <iostream>

#define QR_SIZE 64  // 二维码的大小
#define RECT_SIZE 12  // 定位图案的大小
#define colorThreshold 128

// 从二维码图片中读取数据并转换为二进制串
void ExtractQRCode(const std::string& filePath, std::vector<int>& data) {
    cv::Mat image = cv::imread(filePath); // 读取二维码图片
    if (image.empty()) {
        std::cerr << "Error: Unable to open image file." << std::endl;
        return;
    }

    cv::resize(image, image, cv::Size(QR_SIZE, QR_SIZE)); // 调整图像尺寸为二维码大小
    int borderSize = RECT_SIZE / 9;

    for (int i = borderSize - 1; i <= QR_SIZE - borderSize; i++) {
        for (int j = borderSize - 1; j <= QR_SIZE - borderSize; j++) {
            // 跳过定位图案区域
            if ((i < RECT_SIZE + borderSize && j < RECT_SIZE + borderSize) ||
                (i < RECT_SIZE + borderSize && j >= QR_SIZE - RECT_SIZE - borderSize) ||
                (i >= QR_SIZE - RECT_SIZE - borderSize && j < RECT_SIZE + borderSize)) {
                continue;
            }
            cv::Vec3b pixel = image.at<cv::Vec3b>(i, j);
            int grayscale = (pixel[0] + pixel[1] + pixel[2]) / 3; // 灰度值为RGB分量的平均值
            data.push_back(grayscale > colorThreshold ? 1 : 0); // 根据阈值判断黑白
        }
    }
}
void WriteBinaryDataToBinFile(const std::string& filePath, const std::vector<int>& data) {
    std::ofstream outputFile(filePath, std::ios::binary | std::ios::app);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Unable to open output file." << std::endl;
        return;
    }

    unsigned char byte = 0; // 用于保存每个字节的数据
    int bitCount = 0; // 记录当前字节中已经写入的比特数量

    for (int value : data) {
        byte |= (value & 0x01) << bitCount; // 将当前比特写入 byte 中
        bitCount++;

        if (bitCount == 8) { // 如果当前字节已满，则写入文件并重置
            outputFile.write(reinterpret_cast<const char*>(&byte), sizeof(byte));
            byte = 0;
            bitCount = 0;
        }
    }

    // 如果还有剩余比特未写入文件，则写入最后一个字节
    if (bitCount > 0) {
        outputFile.write(reinterpret_cast<const char*>(&byte), sizeof(byte));
    }

    outputFile.close();
}




int main() {
    std::string folderPath = "code/";
    std::vector<cv::String> fileNames;
    cv::glob(folderPath, fileNames);

    for (const auto& fileName : fileNames) {
        std::vector<int> binaryData;
        ExtractQRCode(fileName, binaryData);

        std::string outputFilePath = folderPath + "output.bin";
        WriteBinaryDataToBinFile(outputFilePath, binaryData);
    }
    std::cout << "已成功将解码的二维码数据写入 code 文件夹 output.bin 文件" << std::endl;
    return 0;
}

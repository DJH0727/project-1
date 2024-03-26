#include <opencv2/opencv.hpp>
#include <fstream>
#include <vector>
#define QR_SIZE 64  // 二维码的大小
#define RECT_SIZE 12  // 定位图案的大小
#define COLOR_THRESHOLD 128  // 颜色阈值

// 从二维码图片中读取数据并转换为二进制串
void ExtractQRCode(const std::string& filePath, std::vector<int>& data)
{
    cv::Mat image = cv::imread(filePath); // 读取二维码图片

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
            cv::Vec3b pixel = image.at<cv::Vec3b>(i, j);

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





// 将二进制数据写入二进制文件
void WriteBinaryDataToBinFile(const std::string& filePath, const std::vector<int>& data)
{
    std::ofstream file(filePath, std::ios::binary); // 以二进制模式打开文件

    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file for writing." << std::endl;
        return;
    }

    // 将二进制数据写入文件
    for (int bit : data) {
        char byte = static_cast<char>(bit);
        file.write(&byte, sizeof(char));
    }

    file.close();
}

int main()
{
    std::string folderPath = "code/";
    std::vector<cv::String> fileNames;
    cv::glob(folderPath, fileNames);

    for (const auto& fileName : fileNames) {
        std::vector<int> binaryData;
        ExtractQRCode(fileName, binaryData);

        std::string outputFilePath = folderPath + "output.bin";
        WriteBinaryDataToBinFile(outputFilePath, binaryData);
    }
    std::cout << "已成功将解码的二维码数据写入code文件夹output.bin文件"  << std::endl;
    return 0;
}

#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <Windows.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>

#define QR_SIZE 128  // 定义二维码的大小
#define BLACK cv::Vec3b(0, 0, 0)  // 定义颜色黑色
#define WHITE cv::Vec3b(255, 255, 255)  // 定义颜色白色
#define RECT_SIZE 18  // 定义定位图案的大小

int OpenFile(const std::string& filePath, std::vector<int>& data);
void DrawRect(cv::Mat& image);
void DrawQRPoint(const std::vector<int>& data, cv::Mat& image);
cv::Mat ScaleMat(const cv::Mat& image);
void CreatQRcode(const std::string& filePath);
int ImagetoVideo(const char* ffmPath, unsigned int videofps, const char* imagePath,
    const char* imageFormat, const char* videoOutput, unsigned int bitRates);

//读取文件二进制数据
int OpenFile(const std::string& filePath, std::vector<int>& data)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return 1;
    }

    // 逐位读取文件内容并转换为0和1
    char byte;
    while (file.get(byte)) {
        for (int i = 7; i >= 0; --i) {
            data.push_back((byte >> i) & 1);
        }
    }

    file.close();

    return 0;
}

// 绘制定位图案
void DrawRect(cv::Mat& image)
{
    int BorderSize = RECT_SIZE / 9;  // 边框大小
    int axis[3][2] = { {0, 0},{QR_SIZE - RECT_SIZE - 1, 0},{0, QR_SIZE - RECT_SIZE - 1} }; // 左上角、右上角、左下角
    for (int j = 0; j < 3; j++)
    {
        int x = axis[j][0], y = axis[j][1];

        for (int i = 0; i <= 3; i++)
        {
            if (i % 2 == 0)
                cv::rectangle(image, cv::Point(x + i * BorderSize, y + i * BorderSize), cv::Point(x + RECT_SIZE - i * BorderSize, y + RECT_SIZE - i * BorderSize), WHITE, -1);  // 绘制白色方块
            else
                cv::rectangle(image, cv::Point(x + i * BorderSize, y + i * BorderSize), cv::Point(x + RECT_SIZE - i * BorderSize, y + RECT_SIZE - i * BorderSize), BLACK, -1);  // 绘制黑色方块
        }
    }
}

// 从上往下绘制二维码点
void DrawQRPoint(const std::vector<int>& data, cv::Mat& image)
{
    int cnt = 0;
    for (int i = 0; i < QR_SIZE - RECT_SIZE / 9; i++)
    {
        if (i <= RECT_SIZE) // 在两个定位图案之间
        {
            for (int j = 0; j < QR_SIZE - 2 * RECT_SIZE - 2; j++)
            {
                if (cnt < data.size())
                {
                    if (data[cnt] == 1)
                    {
                        image.at<cv::Vec3b>(RECT_SIZE / 9 + i, RECT_SIZE + j + 1) = BLACK;  // 设置黑色点
                    }
                    cnt++;
                }
            }
        }
        else if (i > RECT_SIZE && i <= QR_SIZE - RECT_SIZE - RECT_SIZE / 9) // 图像中间部分
        {
            for (int j = 0; j < QR_SIZE - 2 * RECT_SIZE / 9; j++)
            {
                if (cnt < data.size())
                {
                    if (data[cnt] == 1)
                    {
                        image.at<cv::Vec3b>(i, RECT_SIZE / 9 + j) = BLACK;  // 设置黑色点
                    }
                    cnt++;
                }
            }
        }
        else // 底部右侧定位图案的中间部分
        {
            for (int j = 0; j < QR_SIZE - RECT_SIZE - RECT_SIZE / 9 - 1; j++)
            {
                if (cnt < data.size())
                {
                    if (data[cnt] == 1)
                    {
                        image.at<cv::Vec3b>(i, RECT_SIZE + j + 1) = BLACK;  // 设置黑色点
                    }
                    cnt++;
                }
            }
        }
    }
}

// 放大图像
cv::Mat ScaleMat(const cv::Mat& image)
{
    int rate = 10;  // 放大倍数
    int BiggerSize = rate * QR_SIZE;  // 放大后的尺寸
    cv::Mat BiggerMat(BiggerSize, BiggerSize, CV_8UC3);  // 创建放大后的图像
    for (int i = 0; i < BiggerSize; i++)
    {
        for (int j = 0; j < BiggerSize; j++)
            BiggerMat.at<cv::Vec3b>(i, j) = image.at<cv::Vec3b>(i / rate, j / rate);  // 根据放大倍数进行插值放大
    }
    return BiggerMat;
}

// 生成二维码
void CreatQRcode(const std::string& filePath)
{
    std::vector<int> DATA;
    OpenFile(filePath, DATA);  // 打开二进制文件并读取数据

    int numQRCodes = (DATA.size() + QR_SIZE * QR_SIZE - 1) / (QR_SIZE * QR_SIZE);  // 计算生成的二维码数量

    for (int i = 0; i < numQRCodes; i++)
    {
        cv::Mat QR(QR_SIZE, QR_SIZE, CV_8UC3, WHITE);  // 创建白色背景的图像
        cv::Mat BigQR;

        DrawRect(QR);  // 绘制定位图案

        std::vector<int> subData(DATA.begin() + i * QR_SIZE * QR_SIZE, DATA.begin() + std::min<size_t>((i + 1) * QR_SIZE * QR_SIZE, DATA.size()));

        DrawQRPoint(subData, QR);  // 绘制二维码点

        BigQR = ScaleMat(QR);  // 放大图像

        std::string fileName = "code\\" + std::to_string(i) + ".jpg";
        cv::imwrite(fileName, BigQR);  // 将二维码保存为图片文件
    }
}

int ImagetoVideo(const char* ffmPath, unsigned int videofps, const char* imagePath,
    const char* imageFormat, const char* videoOutput, unsigned int bitRates)
{
    char systemCommand[256];
    snprintf(systemCommand, 256, "\"%s\"ffmpeg.exe -r %u -f image2 -i %s\\%%d.%s -b:v %uK %s",
        ffmPath, videofps, imagePath, imageFormat, bitRates, videoOutput);
    //std::cout << systemCommand<<std::endl;
    return system(systemCommand);
}

int main()
{
    std::string filePath, videoOutput;
    unsigned int videoLength, videoFps;
    std::cout << "请输入二进制文件的路径：";
    std::cin >> filePath;
    std::cout << "请输入视频输出路径：";
    std::cin >> videoOutput;
    std::cout << "请输入输出视频长度（秒）：";
    std::cin >> videoLength;
    std::cout << "请输入输出视频帧率：";
    std::cin >> videoFps;

    // 创建存放二维码的文件夹
    system("mkdir code");

    CreatQRcode(filePath);  // 生成二维码

    // 将生成的二维码图片转换成视频
    ImagetoVideo("", videoFps, "code", "jpg", videoOutput.c_str(), 2000);

    std::cout << "已成功将二维码存至code文件夹处，并转换为视频并保存至指定路径" << std::endl;

    return 0;
}


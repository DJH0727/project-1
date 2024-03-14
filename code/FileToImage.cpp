#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

#define QR_SIZE 64  // 定义二维码的大小
#define BLACK cv::Vec3b(0, 0, 0)  // 定义颜色黑色
#define WHITE cv::Vec3b(255, 255, 255)  // 定义颜色白色
#define RECT_SIZE 12  // 定义定位图案的大小

// 打开二进制文件并读取数据
int OpenFile(const std::string& filePath, std::vector<int>& data)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return 1;
    }

    char c;
    while (file.get(c)) {
        if (c == '0') {
            data.push_back(0);
        }
        else if (c == '1') {
            data.push_back(1);
        }
    }

    file.close();

    return 0;
}

// 绘制定位图案
void DrowRect(cv::Mat& image)
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

        DrowRect(QR);  // 绘制定位图案

        std::vector<int> subData(DATA.begin() + i * QR_SIZE * QR_SIZE, DATA.begin() + std::min<size_t>((i + 1) * QR_SIZE * QR_SIZE, DATA.size()));

        DrawQRPoint(subData, QR);  // 绘制二维码点

        BigQR = ScaleMat(QR);  // 放大图像

        std::string fileName = "code/QR_" + std::to_string(i) + ".jpg";
        cv::imwrite(fileName, BigQR);  // 将二维码保存为图片文件
    }
}

int main()
{
    std::string filePath;
    std::cout << "请输入二进制文件的路径：";
    std::cin >> filePath;

    // 创建存放二维码的文件夹
    system("mkdir code");

    CreatQRcode(filePath);  // 生成二维码

    std::cout << "已成功将二维码存至code文件夹处" << std::endl;

    return 0;
}

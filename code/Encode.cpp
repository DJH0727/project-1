#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <Windows.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>

#define QR_SIZE 128  // �����ά��Ĵ�С
#define BLACK cv::Vec3b(0, 0, 0)  // ������ɫ��ɫ
#define WHITE cv::Vec3b(255, 255, 255)  // ������ɫ��ɫ
#define RECT_SIZE 18  // ���嶨λͼ���Ĵ�С

int OpenFile(const std::string& filePath, std::vector<int>& data);
void DrawRect(cv::Mat& image);
void DrawQRPoint(const std::vector<int>& data, cv::Mat& image);
cv::Mat ScaleMat(const cv::Mat& image);
void CreatQRcode(const std::string& filePath);
int ImagetoVideo(const char* ffmPath, unsigned int videofps, const char* imagePath,
    const char* imageFormat, const char* videoOutput, unsigned int bitRates);

// �򿪶������ļ�����ȡ����
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

// ���ƶ�λͼ��
void DrawRect(cv::Mat& image)
{
    int BorderSize = RECT_SIZE / 9;  // �߿��С
    int axis[3][2] = { {0, 0},{QR_SIZE - RECT_SIZE - 1, 0},{0, QR_SIZE - RECT_SIZE - 1} }; // ���Ͻǡ����Ͻǡ����½�
    for (int j = 0; j < 3; j++)
    {
        int x = axis[j][0], y = axis[j][1];

        for (int i = 0; i <= 3; i++)
        {
            if (i % 2 == 0)
                cv::rectangle(image, cv::Point(x + i * BorderSize, y + i * BorderSize), cv::Point(x + RECT_SIZE - i * BorderSize, y + RECT_SIZE - i * BorderSize), WHITE, -1);  // ���ư�ɫ����
            else
                cv::rectangle(image, cv::Point(x + i * BorderSize, y + i * BorderSize), cv::Point(x + RECT_SIZE - i * BorderSize, y + RECT_SIZE - i * BorderSize), BLACK, -1);  // ���ƺ�ɫ����
        }
    }
}

// �������»��ƶ�ά���
void DrawQRPoint(const std::vector<int>& data, cv::Mat& image)
{
    int cnt = 0;
    for (int i = 0; i < QR_SIZE - RECT_SIZE / 9; i++)
    {
        if (i <= RECT_SIZE) // ��������λͼ��֮��
        {
            for (int j = 0; j < QR_SIZE - 2 * RECT_SIZE - 2; j++)
            {
                if (cnt < data.size())
                {
                    if (data[cnt] == 1)
                    {
                        image.at<cv::Vec3b>(RECT_SIZE / 9 + i, RECT_SIZE + j + 1) = BLACK;  // ���ú�ɫ��
                    }
                    cnt++;
                }
            }
        }
        else if (i > RECT_SIZE && i <= QR_SIZE - RECT_SIZE - RECT_SIZE / 9) // ͼ���м䲿��
        {
            for (int j = 0; j < QR_SIZE - 2 * RECT_SIZE / 9; j++)
            {
                if (cnt < data.size())
                {
                    if (data[cnt] == 1)
                    {
                        image.at<cv::Vec3b>(i, RECT_SIZE / 9 + j) = BLACK;  // ���ú�ɫ��
                    }
                    cnt++;
                }
            }
        }
        else // �ײ��Ҳඨλͼ�����м䲿��
        {
            for (int j = 0; j < QR_SIZE - RECT_SIZE - RECT_SIZE / 9 - 1; j++)
            {
                if (cnt < data.size())
                {
                    if (data[cnt] == 1)
                    {
                        image.at<cv::Vec3b>(i, RECT_SIZE + j + 1) = BLACK;  // ���ú�ɫ��
                    }
                    cnt++;
                }
            }
        }
    }
}

// �Ŵ�ͼ��
cv::Mat ScaleMat(const cv::Mat& image)
{
    int rate = 10;  // �Ŵ���
    int BiggerSize = rate * QR_SIZE;  // �Ŵ��ĳߴ�
    cv::Mat BiggerMat(BiggerSize, BiggerSize, CV_8UC3);  // �����Ŵ���ͼ��
    for (int i = 0; i < BiggerSize; i++)
    {
        for (int j = 0; j < BiggerSize; j++)
            BiggerMat.at<cv::Vec3b>(i, j) = image.at<cv::Vec3b>(i / rate, j / rate);  // ���ݷŴ������в�ֵ�Ŵ�
    }
    return BiggerMat;
}

// ���ɶ�ά��
void CreatQRcode(const std::string& filePath)
{
    std::vector<int> DATA;
    OpenFile(filePath, DATA);  // �򿪶������ļ�����ȡ����

    int numQRCodes = (DATA.size() + QR_SIZE * QR_SIZE - 1) / (QR_SIZE * QR_SIZE);  // �������ɵĶ�ά������

    for (int i = 0; i < numQRCodes; i++)
    {
        cv::Mat QR(QR_SIZE, QR_SIZE, CV_8UC3, WHITE);  // ������ɫ������ͼ��
        cv::Mat BigQR;

        DrawRect(QR);  // ���ƶ�λͼ��

        std::vector<int> subData(DATA.begin() + i * QR_SIZE * QR_SIZE, DATA.begin() + std::min<size_t>((i + 1) * QR_SIZE * QR_SIZE, DATA.size()));

        DrawQRPoint(subData, QR);  // ���ƶ�ά���

        BigQR = ScaleMat(QR);  // �Ŵ�ͼ��

        std::string fileName = "code\\" + std::to_string(i) + ".jpg";
        cv::imwrite(fileName, BigQR);  // ����ά�뱣��ΪͼƬ�ļ�
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
    std::cout << "������������ļ���·����";
    std::cin >> filePath;
    std::cout << "��������Ƶ���·����";
    std::cin >> videoOutput;
    std::cout << "�����������Ƶ���ȣ��룩��";
    std::cin >> videoLength;
    std::cout << "�����������Ƶ֡�ʣ�";
    std::cin >> videoFps;

    // ������Ŷ�ά����ļ���
    system("mkdir code");

    CreatQRcode(filePath);  // ���ɶ�ά��

    // �����ɵĶ�ά��ͼƬת������Ƶ
    ImagetoVideo("", videoFps, "code", "jpg", videoOutput.c_str(), 2000);

    std::cout << "�ѳɹ�����ά�����code�ļ��д�����ת��Ϊ��Ƶ��������ָ��·��" << std::endl;

    return 0;
}


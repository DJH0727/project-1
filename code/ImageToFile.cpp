#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>


#define QR_SIZE 64  // ��ά��Ĵ�С
#define RECT_SIZE 12  // ��λͼ���Ĵ�С

// �Ӷ�ά��ͼƬ�ж�ȡ���ݲ�ת��Ϊ�����ƴ�
void ExtractQRCode(const std::string& filePath, std::vector<int>& data)
{
    cv::Mat image = cv::imread(filePath); // ��ȡ��ά��ͼƬ

    
    cv::resize(image, image, cv::Size(QR_SIZE, QR_SIZE));// opencv��СͼƬ�ߴ纯��

    for (int i = 0; i < QR_SIZE; ++i) {
        for (int j = 0; j < QR_SIZE; ++j) {
            cv::Vec3b pixel = image.at<cv::Vec3b>(i, j);

            if ((i < RECT_SIZE && j < RECT_SIZE) ||
                (i < RECT_SIZE && j >= QR_SIZE - RECT_SIZE) ||
                (i >= QR_SIZE - RECT_SIZE && j < RECT_SIZE)) {
                continue; // ������λ��
            }
            if (pixel == cv::Vec3b(0, 0, 0)) {
                data.push_back(1); // ��ɫ���ض�Ӧ�������е�1
            }
            else {
                data.push_back(0); // ��ɫ���ض�Ӧ�������е�0
            }
        }
    }
}

// ��������ƴ�����Ļ
void PrintBinaryData(const std::vector<int>& data)
{
    std::cout << "��ά��Ķ����ƴ���" << std::endl;
    for (int bit : data) {
        std::cout << bit;
    }
    std::cout << std::endl;
}

// ������������д���ļ�
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
    std::cout << "�������ά��ͼƬ��·����";
    std::cin >> filePath;

    std::vector<int> binaryData;
    ExtractQRCode(filePath, binaryData); // �Ӷ�ά��ͼƬ����ȡ����

    PrintBinaryData(binaryData); // ��������ƴ�����Ļ

    std::string outputFilePath = "output.txt"; // �ļ����·��
    WriteBinaryDataToFile(outputFilePath, binaryData); // ������������д���ļ�

    std::cout << "�ѳɹ�����ά������д���ļ���output.txt" <<std::endl;
    return 0;
}

#pragma once
#include <opencv2/opencv.hpp>
#include <fstream>
#include <vector>
#include <iostream>

#define QR_SIZE 64  // ��ά��Ĵ�С
#define RECT_SIZE 12  // ��λͼ���Ĵ�С
#define colorThreshold 128

// �Ӷ�ά��ͼƬ�ж�ȡ���ݲ�ת��Ϊ�����ƴ�
void ExtractQRCode(const std::string& filePath, std::vector<int>& data) {
    cv::Mat image = cv::imread(filePath); // ��ȡ��ά��ͼƬ
    if (image.empty()) {
        std::cerr << "Error: Unable to open image file." << std::endl;
        return;
    }

    cv::resize(image, image, cv::Size(QR_SIZE, QR_SIZE)); // ����ͼ��ߴ�Ϊ��ά���С
    int borderSize = RECT_SIZE / 9;

    for (int i = borderSize - 1; i <= QR_SIZE - borderSize; i++) {
        for (int j = borderSize - 1; j <= QR_SIZE - borderSize; j++) {
            // ������λͼ������
            if ((i < RECT_SIZE + borderSize && j < RECT_SIZE + borderSize) ||
                (i < RECT_SIZE + borderSize && j >= QR_SIZE - RECT_SIZE - borderSize) ||
                (i >= QR_SIZE - RECT_SIZE - borderSize && j < RECT_SIZE + borderSize)) {
                continue;
            }
            cv::Vec3b pixel = image.at<cv::Vec3b>(i, j);
            int grayscale = (pixel[0] + pixel[1] + pixel[2]) / 3; // �Ҷ�ֵΪRGB������ƽ��ֵ
            data.push_back(grayscale > colorThreshold ? 1 : 0); // ������ֵ�жϺڰ�
        }
    }
}
void WriteBinaryDataToBinFile(const std::string& filePath, const std::vector<int>& data) {
    std::ofstream outputFile(filePath, std::ios::binary | std::ios::app);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Unable to open output file." << std::endl;
        return;
    }

    unsigned char byte = 0; // ���ڱ���ÿ���ֽڵ�����
    int bitCount = 0; // ��¼��ǰ�ֽ����Ѿ�д��ı�������

    for (int value : data) {
        byte |= (value & 0x01) << bitCount; // ����ǰ����д�� byte ��
        bitCount++;

        if (bitCount == 8) { // �����ǰ�ֽ���������д���ļ�������
            outputFile.write(reinterpret_cast<const char*>(&byte), sizeof(byte));
            byte = 0;
            bitCount = 0;
        }
    }

    // �������ʣ�����δд���ļ�����д�����һ���ֽ�
    if (bitCount > 0) {
        outputFile.write(reinterpret_cast<const char*>(&byte), sizeof(byte));
    }

    outputFile.close();
}
int Imagetofile() {
    std::string folderPath = "code/";
    std::vector<cv::String> fileNames;
    cv::glob(folderPath, fileNames);

    for (const auto& fileName : fileNames) {
        std::vector<int> binaryData;
        ExtractQRCode(fileName, binaryData);

        std::string outputFilePath = folderPath + "output.bin";
        WriteBinaryDataToBinFile(outputFilePath, binaryData);
    }
    std::cout << "�ѳɹ�������Ķ�ά������д�� code �ļ��� output.bin �ļ�" << std::endl;
    return 0;
}
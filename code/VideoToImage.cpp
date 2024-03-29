#define _CRT_SECURE_NO_WARNINGS
#include "VideoToImage.h"
#include <opencv2/opencv.hpp>


#define QR_SIZE 136
#define BLACK Scalar(0, 0, 0, 255)
#define WHITE Scalar(255, 255, 255, 255)
#define RED   Scalar(0,0,255,255)//测试用
#define RECT_SIZE 18

int VideotoImage(const char* ffmPath, const char* imagePath,
	const char* imageFormat, const char* videoPath)
{
	char systemCommand[256];
	snprintf(systemCommand, 256, "%s\\ffmpeg.exe -i %s -f image2  %s\\%%04d.%s"
		, ffmPath, videoPath, imagePath, imageFormat);
	char CreatImagePath[20];
	snprintf(CreatImagePath, 20, "md %s", imagePath);
	system(CreatImagePath);
  
	return system(systemCommand);

    //char DeleteImagePath[20];
    // snprintf(CreatImagePath, 20, "rmdir /S /Q %s", imagePath);
    // system(DeleteImagePath);
}

//画出二维码的识别矩阵
void DrowRect(Mat& image)
{
    int BorderSize = RECT_SIZE / 9;
    int axis[3][2] = { {0,0},{QR_SIZE - RECT_SIZE-1, 0},{0, QR_SIZE - RECT_SIZE-1} };//左上，右上，左下
    for (int j = 0; j < 3; j++)
    {
        int x = axis[j][0], y = axis[j][1];
        
        for (int i = 0; i <= 3; i++)
        {
            if (i % 2 == 0)
                rectangle(image, Point(x + i * BorderSize, y + i * BorderSize), Point(x + RECT_SIZE - i * BorderSize, y + RECT_SIZE - i * BorderSize), WHITE, -1);
            else
                rectangle(image, Point(x + i * BorderSize, y + i * BorderSize), Point(x + RECT_SIZE - i * BorderSize, y + RECT_SIZE - i * BorderSize), BLACK, -1);
        }
    }
}

void OpenFile(vector<char>& data, Mat& image)
{
    std::ifstream file("input.bin", std::ios::binary); 
    if (!file) {
        std::cerr << "无法打开bin文件" << std::endl;
        return;
    }
    char byte;
    while (file.read(&byte, 1))
    {
        data.push_back(byte - '0');
    }
    file.close(); 
    return;
}
//放大图像
Mat BigMat(Mat& image)
{
    int rate = 5;
    int BiggerSize = rate * QR_SIZE;
    Mat BiggerMat(BiggerSize, BiggerSize, CV_8UC3);
    for (int i = 0; i < BiggerSize; i++)
    {
        for (int j = 0; j < BiggerSize; j++)
            BiggerMat.at<Vec3b>(i, j) = image.at<Vec3b>(i / rate, j / rate);
    }
    return BiggerMat;
}
//从上到下画
void DrowQRPoint(vector<char>& data, Mat& image)
{
    int cnt = 0;
    for (int i = 0; i <QR_SIZE - RECT_SIZE / 9;i++)
    {
        if (i <= RECT_SIZE)//两个矩阵中间
        {
            for (int j = 0; j < QR_SIZE - 2 * RECT_SIZE - 2; j++)
            {

                if (data[cnt] == 1)
                {
                    image.at<cv::Vec3b>(RECT_SIZE / 9 + i, RECT_SIZE + j + 1) = cv::Vec3b(0, 0, 0);
                }
                cnt++;
            }
        }
        else if (i > RECT_SIZE && i <= QR_SIZE - RECT_SIZE- RECT_SIZE / 9)//图像中间
        {
            for (int j = 0; j < QR_SIZE - 2 * RECT_SIZE/9; j++)
            {

                if (data[cnt] == 1)
                {
                    image.at<cv::Vec3b>( i , RECT_SIZE / 9 +j) = cv::Vec3b(0, 0, 0);
                }
                cnt++;
            }
        }
        else//右下方矩阵中间
        {
            for (int j = 0; j < QR_SIZE - RECT_SIZE- RECT_SIZE / 9-1; j++)
            {

                if (data[cnt] == 1)
                {
                    image.at<cv::Vec3b>(i, RECT_SIZE + j + 1) = cv::Vec3b(0, 0, 0);
                }
                cnt++;
            }
        }

    }
}

void CreatQRcode()
{
    Mat QR(QR_SIZE, QR_SIZE, CV_8UC3, Scalar::all(255));  // 白色画布
    Mat BigQR;
    DrowRect(QR);
    vector<char> DATA;
    OpenFile(DATA, QR);
    DrowQRPoint(DATA,QR);
    BigQR = BigMat(QR);
    imwrite("QR.jpg", BigQR);
    imshow("Display Window", BigQR);
    waitKey();
}
/*int main()
{
	VideotoImage("", "test", "png", "out.mp4");
	return 0;
}*/
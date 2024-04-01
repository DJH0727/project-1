#define _CRT_SECURE_NO_WARNINGS
#include "Encode.h"



#define QR_SIZE 136
#define BLACK Vec3b(0, 0, 0)
#define WHITE Vec3b(255, 255, 255)
#define RED   Vec3b(0,0,255)//测试用
#define BLUE  Vec3b(255,0,0)
#define RECT_SIZE 18
#define QR_MAX_BIT 16753
const Vec3b pixel[2] = { Vec3b(0,0,0), Vec3b(255,255,255) };


void drawOuterEdge(Mat& mat, int i, int j, int len, int color) {   // 用颜色color给一个正方形外沿上色
    for (int t = 0; t < len; t++) {
        mat.at<Vec3b>(i + t, j) = pixel[color];
        mat.at<Vec3b>(i, j + t) = pixel[color];
        mat.at<Vec3b>(i + len - 1, j + t) = pixel[color];
        mat.at<Vec3b>(i + t, j + len - 1) = pixel[color];
    }
}


    //画出二维码的识别矩阵
    void DrowRect(Mat& image)
    {
        float BorderSize = RECT_SIZE / 9;
        float axis[3][2] = { {0,0},{QR_SIZE - RECT_SIZE, 0},{0, QR_SIZE - RECT_SIZE} };//左上，右上，左下
        for (int j = 0; j < 3; j++)
        {
            float x = axis[j][0], y = axis[j][1];

            for (int i = 0; i <= 3; i++)
            {
                if (i % 2 == 0)
                    rectangle(image, Point(x + i * BorderSize, y + i * BorderSize),
                        Point(x + RECT_SIZE - i * BorderSize-1, y + RECT_SIZE - i * BorderSize-1), WHITE, -1);
                else
                    rectangle(image, Point(x + i * BorderSize, y + i * BorderSize),
                        Point(x + RECT_SIZE - i * BorderSize-1, y + RECT_SIZE - i * BorderSize-1), BLACK, -1);
            }
        }
        const int mincolor[5] = { 1, 0, 1, 0, 0 };
        for (int k = 0; k < 5; k++) drawOuterEdge(image, 126 + k, 126 + k, 9 - 2 * k, mincolor[k]);
    }

    void OpenFile(vector<int>& data, const char* BinFileName)
    {
        FILE* file = fopen(BinFileName, "rb");
        if (!file) {
            std::cerr << "无法打开bin文件" << std::endl;
            return;
        }
        uchar byte;
        while (fread(&byte, sizeof(unsigned char), 1, file) == 1)
        {
            for (int i = 7; i >= 0; i--)
            {
                int bit = (byte >> i) & 1;
                data.push_back(bit);
            }
        }
        fclose(file);
        return;
    }

    //放大图像
    Mat ScaleMat(Mat& image)
    {
        int rate = 10;
        int BiggerSize = rate * QR_SIZE;
        Mat BiggerMat(BiggerSize, BiggerSize, CV_8UC3);
        for (int i = 0; i < BiggerSize; i++)
        {
            for (int j = 0; j < BiggerSize; j++)

            {
                BiggerMat.at<Vec3b>(i, j) = image.at<Vec3b>(i / rate, j / rate);
               // if (i / rate == 1 && j / rate == 1)cout << i << ' ' << j << endl;
            }
        }
        return BiggerMat;
    }
    //从上到下画
    int DrowQRPoint(vector<int>& data, Mat& image, int cnt)
    {
        for (int i = RECT_SIZE / 9; i < QR_SIZE - RECT_SIZE / 9; i++)
        {

            if (i < RECT_SIZE)//两个矩阵中间
            {
                for (int j = RECT_SIZE; j < QR_SIZE -  RECT_SIZE; j++)
                {

                    if (data[cnt] == 1)
                    {
                        image.at<cv::Vec3b>(i,j) = BLACK;
                        
                    }
                    cnt++;
                }
            }
            else if (i >= RECT_SIZE && i < QR_SIZE - RECT_SIZE)//图像中间
            {
                for (int j = RECT_SIZE / 9; j < QR_SIZE -  RECT_SIZE / 9; j++)
                {

                    if (data[cnt] == 1)
                    {
                        image.at<cv::Vec3b>(i, j) = BLACK;
                    }
                    cnt++;
                }
            }
            else if(i<125)
            {
                for (int j = RECT_SIZE; j < QR_SIZE - RECT_SIZE / 9; j++)
                {

                    if (data[cnt] == 1)
                    {
                        image.at<cv::Vec3b>(i, j) = BLACK;
                    }
                    cnt++;
                }
            }
            else
            {
                for (int j = RECT_SIZE; j < QR_SIZE - RECT_SIZE / 9-9; j++)
                {

                    if (data[cnt] == 1)
                    {
                        image.at<cv::Vec3b>(i, j) = BLACK;
                    }
                    cnt++;
                }

            }

        }
       // cout << cnt<<endl;
        return cnt;
    }



    void CreatQRcode(const char* BinFileName, int QRnum)
    {
        vector<int> DATA;
        OpenFile(DATA, BinFileName);
        int BitCnt = 0;
        system("mkdir QRcode");
        for (int i = 1; i <= QRnum; i++)
        {
            Mat QR(QR_SIZE, QR_SIZE, CV_8UC3, Scalar::all(255));  // 白色画布
            char OutputQRName[20];
            sprintf(OutputQRName, "QRcode/%04d.jpg", i);
            if (i == 1||i==2 || i == QRnum||i==QRnum-1)
            {
                imwrite(OutputQRName, ScaleMat(QR));
                continue;
            }
            Mat BigQR;
            DrowRect(QR);
            BitCnt = DrowQRPoint(DATA, QR, BitCnt);
            BigQR = ScaleMat(QR);

           // char buf[10];
           // sprintf(buf, "NO.%d", i);
           // putText(BigQR, buf, Point(70, 50), FONT_HERSHEY_SIMPLEX, 2, RED, 2);
            // imshow("tets", BigQR);
             //waitKey();

            imwrite(OutputQRName, BigQR);
        }



    }




int PictureEncode(const char* imageName, const char* imageFormat, const char* BinFileName)
{
    {
        std::vector<uchar> binaryStream;
        char image[256];
        char format[256];
        snprintf(image, 256, "%s.%s", imageName, imageFormat);
        snprintf(format, 256, ".%s", imageFormat);
        //std::cout << image << std::endl << format << std::endl;
        cv::Mat img = cv::imread(image, cv::IMREAD_COLOR);//注意后缀
        if (img.empty())
        {
            std::cout << "没有这个东西姐姐" << std::endl;
            return -1;
        }
        cv::imencode(format, img, binaryStream);//特别这里
        /*for (std::vector<uchar>::iterator it = binaryStream.begin(); it != binaryStream.end(); it++)
        {
            std::cout << *it << " ";
        }*/
        //std::cout << "Binary stream length: " << binaryStream.size() << std::endl;
        std::ofstream output_file(BinFileName, std::ios::binary);
        //Mat img_decode = imdecode(binaryStream, IMREAD_COLOR);
        //cv::imshow("Image", img_decode);
        //cv::waitKey(0);
        output_file.write(reinterpret_cast<const char*>(binaryStream.data()), binaryStream.size());
        output_file.close();

        return 0;
    }
}

int BinFiletoVideo(const char* BinFileName, const char* videoOutput, int TimeLimit, unsigned int videofps)
{
    int QRnum = (TimeLimit + 0.5) / 1000 * videofps;
    CreatQRcode(BinFileName, QRnum);
    char systemCommand[256];
    sprintf(systemCommand, "bin\\ffmpeg.exe  -r %d -i %s %s", videofps, "QRcode/%04d.jpg", videoOutput);
    system(systemCommand);

    


    return 0;
}

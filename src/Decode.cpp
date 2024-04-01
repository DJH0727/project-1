#include "Decode.h"
#define QR_SIZE 136
#define RECT_SIZE 18
#define RATE 10
#define BLACK Vec3b(0, 0, 0)
#define WHITE Vec3b(255, 255, 255)
#define RED   Vec3b(0,0,255)//测试用

#define DEBUG 1

using namespace cv;


int QRtoUchar(Mat& image, std::vector<int>& data)
{
    int qr_code[136][136] = { 0 };
    //GaussianBlur(image, image, cv::Size(3, 3), 0);
    int rows = image.rows;
    int cols = image.cols;

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {

            Vec3b BGR = image.at<Vec3b>(i, j);
            int pixel_value = (BGR[0] > 127&&BGR[1]>127&&BGR[2]>127) ? 0 : 1;
            if (pixel_value)qr_code[i / RATE][j / RATE]++;
            //if (i / 10 == 3)image.at < Vec3b >(i,j) = RED;
          
        }

    }
    //cv::resize(image, image, cv::Size(680, 680));
    //imshow("qr", image);
   // waitKey();
    for (int i = 0; i < rows/10; i++)
    {
        for (int j = 0; j < cols/10; j++)
        {

            
          // cout << qr_code[i][j] << ' ';

        }
       // cout << endl<<endl;
    }
   // image.at<cv::Vec3b>(134, 134) = RED;

    int up = 80, down = 20;
    for (int i = RECT_SIZE / 9; i < QR_SIZE - RECT_SIZE / 9; i++)
    {

        if (i < RECT_SIZE)//两个矩阵中间
        {
            for (int j = RECT_SIZE; j < QR_SIZE - RECT_SIZE; j++)
            {

                if (qr_code[i][j] >= up)data.push_back(1);
                else if (qr_code[i][j] <= down) data.push_back(0);
                else data.push_back(3);
            }
        }
        else if (i >= RECT_SIZE && i < QR_SIZE - RECT_SIZE)//图像中间
        {
            for (int j = RECT_SIZE / 9; j < QR_SIZE - RECT_SIZE / 9; j++)
            {

                if (qr_code[i][j] >= up)data.push_back(1);
                else if (qr_code[i][j] <= down) data.push_back(0);
                else data.push_back(3);
            }
        }
        else if (i < 125)
        {
            for (int j = RECT_SIZE; j < QR_SIZE - RECT_SIZE / 9; j++)
            {

                if (qr_code[i][j] >= up)data.push_back(1);
                else if (qr_code[i][j] <= down) data.push_back(0);
                else data.push_back(3);
            }
        }
        else
        {
            for (int j = RECT_SIZE; j < QR_SIZE - RECT_SIZE / 9 - 9; j++)
            {

                if (qr_code[i][j] >= up)data.push_back(1);
                else if (qr_code[i][j] <= down) data.push_back(0);
                else data.push_back(3);
            }

        }

    }


    //Mat view;
    
    //cv::imwrite("view.jpg", image);
    cv::waitKey();

    return 0;
}


void GetQR(const char *VideoName,const char *OutBinName,const char *VoutBinName)
{
    system("mkdir QRcodeOut");
    char SystemCmd[256];
    sprintf(SystemCmd, "bin\\ffmpeg.exe -i %s -q:v 2 -f image2 QRcodeOut\\%%04d.jpg", VideoName);
    system(SystemCmd);
    char Buf[100] = { 0 };
    vector<Mat> QRcode;
    vector<int> data1,//解码数据
        data2,//bin数据
        data3;//原始二维码数据
    //cout << "请稍等。。。" << endl;
    for (int i = 1; i <1000; i++)
    {
        sprintf(Buf, "QRcodeOut/%04d.jpg", i);
        

        Mat image = imread(Buf), qrCode;
        if (image.empty())break;
        //imshow("QR", image);
        //waitKey();
        if (!ImgParse::Main(image, qrCode))
        {
           // cout << "sussuss" << endl;
            
            if (QRcode.size() < 1)
            {
                QRcode.push_back(qrCode);
                cout << Buf << "录入成功---" << QRcode.size() << endl;
            }
            if (QRcode.size() >= 1 && !areImagesEqual(qrCode, QRcode[QRcode.size() - 1]))
            {
                QRcode.push_back(qrCode);
                cout << Buf << "录入成功---" << QRcode.size() << endl;
                //cout << "fafafafafa" << endl;
            }
            //imshow(Buf, qrCode);
           // waitKey();
           // 
        }
        else
        {
            cout << Buf << " 未检出二维码" << endl;
        }
        if (QRcode.size() >= 1)i += 2;
    }
#ifdef DEBUG
   OpenFile(data2, "1.bin");
    int cnt = 0;
    for (Mat image : QRcode)
    {
        cnt++;
        cout << "No." << cnt;
        QRtoUchar(image, data1);
        cout << "---succeed" << endl;
        
        int Rcnt = 0;
        int Dcnt = 0;
        int bit = data1.size();

        for (int i = 0; i < bit; i++)
        {
            //cout << data2[i] << '-'<<data1[i] << "  ";
            if (data1[i] != data2[i] && data1[i] != 3)
            {
                Rcnt++;
                // cout << "wrong";
            }
            if (data1[i] == 3)Dcnt++;
            //if (i % 20 == 0&&i!=0)cout << endl;
        }

        cout << endl << "解析共 " << bit << " 个bit" << endl;
        cout << "共 " << Dcnt << " 个bit弃用" << endl << "弃用率：" << (double)Dcnt / bit << endl;
        cout << "共 " << Rcnt << " 个bit错误" << endl << "错误率：" << (double)Rcnt / bit << endl;
        resize(image, image, Size(680,680));
        imshow("currentQR", image);
        waitKey();
    }
#endif // DEBUG
    DataToFile(data1, OutBinName, VoutBinName);
    

}

bool DataToFile(const vector<int>& data,const char *BinFileName,const char * VoutBinFileName)
{
    std::ofstream BinFile(BinFileName, std::ios::binary);
    std::ofstream VoutBinFile(VoutBinFileName, std::ios::binary);
    if (!BinFile.is_open()|| !VoutBinFile.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }

    std::vector<unsigned char> bytes;
    std::vector<unsigned char> Vbytes;
    srand(time(NULL)); // 初始化随机数种子

    for (size_t i = 0; i+8 < data.size(); i += 8) 
    {
        std::vector<int> chunk(data.begin() + i, data.begin() + i + 8);
        for (size_t j = 0; j < 8; ++j) {
            if (chunk[j] == 3) {
                chunk[j] = rand() % 2; // 随机生成0或1来替代3
            }
        }
        unsigned char byte = 0;
        for (size_t j = 0; j < 8; ++j) {
            byte |= (chunk[j] << (7 - j));
        }
        bytes.push_back(byte);
    }

    for (size_t i = 0; i + 8 < data.size(); i += 8)
    {
        std::vector<int> chunk(data.begin() + i, data.begin() + i + 8);
        for (size_t j = 0; j < 8; ++j)
        {
            if (chunk[j] == 3)chunk[j] = 0; 
            else chunk[j] = 1;
        }
        unsigned char byte = 0;
        for (size_t j = 0; j < 8; ++j) {
            byte |= (chunk[j] << (7 - j));
        }
        Vbytes.push_back(byte);
    }


    BinFile.write(reinterpret_cast<const char*>(bytes.data()), bytes.size() * sizeof(unsigned char)); // 写入uchar类型数据
    VoutBinFile.write(reinterpret_cast<const char*>(Vbytes.data()), Vbytes.size() * sizeof(unsigned char));
    BinFile.close();
    VoutBinFile.close();
    std::cout << "Data written to file successfully." << std::endl;

    return 0;


}

bool areImagesEqual(const Mat& image1, const Mat& image2)
{
    int qr_code1[136] = { 0 }, qr_code2[136] = { 0 };

   // imshow("image1", image1);
   // waitKey();
   // imshow("image2", image2);
   // waitKey();
        for (int j = 0; j < image2.cols; j++)
        {
            Vec3b BGR = image2.at<Vec3b>(30, j);
            int pixel_value = (BGR[0] > 127 && BGR[1] > 127 && BGR[2] > 127) ? 0 : 1;
            if (pixel_value)qr_code2[j / RATE]++;

        }
        for (int j = 0; j < image1.cols; j++)
        {
            Vec3b BGR = image1.at<Vec3b>(30, j);
            int pixel_value = (BGR[0] > 127 && BGR[1] > 127 && BGR[2] > 127) ? 0 : 1;
            if (pixel_value)qr_code1[j / RATE]++;

        }
      //  for (int j = 0; j < 136; j++)
       // {
            //cout << qr_code1[j] << ' ' << qr_code2[j] << endl;;

      //  }
        int cnt = 0;
        for (int j = RECT_SIZE; j < QR_SIZE - RECT_SIZE; j++)
        {
            if (qr_code1[j] > 5 && qr_code2[j] > 5)cnt++;
            else if (qr_code1[j] <= 5 && qr_code2[j] <= 5)cnt++;
        }

       // cout << cnt <<endl;
        return cnt > 85;//相似度大于85
}


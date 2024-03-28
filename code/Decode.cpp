#include "Decode.h"
#define QR_SIZE 136
#define RECT_SIZE 18
#define RATE 5
#define BLACK Vec3b(0, 0, 0)
#define WHITE Vec3b(255, 255, 255)
#define RED   Vec3b(0,0,255)//测试用

int QRtoUchar(Mat& image, std::vector<int>& data)
{
    Mat pic;
    Size new_size(136 * 5, 136 * 5);
    resize(image, pic, new_size, 0, 0, INTER_NEAREST);
    int rows = pic.rows;
    int cols = pic.cols;
    int qr_code[136][136] = { 0 };

    for (int i = 0; i < rows; i++)
    {
        std::vector<int> row;
        for (int j = 0; j < cols; j++)
        {

            Vec3b BGR = pic.at<Vec3b>(i, j);
            int pixel_value = BGR[0] > 100 ? 0 : 1;
            if (pixel_value)qr_code[i/RATE][j/RATE]++;

        }
        
    }

    for (int i = RECT_SIZE / 9; i < QR_SIZE - RECT_SIZE / 9; i++)
    {
        if (i <= RECT_SIZE)//两个矩阵中间
        {
            for (int j = 0; j < QR_SIZE - 2 * RECT_SIZE - 2; j++)
            {
                if (qr_code[i][RECT_SIZE + j + 1] > 20)data.push_back(1);
                else data.push_back(0);
                
            }
        }
        else if (i > RECT_SIZE && i <= QR_SIZE - RECT_SIZE - RECT_SIZE / 9)//图像中间
        {
            for (int j = 0; j < QR_SIZE - 2 * RECT_SIZE / 9; j++)
            {
                if (qr_code[i][RECT_SIZE / 9 + j] > 20)data.push_back(1);
                else data.push_back(0);
                
            }
        }
        else//右下方矩阵中间
        {
            for (int j = 0; j < QR_SIZE - RECT_SIZE - RECT_SIZE / 9 - 1; j++)
            {
                if (qr_code[i][RECT_SIZE + j + 1] > 20)data.push_back(1);
                else data.push_back(0);
                
            }
        }

    }

    for (int i = 0; i < 136; i++) 
    {
        for (int j = 0; j < 136; j++) 
        {
            //std::cout << qr_code[i][j] << " ";
        }
        //std::cout << std::endl<<endl;
    }
 
   // imshow("ada", pic);
    waitKey();

    return 0;
}





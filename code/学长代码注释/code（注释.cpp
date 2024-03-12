
//这个文件负责二维码的编码
#include"code.h"
//定义下面一个宏来开启debug
//#define Code_DEBUG
#define Show_Scale_Img(src) do\
{\
	Mat temp=ScaleToDisSize(src);\
	imshow("Code_DEBUG", temp);\
	waitKey();\
}while (0);
namespace Code
{
// 定义每帧字节数
constexpr int BytesPerFrame = 1242;

// 定义帧的大小
constexpr int FrameSize = 108;

// 定义帧输出的倍率
constexpr int FrameOutputRate = 10;

// 安全区域的宽度
constexpr int SafeAreaWidth = 2;

// 定义二维码点的大小
constexpr int QrPointSize = 18;

// 定义小二维码点的偏移
constexpr int SmallQrPointbias = 6;

// 定义矩形区域的数量
constexpr int RectAreaCount = 7;

// 定义颜色枚举
enum color
{
    Black = 0,
    White = 7
};

// 定义帧类型枚举
enum class FrameType
{
    Start = 0,
    End = 1,
    StartAndEnd = 2,
    Normal = 3
};

// 定义颜色值
const Vec3b pixel[8] =
{
    Vec3b(0, 0, 0), Vec3b(0, 0, 255), Vec3b(0, 255, 0), Vec3b(0, 255, 255),
    Vec3b(255, 0, 0), Vec3b(255, 0, 255), Vec3b(255, 255, 0), Vec3b(255, 255, 255)
};

// 定义矩形区域的长度限制
const int lenlim[RectAreaCount] = { 138, 144, 648, 144, 144, 16, 8 };

// 定义矩形区域的位置
const int areapos[RectAreaCount][2][2] = //[2][2],第一维度代表高宽，第二维度代表左上角坐标
{
    {{69, 16}, {QrPointSize + 3, SafeAreaWidth}},
    {{16, 72}, {SafeAreaWidth, QrPointSize}},
    {{72, 72}, {QrPointSize, QrPointSize}},
    {{72, 16}, {QrPointSize, FrameSize - QrPointSize}},
    {{16, 72}, {FrameSize - QrPointSize, QrPointSize}},
    {{8, 16}, {FrameSize  - QrPointSize, FrameSize - QrPointSize}},
    {{8, 8}, {FrameSize - QrPointSize + 8, FrameSize - QrPointSize}}
};

// 将原始图像缩放到显示大小的函数
Mat ScaleToDisSize(const Mat& src)
{
    Mat dis;
    constexpr int FrameOutputSize = FrameSize * FrameOutputRate;
    dis = Mat(FrameOutputSize, FrameOutputSize, CV_8UC3);

    for (int i = 0; i < FrameOutputSize; ++i)
    {
        for (int j = 0; j < FrameOutputSize; ++j)
        {
            dis.at<Vec3b>(i, j) = src.at<Vec3b>(i / 10, j / 10);
        }
    }

    return dis;
}

uint16_t CalCheckCode(const unsigned char* info, int len, bool isStart, bool isEnd, uint16_t frameBase)
{
    uint16_t ans = 0;
    int cutlen = (len / 2) * 2;
    for (int i = 0; i < cutlen; i += 2)
        ans ^= ((uint16_t)info[i] << 8) | info[i + 1];
    if (len & 1)
        ans ^= (uint16_t)info[cutlen] << 8;
    ans ^= len;
    ans ^= frameBase;
    uint16_t temp = (isStart << 1) + isEnd;
    ans ^= temp;
    return ans;
}/*计算检验码,将输入的信息中的每两个字节（16位）合并为一个整数，并将这两个整数依次进行异或运算。
如果信息长度是奇数，将最后一个字节合并为一个整数，再次进行异或运算。
将信息的长度（len）与之前的结果进行异或运算。
将帧基准（frameBase）与之前的结果进行异或运算。
将isStart左移一位，与isEnd相加，再与之前的结果进行异或运算。*/


void BulidSafeArea(Mat& mat)
{
    constexpr int pos[4][2][2] =
    {
        {{0, FrameSize}, {0, SafeAreaWidth}},
        {{0, FrameSize}, {FrameSize - SafeAreaWidth, FrameSize}},
        {{0, SafeAreaWidth }, {0, FrameSize}},
        {{FrameSize - SafeAreaWidth, FrameSize}, {0, FrameSize}}
    };
    for (int k = 0; k < 4; ++k)
        for (int i = pos[k][0][0]; i < pos[k][0][1]; ++i)
            for (int j = pos[k][1][0]; j < pos[k][1][1]; ++j)
                mat.at<Vec3b>(i, j) = pixel[White];
#ifdef Code_DEBUG
    Show_Scale_Img(mat);
#endif
    return;
}/*这是一个名为BulidSafeArea的函数，其目的是在给定的Mat对象（OpenCV中的图像矩阵）上构建一个安全区域。以下是函数的详细解释：

输入参数：

mat：引用传递的Mat对象，表示图像矩阵。
常量定义：

pos[4][2][2]：一个三维数组，表示四个矩形区域的坐标范围。每个区域有两对坐标，分别表示高度和宽度的范围。
循环过程：

使用嵌套的三层循环遍历四个区域内的所有像素点。
外层循环（k循环）遍历四个区域。
中层循环（i循环）遍历当前区域的高度范围。
内层循环（j循环）遍历当前区域的宽度范围。
在每次迭代中，将当前像素点的颜色设置为pixel[White]，即白色。*/


void BulidQrPoint(Mat& mat)
{
    //绘制大二维码识别点
    constexpr int pointPos[4][2] =
    {
        {0, 0},
        {0, FrameSize - QrPointSize},
        {FrameSize - QrPointSize, 0}
    };
    const Vec3b vec3bBig[9] =
    {
        pixel[Black],
        pixel[Black],
        pixel[Black],
        pixel[White],
        pixel[White],
        pixel[Black],
        pixel[Black],
        pixel[White],
        pixel[White]
    };
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < QrPointSize; ++j)
            for (int k = 0; k < QrPointSize; ++k)
                mat.at<Vec3b>(pointPos[i][0] + j, pointPos[i][1] + k) =
                    vec3bBig[(int)max(fabs(j - 8.5), fabs(k - 8.5))];
    //绘制小二维码识别点
    constexpr int posCenter[2] = { FrameSize - SmallQrPointbias, FrameSize - SmallQrPointbias };
    const Vec3b vec3bsmall[5] =
    {
        pixel[Black],
        pixel[Black],
        pixel[White],
        pixel[Black],
        pixel[White],
    };
    for (int i = -4; i <= 4; ++i)
        for (int j = -4; j <= 4; ++j)
            mat.at<Vec3b>(posCenter[0] + i, posCenter[1] + j) =
                vec3bsmall[max(abs(i), abs(j))];
#ifdef Code_DEBUG
    Show_Scale_Img(mat);
#endif
}
/*这是一个名为BulidQrPoint的函数，其目的是在给定的Mat对象（OpenCV中的图像矩阵）上绘制大二维码和小二维码的识别点。以下是函数的详细解释：

输入参数：

mat：引用传递的Mat对象，表示图像矩阵。
常量定义：

pointPos[4][2]：一个二维数组，表示大二维码的三个识别点的左上角坐标。
vec3bBig[9]：一个一维数组，表示大二维码识别点的颜色信息，其中包含了黑色和白色。
posCenter[2]：一个包含两个元素的数组，表示小二维码识别点的中心坐标。
vec3bsmall[5]：一个一维数组，表示小二维码识别点的颜色信息，同样包含了黑色和白色。
循环过程：

第一个循环（i循环）用于绘制大二维码的识别点。

在pointPos[i]位置开始，使用嵌套的两层循环遍历大二维码识别点的范围。
对于每个像素点，根据距离中心的距离来选择颜色，使用vec3bBig中的颜色信息来设置对应的颜色。
第二个循环（i循环）用于绘制小二维码的识别点。

在posCenter位置开始，使用嵌套的两层循环遍历小二维码识别点的范围。
对于每个像素点，根据距离中心的距离来选择颜色，使用vec3bsmall中的颜色信息来设置对应的颜色。
条件编译：

代码块 #ifdef Code_DEBUG 和 #endif 之间的部分表示在调试模式下会执行的代码。如果定义了 Code_DEBUG 这个宏，那么 Show_Scale_Img(mat) 会被调用，显示生成的图像。*/

void BulidCheckCodeAndFrameNo(Mat& mat, uint16_t checkcode, uint16_t FrameNo)
{
    //uint32_t outputCode = (checkcode << 8) | (FrameNo);
    /*for (int i = 8; i < 16; ++i)
    {
    	mat.at<Vec3b>(QrPointSize+1, SafeAreaWidth + i) = pixel[outputCode & 7];
    	outputCode >>= 3;
    }*/
    for (int i = 0; i < 16; ++i)
    {
        mat.at<Vec3b>(QrPointSize + 1, SafeAreaWidth + i) = pixel[(checkcode & 1) ? 7 : 0];
        checkcode >>= 1;
    }
    for (int i = 0; i < 16; ++i)
    {
        mat.at<Vec3b>(QrPointSize + 2, SafeAreaWidth + i) = pixel[(FrameNo & 1) ? 7 : 0];
        FrameNo >>= 1;
    }
#ifdef Code_DEBUG
    Show_Scale_Img(mat);
#endif
}/*这是一个名为BulidCheckCodeAndFrameNo的函数，其目的是在给定的Mat对象上绘制校验码和帧号的信息。以下是函数的详细解释：

输入参数：

mat：引用传递的Mat对象，表示图像矩阵。
checkcode：表示校验码。
FrameNo：表示帧号。
循环过程：

第一个循环（i循环）用于绘制校验码的信息。

从QrPointSize + 1, SafeAreaWidth位置开始，使用循环遍历16个像素。
对于每个像素点，根据checkcode的最低位（LSB），选择颜色信息（黑色或白色），并将颜色信息设置到mat中。
checkcode右移一位，继续处理下一个位。
第二个循环（i循环）用于绘制帧号的信息。

从QrPointSize + 2, SafeAreaWidth位置开始，使用循环遍历16个像素。
对于每个像素点，根据FrameNo的最低位（LSB），选择颜色信息（黑色或白色），并将颜色信息设置到mat中。
FrameNo右移一位，继续处理下一个位。
条件编译：

代码块 #ifdef Code_DEBUG 和 #endif 之间的部分表示在调试模式下会执行的代码。如果定义了 Code_DEBUG 这个宏，那么 Show_Scale_Img(mat) 会被调用，显示生成的图像。*/

void BulidInfoRect(Mat& mat, const char* info, int len, int areaID)
{
    const unsigned char* pos = (const unsigned char*)info;
    const unsigned char* end = pos + len;
    for (int i = 0; i < areapos[areaID][0][0]; ++i)
    {
        uint32_t outputCode = 0;
        for (int j = 0; j < areapos[areaID][0][1] / 8; ++j)
        {
            outputCode |= *pos++;
            /*for (int k = 0; k < 3; ++k)
            {
            	outputCode <<= 8;
            	if (pos != end)
            		outputCode |= *pos++;
            }*/
            for (int k = areapos[areaID][1][1]; k < areapos[areaID][1][1] + 8; ++k)
            {
                //mat.at<Vec3b>(i+areapos[areaID][1][0], j*8+k) = pixel[outputCode&7];
                //outputCode >>= 3;
                mat.at<Vec3b>(i + areapos[areaID][1][0], j * 8 + k) = pixel[(outputCode & 1) ? 7 : 0];
                outputCode >>= 1;
            }
            if (pos == end) break;
        }
        if (pos == end) break;
    }
#ifdef Code_DEBUG
    Show_Scale_Img(mat);
#endif
}/*这是一个名为BulidInfoRect的函数，其目的是在给定的Mat对象上绘制信息区域的内容。以下是函数的详细解释：

输入参数：

mat：引用传递的Mat对象，表示图像矩阵。
info：指向存储信息的字符数组的指针。
len：信息的长度。
areaID：表示信息区域的标识符。
循环过程：

外部循环（i循环）用于遍历信息区域的垂直方向，即areapos[areaID][0][0]次。

内部循环（j循环）用于遍历信息区域的水平方向，即areapos[areaID][0][1] / 8次。

在内部循环中，将每个字节的信息（8位）映射到颜色信息。

使用 outputCode |= *pos++; 将字节的值赋给 outputCode。
内部的第二个循环（k循环）用于在信息区域中的每个像素位置绘制颜色。
根据 outputCode 的最低位（LSB），选择颜色信息（黑色或白色），并将颜色信息设置到 mat 中。
outputCode 右移一位，继续处理下一个位。
如果 pos 到达信息末尾，跳出循环。
如果 pos 到达信息末尾，跳出外部循环。

条件编译：

代码块 #ifdef Code_DEBUG 和 #endif 之间的部分表示在调试模式下会执行的代码。如果定义了 Code_DEBUG 这个宏，那么 Show_Scale_Img(mat) 会被调用，显示生成的图像。*/


void BulidFrameFlag(Mat& mat, FrameType frameType, int tailLen)
{
    switch (frameType)
    {
    case FrameType::Start:
        mat.at<Vec3b>(QrPointSize, SafeAreaWidth) = pixel[White];
        mat.at<Vec3b>(QrPointSize, SafeAreaWidth + 1) = pixel[White];
        mat.at<Vec3b>(QrPointSize, SafeAreaWidth + 2) = pixel[Black];
        mat.at<Vec3b>(QrPointSize, SafeAreaWidth + 3) = pixel[Black];
        break;
    case FrameType::End:
        mat.at<Vec3b>(QrPointSize, SafeAreaWidth) = pixel[Black];
        mat.at<Vec3b>(QrPointSize, SafeAreaWidth + 1) = pixel[Black];
        mat.at<Vec3b>(QrPointSize, SafeAreaWidth + 2) = pixel[White];
        mat.at<Vec3b>(QrPointSize, SafeAreaWidth + 3) = pixel[White];
        break;
    case FrameType::StartAndEnd:
        mat.at<Vec3b>(QrPointSize, SafeAreaWidth) = pixel[White];
        mat.at<Vec3b>(QrPointSize, SafeAreaWidth + 1) = pixel[White];
        mat.at<Vec3b>(QrPointSize, SafeAreaWidth + 2) = pixel[White];
        mat.at<Vec3b>(QrPointSize, SafeAreaWidth + 3) = pixel[White];
        break;
    default:
        mat.at<Vec3b>(QrPointSize, SafeAreaWidth) = pixel[Black];
        mat.at<Vec3b>(QrPointSize, SafeAreaWidth + 1) = pixel[Black];
        mat.at<Vec3b>(QrPointSize, SafeAreaWidth + 2) = pixel[Black];
        mat.at<Vec3b>(QrPointSize, SafeAreaWidth + 3) = pixel[Black];
        break;
    }
    for (int i = 4; i < 16; ++i)
    {
        mat.at<Vec3b>(QrPointSize, SafeAreaWidth + i) = pixel[(tailLen & 1) ? 7 : 0];
        tailLen >>= 1;
    }
    /*for (int i = 4; i < 8; ++i)
    {
    	mat.at<Vec3b>(QrPointSize, SafeAreaWidth + i) = pixel[tailLen & 7];
    	tailLen >>= 3;
    }*/
#ifdef Code_DEBUG
    Show_Scale_Img(mat);
#endif
}/*输入参数：

mat：引用传递的Mat对象，表示图像矩阵。
frameType：枚举类型 FrameType，表示帧类型（开始、结束、开始和结束、普通）。
tailLen：整数，表示尾部长度。
帧标识绘制：

使用 switch 语句根据不同的 frameType 绘制不同的帧标识。

如果是 FrameType::Start，绘制白黑黑白的标识。
如果是 FrameType::End，绘制黑白白黑的标识。
如果是 FrameType::StartAndEnd，绘制全白的标识。
如果是其他情况，绘制全黑的标识。
使用 for 循环从第4列到第15列，绘制尾部长度信息。尾部长度信息由最低位（LSB）开始绘制，每次右移一位。

根据 tailLen 的最低位（LSB），选择颜色信息（黑色或白色），并将颜色信息设置到 mat 中。
tailLen 右移一位，继续处理下一个位。*/

Mat CodeFrame(FrameType frameType, const char* info, int tailLen, int FrameNo)
{
    Mat codeMat = Mat(FrameSize, FrameSize, CV_8UC3, Vec3d(255, 255, 255));
    if (frameType != FrameType::End && frameType != FrameType::StartAndEnd)
        tailLen = BytesPerFrame;
    BulidSafeArea(codeMat);
    BulidQrPoint(codeMat);

    int checkCode = CalCheckCode((const unsigned char*)info, tailLen,
                                 frameType == FrameType::Start || frameType == FrameType::StartAndEnd,
                                 frameType == FrameType::End || frameType == FrameType::StartAndEnd, FrameNo);
    BulidFrameFlag(codeMat, frameType, tailLen);
    BulidCheckCodeAndFrameNo(codeMat, checkCode, FrameNo % 65536);
    if (tailLen != BytesPerFrame)
        tailLen = BytesPerFrame;
    for (int i = 0; i < RectAreaCount && tailLen > 0; ++i)
    {
        int lennow = std::min(tailLen, lenlim[i]);
        BulidInfoRect(codeMat, info, lennow, i);
        tailLen -= lennow;
        info += lennow;
    }
    return codeMat;
}
/*这是一个名为 CodeFrame 的函数，用于生成带有特定帧信息的二维码图像。以下是函数的详细解释：

输入参数：

frameType：枚举类型 FrameType，表示帧类型（开始、结束、开始和结束、普通）。
info：指向字符数组的指针，表示要编码的信息。
tailLen：整数，表示尾部长度。
FrameNo：整数，表示帧编号。
生成图像过程：

创建一个大小为 FrameSize x FrameSize 的 Mat 对象，初始值为全白（255, 255, 255）。

根据帧类型，调整尾部长度 tailLen：

如果是 FrameType::End 或 FrameType::StartAndEnd，tailLen 被设置为 BytesPerFrame。
其他情况下，保持 tailLen 不变。
调用 BulidSafeArea 函数，在图像上构建安全区域。

调用 BulidQrPoint 函数，在图像上构建二维码识别点。

计算校验码并调用 BulidFrameFlag 函数，在图像上构建帧标识。

将校验码和帧编号添加到图像中，调用 BulidCheckCodeAndFrameNo 函数。

遍历信息矩形区域，调用 BulidInfoRect 函数，逐步在图像中添加信息。

返回值：

函数返回生成的二维码图像 Mat 对象。*/

void Main(const char* info, int len, const char * savePath, const char * outputFormat, int FrameCountLimit)
{
    Mat output;
    char fileName[128];
    int counter = 0;
    if (FrameCountLimit == 0) return;
    if (len <= 0);
    else if (len <= BytesPerFrame)
    {
        unsigned char BUF[BytesPerFrame + 5];
        memcpy(BUF, info, sizeof(unsigned char) * len);
        for (int i = len; i <= BytesPerFrame; ++i)
            BUF[i] = rand() % 256;
        output = ScaleToDisSize(CodeFrame(FrameType::StartAndEnd, (char*)BUF, len, 0));
        sprintf_s(fileName, "%s\\%05d.%s", savePath, counter++, outputFormat);
        imwrite(fileName, output);
    }
    else
    {
        int i = 0;
        len -= BytesPerFrame;
        output = ScaleToDisSize(CodeFrame(FrameType::Start, info, len, 0));
        --FrameCountLimit;

        sprintf_s(fileName, "%s\\%05d.%s", savePath, counter++, outputFormat);
        imwrite(fileName, output);

        while (len > 0 && FrameCountLimit > 0)
        {
            info += BytesPerFrame;
            --FrameCountLimit;
            if (len - BytesPerFrame > 0)
            {
                if (FrameCountLimit > 0)
                    output = ScaleToDisSize(CodeFrame(FrameType::Normal, info, BytesPerFrame, ++i));
                else output = ScaleToDisSize(CodeFrame(FrameType::End, info, BytesPerFrame, ++i));
            }
            else
            {
                unsigned char BUF[BytesPerFrame + 5];
                memcpy(BUF, info, sizeof(unsigned char) * len);
                for (int i = len; i <= BytesPerFrame; ++i)
                    BUF[i] = rand() % 256;
                output = ScaleToDisSize(CodeFrame(FrameType::End, (char*)BUF, len, ++i));
            }
            len -= BytesPerFrame;
            sprintf_s(fileName, "%s\\%05d.%s", savePath, counter++, outputFormat);
            imwrite(fileName, output);
        };
    }
    return;
}
}/*输入参数：

info：指向字符数组的指针，表示要编码的信息。
len：整数，表示信息的长度。
savePath：保存图像的路径。
outputFormat：输出图像的格式。
FrameCountLimit：整数，表示生成的帧数限制。

生成图像过程：
创建一个 Mat 对象 output 用于保存生成的图像。
创建一个计数器 counter 并初始化为 0。
如果 FrameCountLimit 为 0，直接返回。
如果 len 小于等于 0，什么都不做。
如果 len 小于等于 BytesPerFrame：
创建一个大小为 BytesPerFrame 的缓冲区 BUF。
将 info 中的内容复制到 BUF。
用随机数填充 BUF 直到达到 BytesPerFrame 的长度。
调用 CodeFrame 函数生成带有 StartAndEnd 类型标识的二维码图像。
将生成的图像按比例缩放，然后保存到文件中，文件名包含计数器和指定的输出格式。
如果 len 大于 BytesPerFrame：
减去 BytesPerFrame 的长度。
调用 CodeFrame 函数生成带有 Start 类型标识的二维码图像，并将其保存到文件。
将计数器减一。
循环处理剩余信息，生成 Normal 或 End 类型标识的二维码图像，并保存到文件。*/

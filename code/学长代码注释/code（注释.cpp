
//����ļ������ά��ı���
#include"code.h"
//��������һ����������debug
//#define Code_DEBUG
#define Show_Scale_Img(src) do\
{\
	Mat temp=ScaleToDisSize(src);\
	imshow("Code_DEBUG", temp);\
	waitKey();\
}while (0);
namespace Code
{
// ����ÿ֡�ֽ���
constexpr int BytesPerFrame = 1242;

// ����֡�Ĵ�С
constexpr int FrameSize = 108;

// ����֡����ı���
constexpr int FrameOutputRate = 10;

// ��ȫ����Ŀ��
constexpr int SafeAreaWidth = 2;

// �����ά���Ĵ�С
constexpr int QrPointSize = 18;

// ����С��ά����ƫ��
constexpr int SmallQrPointbias = 6;

// ����������������
constexpr int RectAreaCount = 7;

// ������ɫö��
enum color
{
    Black = 0,
    White = 7
};

// ����֡����ö��
enum class FrameType
{
    Start = 0,
    End = 1,
    StartAndEnd = 2,
    Normal = 3
};

// ������ɫֵ
const Vec3b pixel[8] =
{
    Vec3b(0, 0, 0), Vec3b(0, 0, 255), Vec3b(0, 255, 0), Vec3b(0, 255, 255),
    Vec3b(255, 0, 0), Vec3b(255, 0, 255), Vec3b(255, 255, 0), Vec3b(255, 255, 255)
};

// �����������ĳ�������
const int lenlim[RectAreaCount] = { 138, 144, 648, 144, 144, 16, 8 };

// ������������λ��
const int areapos[RectAreaCount][2][2] = //[2][2],��һά�ȴ���߿��ڶ�ά�ȴ������Ͻ�����
{
    {{69, 16}, {QrPointSize + 3, SafeAreaWidth}},
    {{16, 72}, {SafeAreaWidth, QrPointSize}},
    {{72, 72}, {QrPointSize, QrPointSize}},
    {{72, 16}, {QrPointSize, FrameSize - QrPointSize}},
    {{16, 72}, {FrameSize - QrPointSize, QrPointSize}},
    {{8, 16}, {FrameSize  - QrPointSize, FrameSize - QrPointSize}},
    {{8, 8}, {FrameSize - QrPointSize + 8, FrameSize - QrPointSize}}
};

// ��ԭʼͼ�����ŵ���ʾ��С�ĺ���
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
}/*���������,���������Ϣ�е�ÿ�����ֽڣ�16λ���ϲ�Ϊһ�������������������������ν���������㡣
�����Ϣ�����������������һ���ֽںϲ�Ϊһ���������ٴν���������㡣
����Ϣ�ĳ��ȣ�len����֮ǰ�Ľ������������㡣
��֡��׼��frameBase����֮ǰ�Ľ������������㡣
��isStart����һλ����isEnd��ӣ�����֮ǰ�Ľ������������㡣*/


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
}/*����һ����ΪBulidSafeArea�ĺ�������Ŀ�����ڸ�����Mat����OpenCV�е�ͼ������Ϲ���һ����ȫ���������Ǻ�������ϸ���ͣ�

���������

mat�����ô��ݵ�Mat���󣬱�ʾͼ�����
�������壺

pos[4][2][2]��һ����ά���飬��ʾ�ĸ�������������귶Χ��ÿ���������������꣬�ֱ��ʾ�߶ȺͿ�ȵķ�Χ��
ѭ�����̣�

ʹ��Ƕ�׵�����ѭ�������ĸ������ڵ��������ص㡣
���ѭ����kѭ���������ĸ�����
�в�ѭ����iѭ����������ǰ����ĸ߶ȷ�Χ��
�ڲ�ѭ����jѭ����������ǰ����Ŀ�ȷ�Χ��
��ÿ�ε����У�����ǰ���ص����ɫ����Ϊpixel[White]������ɫ��*/


void BulidQrPoint(Mat& mat)
{
    //���ƴ��ά��ʶ���
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
    //����С��ά��ʶ���
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
/*����һ����ΪBulidQrPoint�ĺ�������Ŀ�����ڸ�����Mat����OpenCV�е�ͼ������ϻ��ƴ��ά���С��ά���ʶ��㡣�����Ǻ�������ϸ���ͣ�

���������

mat�����ô��ݵ�Mat���󣬱�ʾͼ�����
�������壺

pointPos[4][2]��һ����ά���飬��ʾ���ά�������ʶ�������Ͻ����ꡣ
vec3bBig[9]��һ��һά���飬��ʾ���ά��ʶ������ɫ��Ϣ�����а����˺�ɫ�Ͱ�ɫ��
posCenter[2]��һ����������Ԫ�ص����飬��ʾС��ά��ʶ�����������ꡣ
vec3bsmall[5]��һ��һά���飬��ʾС��ά��ʶ������ɫ��Ϣ��ͬ�������˺�ɫ�Ͱ�ɫ��
ѭ�����̣�

��һ��ѭ����iѭ�������ڻ��ƴ��ά���ʶ��㡣

��pointPos[i]λ�ÿ�ʼ��ʹ��Ƕ�׵�����ѭ���������ά��ʶ���ķ�Χ��
����ÿ�����ص㣬���ݾ������ĵľ�����ѡ����ɫ��ʹ��vec3bBig�е���ɫ��Ϣ�����ö�Ӧ����ɫ��
�ڶ���ѭ����iѭ�������ڻ���С��ά���ʶ��㡣

��posCenterλ�ÿ�ʼ��ʹ��Ƕ�׵�����ѭ������С��ά��ʶ���ķ�Χ��
����ÿ�����ص㣬���ݾ������ĵľ�����ѡ����ɫ��ʹ��vec3bsmall�е���ɫ��Ϣ�����ö�Ӧ����ɫ��
�������룺

����� #ifdef Code_DEBUG �� #endif ֮��Ĳ��ֱ�ʾ�ڵ���ģʽ�»�ִ�еĴ��롣��������� Code_DEBUG ����꣬��ô Show_Scale_Img(mat) �ᱻ���ã���ʾ���ɵ�ͼ��*/

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
}/*����һ����ΪBulidCheckCodeAndFrameNo�ĺ�������Ŀ�����ڸ�����Mat�����ϻ���У�����֡�ŵ���Ϣ�������Ǻ�������ϸ���ͣ�

���������

mat�����ô��ݵ�Mat���󣬱�ʾͼ�����
checkcode����ʾУ���롣
FrameNo����ʾ֡�š�
ѭ�����̣�

��һ��ѭ����iѭ�������ڻ���У�������Ϣ��

��QrPointSize + 1, SafeAreaWidthλ�ÿ�ʼ��ʹ��ѭ������16�����ء�
����ÿ�����ص㣬����checkcode�����λ��LSB����ѡ����ɫ��Ϣ����ɫ���ɫ����������ɫ��Ϣ���õ�mat�С�
checkcode����һλ������������һ��λ��
�ڶ���ѭ����iѭ�������ڻ���֡�ŵ���Ϣ��

��QrPointSize + 2, SafeAreaWidthλ�ÿ�ʼ��ʹ��ѭ������16�����ء�
����ÿ�����ص㣬����FrameNo�����λ��LSB����ѡ����ɫ��Ϣ����ɫ���ɫ����������ɫ��Ϣ���õ�mat�С�
FrameNo����һλ������������һ��λ��
�������룺

����� #ifdef Code_DEBUG �� #endif ֮��Ĳ��ֱ�ʾ�ڵ���ģʽ�»�ִ�еĴ��롣��������� Code_DEBUG ����꣬��ô Show_Scale_Img(mat) �ᱻ���ã���ʾ���ɵ�ͼ��*/

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
}/*����һ����ΪBulidInfoRect�ĺ�������Ŀ�����ڸ�����Mat�����ϻ�����Ϣ��������ݡ������Ǻ�������ϸ���ͣ�

���������

mat�����ô��ݵ�Mat���󣬱�ʾͼ�����
info��ָ��洢��Ϣ���ַ������ָ�롣
len����Ϣ�ĳ��ȡ�
areaID����ʾ��Ϣ����ı�ʶ����
ѭ�����̣�

�ⲿѭ����iѭ�������ڱ�����Ϣ����Ĵ�ֱ���򣬼�areapos[areaID][0][0]�Ρ�

�ڲ�ѭ����jѭ�������ڱ�����Ϣ�����ˮƽ���򣬼�areapos[areaID][0][1] / 8�Ρ�

���ڲ�ѭ���У���ÿ���ֽڵ���Ϣ��8λ��ӳ�䵽��ɫ��Ϣ��

ʹ�� outputCode |= *pos++; ���ֽڵ�ֵ���� outputCode��
�ڲ��ĵڶ���ѭ����kѭ������������Ϣ�����е�ÿ������λ�û�����ɫ��
���� outputCode �����λ��LSB����ѡ����ɫ��Ϣ����ɫ���ɫ����������ɫ��Ϣ���õ� mat �С�
outputCode ����һλ������������һ��λ��
��� pos ������Ϣĩβ������ѭ����
��� pos ������Ϣĩβ�������ⲿѭ����

�������룺

����� #ifdef Code_DEBUG �� #endif ֮��Ĳ��ֱ�ʾ�ڵ���ģʽ�»�ִ�еĴ��롣��������� Code_DEBUG ����꣬��ô Show_Scale_Img(mat) �ᱻ���ã���ʾ���ɵ�ͼ��*/


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
}/*���������

mat�����ô��ݵ�Mat���󣬱�ʾͼ�����
frameType��ö������ FrameType����ʾ֡���ͣ���ʼ����������ʼ�ͽ�������ͨ����
tailLen����������ʾβ�����ȡ�
֡��ʶ���ƣ�

ʹ�� switch �����ݲ�ͬ�� frameType ���Ʋ�ͬ��֡��ʶ��

����� FrameType::Start�����ư׺ںڰ׵ı�ʶ��
����� FrameType::End�����ƺڰװ׺ڵı�ʶ��
����� FrameType::StartAndEnd������ȫ�׵ı�ʶ��
������������������ȫ�ڵı�ʶ��
ʹ�� for ѭ���ӵ�4�е���15�У�����β��������Ϣ��β��������Ϣ�����λ��LSB����ʼ���ƣ�ÿ������һλ��

���� tailLen �����λ��LSB����ѡ����ɫ��Ϣ����ɫ���ɫ����������ɫ��Ϣ���õ� mat �С�
tailLen ����һλ������������һ��λ��*/

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
/*����һ����Ϊ CodeFrame �ĺ������������ɴ����ض�֡��Ϣ�Ķ�ά��ͼ�������Ǻ�������ϸ���ͣ�

���������

frameType��ö������ FrameType����ʾ֡���ͣ���ʼ����������ʼ�ͽ�������ͨ����
info��ָ���ַ������ָ�룬��ʾҪ�������Ϣ��
tailLen����������ʾβ�����ȡ�
FrameNo����������ʾ֡��š�
����ͼ����̣�

����һ����СΪ FrameSize x FrameSize �� Mat ���󣬳�ʼֵΪȫ�ף�255, 255, 255����

����֡���ͣ�����β������ tailLen��

����� FrameType::End �� FrameType::StartAndEnd��tailLen ������Ϊ BytesPerFrame��
��������£����� tailLen ���䡣
���� BulidSafeArea ��������ͼ���Ϲ�����ȫ����

���� BulidQrPoint ��������ͼ���Ϲ�����ά��ʶ��㡣

����У���벢���� BulidFrameFlag ��������ͼ���Ϲ���֡��ʶ��

��У�����֡�����ӵ�ͼ���У����� BulidCheckCodeAndFrameNo ������

������Ϣ�������򣬵��� BulidInfoRect ����������ͼ���������Ϣ��

����ֵ��

�����������ɵĶ�ά��ͼ�� Mat ����*/

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
}/*���������

info��ָ���ַ������ָ�룬��ʾҪ�������Ϣ��
len����������ʾ��Ϣ�ĳ��ȡ�
savePath������ͼ���·����
outputFormat�����ͼ��ĸ�ʽ��
FrameCountLimit����������ʾ���ɵ�֡�����ơ�

����ͼ����̣�
����һ�� Mat ���� output ���ڱ������ɵ�ͼ��
����һ�������� counter ����ʼ��Ϊ 0��
��� FrameCountLimit Ϊ 0��ֱ�ӷ��ء�
��� len С�ڵ��� 0��ʲô��������
��� len С�ڵ��� BytesPerFrame��
����һ����СΪ BytesPerFrame �Ļ����� BUF��
�� info �е����ݸ��Ƶ� BUF��
���������� BUF ֱ���ﵽ BytesPerFrame �ĳ��ȡ�
���� CodeFrame �������ɴ��� StartAndEnd ���ͱ�ʶ�Ķ�ά��ͼ��
�����ɵ�ͼ�񰴱������ţ�Ȼ�󱣴浽�ļ��У��ļ���������������ָ���������ʽ��
��� len ���� BytesPerFrame��
��ȥ BytesPerFrame �ĳ��ȡ�
���� CodeFrame �������ɴ��� Start ���ͱ�ʶ�Ķ�ά��ͼ�񣬲����䱣�浽�ļ���
����������һ��
ѭ������ʣ����Ϣ������ Normal �� End ���ͱ�ʶ�Ķ�ά��ͼ�񣬲����浽�ļ���*/

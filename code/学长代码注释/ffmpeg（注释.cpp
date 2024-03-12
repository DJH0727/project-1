#include "ffmpeg.h"

namespace FFMPEG
{
    constexpr int MAXBUFLEN = 256; // 定义字符数组的最大长度

    const char ffmpegPath[] = "ffmpeg\\bin\\"; // FFmpeg可执行文件的路径
    const char tmpPath[] = "tmpdir"; // 临时文件目录的路径

    // 将图像转换为视频
    int ImagetoVideo(const char* imagePath,
                     const char* imageFormat,
                     const char* videoPath,
                     unsigned rawFrameRates,
                     unsigned outputFrameRates,
                     unsigned kbps)/*输入参数：

                    imagePath：包含图像序列的路径。
                    imageFormat：图像的格式。
                    videoPath：输出视频的路径。
                    rawFrameRates：原始图像的帧率。
                    outputFrameRates：输出视频的帧率。
                    kbps：输出视频的比特率（可选，如果为0则不指定比特率）。*/
    {
        char BUF[MAXBUFLEN];

        // 构建FFmpeg命令行字符串
        if (kbps)
            snprintf(BUF, MAXBUFLEN,
                     "\"%s\"ffmpeg.exe -r %u  -f image2 -i %s\\%%05d.%s -b:v %uK -vcodec libx264  -r %u %s",
                     ffmpegPath, rawFrameRates, imagePath, imageFormat, kbps, outputFrameRates, videoPath);
        else
            snprintf(BUF, MAXBUFLEN,
                     "\"%s\"ffmpeg.exe -r %u -f image2 -i %s\\%%05d.%s  -vcodec libx264 -r %u %s",
                     ffmpegPath, rawFrameRates, imagePath, imageFormat, outputFrameRates, videoPath);

        return system(BUF); // 调用FFmpeg执行图像到视频的转换
    }

    // 将视频转换为图像序列
    int VideotoImage(const char* videoPath,
                     const char* imagePath,
                     const char* imageFormat)
    {
        char BUF[MAXBUFLEN];

        // 创建文件目录
        snprintf(BUF, MAXBUFLEN, "md %s", imagePath);
        system(BUF);

        // 构建FFmpeg命令行字符串
        snprintf(BUF, MAXBUFLEN,
                 "\"%s\"ffmpeg.exe -i %s -q:v 2 -f image2  %s\\%%05d.%s",
                 ffmpegPath, videoPath, imagePath, imageFormat);

        return system(BUF); // 调用FFmpeg执行视频到图像序列的转换
    }

    // 测试函数，调用VideotoImage和ImagetoVideo进行图像和视频的转换
    int test(void)
    {
        bool tag = VideotoImage("test.mp4", tmpPath, "png"); // 将视频转换为PNG格式的图像序列
        if (tag)
            return tag;

        // 将PNG图像序列转换为视频
        tag = ImagetoVideo(tmpPath, "png", "out.mp4", 30, 30);
        return tag;
    }
} // namespace FFMPEG

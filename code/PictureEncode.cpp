#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <fstream>
int PictureEncode(const char* imageName,const char*imageFormat)
{
	std::vector<uchar> binaryStream;
	char image[256];
	char format[256];
	snprintf(image, 256, "%s.%s",imageName,imageFormat);
	snprintf(format, 256, ".%s",imageFormat);
	//std::cout << image << std::endl << format << std::endl;
	cv::Mat img = cv::imread(image,cv::IMREAD_COLOR);//注意后缀
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
	std::cout << "Binary stream length: " << binaryStream.size() << std::endl;
	std::ofstream output_file("output_image.bin", std::ios::binary);
	output_file.write(reinterpret_cast<const char*>(binaryStream.data()), binaryStream.size());
	output_file.close();

	return 0;
}
/*int main()
{
	PictureEncode("Robot","jpg");
	return 0;
}*/

#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include "Encode.h"
#include "2.h"
#include"Decode.h"
#include"pic.h"
#include<Windows.h>
int main(int argc, char* argv[])
{
	
	//PictureEncode(ImageName, "jpg", OutBin);
	//BinFiletoVideo("1.bin", "OutVideo.mp4", 2000, 15);
	//system("bin\\ffmpeg.exe -i 1.mp4 -f image2 -vf fps=30/1 -qscale:v 2 QRcodeOut\\%04d.png");
	GetQR("1.mp4", "out.bin", "vout.bin");
	//char a[] = "input", b[] = "jpg";
	///FuckingShit(a, b);
#ifdef RELEASE



	const bool flag = false;
	if (flag) 
	{
		char* InputBinName = argv[1];
		char* OutVedeoName = argv[2];
		int  TimeLimit = stoi(argv[3]);
		int VideoFps = 15;
		if(argc==5)VideoFps=stoi(argv[4]);
		BinFiletoVideo(InputBinName, OutVedeoName, TimeLimit, VideoFps);
		//string path = "QRcode";
		system("rmdir /s /q QRcode");
		

	}
	else 
	{
		char* VideoName = argv[1];
		char* OutBinName = argv[2];
		char* VOutBinName = argv[3];
		GetQR(VideoName, OutBinName, VOutBinName);
	}
	
#endif // RELEASE

	return 0;
}




#pragma once
#include <cstdlib>
#include <cstring>
#include <cstdio>
int ImagetoVideo(const char* ffmPath, unsigned int videofps, const char* imagePath,
	const char* imageFormat, const char* videoOutput, unsigned int bitRates);
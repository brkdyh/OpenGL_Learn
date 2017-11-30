#pragma once
#ifndef TGAPROCESSER_H  
#define TGAPROCESSER_H
#include <iostream>

typedef struct
{
	unsigned char imageTypeCode;
	short int imageWidth;
	short int imageHeight;
	unsigned char bitCount;
	unsigned char *imageData;
}TGAFILE;

static int LoadTGAFile(std::string filename, TGAFILE *tgaFile)
{
	FILE *filePtr;
	unsigned char ucharBad;
	short int sintBad;
	long imageSize;
	int colorMode;
	long imageIdx;
	unsigned char colorSwap;

	filePtr = fopen(filename.c_str(), "rb");
	if (!filePtr)
	{
		return 0;
	}

	fread(&ucharBad, sizeof(unsigned char), 1, filePtr);
	fread(&ucharBad, sizeof(unsigned char), 1, filePtr);

	fread(&tgaFile->imageTypeCode, sizeof(unsigned char), 1, filePtr);
	if ((tgaFile->imageTypeCode != 2) && (tgaFile->imageTypeCode != 3))
	{
		fclose(filePtr);
		return 0;
	}

	fread(&sintBad, sizeof(short int), 1, filePtr);
	fread(&sintBad, sizeof(short int), 1, filePtr);
	fread(&ucharBad, sizeof(unsigned char), 1, filePtr);
	fread(&sintBad, sizeof(short int), 1, filePtr);
	fread(&sintBad, sizeof(short int), 1, filePtr);

	//读取图像尺寸大小
	fread(&tgaFile->imageWidth, sizeof(short int), 1, filePtr);
	fread(&tgaFile->imageHeight, sizeof(short int), 1, filePtr);

	//读取图像位深度
	fread(&tgaFile->bitCount, sizeof(unsigned char), 1, filePtr);

	//1字节
	fread(&ucharBad, sizeof(unsigned char), 1, filePtr);

	colorMode = tgaFile->bitCount / 8;
	imageSize = tgaFile->imageWidth * tgaFile->imageHeight *colorMode;

	//为图像数据分配内存
	tgaFile->imageData = (unsigned char*)malloc(sizeof(unsigned char)*imageSize);

	//读入图像数据
	fread(tgaFile->imageData, sizeof(unsigned char), imageSize, filePtr);

	for (imageIdx = 0; imageIdx < imageSize; imageIdx += colorMode)
	{
		colorSwap = tgaFile->imageData[imageIdx];
		tgaFile->imageData[imageIdx] = tgaFile->imageData[imageIdx + 2];
		tgaFile->imageData[imageIdx + 2] = colorSwap;
	}

	fclose(filePtr);

	return 1;
}

#endif
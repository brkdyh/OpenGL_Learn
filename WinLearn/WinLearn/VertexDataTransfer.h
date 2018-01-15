#pragma once

#ifndef VERTEXDATATRANSFER_H  
#define VERTEXDATATRANSFER_H


#include <iostream>
#include <list>
#include <numeric>
#include <algorithm>
#include <glew.h>

using namespace std;

class VertexData
{
public:
	GLuint location;
	float *vertexData;
	int dataSize;;
	GLenum useage;
	GLenum dataType;
	GLuint groupSize;
	GLuint indexOffset;

public:
	VertexData(GLuint c_location, int c_dataSize, float *c_vertexData, GLenum c_useage, GLenum c_dataType, GLuint c_indexOffset, GLuint c_sgroupSize);
};

class VertexDataTransfer
{
public:
	int size;
	GLuint vaoHandle;
	GLuint *vboHandles;
	list<VertexData> dataList;
	
public:
	VertexDataTransfer(int size);
	void Transfer();
	void AddVertexData(VertexData *data);
	void Tell();
	void Detell();
};

#endif



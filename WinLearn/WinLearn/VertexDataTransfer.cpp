#include <VertexDataTransfer.h>
#include <glew.h>
#include <windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <GLAUX.H>
#include <iostream>

#include "Texture.h"
#include <string.h>
#include <math.h>;
#include "FBXProcesser.h"
#include "_cFBXModel.h"
#include <stdio.h>;
#include <list>;
#include <sstream>;
#include <math.h>
#include "VertexDataTransfer.h"

VertexData::VertexData(GLuint c_location, int c_dataSize, float *c_vertexData,
	GLenum c_useage, GLenum c_dataType, GLuint c_indexOffset, GLuint c_groupSize)
{
	location = c_location;
	dataSize = c_dataSize;
	vertexData = c_vertexData;
	useage = c_useage;
	dataType = c_dataType;
	indexOffset = c_indexOffset;
	groupSize = c_groupSize;
}

VertexDataTransfer::VertexDataTransfer(int size)
{
	this->size = size;
}

void VertexDataTransfer::Transfer()
{
	//��������VAO
	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);

	//����VBO Handles
	vboHandles = new GLuint[size];
	glGenBuffers(size, vboHandles);

	list<VertexData>::iterator ite;

	int i = 0;

	for (ite = dataList.begin(); ite != dataList.end(); ++ite)
	{
		VertexData data = *ite;

		GLuint curHandle = vboHandles[i];

		//�󶨲�����VBO����
		glBindBuffer(GL_ARRAY_BUFFER, curHandle);
		glBufferData(GL_ARRAY_BUFFER, sizeof(data.vertexData[0])*data.dataSize, data.vertexData, GL_STATIC_DRAW);

		//����VBO����
		glEnableVertexAttribArray(data.location);				//����glVertexAttribPointer֮ǰ��Ҫ���а󶨲���
		glBindBuffer(GL_ARRAY_BUFFER, curHandle);
		glVertexAttribPointer(data.location, data.groupSize, data.dataType, GL_FALSE, 0, 0);
		i++;
	}
}

void VertexDataTransfer::AddVertexData(VertexData * data)
{
	dataList.push_back(*data);
}
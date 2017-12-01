#pragma once

#ifndef VBOOBJECT_H
#define VBOOBJECT_H

#include <glew.h>
typedef enum
{
	VDT_VERTEX_POSITION = 0x0001,
	VDT_VERTEX_UV = 0x0002,
	VDT_VERTEX_COLOR = 0x0003,
}VertexDataType;

class VboObject
{
private:
	GLuint vboName;
	GLfloat *vboData;
	unsigned int dataCount;
private:
	void Transfer();

public:
	VertexDataType VDT;

public:
	VboObject(VertexDataType verDataType, GLfloat *data, unsigned int dataCount);
	~VboObject();
	void GenBuffer();	
};

#endif
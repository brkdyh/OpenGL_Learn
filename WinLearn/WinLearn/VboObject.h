#pragma once

#ifndef VBOOBJECT_H
#define VBOOBJECT_H

#include <glew.h>

#define VBO_DATA_VERTEX = 0;
#define VBO_DATA_UV = 1;
#define VBO_DATA_VERTEX_COLOR = 2;

typedef int VBO_DATA_TYPE;

class VboObject
{
private:
	GLuint *vertex;
	GLuint *uv;
	GLuint *vertexColor;

public:
	VboObject();
	~VboObject();
	int Bind(VBO_DATA_TYPE type, GLuint *data);
};

#endif
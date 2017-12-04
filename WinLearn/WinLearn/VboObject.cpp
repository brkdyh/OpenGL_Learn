#include <glew.h>
#include "VboObject.h"

VboObject::VboObject(VertexDataType verDataType, GLfloat *data, unsigned int dataCount)
{
	VDT = verDataType;
	vboData = data;
	this->dataCount = dataCount;
}

VboObject::~VboObject() 
{

}

void VboObject::GenBuffer()
{
	glGenBuffers(1, &vboName);
	Transfer();
}

void VboObject::Transfer()
{
	glBindBuffer(GL_ARRAY_BUFFER, vboName);		//°ó¶¨BO

	glBufferData(vboName, dataCount, vboData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(VDT);
	switch (VDT)
	{
	case VDT_VERTEX_POSITION:
	{
		glVertexAttribPointer(VDT, 4, GL_FLOAT, GL_FALSE, 0, 0);
		break;
	}
	case VDT_VERTEX_UV:
	{
		glVertexAttribPointer(VDT, 2, GL_FLOAT, GL_FALSE, 0, 0);
		break;
	}
	default:
		break;
	}

	//glBindBuffer(GL_ARRAY_BUFFER, 0);		//Çå³ýBO
}
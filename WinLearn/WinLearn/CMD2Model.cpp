#include <CMD2Model.h>;
#include <stdio.h>;
#include <glew.h>;
#include <GL/GLU.h>;
#include <iostream>;
#include <Texture.h>
#include <sstream>
//#include <EasyLog.h>

CMD2Model::CMD2Model()
{
	numFrames = 0;
	numVertices = 0;
	numTriangles = 0;
	numST = 0;
	frameSize = 0;
	currentFrame = 0;
	nextFrame = 1;
	interpol = 0.0;
	triIndex = NULL;
	st = NULL;
	vertexList = NULL;
	modelTex = NULL;
}

CMD2Model::~CMD2Model()
{

}

void CMD2Model::SetupSkin(texture_t *thisTexture)
{
	glGenTextures(1, &thisTexture->texID);
	glBindTexture(GL_TEXTURE_2D, thisTexture->texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	switch (thisTexture->textureType)
	{
	case BMP:
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, thisTexture->width, thisTexture->height,
			GL_RGB, GL_UNSIGNED_BYTE, thisTexture->data);
		break;;
	case  PCX:
			gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, thisTexture->width, thisTexture->height,
				GL_RGBA, GL_UNSIGNED_BYTE, thisTexture->data);
	case TGA:
		break;
	default:
		break;
	}
}

int CMD2Model::Load(char *modelFile, char *skinFile)
{
	FILE *filePtr;
	int fileLen;
	char *buffer;

	modelHeader_t *modelHeader;

	stIndex_t *stPtr;
	frame_t *frame;
	vector3_t *vertexListPtr;
	mesh_t *bufIndexPtr;
	int i, j;

	//打开模型文件
	filePtr = fopen(modelFile, "rb");
	if (filePtr == NULL)
		return 0;

	fseek(filePtr, 0, SEEK_END);
	fileLen = ftell(filePtr);
	fseek(filePtr, 0, SEEK_SET);

	//将文件读入缓存
	buffer = new char[fileLen + 1];
	fread(buffer, sizeof(char), fileLen, filePtr);

	//
	modelHeader = (modelHeader_t *)buffer;
	vertexList = new vector3_t[modelHeader->numXYZ * modelHeader->numFrames];
	numFrames = modelHeader->numFrames;
	frameSize = modelHeader->framesize;

	for (j = 0; j < numFrames; j++)
	{
		frame = (frame_t*)&buffer[modelHeader->offsetFrames + frameSize*j];
		vertexListPtr = (vector3_t*)&vertexList[numVertices*j];
		for (i = 0; i < numVertices; i++)
		{
			vertexListPtr[i].point[0] = frame->scale[0] * frame->fp[i].v[0] + frame->tanslate[0];
			vertexListPtr[i].point[1] = frame->scale[1] * frame->fp[i].v[1] + frame->tanslate[1];
			vertexListPtr[i].point[2] = frame->scale[2] * frame->fp[i].v[2] + frame->tanslate[2];
		}
	}

	//Texture tex = Texture(GL_TEXTURE_2D, skinFile);
	//modelTex->
	//modelTex = 
}

int CMD2Model::LoadModel(char *modelFile)
{
	FILE *filePtr;
	int fileLen;
	char *buffer;

	modelHeader_t *modelHeader;

	stIndex_t *stPtr;
	frame_t *frame;
	vector3_t *vertexListPtr;
	mesh_t *bufIndexPtr;

	int i, j;
	filePtr = fopen(modelFile, "rb");

	if (filePtr == NULL)
	{
		return 0;
	}

	fseek(filePtr, 0, SEEK_END);
	fileLen = ftell(filePtr);
	fseek(filePtr, 0, SEEK_SET);

	buffer = new char[fileLen + 1];
	fread(buffer, sizeof(char), fileLen, filePtr);

	modelHeader = (modelHeader_t*)buffer;
	if (vertexList != NULL)
	{
		delete[]vertexList;
	}

	vertexList = new vector3_t[modelHeader->numXYZ * modelHeader->numFrames];
	numVertices = modelHeader->numXYZ;
	numFrames = modelHeader->numFrames;
	frameSize = modelHeader->framesize;

	for (int j = 0; j < numFrames; j++)
	{
		frame = (frame_t*)&buffer[modelHeader->offsetFrames + frameSize*j];

		vertexListPtr = (vector3_t*)&vertexList[numVertices*j];

		for (i = 0; i < numVertices; i++)
		{
			vertexListPtr[i].point[0] = frame->scale[0] * frame->fp[i].v[0] + frame->tanslate[0];
			vertexListPtr[i].point[1] = frame->scale[1] * frame->fp[i].v[1] + frame->tanslate[1];
			vertexListPtr[i].point[2] = frame->scale[2] * frame->fp[i].v[2] + frame->tanslate[2];
		}
	}

	numST = modelHeader->numST;
	if (st!=NULL)
	{
		delete[] st;
	}

	st = new texCoord_t[numST];

	stPtr = (stIndex_t *)&buffer[modelHeader->offestST];
	for (i = 0; i < numST; i++)
	{
		st[i].s = 0.0;
		st[i].t = 0.0;
	}

	numTriangles = modelHeader->numTris;

	if (triIndex!=NULL)
	{
		delete[] triIndex;
	}

	triIndex = new mesh_t[numTriangles];

	bufIndexPtr = (mesh_t *)&buffer[modelHeader->offsetTris];

	for (j = 0; j < numFrames; j++)
	{
		for (i = 0; i < numTriangles; i++)
		{
			triIndex[i].meshIndex[0] = bufIndexPtr[i].meshIndex[0];
			triIndex[i].meshIndex[1] = bufIndexPtr[i].meshIndex[1];
			triIndex[i].meshIndex[2] = bufIndexPtr[i].meshIndex[2];

			triIndex[i].stIndex[0] = bufIndexPtr[i].stIndex[0];
			triIndex[i].stIndex[1] = bufIndexPtr[i].stIndex[1];
			triIndex[i].stIndex[2] = bufIndexPtr[i].stIndex[2];
		}

		fclose(filePtr);

		delete[] buffer;

		currentFrame = 0;
		nextFrame = 1;
		interpol = 0.0;

		return 0;
	}
}

int CMD2Model::RenderFrame(int keyFrame)
{
	vector3_t *vList;
	int i;

	// create a pointer to the frame we want to show
	vList = &vertexList[numVertices * keyFrame];

	if (modelTex != NULL)
		// set the texture
		glBindTexture(GL_TEXTURE_2D, modelTex->texID);

	std::ostringstream oss;
	oss << "-----------MD2 模型渲染-----------\n";

	glCullFace(GL_BACK);
	// display the textured model with proper lighting normals
	glBegin(GL_TRIANGLES);
	for (i = 0; i < numTriangles; i++)
	{
		//CalculateNormal(vList[triIndex[i].meshIndex[0]].point,
			//vList[triIndex[i].meshIndex[2]].point,
			//vList[triIndex[i].meshIndex[1]].point);

		glColor3f(1.0f, 0.0f, 0.0f);

		if (modelTex != NULL)
			glTexCoord2f(st[triIndex[i].stIndex[0]].s,
				st[triIndex[i].stIndex[0]].t);

		glVertex3fv(vList[triIndex[i].meshIndex[0]].point);
		//oss << "x = " << vList[triIndex[i].meshIndex[0]].point[0] << "   ";
		//oss << "y = " << vList[triIndex[i].meshIndex[0]].point[1] << "   ";
		//oss << "z = " << vList[triIndex[i].meshIndex[0]].point[2] << "\n";

		glColor3f(0.0f, 1.0f,0.0f);

		if (modelTex != NULL)
			glTexCoord2f(st[triIndex[i].stIndex[2]].s,
				st[triIndex[i].stIndex[2]].t);

		glVertex3fv(vList[triIndex[i].meshIndex[2]].point);
		//oss << "x = " << vList[triIndex[i].meshIndex[2]].point[0] << "   ";
		//oss << "y = " << vList[triIndex[i].meshIndex[2]].point[1] << "   ";
		//oss << "z = " << vList[triIndex[i].meshIndex[2]].point[2] << "\n";

		glColor3f(0.0f, 0.0f, 1.0f);
		if (modelTex != NULL)
			glTexCoord2f(st[triIndex[i].stIndex[1]].s,
				st[triIndex[i].stIndex[1]].t);

		glVertex3fv(vList[triIndex[i].meshIndex[1]].point);
		//oss << "x = " << vList[triIndex[i].meshIndex[1]].point[0] << "   ";
		//oss << "y = " << vList[triIndex[i].meshIndex[1]].point[1] << "   ";
		//oss << "z = " << vList[triIndex[i].meshIndex[1]].point[2] << "\n";
	}
	glEnd();

	//EasyLog::Inst()->Log(oss.str());
	return 0;
}
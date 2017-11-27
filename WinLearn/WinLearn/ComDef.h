#pragma once

typedef struct
{
	float point[2];
}vector2_t;

typedef struct
{
	float point[3];
}vector3_t;

inline vector3_t operator+(vector3_t a,vector3_t b)
{
	vector3_t c;
	c.point[0] = a.point[0] + b.point[0];
	c.point[1] = a.point[1] + b.point[1];
	c.point[2] = a.point[2] + b.point[2];

	return c;
}

inline vector3_t operator-(vector3_t a, vector3_t b)
{
	vector3_t c;
	c.point[0] = a.point[0] - b.point[0];
	c.point[1] = a.point[1] - b.point[1];
	c.point[2] = a.point[2] - b.point[2];

	return c;
}

inline vector3_t operator*(float f, vector3_t b)
{
	vector3_t c;
	c.point[0] = f*b.point[0];
	c.point[1] = f*b.point[1];
	c.point[2] = f*b.point[2];

	return c;
}

//////////////////////////////////////////////////////////////////////////纹理坐标//////////////////////////////////////////////////////////////////////////
typedef struct 
{
	float s;
	float t;
}texCoord_t;

typedef struct
{
	short s;
	short t;
}stIndex_t;

//////////////////////////////////////////////////////////////////////////三角形网格//////////////////////////////////////////////////////////////////////////
typedef struct
{
	unsigned short meshIndex[3];
	unsigned short stIndex[3];
}mesh_t;
//////////////////////////////////////////////////////////////////////////纹理数据//////////////////////////////////////////////////////////////////////////
enum texTypes_t
{
	PCX,BMP,TGA
};
typedef struct
{
	texTypes_t textureType;

	int width;
	int height;
	long int scaledWidth;
	long int scaledHeight;
	unsigned int texID;
	unsigned char *data;
	unsigned char *palette;
}texture_t;

//////////////////////////////////////////////////////////////////////////帧数据//////////////////////////////////////////////////////////////////////////
typedef struct
{
	unsigned char v[3];
	unsigned char normalIndex;
}framePoint_t;

typedef struct
{
	float scale[3];
	float tanslate[3];
	char name[16];
	framePoint_t fp[1];
}frame_t;

//////////////////////////////////////////////////////////////////////////MD2模型数据//////////////////////////////////////////////////////////////////////////
typedef struct
{
	int numFrames;			//帧数
	int numPoints;			//顶点数
	int numTriangles;		//三角形数
	int sumST;				//纹理坐标数量
	int frameSize;			//每一帧数据字节数
	int texWidth, texHeight;//纹理尺寸
	int currentFrame;		//当前帧索引
	int nextFrame;			//下一帧索引
	float interpol;			//插值用数据
	mesh_t *triIndex;		//三角形网格列表
	texCoord_t *st;			//纹理坐标
	vector3_t *pointList;	//顶点数据列表
	texture_t *modelTex;	//纹理数据
}md2ModelData_t;

typedef struct
{
	int ident;
	int version;
	int skinwidth;
	int skinheight;
	int framesize;
	int numSkins;
	int numXYZ;
	int numST;
	int numTris;
	int numGLcmds;
	int numFrames;
	int offestSkins;
	int offestST;
	int offsetTris;
	int offsetFrames;
	int offsetGLcmds;
	int offsetEnd;
}modelHeader_t;


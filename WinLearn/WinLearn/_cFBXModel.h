#pragma once


#ifndef _CFBXMODEL_H  
#define _CFBXMODEL_H 

#include <comdef.h>
#include <iostream>
#include <list>
#include <numeric>
#include <algorithm>
#include <Texture.h>

using namespace std;
//FBX网格
class _cFBXMesh
{
public:
	_cFBXMesh() {};
	int polygonCount;		//三角形数量
	int *polygonVex;		//三角形顶点索引
	vector3_t *vexList;		//顶点位置数组
	vector2_t *UvList;		//顶点UV数组
	const char *texFile;
	Texture *tex;
};

//Fbx模型类
class _cFBXModel
{
public:
	_cFBXModel() {};
	//_cFBXMesh *mesh;		//网格
	list<_cFBXMesh> meshList;	//网格列表
};
#endif

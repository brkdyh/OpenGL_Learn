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
//FBX����
class _cFBXMesh
{
public:
	_cFBXMesh() {};
	int polygonCount;		//����������
	int *polygonVex;		//�����ζ�������
	vector3_t *vexList;		//����λ������
	vector2_t *UvList;		//����UV����
	const char *texFile;
	Texture *tex;
};

//Fbxģ����
class _cFBXModel
{
public:
	_cFBXModel() {};
	//_cFBXMesh *mesh;		//����
	list<_cFBXMesh> meshList;	//�����б�
};
#endif



#ifndef FBXPROCESSER_H  
#define FBXPROCESSER_H

#include <_cFBXModel.h>
#include <fbxsdk.h>

class FBXProcesser
{

public:
	FBXProcesser(char *filePath,char *fileName);
	bool Init();
	void LoadNode();
	std::string GetAttributeTypeName(FbxNodeAttribute::EType type);
	void PrintAttribute(FbxNodeAttribute* pattribute);
	void ProcessMesh(FbxMesh *mesh, const char *nodeName);
	void PrintNode(FbxNode* pnode);
	_cFBXModel *model;
};

#endif

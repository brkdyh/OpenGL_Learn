#include <_cFBXModel.h>
#include <fbxsdk.h>

#ifndef FBXPROCESSER_H  
#define FBXPROCESSER_H 

class FBXProcesser
{

public:
	FBXProcesser(char *fileName);
	bool Init();
	void LoadNode();
	std::string GetAttributeTypeName(FbxNodeAttribute::EType type);
	void PrintAttribute(FbxNodeAttribute* pattribute);
	void ProcessMesh(FbxMesh *mesh);
	void PrintNode(FbxNode* pnode);
	_cFBXModel *model;
};

#endif

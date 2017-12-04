#include <fbxsdk.h>
#include <string.h>
#include <iostream>
#include <fbxsdk/fileio/fbximporter.h>;
#include "EasyLog.h"
#include <FBXProcesser.h>
#include <comdef.h>
#include <sstream>
#include <_cFBXModel.h>
#include "core/math/fbxvector4.h"

FbxManager *mFbxManager;
FbxScene *mFbxScene;
const char *fbxFilePath;
const char *fbxFileName;
int curMeshIndex = 0;
ostringstream oss;
std::string s;

FBXProcesser::FBXProcesser(char *filePath, char *fileName)
{
	fbxFilePath = filePath;
	s = filePath;
	s = s.append("/");
	s = s.append(fileName);
	fbxFileName = s.c_str();		//模型路径
}

bool FBXProcesser::Init()
{
	//FBX SDK Manager
	mFbxManager = FbxManager::Create();

	//IOSetting object
	FbxIOSettings* ios = FbxIOSettings::Create(mFbxManager, IOSROOT);
	mFbxManager->SetIOSettings(ios);

	FbxImporter *mFbxImporter = FbxImporter::Create(mFbxManager, fbxFileName);

	if (!mFbxImporter->Initialize(fbxFileName, -1, mFbxManager->GetIOSettings()))
	{
		return -1;
	}

	//load fbx scene
	mFbxScene = FbxScene::Create(mFbxManager, fbxFileName);

	mFbxImporter->Import(mFbxScene);
	mFbxImporter->Destroy();

	return true;
}
int gg = 0;
void FBXProcesser::LoadNode()
{
	this->model = new _cFBXModel();

	FbxNode *root = mFbxScene->GetRootNode();//根节点

	//加载Fbx节点属性
	for (int i = 0; i < root->GetChildCount(false); i++)
	{
		FbxNode *curNode = root->GetChild(i);

		PrintNode(curNode);

		FbxNodeAttribute *curAtt = curNode->GetNodeAttribute();
		switch (curAtt->GetAttributeType())
		{		
		case  FbxNodeAttribute::eMesh:	//网格
		{
			std::string texPath = std::string(fbxFilePath);
			texPath.append("/");
			texPath.append(curNode->GetName());
			ProcessMesh(curNode->GetMesh(), texPath.c_str());
			gg++;
			break;
		}
		case FbxNodeAttribute::eSkeleton:	//骨骼

			break;

		default:
			break;
		}
	}

	//单独加载纹理

	//EasyLog::Inst()->Log(oss.str());
}

std::string FBXProcesser::GetAttributeTypeName(FbxNodeAttribute::EType type)
{
	switch (type)
	{
	case FbxNodeAttribute::eUnknown: return "UnknownAttribute";
	case FbxNodeAttribute::eNull: return "Null";
	case FbxNodeAttribute::eMarker: return "marker";  //马克……  
	case FbxNodeAttribute::eSkeleton: return "Skeleton"; //骨骼  
	case FbxNodeAttribute::eMesh: return "Mesh"; //网格  
	case FbxNodeAttribute::eNurbs: return "Nurbs"; //曲线  
	case FbxNodeAttribute::ePatch: return "Patch"; //Patch面片  
	case FbxNodeAttribute::eCamera: return "Camera"; //摄像机  
	case FbxNodeAttribute::eCameraStereo: return "CameraStereo"; //立体  
	case FbxNodeAttribute::eCameraSwitcher: return "CameraSwitcher"; //切换器  
	case FbxNodeAttribute::eLight: return "Light"; //灯光  
	case FbxNodeAttribute::eOpticalReference: return "OpticalReference"; //光学基准  
	case FbxNodeAttribute::eOpticalMarker: return "OpticalMarker";
	case FbxNodeAttribute::eNurbsCurve: return "Nurbs Curve";//NURBS曲线  
	case FbxNodeAttribute::eTrimNurbsSurface: return "Trim Nurbs Surface"; //曲面剪切？  
	case FbxNodeAttribute::eBoundary: return "Boundary"; //边界  
	case FbxNodeAttribute::eNurbsSurface: return "Nurbs Surface"; //Nurbs曲面  
	case FbxNodeAttribute::eShape: return "Shape"; //形状  
	case FbxNodeAttribute::eLODGroup: return "LODGroup"; //  
	case FbxNodeAttribute::eSubDiv: return "SubDiv";
	default: return "UnknownAttribute";
	}
}

void FBXProcesser::PrintAttribute(FbxNodeAttribute* pattribute)
{
	if (!pattribute)
	{
		return;
	}
	std::string typeName = GetAttributeTypeName(pattribute->GetAttributeType());
	std::string attrName = pattribute->GetName();
	//PrintTabs();

	//FbxString.Buffer() 才是我们需要的字符数组  
	oss << "<attribute type='" << typeName << "' name='" << attrName << "'/>\n ";
}

void FBXProcesser::PrintNode(FbxNode* pnode)
{
	const char* nodeName = pnode->GetName(); //获取节点名字  

	FbxDouble3 translation = pnode->LclTranslation.Get();//获取这个node的位置、旋转、缩放  
	FbxDouble3 rotation = pnode->LclRotation.Get();
	FbxDouble3 scaling = pnode->LclScaling.Get();

	//打印出这个node的概览属性  
	//printf("<node name='%s' translation='(%f,%f,%f)' rotation='(%f,%f,%f)' scaling='(%f,%f,%f)'>\n",
	//	nodeName,
	//	translation[0], translation[1], translation[2],
	//	rotation[0], rotation[1], rotation[2],
	//	scaling[0], scaling[1], scaling[2]);

	oss << "<node name='" << nodeName << "' translation='(" << translation[0] << "," << translation[1] << ","
		<< translation[2] << ")' rotation='(" << rotation[0] << "," << rotation[1] << "," << rotation[2] << ")' scaling='("
		<< scaling[0] << "," << scaling[1] << "," << scaling[2] << ")'>\n";

	//打印这个node 的所有属性  
	for (int i = 0; i < pnode->GetNodeAttributeCount(); i++)
	{
		PrintAttribute(pnode->GetNodeAttributeByIndex(i));
	}

	//递归打印所有子node的属性  
	for (int j = 0; j < pnode->GetChildCount(); j++)
	{
		PrintNode(pnode->GetChild(j));
	}

	oss << "</node>\n";
}

void ReadVertex(FbxMesh* pMesh, int ctrlPointIndex, vector3_t* pVertex)
{
	FbxVector4* pCtrlPoint = pMesh->GetControlPoints();

	pVertex->point[0] = pCtrlPoint[ctrlPointIndex].mData[0] * 0.01f;
	pVertex->point[1] = pCtrlPoint[ctrlPointIndex].mData[1] * 0.01f;
	pVertex->point[2] = pCtrlPoint[ctrlPointIndex].mData[2] * 0.01f;
}

bool ReadUV(FbxMesh* pMesh, int ctrlPointIndex, int textureUVIndex, int uvLayer, vector2_t* pUV)
{
	if (uvLayer >= 2 || pMesh->GetElementUVCount() <= uvLayer)
	{
		return false;
	}

	FbxGeometryElementUV* pVertexUV = pMesh->GetElementUV(uvLayer);

	switch (pVertexUV->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
	{
		switch (pVertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			pUV->point[0] = pVertexUV->GetDirectArray().GetAt(ctrlPointIndex).mData[0];
			pUV->point[1] = pVertexUV->GetDirectArray().GetAt(ctrlPointIndex).mData[1];
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int id = pVertexUV->GetIndexArray().GetAt(ctrlPointIndex);
			pUV->point[0] = pVertexUV->GetDirectArray().GetAt(id).mData[0];
			pUV->point[1] = pVertexUV->GetDirectArray().GetAt(id).mData[1];
		}
		break;

		default:
			break;
		}
	}
	break;

	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (pVertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		case FbxGeometryElement::eIndexToDirect:
		{
			pUV->point[0] = pVertexUV->GetDirectArray().GetAt(textureUVIndex).mData[0];
			pUV->point[1] = pVertexUV->GetDirectArray().GetAt(textureUVIndex).mData[1];
		}
		break;

		default:
			break;
		}
	}
	break;
	}
}

void FBXProcesser::ProcessMesh(FbxMesh *pMesh, const char *texFilePath)
{
	if (pMesh == NULL)
	{
		return;
	}

	//GL_TEXTURE

	_cFBXMesh *cMesh = new _cFBXMesh();
	cMesh->polygonCount = pMesh->GetPolygonCount();
	cMesh->vexList = new vector3_t[cMesh->polygonCount * 3];
	cMesh->UvList = new vector2_t[cMesh->polygonCount * 3];

	int triangleCount = pMesh->GetPolygonCount();
	int polygonVertexCounter = 0;			//多边形顶点索引

	ostringstream oss;

	vector3_t vertex;
	vector2_t uv;

	for (int i = 0; i < triangleCount; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			int ctrlPointIndex = pMesh->GetPolygonVertex(i, j);

			// Read the vertex  
			ReadVertex(pMesh, ctrlPointIndex, &(vertex));

			ReadUV(pMesh, ctrlPointIndex, pMesh->GetTextureUVIndex(i, j), 0, &(uv));

			//// Read the normal of each vertex  
			//ReadNormal(pMesh, ctrlPointIndex, vertexCounter, &normal[j]);

			//// Read the tangent of each vertex  
			//ReadTangent(pMesh, ctrlPointIndex, vertexCounter, &tangent[j]);

			// 根据读入的信息组装三角形，并以某种方式使用即可，比如存入到列表中、保存到文件等...
			cMesh->vexList[polygonVertexCounter] = vertex;
			cMesh->UvList[polygonVertexCounter] = uv;
			polygonVertexCounter++;
		}
	}

	//纹理
	cMesh->tex = new Texture(GL_TEXTURE_2D, texFilePath, "tga");
	oss << "纹理名称 = " << texFilePath << "\n";
	EasyLog::Inst()->Log(oss.str());

	this->model->meshList.push_back(*cMesh);
}



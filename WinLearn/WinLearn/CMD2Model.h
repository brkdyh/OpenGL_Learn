#include "ComDef.h";

class CMD2Model
{
private:
	int numFrames;
	int numVertices;
	int numTriangles;
	int numST;
	int frameSize;
	int currentFrame;
	int nextFrame;
	float interpol;
	mesh_t *triIndex;
	texCoord_t *st;
	vector3_t *vertexList;
	texture_t *modelTex;

	void SetupSkin(texture_t *thisTexture);

public:
	CMD2Model();
	~CMD2Model();

	//同时载入模型和纹理
	int Load(char *modelFile, char *skinFile);

	//仅载入模型
	int LoadModel(char *modelFile);

	//仅载入纹理
	int LoadSkin(char *skinFile);

	//
	int SetTexture(texture_t *texture);

	int Animate(int startFrame, int endFrame, float percent);

	int RenderFrame(int ketFrame);

	//释放内存
	int Unload();
};
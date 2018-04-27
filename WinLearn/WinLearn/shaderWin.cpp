#define WIN32_LEAN_AND_MEAN

#include <glew.h>
#include <windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <GLAUX.H>
#include <iostream>

// textfile.cpp    
// simple reading and writing for text files
#include "Texture.h"
#include <string.h>
#include <math.h>;
#include "FBXProcesser.h"
#include "_cFBXModel.h"
#include <stdio.h>;
#include <list>;
#include <sstream>;
//#include "EasyLog.h"
#include <math.h>
#include "VertexDataTransfer.h"
#include <FileReadUtils.h>
#include "EasyLog.h"

#define PI	3.14159

using namespace std;

//unsigned char * readDataFromFile(char *fn)
//{
//	FILE *fp;
//	unsigned char *content = NULL;
//	int count = 0;
//	if (fn != NULL) {
//		fp = fopen(fn, "rb");
//		if (fp != NULL) {
//			fseek(fp, 0, SEEK_END);
//			count = ftell(fp);
//			rewind(fp);
//			if (count > 0) {
//				content = (unsigned char *)malloc(sizeof(unsigned char) * (count + 1));
//				count = fread(content, sizeof(unsigned char), count, fp);
//				content[count] = '\0';
//			}
//			fclose(fp);
//		}
//	}
//	return content;
//
//}
//
////读入字符流  
//char *textFileRead(const char *fn)
//{
//	FILE *fp;
//	char *content = NULL;
//	int count = 0;
//	if (fn != NULL)
//	{
//		fp = fopen(fn, "rt");
//		if (fp != NULL)
//		{
//			fseek(fp, 0, SEEK_END);
//			count = ftell(fp);
//			rewind(fp);
//			if (count > 0)
//			{
//				content = (char *)malloc(sizeof(char) * (count + 1));
//				count = fread(content, sizeof(char), count, fp);
//				content[count] = '\0';
//			}
//			fclose(fp);
//		}
//	}
//	return content;
//}
//
//int textFileWrite(char *fn, char *s)
//{
//	FILE *fp;
//	int status = 0;
//	if (fn != NULL) {
//		fp = fopen(fn, "w");
//		if (fp != NULL) {
//			if (fwrite(s, sizeof(char), strlen(s), fp) == strlen(s))
//				status = 1;
//			fclose(fp);
//		}
//	}
//	return(status);
//}

							////// Mouse/Camera Variables
int mouseX, mouseY;		// mouse coordinates
float cameraX, cameraY, cameraZ=100.0f;	// camera coordinates
float lookX, lookY, lookZ = -100.0f;		// camera look-at coordinates

using namespace std;

WNDCLASSEX windowClass;
HDC g_HDC;

/*Shader*/
GLuint programHandle;
GLuint vShader, fShader;

float rAngle = 0.0f;

//顶点位置数组    
float positionData[] =
{
	-5.0f,-5.0f,20.0f,1.0f,
	5.0f,-5.0f,20.0f,1.0f,
	5.0f,5.0f,20.0f,1.0f,
	-5.0f,5.0f,20.0f,1.0f
};

float positionData_2[] =
{
	-0.5f,-0.5f,0.0f,
	0.5f,-0.5f,0.0f,
	0.5f,0.5f,0.0f,
	-0.5f,0.5f,0.0f,
};

float positionData_1[] =
{
	-5.0f,-5.0f,20.0f,
	5.0f,-5.0f,20.0f,
	5.0f,5.0f,20.0f,
	-5.0f,5.0f,20.0f,
};

//顶点uv数据
float uvData[] = {
	0.0f,1.0f,
	1.0f,1.0f,
	0.0f,0.0f,
	1.0f,0.0f,
};

//顶点颜色数组    
float colorData[] = {
	1.0f, 0.0f, 0.0f,1.0f,
	0.0f, 1.0f, 0.0f,1.0f,
	0.0f, 0.0f, 1.0f,1.0f,
	1.0f,1.0f,0.0f,1.0f
};

ostringstream oss_log;

void LoadShader(const char *VShaderFile, const char *FShaderFile)
{
#pragma region 顶点着色器

	vShader = glCreateShader(GL_VERTEX_SHADER);

	if (0 == vShader)
	{
		//错误处理
	}

	const GLchar *vShaderCode = textFileRead(VShaderFile);
	const GLchar *vCodeArray[1] = { vShaderCode };

	//将字符数组绑定到对应的着色器对象上
	glShaderSource(vShader, 1, vCodeArray, NULL);

	//编译着色器对象
	glCompileShader(vShader);

	GLint compileResult;
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &compileResult);

	if (GL_FALSE == &compileResult)
	{
		GLint logLen;

		//得到编译日志长度    
		glGetShaderiv(vShader, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0)
		{
			char *log = (char *)malloc(logLen);
			GLsizei written;
			//得到日志信息并输出    
			glGetShaderInfoLog(vShader, logLen, &written, log);
			//cerr << "vertex shader compile log : " << endl;
			//cerr << log << endl;
			//free(log);//释放空间    
		}
	}
#pragma endregion

#pragma region 片段着色器
	fShader = glCreateShader(GL_FRAGMENT_SHADER);

	if (0 == fShader)
	{
		//错误处理
	}

	const GLchar *fShaderCode = textFileRead(FShaderFile);
	const GLchar *fCodeArray[1] = { fShaderCode };

	glShaderSource(fShader, 1, fCodeArray, NULL);

	//编译着色器对象    
	glCompileShader(fShader);

	//检查编译是否成功    
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &compileResult);
	if (GL_FALSE == compileResult)
	{
		GLint logLen;
		//得到编译日志长度    
		glGetShaderiv(fShader, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0)
		{
			char *log = (char *)malloc(logLen);
			GLsizei written;
			//得到日志信息并输出    
			glGetShaderInfoLog(fShader, logLen, &written, log);
			//cerr << "fragment shader compile log : " << endl;
			//cerr << log << endl;

			//oss_1 << "error = " << log;
			free(log);//释放空间
			//EasyLog::Inst()->Log(oss_1.str());
		}
	}

#pragma endregion

	//3、链接着色器对象

	//创建着色器程序    
	programHandle = glCreateProgram();
	if (!programHandle)
	{
		//cerr << "ERROR : create program failed" << endl;
		//exit(1);
	}
	//将着色器程序链接到所创建的程序中    
	glAttachShader(programHandle, vShader);
	glAttachShader(programHandle, fShader);
	//将这些对象链接成一个可执行程序    
	glLinkProgram(programHandle);
	//查询链接的结果    
	GLint linkStatus;
	glGetProgramiv(programHandle, GL_LINK_STATUS, &linkStatus);
	if (GL_FALSE == linkStatus)
	{
		GLint logLen;
		glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH,
			&logLen);
		if (logLen > 0)
		{
			char *log = (char *)malloc(logLen);
			GLsizei written;
			glGetProgramInfoLog(programHandle, logLen,
				&written, log);

			oss_log << "链接失败 --  Program log : " << log;

			EasyLog::Inst()->Log(oss_log.str());
		}
	}
	else//链接成功，在OpenGL管线中使用渲染程序    
	{
		//oss_1 << "链接成功，使用着色器";
		//EasyLog::Inst()->Log(oss_1.str());
	}
}

float *vertexFloatArray;
float *vertexUvArray;
int vertexFloatCount;
int vertexUvFloatCount;

//void initVBO()
//{
//	//绑定VAO  
//	glGenVertexArrays(1, &vaoHandle);
//	glBindVertexArray(vaoHandle);
//
//	// Create and populate the buffer objects    
//	GLuint vboHandles[2];
//	glGenBuffers(2, vboHandles);
//	GLuint positionBufferHandle = vboHandles[0];
//	GLuint uvBufferHandle = vboHandles[1];
//
//	//绑定VBO以供使用    
//	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexFloatArray[0])*vertexFloatCount, vertexFloatArray, GL_STATIC_DRAW);
//
//	glBindBuffer(GL_ARRAY_BUFFER, uvBufferHandle);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexUvArray[0])*vertexUvFloatCount, vertexUvArray, GL_STATIC_DRAW);
//
//	////////////////////////////////////////////////////////////////////////// VAO--解释顶点属性
//	glEnableVertexAttribArray(0);//顶点坐标    
//	//调用glVertexAttribPointer之前需要进行绑定操作    
//	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
//
//
//	glEnableVertexAttribArray(1);//顶点uv
//	glBindBuffer(GL_ARRAY_BUFFER, uvBufferHandle);
//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
//}

void SetupPixelFromat(HDC hDC)
{
	int nPixelFormat;

	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32,
		0,0,0,0,0,0,
		0,
		0,
		0,
		0,0,0,0,
		16,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0,0,0
	};

	nPixelFormat = ChoosePixelFormat(hDC, &pfd);

	SetPixelFormat(hDC, nPixelFormat, &pfd);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HGLRC hRC;					// rendering context
	static HDC hDC;						// device context
	int width, height;					// window width and height
	int oldMouseX, oldMouseY;

	switch (message)
	{
	case WM_CREATE:					// window is being created

		hDC = GetDC(hwnd);			// get current window's device context
		g_HDC = hDC;
		SetupPixelFromat(hDC);		// call our pixel format setup function

									// create rendering context and make it current
		hRC = wglCreateContext(hDC);
		wglMakeCurrent(hDC, hRC);

		return 0;
		break;

	case WM_CLOSE:					// windows is closing
									// deselect rendering context and delete it
		wglMakeCurrent(hDC, NULL);
		wglDeleteContext(hRC);

		// send WM_QUIT to message queue
		PostQuitMessage(0);

		return 0;
		break;

	case WM_SIZE:
		height = HIWORD(lParam);		// retrieve width and height
		width = LOWORD(lParam);

		if (height == 0)					// don't want a divide by zero
		{
			height = 1;
		}

		glViewport(0, 0, width, height);	// reset the viewport to new dimensions
		glMatrixMode(GL_PROJECTION);		// set projection matrix current matrix
		glLoadIdentity();					// reset projection matrix
											// calculate aspect ratio of window

		gluPerspective(60.0f, (GLfloat)width / (GLfloat)height, 1.0f, 1000.0f);

		glMatrixMode(GL_MODELVIEW);			// set modelview matrix
		glLoadIdentity();					// reset modelview matrix

		return 0;
		break;

	case WM_KEYDOWN:					// is a key pressed?
		//keyPressed[wParam] = true;
		switch (wParam)
		{
		case 'Q':
			lookX -= 3.0f;
			break;
		case 'E':
			lookX += 3.0f;
			break;
		case 'A':
			cameraX -= 1.0f;
			break;
		case 'D':
			cameraX += 1.0f;
			break;
		case 'W':
			cameraZ -= 1.0f;
			break;
		case 'S':
			cameraZ += 1.0f;
			break;
		default:
			break;
		}
		return 0;
		break;

	case WM_KEYUP:
		//keyPressed[wParam] = false;
		return 0;
		break;

	case WM_MOUSEMOVE:
		// save old mouse coordinates
		oldMouseX = mouseX;
		oldMouseY = mouseY;

		// get mouse coordinates from Windows
		mouseX = LOWORD(lParam);
		mouseY = HIWORD(lParam);

		// these lines limit the camera's range
		if (mouseY < 60)
			mouseY = 60;
		if (mouseY > 450)
			mouseY = 450;

		//if ((mouseX - oldMouseX) > 0)		// mouse moved to the right
		//	angle += 3.0f;
		//else if ((mouseX - oldMouseX) < 0)	// mouse moved to the left
		//	angle -= 3.0f;

		return 0;
		break;
	default:
		break;
	}

	return (DefWindowProc(hwnd, message, wParam, lParam));
}

void GLReset()
{
	//清除帧缓存数据
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//重置GL 设置
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	//重置变换矩阵
	glLoadIdentity();
}

void RenderFbxModel(_cFBXModel *model)
{
	ostringstream oss;

	list<_cFBXMesh>::iterator ite;

	int i = 0;
	for (ite = model->meshList.begin(); ite != model->meshList.end(); ++ite)
	{

		_cFBXMesh curMesh = *ite;
		curMesh.tex->Load();
		curMesh.tex->Bind(GL_TEXTURE0);
		i++;

		vector3_t *vexList = &curMesh.vexList[0];
		vector2_t *uvList = &curMesh.UvList[0];

		glEnable(GL_TEXTURE_2D);					// 开启2D纹理

		glEnable(GL_DEPTH_TEST);					// 开启深度测试

		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //指定混合函数
		glEnable(GL_BLEND);

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glShadeModel(GL_SMOOTH);

		//渲染多边形网格
		for (int i = 0; i < curMesh.polygonCount; i++)
		{
			glBegin(GL_TRIANGLES);

			glTexCoord2f(uvList[3 * i].point[0], uvList[3 * i].point[1]);
			glVertex3fv(vexList[3 * i].point);

			glTexCoord2f(uvList[3 * i + 2].point[0], uvList[3 * i + 2].point[1]);
			glVertex3fv(vexList[3 * i + 2].point);

			glTexCoord2f(uvList[3 * i + 1].point[0], uvList[3 * i + 1].point[1]);
			glVertex3fv(vexList[3 * i + 1].point);

			glEnd();
		}

		glDisable(GL_TEXTURE_2D);					// 关闭2D纹理
		glDisable(GL_BLEND);						// 关闭混合
	}
}

//渲染quad
void RenderQuad()
{
	//glEnable(GL_TEXTURE_2D);					// 开启2D纹理
	glEnable(GL_DEPTH_TEST);					// 开启深度测试

	glEnable(GL_LIGHTING);

	//ambient
	GLfloat light0_position[] = { 0.0, 10.0, 0.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

	GLfloat light_ambient[] = { 1.0, 0.0, 0.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);

	glEnable(GL_LIGHT0);

	//diffuse
	GLfloat light1_position[] = { 0.0, 10.0, 0.0, 1.0 };
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

	GLfloat light_diffuse[] = { 0.0, 0.0, 1.0, 1.0 };
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_diffuse);


	glEnable(GL_LIGHT1);

	GLUquadricObj *qobj = gluNewQuadric();
	gluSphere(qobj, 10.0, 20, 16);

	glDisable(GL_DEPTH_TEST);					// 开启深度测试

	//glBegin(GL_QUADS);
	//glNormal3f(0.0F, 0.0F, 1.0F);
	//glVertex3f(10.0f, 10.0f, 10.0f);
	//glVertex3f(-10.0f, 10.0f, 10.0f);
	//glVertex3f(-10.0f, -10.0f, 10.0f);
	//glVertex3f(10.0f, -10.0f, 10.0f);
	////1----------------------------    
	//glNormal3f(0.0F, 0.0F, -1.0F);
	//glVertex3f(-10.0f, -10.0f, -10.0f);
	//glVertex3f(-10.0f, 10.0f, -10.0f);
	//glVertex3f(10.0f, 10.0f, -10.0f);
	//glVertex3f(10.0f, -10.0f, -10.0f);
	////2----------------------------    
	//glNormal3f(0.0F, 1.0F, 0.0F);
	//glVertex3f(10.0f, 10.0f, 10.0f);
	//glVertex3f(10.0f, 10.0f, -10.0f);
	//glVertex3f(-10.0f, 10.0f, -10.0f);
	//glVertex3f(-10.0f, 10.0f, 10.0f);
	////3----------------------------    
	//glNormal3f(0.0F, -1.0F, 0.0F);
	//glVertex3f(-10.0f, -10.0f, -10.0f);
	//glVertex3f(10.0f, -10.0f, -10.0f);
	//glVertex3f(10.0f, -10.0f, 10.0f);
	//glVertex3f(-10.0f, -10.0f, 10.0f);
	////4----------------------------    
	//glNormal3f(1.0F, 0.0F, 0.0F);
	//glVertex3f(10.0f, 10.0f, 10.0f);
	//glVertex3f(10.0f, -10.0f, 10.0f);
	//glVertex3f(10.0f, -10.0f, -10.0f);
	//glVertex3f(10.0f, 10.0f, -10.0f);
	////5----------------------------    
	//glNormal3f(-1.0F, 0.0F, 0.0F);
	//glVertex3f(-10.0f, -10.0f, -10.0f);
	//glVertex3f(-10.0f, -10.0f, 10.0f);
	//glVertex3f(-10.0f, 10.0f, 10.0f);
	//glVertex3f(-10.0f, 10.0f, -10.0f);
	////6----------------------------*/    
	//glEnd();
}

const int WORLD_SIZE = 250;
/*****************************************************************************
DrawGround()

Draw a grid of blue lines to represent the ground.
*****************************************************************************/
GLvoid DrawGround()
{
	//set color
	glColor3f(1.0f, 1.0f, 1.0f);

	// draw the lines
	glBegin(GL_LINES);

	for (int x = -WORLD_SIZE; x < WORLD_SIZE; x += 10)
	{
		glVertex3i(x, 0, -WORLD_SIZE);
		glVertex3i(x, 0, WORLD_SIZE);
	}

	for (int z = -WORLD_SIZE; z < WORLD_SIZE; z += 10)
	{
		glVertex3i(-WORLD_SIZE, 0, z);
		glVertex3i(WORLD_SIZE, 0, z);
	}

	glEnd();
}

//GLvoid DrawCube()
//{
//	glEnable(GL_TEXTURE_2D);					// 开启2D纹理
//	glEnable(GL_DEPTH_TEST);					// 开启深度测试
//
//	//GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
//	//glLightfv(GL_LIGHT0, GL_POSITION, light_position);
//	//glEnable(GL_LIGHTING);
//	//glEnable(GL_LIGHT0);
//
//	// enable blending for anti-aliased lines
//	glEnable(GL_BLEND);
//
//	// set the color to a bright blue
//	glColor3f(1.0f, 0.7f, 0.5f);
//
//	// draw the lines
//	glBegin(GL_QUADS);
//	glVertex3f(1.0f, 0.0f, 0.0f);
//	glVertex3f(0.0f, 1.0f, 0.0f);
//	glVertex3f(0.0f, 0.0f, 1.0f);
//	glVertex3f(1.0f, 0.0f, 0.0f);
//	glVertex3f(1.0f, 0.0f, 0.0f);
//	glVertex3f(1.0f, 0.0f, 0.0f);
//	glVertex3f(1.0f, 0.0f, 0.0f);
//	glVertex3f(1.0f, 0.0f, 0.0f);
//	glEnd();
//
//	// turn blending off
//	glDisable(GL_BLEND);
//} // end DrawGround()

GLvoid RenderVBO(GLsizei vertexCount)
{
	glEnable(GL_TEXTURE_2D);					// 开启2D纹理
	glEnable(GL_DEPTH_TEST);					// 开启深度测试

	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);

	////ambient
	//GLfloat light0_position[] = { 0.0, 5, 0.0, 1.0 };
	//glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	//GLfloat light_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	//glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);


	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPervInstance, LPSTR lpCmdLine, int nShowCmd)
{
	HWND hwnd;
	bool done;
	MSG msg;

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.hInstance = hInstance;
	windowClass.lpfnWndProc = WndProc;
	windowClass.style = CS_VREDRAW | CS_HREDRAW;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;

	windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = "MyClass";

	if (!RegisterClassEx(&windowClass))
	{
		return 0;
	}

	hwnd = CreateWindowEx(NULL,
		"MyClass",
		"OpenGL Display!",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU,
		100, 100,
		400, 400,
		NULL, NULL,
		hInstance,
		NULL);

	if (!hwnd)
	{
		return 0;
	}

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	done = false;


	//初始化glew扩展库--需要在窗口创建完成之后进行   
	GLenum err = glewInit();

	if (GLEW_OK != err)
	{
		//cout << "Error initializing GLEW: " << glewGetErrorString(err) << endl;
	}

	FBXProcesser *process = new FBXProcesser("Models", "shayu.FBX");
	process->Init();
	process->LoadNode();

	_cFBXMesh curMesh = process->model->meshList.front();

	//加载并绑定材质
	curMesh.material->LoadTexture();
	curMesh.material->Bind();

	//curMesh.material->vertexShaderName = "Shader/DefaultLighting.vert";
	//curMesh.material->FragmentShaderName = "Shader/DefaultLighting.frag";
	//加载着色器 
	LoadShader(curMesh.material->vertexShaderName.c_str(), curMesh.material->FragmentShaderName.c_str());

	vertexFloatCount = curMesh.polygonCount * 3 * 3;
	vertexFloatArray = curMesh.polygonPointArray;

	vertexUvFloatCount = curMesh.polygonCount * 3 * 2;
	vertexUvArray = curMesh.polygonUvArray;

	int vertexCount = curMesh.polygonCount * 3;

	VertexDataTransfer * transfer = new VertexDataTransfer(2);
	VertexData *vexData = new VertexData(0, vertexFloatCount, vertexFloatArray, GL_STATIC_DRAW, GL_FLOAT, 0, 3);
	VertexData *uvData = new VertexData(1, vertexUvFloatCount, vertexUvArray, GL_STATIC_DRAW, GL_FLOAT, 0, 2);
	transfer->AddVertexData(vexData);
	transfer->AddVertexData(uvData);
	transfer->Transfer();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);		// clear to black

	while (!done)
	{
		PeekMessage(&msg, hwnd, NULL, NULL, PM_REMOVE);

		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			//Sleep(16);

			GLReset();

			// set the camera position
			gluLookAt(cameraX, 10.0f, cameraZ, lookX, 0.0f, lookZ, 0.0, 1.0, 0.0);

			//Draw Ground
			DrawGround();

			//测试光照
			//RenderQuad();

			//Do Transform
			glTranslatef(0.0f, 0.0f, 0.0f);
			glRotatef(-45.0, 1.0, 0.0, 0.0);
			glRotatef(45.0, 0.0, 0.0, 1.0);

			//Get Transform Matrix
			GLfloat *modle_view_mat = new GLfloat[16];
			glGetFloatv(GL_MODELVIEW_MATRIX, modle_view_mat);

			GLfloat *projection_mat = new GLfloat[16];
			glGetFloatv(GL_PROJECTION_MATRIX, projection_mat);

			GLfloat light_ambient[] = { 1.0, 1.0, 1.0, 1.0 };

			GLuint uniBlock = glGetUniformBlockIndex(programHandle, "Matrix");

			if (uniBlock != GL_INVALID_INDEX)
			{
				//todo 传递 uniform 变量
				GLint  blockDataSize = 0;
				glGetActiveUniformBlockiv(programHandle, uniBlock, GL_UNIFORM_BLOCK_DATA_SIZE, &blockDataSize);

				GLuint uboHandle;
				glGenBuffers(1, &uboHandle);
				glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
				glBufferData(GL_UNIFORM_BUFFER, blockDataSize, NULL, GL_STATIC_DRAW);
				glUniformBlockBinding(programHandle, uniBlock, 0);
				glBindBuffer(GL_UNIFORM_BUFFER, NULL);
			}

			//绑定当前的 Shader
			glUseProgram(programHandle);

			//Push Matrix
			glPushMatrix();

			//Rendering
			RenderVBO(vertexCount);

			//Pop Matrix
			glPopMatrix();

			//解除 Shader
			glUseProgram(NULL);

			//Refresh Frame Buffer
			glFlush();
			SwapBuffers(g_HDC);			// bring backbuffer to foreground

			//Window Message Handle
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

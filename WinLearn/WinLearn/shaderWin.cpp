#define WIN32_LEAN_AND_MEAN

#include <glew.h>
#include <windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <GLAUX.H>
#include "Textfile.h"
#include <iostream>
#include <Magick++.h>

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
#include "VboObject.h"
#include "EasyLog.h"

#define PI	3.14159

using namespace std;

unsigned char * readDataFromFile(char *fn)
{
	FILE *fp;
	unsigned char *content = NULL;
	int count = 0;
	if (fn != NULL) {
		fp = fopen(fn, "rb");
		if (fp != NULL) {
			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);
			if (count > 0) {
				content = (unsigned char *)malloc(sizeof(unsigned char) * (count + 1));
				count = fread(content, sizeof(unsigned char), count, fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;

}

//读入字符流  
char *textFileRead(const char *fn)
{
	FILE *fp;
	char *content = NULL;
	int count = 0;
	if (fn != NULL)
	{
		fp = fopen(fn, "rt");
		if (fp != NULL)
		{
			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);
			if (count > 0)
			{
				content = (char *)malloc(sizeof(char) * (count + 1));
				count = fread(content, sizeof(char), count, fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
}

int textFileWrite(char *fn, char *s)
{
	FILE *fp;
	int status = 0;
	if (fn != NULL) {
		fp = fopen(fn, "w");
		if (fp != NULL) {
			if (fwrite(s, sizeof(char), strlen(s), fp) == strlen(s))
				status = 1;
			fclose(fp);
		}
	}
	return(status);
}

float angle = 0.0f;			// camera angle 
float radians = 0.0f;		// camera angle in radians

							////// Mouse/Camera Variables
int mouseX, mouseY;		// mouse coordinates
float cameraX, cameraY, cameraZ=100.0f;	// camera coordinates
float lookX, lookY, lookZ = -100.0f;		// camera look-at coordinates

using namespace std;

WNDCLASSEX windowClass;
HDC g_HDC;

/*Shader*/
GLuint vShader, fShader;
GLuint vaoHandle;

float rAngle = 0.0f;

//顶点位置数组    
float positionData[] =
{
	-5.0f,-5.0f,20.0f,1.0f,
	5.0f,-5.0f,20.0f,1.0f,
	5.0f,5.0f,20.0f,1.0f,
	-5.0f,5.0f,20.0f,1.0f
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

GLuint programHandle;

ostringstream oss_1;

//////////////////////////////////////////////////////////////////////////初始化shader
void InitShader(const char *VShaderFile, const char *FShaderFile)
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

			oss_1 << "error = " << log;
			free(log);//释放空间
			EasyLog::Inst()->Log(oss_1.str());
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
		cerr << "ERROR : link shader program failed" << endl;
		GLint logLen;
		glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH,
			&logLen);
		if (logLen > 0)
		{
			char *log = (char *)malloc(logLen);
			GLsizei written;
			glGetProgramInfoLog(programHandle, logLen,
				&written, log);
			cerr << "Program log : " << endl;
			cerr << log << endl;

			oss_1 << "链接失败 --  Program log : " << log;
		}
	}
	else//链接成功，在OpenGL管线中使用渲染程序    
	{
		//glUseProgram(programHandle);
		//oss_1 << "链接成功，使用着色器";
		//EasyLog::Inst()->Log(oss_1.str());
	}
}

float *fa;
int fcount;
void initVBO()
{
	//绑定VAO  
	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);

	// Create and populate the buffer objects    
	GLuint vboHandles[1];
	glGenBuffers(1, vboHandles);
	GLuint positionBufferHandle = vboHandles[0];
	//GLuint uvBufferHandle = vboHandles[1];
	//GLuint colorBufferHandle = vboHandles[2];

	//绑定VBO以供使用    
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	//加载数据到VBO    
	int size_i = sizeof(fa[0])*fcount;
	oss_1 << "float array size = " << size_i << "\n";
	EasyLog::Inst()->Log(oss_1.str());

	glBufferData(GL_ARRAY_BUFFER, size_i,
		fa, GL_STATIC_DRAW);

	//glBindBuffer(GL_ARRAY_BUFFER, uvBufferHandle);
	//glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float),
	//	uvData, GL_STATIC_DRAW);

	////绑定VBO以供使用    
	//glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
	////加载数据到VBO    
	//glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float),
	//	colorData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);//顶点坐标    
	//调用glVertexAttribPointer之前需要进行绑定操作    
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);


	//glEnableVertexAttribArray(1);//顶点uv
	//glBindBuffer(GL_ARRAY_BUFFER, uvBufferHandle);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);

	//glEnableVertexAttribArray(2);//顶点颜色
	//glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
	//glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
}

void BindVBO(GLuint bindVaoHandle,GLuint *bindPositionData,GLuint *bindUvData,GLuint bindColorBufferHandle)
{
	//绑定VAO  
	glGenVertexArrays(1, &bindVaoHandle);
	glBindVertexArray(bindVaoHandle);

	// Create and populate the buffer objects    
	GLuint vboHandles[3];
	glGenBuffers(3, vboHandles);
	GLuint positionBufferHandle = vboHandles[0];
	GLuint uvBufferHandle = vboHandles[1];
	GLuint colorBufferHandle = vboHandles[2];

	//绑定VBO以供使用    
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	//加载数据到VBO    
	glBufferData(GL_ARRAY_BUFFER, sizeof(bindPositionData),
		positionData, GL_STATIC_DRAW);

	//uv-st
	glBindBuffer(GL_ARRAY_BUFFER, uvBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bindUvData),
		uvData, GL_STATIC_DRAW);

	//color
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);  
	glBufferData(GL_ARRAY_BUFFER, sizeof(bindColorBufferHandle),
		colorData, GL_STATIC_DRAW);

	//传送数据
	//调用glVertexAttribPointer之前需要进行绑定操作  
	glEnableVertexAttribArray(0);//顶点坐标    
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);


	glEnableVertexAttribArray(1);//顶点uv
	glBindBuffer(GL_ARRAY_BUFFER, uvBufferHandle);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);


	glEnableVertexAttribArray(2);//顶点颜色
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
}

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
		gluPerspective(54.0f, (GLfloat)width / (GLfloat)height, 1.0f, 1000.0f);

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

		if ((mouseX - oldMouseX) > 0)		// mouse moved to the right
			angle += 3.0f;
		else if ((mouseX - oldMouseX) < 0)	// mouse moved to the left
			angle -= 3.0f;

		return 0;
		break;
	default:
		break;
	}

	return (DefWindowProc(hwnd, message, wParam, lParam));
}

GLenum GetTexUnit(int index)
{
	switch (index)
	{
	case 0:
		return GL_TEXTURE0;
	case 1:
		return GL_TEXTURE1;
	default:
		break;
	}
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
		curMesh.tex->Bind(GetTexUnit(i));
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
	float leftTop[3] = { -10.0f,10.0f,0.0f };
	float rightTop[3] = { 10.0f,10.0f,0.0f };
	float leftBottom[3] = { -10.0f,-10.0f,0.0f };
	float rightBottom[3] = { 10.0f,-10.0f,0.0f };

	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(leftTop);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(rightTop);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(rightBottom);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(leftBottom);

	glEnd();
}

const int WORLD_SIZE = 250;
/*****************************************************************************
DrawGround()

Draw a grid of blue lines to represent the ground.
*****************************************************************************/
GLvoid DrawGround()
{
	// enable blending for anti-aliased lines
	//glEnable(GL_BLEND);

	// set the color to a bright blue
	glColor3f(0.0f, 0.7f, 1.0f);

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

	// turn blending off
	//glDisable(GL_BLEND);
} // end DrawGround()

GLvoid DrawLookAtLine()
{
	// enable blending for anti-aliased lines
	glEnable(GL_BLEND);

	// set the color to a bright blue
	glColor3f(0.5f, 0.7f, 0.5f);

	// draw the lines
	glBegin(GL_LINES);
	glVertex3f(-250.0f, 50.0f, 150.0f);
	glVertex3f(-250.0f, -50.0f, 50.0f);
	glEnd();

	// turn blending off
	glDisable(GL_BLEND);
} // end DrawGround()

GLvoid DrawCube()
{
	// enable blending for anti-aliased lines
	glEnable(GL_BLEND);

	// set the color to a bright blue
	glColor3f(0.5f, 0.7f, 0.5f);

	// draw the lines
	glBegin(GL_TRIANGLES);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glEnd();

	// turn blending off
	glDisable(GL_BLEND);
} // end DrawGround()

GLvoid RenderVBO()
{
	glDrawArrays(GL_TRIANGLES, 0, 2388);
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

	FBXProcesser *process = new FBXProcesser("FbxModels", "shayu.FBX");
	process->Init();
	process->LoadNode();

	//加载顶点和片段着色器对象并链接到一个程序对象上  
	InitShader("VertexShader_raw.vert", "FragmentShader_raw.frag");

	//fcount = 12;

	_cFBXMesh curMesh = process->model->meshList.front();
	vector3_t *vexList = &curMesh.vexList[0];

	fcount = curMesh.polygonCount * 3 * 3;

	oss_1 << "多边形数量 = " << curMesh.polygonCount << "\n\n";

	fa = new float[fcount];

	for (int i = 0; i < curMesh.polygonCount; i++)
	{
		int startIndex = i * 9;
		fa[startIndex] = vexList[3 * i].point[0];
		//oss_1 << 3 * i << " -顶点：" << fa[startIndex] << ",";
		fa[startIndex + 1] = vexList[3 * i].point[1];
		//oss_1 << "" << fa[startIndex + 1] << ",";
		fa[startIndex + 2] = vexList[3 * i].point[2];
		//oss_1 << "" << fa[startIndex + 2] << "\n";

		fa[startIndex + 3] = vexList[3 * i + 2].point[0];
		//oss_1 << 3 * i + 2 << " -顶点：" << fa[startIndex + 3] << ",";
		fa[startIndex + 4] = vexList[3 * i + 2].point[1];
		//oss_1 << "" << fa[startIndex + 4] << ",";
		fa[startIndex + 5] = vexList[3 * i + 2].point[2];
		//oss_1 << "" << fa[startIndex + 5] << "\n";

		fa[startIndex + 6] = vexList[3 * i + 1].point[0];
		//oss_1 << 3 * i + 1 << " -顶点：" << fa[startIndex + 6] << ",";
		fa[startIndex + 7] = vexList[3 * i + 1].point[1];
		//oss_1 << "" << fa[startIndex + 7] << ",";
		fa[startIndex + 8] = vexList[3 * i + 1].point[2];
		//oss_1 << "" << fa[startIndex + 8] << "\n\n";
	}

	EasyLog::Inst()->Log(oss_1.str());

	initVBO();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);		// clear to black

	while (!done)
	{
		PeekMessage(&msg, hwnd, NULL, NULL, PM_REMOVE);

		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			Sleep(16);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glLoadIdentity();

			// set the camera position
			gluLookAt(cameraX, 10.0f, cameraZ, lookX, 0.0f, lookZ, 0.0, 1.0, 0.0);

			glPushMatrix();
			DrawGround();

			//glTranslatef(0.0f, 0.0f, 50.0f);
			//glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
			//RenderFbxModel(process->model);

			//glUniform4dv()

			glUseProgram(programHandle);
			RenderVBO();
			glUseProgram(NULL);
			glPopMatrix();

			//RenderQuad();

			glFlush();
			SwapBuffers(g_HDC);			// bring backbuffer to foreground

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

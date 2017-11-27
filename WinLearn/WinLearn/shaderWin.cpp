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
#include "Textfile.h"  
#include "Texture.h"
#include "CMD2Model.h";
#include <string.h>
#include <math.h>;
#include "FBXProcesser.h"
#include "_cFBXModel.h"
#include <stdio.h>;
#include <list>;
#include <sstream>;
//#include "EasyLog.h"

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

//�����ַ���  
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
float cameraX, cameraY, cameraZ;	// camera coordinates
float lookX, lookY, lookZ;		// camera look-at coordinates

using namespace std;

WNDCLASSEX windowClass;
HDC g_HDC;

/*Shader*/
GLuint vShader, fShader;
GLuint vaoHandle;

//����λ������    
float positionData[] = {
	-0.5f,-0.5f,0.0f,1.0f,
	0.5f,-0.5f,0.0f,1.0f,
	0.5f,0.5f,0.0f,1.0f,
	-0.5f,0.5f,0.0f,1.0f,
};

//����uv����
float uvData[] = {
	0.0f,1.0f,
	1.0f,1.0f,
	0.0f,0.0f,
	1.0f,0.0f,
};

//������ɫ����    
float colorData[] = {
	1.0f, 0.0f, 0.0f,1.0f,
	0.0f, 1.0f, 0.0f,1.0f,
	0.0f, 0.0f, 1.0f,1.0f,
	1.0f,1.0f,0.0f,1.0f
};

//////////////////////////////////////////////////////////////////////////��ʼ��shader
void InitShader(const char *VShaderFile, const char *FShaderFile)
{


#pragma region ������ɫ��

	vShader = glCreateShader(GL_VERTEX_SHADER);

	if (0 == vShader)
	{
		//������
	}

	const GLchar *vShaderCode = textFileRead(VShaderFile);
	const GLchar *vCodeArray[1] = { vShaderCode };

	//���ַ�����󶨵���Ӧ����ɫ��������
	glShaderSource(vShader, 1, vCodeArray, NULL);

	//������ɫ������
	glCompileShader(vShader);

	GLint compileResult;
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &compileResult);

	if (GL_FALSE == &compileResult)
	{
		GLint logLen;

		//�õ�������־����    
		glGetShaderiv(vShader, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0)
		{
			char *log = (char *)malloc(logLen);
			GLsizei written;
			//�õ���־��Ϣ�����    
			glGetShaderInfoLog(vShader, logLen, &written, log);
			//cerr << "vertex shader compile log : " << endl;
			//cerr << log << endl;
			//free(log);//�ͷſռ�    
		}
	}
#pragma endregion

#pragma region Ƭ����ɫ��
	fShader = glCreateShader(GL_FRAGMENT_SHADER);

	if (0 == fShader)
	{
		//������
	}

	const GLchar *fShaderCode = textFileRead(FShaderFile);
	const GLchar *fCodeArray[1] = { fShaderCode };

	glShaderSource(fShader, 1, fCodeArray, NULL);

	//������ɫ������    
	glCompileShader(fShader);

	//�������Ƿ�ɹ�    
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &compileResult);
	if (GL_FALSE == compileResult)
	{
		GLint logLen;
		//�õ�������־����    
		glGetShaderiv(fShader, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0)
		{
			char *log = (char *)malloc(logLen);
			GLsizei written;
			//�õ���־��Ϣ�����    
			glGetShaderInfoLog(fShader, logLen, &written, log);
			//cerr << "fragment shader compile log : " << endl;
			//cerr << log << endl;
			//free(log);//�ͷſռ�    
		}
	}

#pragma endregion

	//3��������ɫ������

	//������ɫ������    
	GLuint programHandle = glCreateProgram();
	if (!programHandle)
	{
		//cerr << "ERROR : create program failed" << endl;
		//exit(1);
	}
	//����ɫ���������ӵ��������ĳ�����    
	glAttachShader(programHandle, vShader);
	glAttachShader(programHandle, fShader);
	//����Щ�������ӳ�һ����ִ�г���    
	glLinkProgram(programHandle);
	//��ѯ���ӵĽ��    
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
		}
	}
	else//���ӳɹ�����OpenGL������ʹ����Ⱦ����    
	{
		glUseProgram(programHandle);
	}
}

void initVBO()
{
	//��VAO  
	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);

	// Create and populate the buffer objects    
	GLuint vboHandles[3];
	glGenBuffers(3, vboHandles);
	GLuint positionBufferHandle = vboHandles[0];
	GLuint uvBufferHandle = vboHandles[1];
	GLuint colorBufferHandle = vboHandles[2];

	//��VBO�Թ�ʹ��    
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	//�������ݵ�VBO    
	glBufferData(GL_ARRAY_BUFFER,  sizeof(positionData),
		positionData, GL_STATIC_DRAW);

	//
	glBindBuffer(GL_ARRAY_BUFFER, uvBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float),
		uvData, GL_STATIC_DRAW);

	//��VBO�Թ�ʹ��    
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
	//�������ݵ�VBO    
	glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float),
		colorData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);//��������    
	//����glVertexAttribPointer֮ǰ��Ҫ���а󶨲���    
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);


	glEnableVertexAttribArray(1);//����uv
	glBindBuffer(GL_ARRAY_BUFFER, uvBufferHandle);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);

	glEnableVertexAttribArray(2);//������ɫ
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
}

void BindVBO(GLuint bindVaoHandle,GLuint *bindPositionData,GLuint *bindUvData,GLuint bindColorBufferHandle)
{
	//��VAO  
	glGenVertexArrays(1, &bindVaoHandle);
	glBindVertexArray(bindVaoHandle);

	// Create and populate the buffer objects    
	GLuint vboHandles[3];
	glGenBuffers(3, vboHandles);
	GLuint positionBufferHandle = vboHandles[0];
	GLuint uvBufferHandle = vboHandles[1];
	GLuint colorBufferHandle = vboHandles[2];

	//��VBO�Թ�ʹ��    
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	//�������ݵ�VBO    
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

	//��������
	//����glVertexAttribPointer֮ǰ��Ҫ���а󶨲���  
	glEnableVertexAttribArray(0);//��������    
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);


	glEnableVertexAttribArray(1);//����uv
	glBindBuffer(GL_ARRAY_BUFFER, uvBufferHandle);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);


	glEnableVertexAttribArray(2);//������ɫ
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

bool l = true;
void RenderFbxModel(_cFBXModel *model)
{
	ostringstream oss;

	list<_cFBXMesh>::iterator ite;
	for (ite = model->meshList.begin(); ite != model->meshList.end(); ++ite)
	{
		_cFBXMesh curMesh = *ite;
		//oss << "render polygon count = " << curMesh.polygonCount;
		//��Ⱦ����
		glCullFace(GL_BACK);
		// display the textured model with proper lighting normals

		vector3_t *vexList = &curMesh.vexList[0];
		vector2_t *uvList = &curMesh.UvList[0];

		//glColor3f(1.0f, 0.0f, 0.0f);

		for (int i = 0; i < curMesh.polygonCount; i++)
		{
			glBegin(GL_TRIANGLE_FAN);
			for (int j = 0; j < 3; j++)
			{
				glTexCoord2f(uvList[3 * i + j].point[0], uvList[3 * i + j].point[1]);
				glVertex3fv(vexList[3 * i + j].point);

				//oss << uvList[3 * i + j].point[0] << "," << uvList[3 * i + j].point[1] << "\n";
				//oss << vexList[3 * i + j].point[0] << "," << vexList[3 * i + j].point[1] << "," << vexList[3 * i + j].point[2] << "\n";
			}
			glEnd();
			//int polygonStartIndex = i * 3;

			//int polygonVexIndex_0 = curMesh.polygonVex[polygonStartIndex];
			//glTexCoord2f(uvList[polygonStartIndex].point[0], uvList[polygonStartIndex].point[1]);
			//glVertex3fv(vexList[polygonStartIndex].point);

			////glColor3f(0.0f, 0.0f, 1.0f);
			//int polygonVexIndex_2 = i + 2;//curMesh.polygonVex[polygonStartIndex + 2];
			//glTexCoord2f(uvList[polygonStartIndex+1].point[0], uvList[polygonStartIndex+1].point[1]);
			//glVertex3fv(vexList[polygonStartIndex+1].point);

			////glColor3f(0.0f, 1.0f, 0.0f);
			//int polygonVexIndex_1 = i + 1;//curMesh.polygonVex[polygonStartIndex + 1];
			//glTexCoord2f(uvList[polygonVexIndex_1].point[0], uvList[polygonVexIndex_1].point[1]);
			//glVertex3fv(vexList[polygonVexIndex_1].point);
		}

		if (l)
		{
			//EasyLog::Inst()->Log(oss.str());
			l = false;
		}
	}
}

//��Ⱦquad
void RenderQuad()
{
	float leftTop[3] = { -10.0f,10.0f,0.0f };
	float rightTop[3] = { 10.0f,10.0f,0.0f };
	float leftBottom[3] = { -10.0f,-10.0f,0.0f };
	float rightBottom[3] = { 10.0f,-10.0f,0.0f };

	glBegin(GL_QUADS);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(leftTop);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(rightTop);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(rightBottom);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(leftBottom);
	glEnd();
}

const int   WORLD_SIZE = 250;
/*****************************************************************************
DrawGround()

Draw a grid of blue lines to represent the ground.
*****************************************************************************/
GLvoid DrawGround()
{
	// enable blending for anti-aliased lines
	glEnable(GL_BLEND);

	// set the color to a bright blue
	glColor3f(0.5f, 0.7f, 1.0f);

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
	glDisable(GL_BLEND);
} // end DrawGround()

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
		"A REAL Window Applacation!",
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


	//��ʼ��glew��չ��--��Ҫ�ڴ��ڴ������֮�����   
	GLenum err = glewInit();

	if (GLEW_OK != err)
	{
		//cout << "Error initializing GLEW: " << glewGetErrorString(err) << endl;
	}

	//�󶨲�����VAO��VBO
	//initVBO();

	//����ģ��
	CMD2Model myModel = CMD2Model();
	myModel.LoadModel("models\\ogro\\tris.md2");

	//BindVBO()

	//���ض����Ƭ����ɫ���������ӵ�һ�����������  
	//InitShader("VertexShader.vert", "FragmentShader.frag");

	FBXProcesser *process = new FBXProcesser("FbxModels/paodan.FBX");
	process->Init();
	process->LoadNode();

	Texture tex = Texture(GL_TEXTURE_2D, "paodan.png");
	tex.Load();

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

			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//glLoadIdentity();


			//glClear(GL_COLOR_BUFFER_BIT);

			//////ʹ��VAO��VBO����    
			////glBindVertexArray(vaoHandle);
			////tex.Bind(GL_TEXTURE0);

			////glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			////glBindVertexArray(0);

			//SwapBuffers(g_HDC);
			//TranslateMessage(&msg);
			//DispatchMessage(&msg);


			radians = float(PI*(angle - 90.0f) / 180.0f);

			// calculate the camera's position
			cameraX = lookX + (float)sin(radians)*mouseY;	// multiplying by mouseY makes the
			cameraZ = lookZ + (float)cos(radians)*mouseY;  // camera get closer/farther away with mouseY
			cameraY = lookY + mouseY / 2.0f;

			// calculate the camera look-at coordinates as the center
			lookX = 0.0f;
			lookY = 2.0f;
			lookZ = 0.0f;

			// clear screen and depth buffer
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glLoadIdentity();

			// set the camera position
			gluLookAt(cameraX, cameraY, cameraZ, lookX, lookY, lookZ, 0.0, 1.0, 0.0);

			glPushMatrix();
			glRotatef(90.0f, -1.0f, 0.0f, 0.0f);
			glColor3f(1.0, 1.0, 1.0);

			//DrawGround();
			//myModel.RenderFrame(0);
			glEnable(GL_TEXTURE_2D);					// enable 2D texturing														
			tex.Bind(GL_TEXTURE0);
			RenderFbxModel(process->model);
			//RenderQuad();

			glPopMatrix();

			glFlush();
			SwapBuffers(g_HDC);			// bring backbuffer to foreground
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}
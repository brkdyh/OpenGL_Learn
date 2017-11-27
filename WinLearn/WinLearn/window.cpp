//#define WIN32_LEAN_AND_MEAN
//
//#include <windows.h>
//#include <gl/GL.h>
//#include <gl/GLU.h>
//#include <GLAUX.H>
//
////#if _MSC_VER>=1900
////#include "stdio.h" 
////_ACRTIMP_ALT FILE* __cdecl __acrt_iob_func(unsigned);
////#ifdef __cplusplus 
////extern "C"
////#endif 
////FILE* __cdecl __iob_func(unsigned i) {
////	return __acrt_iob_func(i);
////}
////#endif /* _MSC_VER>=1900 */
//
//WNDCLASSEX windowClass;
//
//int nCount = 0;
//
////void Paint(HWND hwnd)
////{
////	nCount++;
////
////	PAINTSTRUCT paintStruct;
////
////	HDC hDC;
////	char my_string[] = "Hello ,World...!";
////
////	hDC = BeginPaint(hwnd, &paintStruct);
////	if (nCount % 2 == 0)
////	{
////		SetTextColor(hDC, COLORREF(0x00ff0000));
////	}
////	else
////	{
////		SetTextColor(hDC, COLORREF(0x0000ff00));
////	}
////
////	TextOut(hDC, 150, 150, my_string, sizeof(my_string) - 1);
////	EndPaint(hwnd, &paintStruct);
////}
//
//float angle = 0.0f;
//HDC g_HDC;
//
//void DrawPoint(GLfloat *verts)
//{
//	//int a = sizeof(verts[0]);
//	//int i = sizeof(verts);
//	glBegin(GL_POINTS);
//		glVertex3f(verts[0], verts[1], verts[2]);
//	glEnd();
//}
//
//void DrawPolygon(GLenum face, GLenum mode, GLfloat *verts)
//{
//	glPolygonMode(face, mode);
//	glBegin(GL_POLYGON);
//		glVertex3f(verts[0], verts[1], verts[2]);
//		glVertex3f(verts[3], verts[4], verts[5]);
//		glVertex3f(verts[6], verts[7], verts[8]);
//		glVertex3f(verts[9], verts[10], verts[11]);
//	glEnd();
//}
//
//void SetupPixelFromat(HDC hDC)
//{
//	int nPixelFormat;
//
//	static PIXELFORMATDESCRIPTOR pfd =
//	{
//		sizeof(PIXELFORMATDESCRIPTOR),
//		1,
//		PFD_DRAW_TO_WINDOW |
//		PFD_SUPPORT_OPENGL |
//		PFD_DOUBLEBUFFER,
//		PFD_TYPE_RGBA,
//		32,
//		0,0,0,0,0,0,
//		0,
//		0,
//		0,
//		0,0,0,0,
//		16,
//		0,
//		0,
//		PFD_MAIN_PLANE,
//		0,
//		0,0,0
//	};
//
//	nPixelFormat = ChoosePixelFormat(hDC, &pfd);
//
//	SetPixelFormat(hDC, nPixelFormat, &pfd);
//}
//
//LRESULT CALLBACK WndPoc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	static HGLRC hRC;
//	static HDC hDC;
//	int width, height;
//
//	switch (message)
//	{
//	case  WM_CREATE:
//		hDC = GetDC(hwnd);
//		g_HDC = hDC;
//		SetupPixelFromat(hDC);
//
//		hRC = wglCreateContext(hDC);
//		wglMakeCurrent(hDC, hRC);
//		return 0;
//		break;
//
//	case  WM_CLOSE:
//		wglMakeCurrent(hDC, NULL);
//		wglDeleteContext(hRC);
//		PostQuitMessage(0);
//		return 0;
//		break;
//
//	case WM_SIZE:
//		height = HIWORD(lParam);
//		width = LOWORD(lParam);
//
//		if (height == 0)
//		{
//			height = 1;
//		}
//
//		glViewport(0, 0, width, height);
//		glMatrixMode(GL_PROJECTION);
//		glLoadIdentity();
//
//		gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 1000.0f);
//		glMatrixMode(GL_MODELVIEW);
//		glLoadIdentity();
//
//		return 0;
//		break;
//
//	//case  WM_PAINT:
//	//	Paint(hwnd);
//	//	return 0;
//	//	break;
//
//	default:
//		break;
//	}
//
//	return DefWindowProc(hwnd, message, wParam, lParam);
//}
//
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPervInstance, LPSTR lpCmdLine, int nShowCmd)
//{
//	HWND hwnd;
//	bool done;
//	MSG msg;
//
//	windowClass.cbSize = sizeof(WNDCLASSEX);
//	windowClass.hInstance = hInstance;
//	windowClass.lpfnWndProc = WndPoc;
//	windowClass.style = CS_VREDRAW | CS_HREDRAW;
//	windowClass.cbClsExtra = 0;
//	windowClass.cbWndExtra = 0;
//
//	windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
//	windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
//	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
//	windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
//	windowClass.lpszMenuName = NULL;
//	windowClass.lpszClassName = "MyClass";
//
//	if (!RegisterClassEx(&windowClass))
//	{
//		return 0;
//	}
//
//	hwnd = CreateWindowEx(NULL,
//		"MyClass",
//		"A REAL Window Applacation!",
//		WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU,
//		100, 100,
//		400, 400,
//		NULL,NULL,
//		hInstance,
//		NULL);
//
//	if (!hwnd)
//	{
//		return 0;
//	}
//
//	ShowWindow(hwnd, SW_SHOW);
//	UpdateWindow(hwnd);
//
//	done = false;
//
//	while (!done)
//	{
//		PeekMessage(&msg, hwnd, NULL, NULL, PM_REMOVE);
//
//		if (msg.message == WM_QUIT)
//		{
//			done = true;
//		}
//		else
//		{
//			//Sleep(16);
//
//			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//			glLoadIdentity();
//			//angle = angle + 0.5f;
//
//			//if (angle >= 360.0f)
//			//{
//			//	angle = 0.0f;
//			//}
//
//			////glTranslatef(0.0f, 0.0f, -5.0f);
//			//glRotatef(angle, 0.0f, 1.0f, 0.0f);
//
//			glColor3f(0.89f, 0.12f, 0.56f);
//
//			//glBegin(GL_TRIANGLES);
//			//	glVertex3f(0.0f, 0.0f, 0.0f);
//			//	glVertex3f(1.0f, 0.0f, 0.0f);
//			//	glVertex3f(1.0f, 1.0f, 0.0f);
//			//glEnd();
//
//			//GLfloat v[] = new GLfloat[2];
//			//{
//			//	0.0f,0.0f,-10.0f,
//			//	0.0f,1.0f,-5.0f,
//			//};
//
//			//DrawPoint(v);
//			//DrawPolygon(GL_FRONT_AND_BACK, GL_LINE, v);
//
//			SwapBuffers(g_HDC);
//			TranslateMessage(&msg);
//			DispatchMessage(&msg);
//		}
//	}
//
//	return msg.wParam;
//}
//

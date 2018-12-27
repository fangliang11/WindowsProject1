// openGL1.cpp: ����Ŀ�ļ���
#include <windows.h>        // Windows��ͷ�ļ�

//GLEW
#include <GL/glew.h>

//FREEGLUT
#include<GL/freeglut.h>

// Other Libs
#include <SOIL.h>

// Other includes
//#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// Shaders
const GLchar* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 color;\n"
"layout (location = 2) in vec2 texCoord;\n"
"out vec3 ourColor;\n"
"out vec2 TexCoord;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(position, 1.0f);\n"
"ourColor = color;\n"
"TexCoord = texCoord;\n"
"}\n\0";
const GLchar* fragmentShaderSource = "#version 330 core\n"
"in vec3 ourColor;\n"
"in vec2 TexCoord;\n"
"out vec4 color;\n"
"uniform sampler2D ourTexture;\n"
"void main()\n"
"{\n"
"color = texture(ourTexture, TexCoord);\n"
"}\n\0";


HGLRC           hRC = NULL;                          // ������ɫ��������
HDC             hDC = NULL;                           // OpenGL��Ⱦ��������
HWND            hWnd = NULL;                          // �������ǵĴ��ھ��
HINSTANCE       hInstance;                          // ��������ʵ��
BOOL    keys[256];                                  // ������̰���������
BOOL    active = TRUE;                              // ���ڵĻ��־��ȱʡΪTRUE
BOOL    fullscreen = TRUE;                            // ȫ����־ȱʡ��ȱʡ�趨��ȫ��ģʽ

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);               // WndProc�Ķ���

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)             // ����OpenGL���ڴ�С
{
	if (height == 0)                              // ��ֹ�����
	{
		height = 1;                           // ��Height��Ϊ1
	}
	glViewport(0, 0, width, height);                    // ���õ�ǰ���ӿ�
	glMatrixMode(GL_PROJECTION);                        // ѡ��ͶӰ����
	glLoadIdentity();                               // ����ͶӰ����
	// �����ӿڵĴ�С
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);                     // ѡ��ģ�͹۲����
	glLoadIdentity();                           // ����ģ�͹۲����
}

int InitGL(GLvoid)                              // �˴���ʼ��OpenGL������������
{
	glShadeModel(GL_SMOOTH);                        // ������Ӱƽ��
	glClearColor(0.0f, 1.0f, 1.0f, 0.0f);                   // ��ɫ����
	glClearDepth(1.0f);                         // ������Ȼ���
	glEnable(GL_DEPTH_TEST);                        // ������Ȳ���
	glDepthFunc(GL_LEQUAL);                         // ������Ȳ��Ե�����
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);          // ����ϵͳ��͸�ӽ�������

		// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	return TRUE;                                // ��ʼ�� OK
}

int DrawGLScene(GLvoid)                             // �����￪ʼ�������еĻ���
{
	// Build and compile our shader program�����ͱ�����ɫ������
// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Link shaders
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);



	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] = {
		// Positions          // Colors           // Texture Coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // Top Right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Bottom Right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Bottom Left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // Top Left 
	};
	GLuint indices[] = {  // Note that we start from 0!
		0, 1, 3, // First Triangle
		1, 2, 3  // Second Triangle
	};
	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO


	// Load and create a texture 
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// Set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load image, create texture and generate mipmaps
	int width, height;
	unsigned char* image = SOIL_load_image("container.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
	
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//glLoadIdentity();                           // ���õ�ǰ��ģ�͹۲����

	//gluLookAt(0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);


	// Bind Texture
	glBindTexture(GL_TEXTURE_2D, texture);

	// Activate shader������ɫ��
	glUseProgram(shaderProgram);

	// Draw container
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Swap the screen buffers
	//glfwSwapBuffers(window);
	//glutSwapBuffers();










	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);         // �����Ļ����Ȼ���

	glLoadIdentity();                           // ���õ�ǰ��ģ�͹۲����

	gluLookAt(0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	GLUquadricObj *objCylinder = gluNewQuadric(); //��������������󡪡�-Բ��
	glPushMatrix();
	glRotatef(45, 1.0, 0.0, 0.0);
	glRotatef(45, 0.0, 1.0, 0.0);
	glColor3f(0.9f, 0.9f, 0.0f);
	glTranslatef(0.3, 0.3, 0.3);
	glRotatef(60, 0.0, 0.0, 1.0);
	gluCylinder(objCylinder, 0.1, 0.05, 0.5, 10, 5);
	glPopMatrix();

	//glutSwapBuffers();

	return TRUE;                                //  һ�� OK
}

GLvoid KillGLWindow(GLvoid)                         // �������ٴ���
{
	//if (fullscreen)                             // ���Ǵ���ȫ��ģʽ��?
	//{
	//	ChangeDisplaySettings(NULL, 0);                  // �ǵĻ����л�������
	//	ShowCursor(TRUE);                       // ��ʾ���ָ��
	//}
	//if (hRC)                                // ����ӵ��OpenGL��Ⱦ��������?
	//{
	//	if (!wglMakeCurrent(NULL, NULL))                 // �����ܷ��ͷ�DC��RC������?
	//	{
	//		MessageBox(NULL, "�ͷ�DC��RCʧ�ܡ�", "�رմ���", MB_OK | MB_ICONINFORMATION);
	//	}

	//	if (!wglDeleteContext(hRC))                 // �����ܷ�ɾ��RC?
	//	{
	//		MessageBox(NULL, "�ͷ�RCʧ�ܡ�", "�رմ���", MB_OK | MB_ICONINFORMATION);
	//	}
	//	hRC = NULL;                           // ��RC��Ϊ NULL
	//}
	//if (hDC && !ReleaseDC(hWnd, hDC))                    // �����ܷ��ͷ� DC?
	//{
	//	MessageBox(NULL, "�ͷ�DCʧ�ܡ�", "�رմ���", MB_OK | MB_ICONINFORMATION);
	//	hDC = NULL;                           // �� DC ��Ϊ NULL
	//}
	//if (hWnd && !DestroyWindow(hWnd))                   // �ܷ����ٴ���?
	//{
	//	MessageBox(NULL, "�ͷŴ��ھ��ʧ�ܡ�", "�رմ���", MB_OK | MB_ICONINFORMATION);
	//	hWnd = NULL;                          // �� hWnd ��Ϊ NULL
	//}

	//if (!UnregisterClass("OpenGL", hInstance))               // �ܷ�ע����?
	//{
	//	MessageBox(NULL, "����ע�������ࡣ", "�رմ���", MB_OK | MB_ICONINFORMATION);
	//	hInstance = NULL;                         // �� hInstance ��Ϊ NULL
	//}
}

BOOL CreateGLWindow(char* title, int width, int height, int bits, BOOL fullscreenflag)
{
	GLuint      PixelFormat;                        // �������ƥ��Ľ��
	WNDCLASS    wc;                         // ������ṹ
	DWORD       dwExStyle;                      // ��չ���ڷ��
	DWORD       dwStyle;                        // ���ڷ��
	RECT        WindowRect;                            // ȡ�þ��ε����ϽǺ����½ǵ�����ֵ
	
	
	static  PIXELFORMATDESCRIPTOR pfd =                 // /pfd ���ߴ���������ϣ���Ķ�����������ʹ�õ����ظ�ʽ
	{
		sizeof(PIXELFORMATDESCRIPTOR),                  // ������ʽ�������Ĵ�С
		1,                              // �汾��
		PFD_DRAW_TO_WINDOW |                        // ��ʽ֧�ִ���
		PFD_SUPPORT_OPENGL |                        // ��ʽ����֧��OpenGL
		PFD_DOUBLEBUFFER,                       // ����֧��˫����
		PFD_TYPE_RGBA,                          // ���� RGBA ��ʽ
		32,                             // ѡ��ɫ�����
		0, 0, 0, 0, 0, 0,                       // ���Ե�ɫ��λ
		0,                              // ��Alpha����
		0,                              // ����Shift Bit
		0,                              // ���ۼӻ���
		0, 0, 0, 0,                         // ���Ծۼ�λ
		16,                             // 16λ Z-���� (��Ȼ���)
		0,                              // ���ɰ建��
		0,                              // �޸�������
		PFD_MAIN_PLANE,                         // ����ͼ��
		0,                              // Reserved
		0, 0, 0                             // ���Բ�����
	};


	WindowRect.left = (long)100;                      // ��Left   ��Ϊ 100
	WindowRect.right = (long)width;                       // ��Right  ��ΪҪ��Ŀ��
	WindowRect.top = (long)100;                           // ��Top    ��Ϊ 100
	WindowRect.bottom = (long)height;                     // ��Bottom ��ΪҪ��ĸ߶�
	fullscreen = fullscreenflag;                      // ����ȫ��ȫ����־
	hInstance = GetModuleHandle(NULL);            // ȡ�����Ǵ��ڵ�ʵ��

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;       // �ƶ�ʱ�ػ�����Ϊ����ȡ��DC
	wc.lpfnWndProc = (WNDPROC)WndProc;                // WndProc������Ϣ
	wc.cbClsExtra = 0;                        // �޶��ⴰ������
	wc.cbWndExtra = 0;                        // �޶��ⴰ������
	wc.hInstance = hInstance;                    // ����ʵ��
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);          // װ��ȱʡͼ��
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);          // װ�����ָ��
	wc.hbrBackground = NULL;                     // GL����Ҫ����
	wc.lpszMenuName = NULL;                     // ����Ҫ�˵�
	wc.lpszClassName = "OpenGL";                 // �趨������

	if (!RegisterClass(&wc))                        // ����ע�ᴰ����
	{
		MessageBox(NULL, "ע�ᴰ��ʧ��", "����", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // �˳�������FALSE
	}

	//if (fullscreen)                             // Ҫ����ȫ��ģʽ��?
	//{
	//	DEVMODE dmScreenSettings;                       // �豸ģʽ
	//	memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));           // ȷ���ڴ����Ϊ��
	//	dmScreenSettings.dmSize = sizeof(dmScreenSettings);           // Devmode �ṹ�Ĵ�С
	//	dmScreenSettings.dmPelsWidth = width;                // ��ѡ��Ļ���
	//	dmScreenSettings.dmPelsHeight = height;               // ��ѡ��Ļ�߶�
	//	dmScreenSettings.dmBitsPerPel = bits;                 // ÿ������ѡ��ɫ�����
	//	dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	//	// ����������ʾģʽ�����ؽ����ע: CDS_FULLSCREEN ��ȥ��״̬����
	//	if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
	//	{
	//		// ��ģʽʧ�ܣ��ṩ����ѡ��˳����ڴ��������С�
	//		if (MessageBox(NULL, "ȫ��ģʽ�ڵ�ǰ�Կ�������ʧ�ܣ�\nʹ�ô���ģʽ��", "NeHe G", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
	//		{
	//			fullscreen = FALSE;               // ѡ�񴰿�ģʽ(Fullscreen=FALSE)
	//		}
	//		else
	//		{
	//			// ����һ���Ի��򣬸����û��������
	//			MessageBox(NULL, "���򽫱��ر�", "����", MB_OK | MB_ICONSTOP);
	//			return FALSE;                   //  �˳������� FALSE
	//		}
	//	}
	//}
	//if (fullscreen)                             // �Դ���ȫ��ģʽ��?
	//{
	//	dwExStyle = WS_EX_APPWINDOW;                  // ��չ������
	//	dwStyle = WS_POPUP;                       // ������
	//	ShowCursor(TRUE);                       // �������ָ��
	//}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;           // ��չ������
		dwStyle = WS_OVERLAPPEDWINDOW;                    //  ������
	}
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);     // �������ڴﵽ����Ҫ��Ĵ�С

	if (!(hWnd = CreateWindowEx(dwExStyle,              // ��չ������
		"OpenGL",               // ������
		title,                  // ���ڱ���
		WS_CLIPSIBLINGS |           // ����Ĵ���������
		WS_CLIPCHILDREN |           // ����Ĵ���������
		dwStyle,                // ѡ��Ĵ�������
		0, 0,                   // ����λ��
		WindowRect.right - WindowRect.left,   // ��������õĴ��ڿ��
		WindowRect.bottom - WindowRect.top,   // ��������õĴ��ڸ߶�
		NULL,                   // �޸�����
		NULL,                   // �޲˵�
		hInstance,              // ʵ��
		NULL)))                 // ����WM_CREATE�����κζ���
	{
		KillGLWindow();                         // ������ʾ��
		MessageBox(NULL, "���ܴ���һ�������豸������", "����", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // ���� FALSE
	}
	if (!(hDC = GetDC(hWnd)))                         // ȡ���豸��������ô?
	{
		KillGLWindow();                         // ������ʾ��
		MessageBox(NULL, "���ܴ���һ����ƥ������ظ�ʽ", "����", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // ���� FALSE
	}
	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))             // Windows �ҵ���Ӧ�����ظ�ʽ����?
	{
		KillGLWindow();                         // ������ʾ��
		MessageBox(NULL, "�����������ظ�ʽ", "����", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // ���� FALSE
	}
	if (!SetPixelFormat(hDC, PixelFormat, &pfd))               // �ܹ��������ظ�ʽô?
	{
		KillGLWindow();                         // ������ʾ��
		MessageBox(NULL, "�����������ظ�ʽ", "����", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // ���� FALSE
	}
	if (!(hRC = wglCreateContext(hDC)))                   // �ܷ�ȡ����ɫ������?
	{
		KillGLWindow();                         // ������ʾ��
		MessageBox(NULL, "���ܴ���OpenGL��Ⱦ������", "����", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // ���� FALSE
	}
	if (!wglMakeCurrent(hDC, hRC))                        // ���Լ�����ɫ������
	{
		KillGLWindow();                         // ������ʾ��
		MessageBox(NULL, "���ܼ��ǰ��OpenGL��Ȼ������", "����", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // ���� FALSE
	}

	ShowWindow(hWnd, SW_SHOW);                       // ��ʾ����

	SetForegroundWindow(hWnd);                      // ����������ȼ�

	SetFocus(hWnd);                             // ���ü��̵Ľ������˴���

	ReSizeGLScene(width, height);                       // ����͸�� GL ��Ļ

	if (!InitGL())                              // ��ʼ���½���GL����
	{
		KillGLWindow();                         // ������ʾ��
		MessageBox(NULL, "Initialization Failed.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // ���� FALSE
	}
	return TRUE;                                // �ɹ�
}

LRESULT CALLBACK WndProc(HWND    hWnd,                   // ���ڵľ��
	UINT    uMsg,                   // ���ڵ���Ϣ
	WPARAM  wParam,                 // ���ӵ���Ϣ����
	LPARAM  lParam)                 // ���ӵ���Ϣ����
{

	switch (uMsg)                               // ���Windows��Ϣ
	{
	case WM_ACTIVATE:                       // ���Ӵ��ڼ�����Ϣ
	{
		if (!HIWORD(wParam))                    // �����С��״̬
		{
			active = TRUE;                    // �����ڼ���״̬
		}
		else
		{
			active = FALSE;                   // �����ټ���
		}

		return 0;                       // ������Ϣѭ��
	}
	case WM_SYSCOMMAND:                     // ϵͳ�ж�����
	{
		switch (wParam)                     // ���ϵͳ����
		{
		case SC_SCREENSAVE:             // ����Ҫ����?
		case SC_MONITORPOWER:               // ��ʾ��Ҫ����ڵ�ģʽ?
			return 0;                   // ��ֹ����
		}
		break;                          // �˳�
	}
	case WM_CLOSE:                          // �յ�Close��Ϣ?
	{
		PostQuitMessage(0);                 // �����˳���Ϣ
		return 0;                       // ����
	}
	case WM_KEYDOWN:                        // �м�����ô?
	{
		keys[wParam] = TRUE;                    // ����ǣ���ΪTRUE
		return 0;                       // ����
	}
	case WM_KEYUP:                          // �м��ſ�ô?
	{
		keys[wParam] = FALSE;                   // ����ǣ���ΪFALSE
		return 0;                       // ����
	}
	case WM_SIZE:                           // ����OpenGL���ڴ�С
	{
		ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));       // LoWord=Width,HiWord=Height
		return 0;                       // ����
	}
	}
	// �� DefWindowProc��������δ�������Ϣ��
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE   hInstance,              // ��ǰ����ʵ��
	HINSTANCE   hPrevInstance,              // ǰһ������ʵ��
	LPSTR       lpCmdLine,              // �����в���
	int     nCmdShow)               // ������ʾ״̬
{

	MSG msg;                                // Windowsx��Ϣ�ṹ
	BOOL    done = FALSE;                         // �����˳�ѭ����Bool ����
	// ��ʾ�û�ѡ������ģʽ
	//if (MessageBox(NULL, "������ȫ��ģʽ������ô��", "����ȫ��ģʽ", MB_YESNO | MB_ICONQUESTION) == IDNO)
	//{
	//	fullscreen = FALSE;                       // FALSEΪ����ģʽ
	//}
	// ����OpenGL����
	if (!CreateGLWindow("OPENGL_WINDOW", 640, 480, 16, fullscreen))
	{
		return 0;                           // ʧ���˳�
	}
	while (!done)                                // ����ѭ��ֱ�� done=TRUE
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))           // ����Ϣ�ڵȴ���?
		{
			if (msg.message == WM_QUIT)               // �յ��˳���Ϣ?
			{
				done = TRUE;                  // �ǣ���done=TRUE
			}
			else                            // ���ǣ���������Ϣ
			{

				TranslateMessage(&msg);             // ������Ϣ
				DispatchMessage(&msg);              // ������Ϣ
			}
		}
		else                                // ���û����Ϣ
		{
			// ���Ƴ���������ESC��������DrawGLScene()���˳���Ϣ
			if (active)                     // ���򼤻��ô?
			{
				if (keys[VK_ESCAPE])                // ESC ������ô?
				{
					done = TRUE;              // ESC �����˳��ź�
				}
				else                        // �����˳���ʱ��ˢ����Ļ
				{
					DrawGLScene();              // ���Ƴ���
					SwapBuffers(hDC);           // �������� (˫����)
				}
			}
			if (keys[VK_F1])                    // F1��������ô?
			{
				keys[VK_F1] = FALSE;              // ���ǣ�ʹ��Ӧ��Key�����е�ֵΪ FALSE
				KillGLWindow();                 // ���ٵ�ǰ�Ĵ���
				fullscreen = !fullscreen;             // �л� ȫ�� / ���� ģʽ
				// �ؽ� OpenGL ����
				if (!CreateGLWindow("OPENGL_WINDOW", 640, 480, 16, fullscreen))
				{
					return 0;               // �������δ�ܴ����������˳�
				}
			}
		}
	}
	// �رճ���
	KillGLWindow();                             // ���ٴ���
	return (msg.wParam);                            // �˳�����
}



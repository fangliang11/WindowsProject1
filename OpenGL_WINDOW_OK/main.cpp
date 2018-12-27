// openGL1.cpp: 主项目文件。
#include <windows.h>        // Windows的头文件

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


HGLRC           hRC = NULL;                          // 窗口着色描述表句柄
HDC             hDC = NULL;                           // OpenGL渲染描述表句柄
HWND            hWnd = NULL;                          // 保存我们的窗口句柄
HINSTANCE       hInstance;                          // 保存程序的实例
BOOL    keys[256];                                  // 保存键盘按键的数组
BOOL    active = TRUE;                              // 窗口的活动标志，缺省为TRUE
BOOL    fullscreen = TRUE;                            // 全屏标志缺省，缺省设定成全屏模式

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);               // WndProc的定义

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)             // 重置OpenGL窗口大小
{
	if (height == 0)                              // 防止被零除
	{
		height = 1;                           // 将Height设为1
	}
	glViewport(0, 0, width, height);                    // 重置当前的视口
	glMatrixMode(GL_PROJECTION);                        // 选择投影矩阵
	glLoadIdentity();                               // 重置投影矩阵
	// 设置视口的大小
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);                     // 选择模型观察矩阵
	glLoadIdentity();                           // 重置模型观察矩阵
}

int InitGL(GLvoid)                              // 此处开始对OpenGL进行所有设置
{
	glShadeModel(GL_SMOOTH);                        // 启用阴影平滑
	glClearColor(0.0f, 1.0f, 1.0f, 0.0f);                   // 黑色背景
	glClearDepth(1.0f);                         // 设置深度缓存
	glEnable(GL_DEPTH_TEST);                        // 启用深度测试
	glDepthFunc(GL_LEQUAL);                         // 所作深度测试的类型
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);          // 告诉系统对透视进行修正

		// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	return TRUE;                                // 初始化 OK
}

int DrawGLScene(GLvoid)                             // 从这里开始进行所有的绘制
{
	// Build and compile our shader program创建和编译着色器程序
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

	//glLoadIdentity();                           // 重置当前的模型观察矩阵

	//gluLookAt(0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);


	// Bind Texture
	glBindTexture(GL_TEXTURE_2D, texture);

	// Activate shader激活着色器
	glUseProgram(shaderProgram);

	// Draw container
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Swap the screen buffers
	//glfwSwapBuffers(window);
	//glutSwapBuffers();










	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);         // 清除屏幕和深度缓存

	glLoadIdentity();                           // 重置当前的模型观察矩阵

	gluLookAt(0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	GLUquadricObj *objCylinder = gluNewQuadric(); //创建二次曲面对象——-圆柱
	glPushMatrix();
	glRotatef(45, 1.0, 0.0, 0.0);
	glRotatef(45, 0.0, 1.0, 0.0);
	glColor3f(0.9f, 0.9f, 0.0f);
	glTranslatef(0.3, 0.3, 0.3);
	glRotatef(60, 0.0, 0.0, 1.0);
	gluCylinder(objCylinder, 0.1, 0.05, 0.5, 10, 5);
	glPopMatrix();

	//glutSwapBuffers();

	return TRUE;                                //  一切 OK
}

GLvoid KillGLWindow(GLvoid)                         // 正常销毁窗口
{
	//if (fullscreen)                             // 我们处于全屏模式吗?
	//{
	//	ChangeDisplaySettings(NULL, 0);                  // 是的话，切换回桌面
	//	ShowCursor(TRUE);                       // 显示鼠标指针
	//}
	//if (hRC)                                // 我们拥有OpenGL渲染描述表吗?
	//{
	//	if (!wglMakeCurrent(NULL, NULL))                 // 我们能否释放DC和RC描述表?
	//	{
	//		MessageBox(NULL, "释放DC或RC失败。", "关闭错误", MB_OK | MB_ICONINFORMATION);
	//	}

	//	if (!wglDeleteContext(hRC))                 // 我们能否删除RC?
	//	{
	//		MessageBox(NULL, "释放RC失败。", "关闭错误", MB_OK | MB_ICONINFORMATION);
	//	}
	//	hRC = NULL;                           // 将RC设为 NULL
	//}
	//if (hDC && !ReleaseDC(hWnd, hDC))                    // 我们能否释放 DC?
	//{
	//	MessageBox(NULL, "释放DC失败。", "关闭错误", MB_OK | MB_ICONINFORMATION);
	//	hDC = NULL;                           // 将 DC 设为 NULL
	//}
	//if (hWnd && !DestroyWindow(hWnd))                   // 能否销毁窗口?
	//{
	//	MessageBox(NULL, "释放窗口句柄失败。", "关闭错误", MB_OK | MB_ICONINFORMATION);
	//	hWnd = NULL;                          // 将 hWnd 设为 NULL
	//}

	//if (!UnregisterClass("OpenGL", hInstance))               // 能否注销类?
	//{
	//	MessageBox(NULL, "不能注销窗口类。", "关闭错误", MB_OK | MB_ICONINFORMATION);
	//	hInstance = NULL;                         // 将 hInstance 设为 NULL
	//}
}

BOOL CreateGLWindow(char* title, int width, int height, int bits, BOOL fullscreenflag)
{
	GLuint      PixelFormat;                        // 保存查找匹配的结果
	WNDCLASS    wc;                         // 窗口类结构
	DWORD       dwExStyle;                      // 扩展窗口风格
	DWORD       dwStyle;                        // 窗口风格
	RECT        WindowRect;                            // 取得矩形的左上角和右下角的坐标值
	
	
	static  PIXELFORMATDESCRIPTOR pfd =                 // /pfd 告诉窗口我们所希望的东东，即窗口使用的像素格式
	{
		sizeof(PIXELFORMATDESCRIPTOR),                  // 上述格式描述符的大小
		1,                              // 版本号
		PFD_DRAW_TO_WINDOW |                        // 格式支持窗口
		PFD_SUPPORT_OPENGL |                        // 格式必须支持OpenGL
		PFD_DOUBLEBUFFER,                       // 必须支持双缓冲
		PFD_TYPE_RGBA,                          // 申请 RGBA 格式
		32,                             // 选定色彩深度
		0, 0, 0, 0, 0, 0,                       // 忽略的色彩位
		0,                              // 无Alpha缓存
		0,                              // 忽略Shift Bit
		0,                              // 无累加缓存
		0, 0, 0, 0,                         // 忽略聚集位
		16,                             // 16位 Z-缓存 (深度缓存)
		0,                              // 无蒙板缓存
		0,                              // 无辅助缓存
		PFD_MAIN_PLANE,                         // 主绘图层
		0,                              // Reserved
		0, 0, 0                             // 忽略层遮罩
	};


	WindowRect.left = (long)100;                      // 将Left   设为 100
	WindowRect.right = (long)width;                       // 将Right  设为要求的宽度
	WindowRect.top = (long)100;                           // 将Top    设为 100
	WindowRect.bottom = (long)height;                     // 将Bottom 设为要求的高度
	fullscreen = fullscreenflag;                      // 设置全局全屏标志
	hInstance = GetModuleHandle(NULL);            // 取得我们窗口的实例

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;       // 移动时重画，并为窗口取得DC
	wc.lpfnWndProc = (WNDPROC)WndProc;                // WndProc处理消息
	wc.cbClsExtra = 0;                        // 无额外窗口数据
	wc.cbWndExtra = 0;                        // 无额外窗口数据
	wc.hInstance = hInstance;                    // 设置实例
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);          // 装入缺省图标
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);          // 装入鼠标指针
	wc.hbrBackground = NULL;                     // GL不需要背景
	wc.lpszMenuName = NULL;                     // 不需要菜单
	wc.lpszClassName = "OpenGL";                 // 设定类名字

	if (!RegisterClass(&wc))                        // 尝试注册窗口类
	{
		MessageBox(NULL, "注册窗口失败", "错误", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // 退出并返回FALSE
	}

	//if (fullscreen)                             // 要尝试全屏模式吗?
	//{
	//	DEVMODE dmScreenSettings;                       // 设备模式
	//	memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));           // 确保内存清空为零
	//	dmScreenSettings.dmSize = sizeof(dmScreenSettings);           // Devmode 结构的大小
	//	dmScreenSettings.dmPelsWidth = width;                // 所选屏幕宽度
	//	dmScreenSettings.dmPelsHeight = height;               // 所选屏幕高度
	//	dmScreenSettings.dmBitsPerPel = bits;                 // 每象素所选的色彩深度
	//	dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	//	// 尝试设置显示模式并返回结果。注: CDS_FULLSCREEN 移去了状态条。
	//	if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
	//	{
	//		// 若模式失败，提供两个选项：退出或在窗口内运行。
	//		if (MessageBox(NULL, "全屏模式在当前显卡上设置失败！\n使用窗口模式？", "NeHe G", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
	//		{
	//			fullscreen = FALSE;               // 选择窗口模式(Fullscreen=FALSE)
	//		}
	//		else
	//		{
	//			// 弹出一个对话框，告诉用户程序结束
	//			MessageBox(NULL, "程序将被关闭", "错误", MB_OK | MB_ICONSTOP);
	//			return FALSE;                   //  退出并返回 FALSE
	//		}
	//	}
	//}
	//if (fullscreen)                             // 仍处于全屏模式吗?
	//{
	//	dwExStyle = WS_EX_APPWINDOW;                  // 扩展窗体风格
	//	dwStyle = WS_POPUP;                       // 窗体风格
	//	ShowCursor(TRUE);                       // 开启鼠标指针
	//}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;           // 扩展窗体风格
		dwStyle = WS_OVERLAPPEDWINDOW;                    //  窗体风格
	}
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);     // 调整窗口达到真正要求的大小

	if (!(hWnd = CreateWindowEx(dwExStyle,              // 扩展窗体风格
		"OpenGL",               // 类名字
		title,                  // 窗口标题
		WS_CLIPSIBLINGS |           // 必须的窗体风格属性
		WS_CLIPCHILDREN |           // 必须的窗体风格属性
		dwStyle,                // 选择的窗体属性
		0, 0,                   // 窗口位置
		WindowRect.right - WindowRect.left,   // 计算调整好的窗口宽度
		WindowRect.bottom - WindowRect.top,   // 计算调整好的窗口高度
		NULL,                   // 无父窗口
		NULL,                   // 无菜单
		hInstance,              // 实例
		NULL)))                 // 不向WM_CREATE传递任何东东
	{
		KillGLWindow();                         // 重置显示区
		MessageBox(NULL, "不能创建一个窗口设备描述表", "错误", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // 返回 FALSE
	}
	if (!(hDC = GetDC(hWnd)))                         // 取得设备描述表了么?
	{
		KillGLWindow();                         // 重置显示区
		MessageBox(NULL, "不能创建一种相匹配的像素格式", "错误", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // 返回 FALSE
	}
	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))             // Windows 找到相应的象素格式了吗?
	{
		KillGLWindow();                         // 重置显示区
		MessageBox(NULL, "不能设置像素格式", "错误", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // 返回 FALSE
	}
	if (!SetPixelFormat(hDC, PixelFormat, &pfd))               // 能够设置象素格式么?
	{
		KillGLWindow();                         // 重置显示区
		MessageBox(NULL, "不能设置像素格式", "错误", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // 返回 FALSE
	}
	if (!(hRC = wglCreateContext(hDC)))                   // 能否取得着色描述表?
	{
		KillGLWindow();                         // 重置显示区
		MessageBox(NULL, "不能创建OpenGL渲染描述表", "错误", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // 返回 FALSE
	}
	if (!wglMakeCurrent(hDC, hRC))                        // 尝试激活着色描述表
	{
		KillGLWindow();                         // 重置显示区
		MessageBox(NULL, "不能激活当前的OpenGL渲然描述表", "错误", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // 返回 FALSE
	}

	ShowWindow(hWnd, SW_SHOW);                       // 显示窗口

	SetForegroundWindow(hWnd);                      // 略略提高优先级

	SetFocus(hWnd);                             // 设置键盘的焦点至此窗口

	ReSizeGLScene(width, height);                       // 设置透视 GL 屏幕

	if (!InitGL())                              // 初始化新建的GL窗口
	{
		KillGLWindow();                         // 重置显示区
		MessageBox(NULL, "Initialization Failed.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // 返回 FALSE
	}
	return TRUE;                                // 成功
}

LRESULT CALLBACK WndProc(HWND    hWnd,                   // 窗口的句柄
	UINT    uMsg,                   // 窗口的消息
	WPARAM  wParam,                 // 附加的消息内容
	LPARAM  lParam)                 // 附加的消息内容
{

	switch (uMsg)                               // 检查Windows消息
	{
	case WM_ACTIVATE:                       // 监视窗口激活消息
	{
		if (!HIWORD(wParam))                    // 检查最小化状态
		{
			active = TRUE;                    // 程序处于激活状态
		}
		else
		{
			active = FALSE;                   // 程序不再激活
		}

		return 0;                       // 返回消息循环
	}
	case WM_SYSCOMMAND:                     // 系统中断命令
	{
		switch (wParam)                     // 检查系统调用
		{
		case SC_SCREENSAVE:             // 屏保要运行?
		case SC_MONITORPOWER:               // 显示器要进入节电模式?
			return 0;                   // 阻止发生
		}
		break;                          // 退出
	}
	case WM_CLOSE:                          // 收到Close消息?
	{
		PostQuitMessage(0);                 // 发出退出消息
		return 0;                       // 返回
	}
	case WM_KEYDOWN:                        // 有键按下么?
	{
		keys[wParam] = TRUE;                    // 如果是，设为TRUE
		return 0;                       // 返回
	}
	case WM_KEYUP:                          // 有键放开么?
	{
		keys[wParam] = FALSE;                   // 如果是，设为FALSE
		return 0;                       // 返回
	}
	case WM_SIZE:                           // 调整OpenGL窗口大小
	{
		ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));       // LoWord=Width,HiWord=Height
		return 0;                       // 返回
	}
	}
	// 向 DefWindowProc传递所有未处理的消息。
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE   hInstance,              // 当前窗口实例
	HINSTANCE   hPrevInstance,              // 前一个窗口实例
	LPSTR       lpCmdLine,              // 命令行参数
	int     nCmdShow)               // 窗口显示状态
{

	MSG msg;                                // Windowsx消息结构
	BOOL    done = FALSE;                         // 用来退出循环的Bool 变量
	// 提示用户选择运行模式
	//if (MessageBox(NULL, "你想在全屏模式下运行么？", "设置全屏模式", MB_YESNO | MB_ICONQUESTION) == IDNO)
	//{
	//	fullscreen = FALSE;                       // FALSE为窗口模式
	//}
	// 创建OpenGL窗口
	if (!CreateGLWindow("OPENGL_WINDOW", 640, 480, 16, fullscreen))
	{
		return 0;                           // 失败退出
	}
	while (!done)                                // 保持循环直到 done=TRUE
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))           // 有消息在等待吗?
		{
			if (msg.message == WM_QUIT)               // 收到退出消息?
			{
				done = TRUE;                  // 是，则done=TRUE
			}
			else                            // 不是，处理窗口消息
			{

				TranslateMessage(&msg);             // 翻译消息
				DispatchMessage(&msg);              // 发送消息
			}
		}
		else                                // 如果没有消息
		{
			// 绘制场景。监视ESC键和来自DrawGLScene()的退出消息
			if (active)                     // 程序激活的么?
			{
				if (keys[VK_ESCAPE])                // ESC 按下了么?
				{
					done = TRUE;              // ESC 发出退出信号
				}
				else                        // 不是退出的时候，刷新屏幕
				{
					DrawGLScene();              // 绘制场景
					SwapBuffers(hDC);           // 交换缓存 (双缓存)
				}
			}
			if (keys[VK_F1])                    // F1键按下了么?
			{
				keys[VK_F1] = FALSE;              // 若是，使对应的Key数组中的值为 FALSE
				KillGLWindow();                 // 销毁当前的窗口
				fullscreen = !fullscreen;             // 切换 全屏 / 窗口 模式
				// 重建 OpenGL 窗口
				if (!CreateGLWindow("OPENGL_WINDOW", 640, 480, 16, fullscreen))
				{
					return 0;               // 如果窗口未能创建，程序退出
				}
			}
		}
	}
	// 关闭程序
	KillGLWindow();                             // 销毁窗口
	return (msg.wParam);                            // 退出程序
}



#pragma warning(disable : 4996)

#include <GL/glew.h>
#include "Textfile.h"
#include <GL/freeglut.h>
#include <iostream>

//#pragma comment(lib,"glew32.lib")  

using namespace std;

GLuint vShader, fShader;//顶点/片段着色器对象  
GLuint vaoHandle;// VAO对象

//顶点位置数组  
float positionData[] = {
						-0.5f,-0.5f,0.0f,1.0f,
						0.5f,-0.5f,0.0f,1.0f,
						0.5f,0.5f,0.0f,1.0f,
						-0.5f,0.5f,0.0f,1.0f
};
//顶点颜色数组  
float colorData[] = {
					 1.0f, 0.0f, 0.0f,1.0f,
					 0.0f, 1.0f, 0.0f,1.0f,
					 0.0f, 0.0f, 1.0f,1.0f,
					 1.0f,1.0f,0.0f,1.0f
};

void initShader(const char *VShaderFile, const char *FShaderFile)
{
	//1、查看显卡、GLSL和OpenGL的信息  
	const GLubyte *vendor = glGetString(GL_VENDOR);
	const GLubyte *renderer = glGetString(GL_RENDERER);
	const GLubyte *version = glGetString(GL_VERSION);
	const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	cout << "显卡供应商   : " << vendor << endl;
	cout << "显卡型号     : " << renderer << endl;
	cout << "OpenGL版本   : " << version << endl;
	cout << "GLSL版本     : " << glslVersion << endl;

	//2、编译着色器  
	//创建着色器对象：顶点着色器  
	vShader = glCreateShader(GL_VERTEX_SHADER);
	//错误检测  
	if (0 == vShader)
	{
		cerr << "ERROR : Create vertex shader failed" << endl;
		exit(1);
	}

	//把着色器源代码和着色器对象相关联  
	const GLchar *vShaderCode = textFileRead(VShaderFile);
	const GLchar *vCodeArray[1] = { vShaderCode };

	//将字符数组绑定到对应的着色器对象上
	glShaderSource(vShader, 1, vCodeArray, NULL);

	//编译着色器对象  
	glCompileShader(vShader);

	//检查编译是否成功  
	GLint compileResult;
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &compileResult);
	if (GL_FALSE == compileResult)
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
			cerr << "vertex shader compile log : " << endl;
			cerr << log << endl;
			free(log);//释放空间  
		}
	}

	//创建着色器对象：片断着色器  
	fShader = glCreateShader(GL_FRAGMENT_SHADER);
	//错误检测  
	if (0 == fShader)
	{
		cerr << "ERROR : Create fragment shader failed" << endl;
		exit(1);
	}

	//把着色器源代码和着色器对象相关联  
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
			cerr << "fragment shader compile log : " << endl;
			cerr << log << endl;
			free(log);//释放空间  
		}
	}

	//3、链接着色器对象  
	//创建着色器程序  
	GLuint programHandle = glCreateProgram();
	if (!programHandle)
	{
		cerr << "ERROR : create program failed" << endl;
		exit(1);
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
		}
	}
	else//链接成功，在OpenGL管线中使用渲染程序  
	{
		glUseProgram(programHandle);
	}
}

void initVBO()
{
	//绑定VAO
	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);

	// Create and populate the buffer objects  
	GLuint vboHandles[2];
	glGenBuffers(2, vboHandles);
	GLuint positionBufferHandle = vboHandles[0];
	GLuint colorBufferHandle = vboHandles[1];

	//绑定VBO以供使用  
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	//加载数据到VBO  
	glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float),
		positionData, GL_STATIC_DRAW);

	//绑定VBO以供使用  
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
	//加载数据到VBO  
	glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float),
		colorData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);//顶点坐标  
	glEnableVertexAttribArray(1);//顶点颜色  

		//调用glVertexAttribPointer之前需要进行绑定操作  
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
}

void init()
{
	//初始化glew扩展库  
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cout << "Error initializing GLEW: " << glewGetErrorString(err) << endl;
	}
	//加载顶点和片段着色器对象并链接到一个程序对象上
	initShader("VertexShader.vert", "FragmentShader.frag");
	//绑定并加载VAO，VBO
	initVBO();
	glClearColor(0.0, 0.0, 0.0, 0.0);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	//使用VAO、VBO绘制  
	glBindVertexArray(vaoHandle);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glBindVertexArray(0);
	glutSwapBuffers();
}

//ESC键用于退出使用着色器
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		glDeleteShader(vShader);
		glUseProgram(0);
		glutPostRedisplay(); //刷新显示
		break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Hello GLSL");
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}

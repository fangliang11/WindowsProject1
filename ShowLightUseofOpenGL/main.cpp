#include "GL/glut.h"
#include <stdio.h>
#include <stdlib.h>

#pragma warning(disable : 4996)

#define WindowWidth  400
#define WindowHeight 400

#define BMP_Header_Length 54
void grab(void)
{
	FILE*    pDummyFile;  //ָ����һbmp�ļ������ڸ��������ļ�ͷ����Ϣͷ����
	FILE*    pWritingFile;  //ָ��Ҫ�����ͼ��bmp�ļ�
	GLubyte* pPixelData;    //ָ���µĿյ��ڴ棬���ڱ����ͼbmp�ļ�����
	GLubyte  BMP_Header[BMP_Header_Length];
	GLint    i, j;
	GLint    PixelDataLength;   //BMP�ļ������ܳ���

	// �����������ݵ�ʵ�ʳ���
	i = WindowWidth * 3;   // �õ�ÿһ�е��������ݳ���
	while (i % 4 != 0)      // �������ݣ�ֱ��i�ǵı���
		++i;
	PixelDataLength = i * WindowHeight;  //��������λ��

	// �����ڴ�ʹ��ļ�
	pPixelData = (GLubyte*)malloc(PixelDataLength);
	if (pPixelData == 0)
		exit(0);

	pDummyFile = fopen("image.bmp", "rb");//ֻ����ʽ��
	if (pDummyFile == 0)
		exit(0);

	pWritingFile = fopen("image2.bmp", "wb"); //ֻд��ʽ��
	if (pWritingFile == 0)
		exit(0);

	//�Ѷ����bmp�ļ����ļ�ͷ����Ϣͷ���ݸ��ƣ����޸Ŀ������
	fread(BMP_Header, sizeof(BMP_Header), 1, pDummyFile);  //��ȡ�ļ�ͷ����Ϣͷ��ռ��54�ֽ�
	fwrite(BMP_Header, sizeof(BMP_Header), 1, pWritingFile);
	fseek(pWritingFile, 0x0012, SEEK_SET); //�ƶ���0X0012����ָ��ͼ���������ڴ�
	i = WindowWidth;
	j = WindowHeight;
	fwrite(&i, sizeof(i), 1, pWritingFile);
	fwrite(&j, sizeof(j), 1, pWritingFile);

	// ��ȡ��ǰ������ͼ�����������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);  //����4λ���뷽ʽ
	glReadPixels(0, 0, WindowWidth, WindowHeight,
		GL_BGR_EXT, GL_UNSIGNED_BYTE, pPixelData);

	// д����������
	fseek(pWritingFile, 0, SEEK_END);
	//��������BMP�ļ�����д��pWritingFile
	fwrite(pPixelData, PixelDataLength, 1, pWritingFile);

	// �ͷ��ڴ�͹ر��ļ�
	fclose(pDummyFile);
	fclose(pWritingFile);
	free(pPixelData);
}

static GLfloat angle = 0.0f;
void myDisplay(void)
{
	glClearColor(0.3, 0.7, 0.5, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  //������ɫ����Ȼ���       

	// ����͸��Ч����ͼ        
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(80.0f, 1.0f, 1.0f, 20.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 12.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	// ����̫����Դ������һ�ְ�ɫ�Ĺ�Դ     
	{
		GLfloat sun_light_position[] = { 0.0f, 0.0f, 0.0f, 1.0f }; //��Դ��λ������������ϵԲ�ģ����������ʽ  
		GLfloat sun_light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f }; //RGBAģʽ�Ļ����⣬Ϊ0  
		GLfloat sun_light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f }; //RGBAģʽ��������⣬ȫ�׹�  
		GLfloat sun_light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };  //RGBAģʽ�µľ���� ��ȫ�׹�  
		glLightfv(GL_LIGHT0, GL_POSITION, sun_light_position);
		glLightfv(GL_LIGHT0, GL_AMBIENT, sun_light_ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_light_diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, sun_light_specular);

		//�����ƹ�  
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
	}

	// ����̫���Ĳ��ʲ�����̫��      
	{
		GLfloat sun_mat_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };  //������ʵĻ�������ɫ��Ϊ0  
		GLfloat sun_mat_diffuse[] = { 0.0f, 0.0f, 0.0f, 1.0f };  //������ʵ����������ɫ��Ϊ0  
		GLfloat sun_mat_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };   //������ʵľ��淴�����ɫ��Ϊ0  
		GLfloat sun_mat_emission[] = { 0.8f, 0.0f, 0.0f, 1.0f };   //������ʵķ������ɫ��Ϊƫ��ɫ  
		GLfloat sun_mat_shininess = 0.0f;
		glMaterialfv(GL_FRONT, GL_AMBIENT, sun_mat_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, sun_mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, sun_mat_specular);
		glMaterialfv(GL_FRONT, GL_EMISSION, sun_mat_emission);
		glMaterialf(GL_FRONT, GL_SHININESS, sun_mat_shininess);
		glutSolidSphere(3.0, 40, 32);
	}

	// �������Ĳ��ʲ����Ƶ���      
	{
		GLfloat earth_mat_ambient[] = { 0.0f, 0.0f, 1.0f, 1.0f };  //������ʵĻ�������ɫ��ƭ��ɫ  
		GLfloat earth_mat_diffuse[] = { 0.0f, 0.0f, 0.5f, 1.0f };  //������ʵ����������ɫ��ƫ��ɫ  
		GLfloat earth_mat_specular[] = { 1.0f, 0.0f, 0.0f, 1.0f };   //������ʵľ��淴�����ɫ����ɫ  
		GLfloat earth_mat_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };   //������ʵķ������ɫ��Ϊ0  
		GLfloat earth_mat_shininess = 30.0f;
		glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat_specular);
		glMaterialfv(GL_FRONT, GL_EMISSION, earth_mat_emission);
		glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
		glRotatef(angle, 0.0f, -1.0f, 0.0f);
		glTranslatef(7.0f, 0.0f, 0.0f);
		glutSolidSphere(3.0, 40, 32);
	}
	glutSwapBuffers();
	grab();
}

void myIdle(void)
{
	angle += 1.0f;
	if (angle >= 360.0f)
		angle = 0.0f;
	myDisplay();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(400, 400);
	glutCreateWindow("OpenGL������ʾ");
	glutDisplayFunc(&myDisplay);
	glutIdleFunc(&myIdle);
	glutMainLoop();
	return 0;
}

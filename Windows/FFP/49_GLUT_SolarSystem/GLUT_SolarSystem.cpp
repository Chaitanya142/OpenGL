#include<C:\freeglut\include\GL\freeglut.h>
#pragma comment(lib,"freeglut.lib")

bool bIsFullScreen = false;
int year=0;
int day=0;

int main(int argc, char *argv[])
{
	//Function Declaration
	void initialize(void);
	void uninitialize(void);
	void reshape(int, int);
	void display(void);
	void keyboard(unsigned char, int, int);
	void mouse(int, int, int, int);

	//Code
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE| GLUT_DEPTH | GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("OpenGL Program - CHAITANYA PRABHUNE");

	initialize();

	//Call Back Func
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutCloseFunc(uninitialize);

	glutMainLoop();

	return(0);
}

void initialize(void) {
	//Code
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glShadeModel(GL_FLAT);
}

void uninitialize(void) {
	//Code
}

void reshape(int width, int height) {
	//Code
	if (height == 0)
	height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f,
		(GLfloat)width / (GLfloat)height,
		1.0f,
		20.0f);
}

void display(void) {
	//Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0f,0.0f,5.0f,
		0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f);
	glPushMatrix();
	glutWireSphere(1.0f,20.0f,16.0f);
	glRotatef((GLfloat)year,0.0f,1.0f,0.0f);
	glTranslatef(2.0f,0.0f,0.0f);
	glRotatef((GLfloat)day, 0.0f, 1.0f, 0.0f);
	glutWireSphere(0.2f, 20.0f, 16.0f);
	glPopMatrix();
	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27://Escape
		glutLeaveMainLoop();
		break;
	case 'F':
	case 'f':
		if (bIsFullScreen == false) {
			glutFullScreen();
			bIsFullScreen = true;
		}
		else {
			glutLeaveFullScreen();
			bIsFullScreen = false;
		}
		break;
	case 'Y':
		year = (year + 5) % 360;
		glutPostRedisplay();
		break;
	case 'y':
		year = (year - 5) % 360;
		glutPostRedisplay();
		break;
	case 'D':
		day = (day + 10) % 360;
		glutPostRedisplay();
		break;
	case 'd':
		day = (day - 10) % 360;
		glutPostRedisplay();
		break;
	}
}

void mouse(int button, int state, int x, int y) {
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		break;
	case GLUT_RIGHT_BUTTON:
		glutLeaveMainLoop();
		break;
	}
}
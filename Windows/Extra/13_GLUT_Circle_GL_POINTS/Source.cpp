#include<C:\freeglut\include\GL\freeglut.h>
#include<math.h>

#define PI 3.1415926535

bool bIsFullScreen = false;

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
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Circle using GL_POINTS - CHAITANYA PRABHUNE");

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
}

void uninitialize(void) {
	//Code
}

void reshape(int width, int height) {
	//Code
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}

void display(void) {
	//Variable Declaration
	double  angle;
	
	//Code
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	glLineWidth(1.0f);

	glBegin(GL_POINTS);

	glColor3f(1.0f, 1.0f, 1.0f);

	for (angle = 0; angle <= 2*PI; angle= angle+0.001) {
		glVertex3f(cos(angle), sin(angle), 0.0f);
	}

	glEnd();



	glFlush();
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

#include<C:\freeglut\include\GL\freeglut.h>
#pragma comment(lib,"freeglut.lib")

bool bIsFullScreen = false;
GLfloat angle_Cube = 0.0f;
GLfloat angle_Pyramid = 0.0f;

int main(int argc, char *argv[])
{
	//Function Declaration
	void initialize(void);
	void uninitialize(void);
	void reshape(int, int);
	void display(void);
	void keyboard(unsigned char, int, int);
	void mouse(int, int, int, int);
	void update(void);

	//Code
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("GLUT 3D Rotation - CHAITANYA PRABHUNE");

	initialize();

	//Call Back Func
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutIdleFunc(update);
	glutCloseFunc(uninitialize);

	glutMainLoop();

	return(0);
}

void initialize(void) {
	//Code
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

}

void uninitialize(void) {
	//Code
}

void reshape(int width, int height) {
	//Code
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,
		(GLfloat)width / (GLfloat)height,
		0.1f,
		100.0f);

}

void display(void) {
	void DrawCube(float offset);
	void DrawPyramid(float offset);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(-1.5f, 0.0f, -6.0f);
	glRotatef(angle_Pyramid, 0.0f, 1.0f, 0.0f);

	DrawPyramid(1.0f);


	glLoadIdentity();
	glTranslatef(+1.5f, 0.0f, -6.0f);
	glScalef(0.75f, 0.75f, 0.75f);
	glRotatef(angle_Cube, 1.0f, 1.0f, 1.0f);

	DrawCube(1.0f);

	glutSwapBuffers();
}

void update(void)
{
	angle_Cube = angle_Cube + 0.2f;
	if (angle_Cube > 360.0f)
		angle_Cube = 0.0f;
	angle_Pyramid = angle_Pyramid + 0.2f;
	if (angle_Pyramid > 360.0f)
		angle_Pyramid = 0.0f;
	glutPostRedisplay();
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


void DrawCube(float offset)
{
	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(offset, offset, -offset);
	glVertex3f(-offset, offset, -offset);
	glVertex3f(-offset, offset, offset);
	glVertex3f(offset, offset, offset);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(offset, -offset, -offset);
	glVertex3f(-offset, -offset, -offset);
	glVertex3f(-offset, -offset, offset);
	glVertex3f(offset, -offset, offset);

	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(offset, offset, offset);
	glVertex3f(-offset, offset, offset);
	glVertex3f(-offset, -offset, offset);
	glVertex3f(offset, -offset, offset);

	glColor3f(0.0f, 1.0f, 1.0f);
	glVertex3f(offset, offset, -offset);
	glVertex3f(-offset, offset, -offset);
	glVertex3f(-offset, -offset, -offset);
	glVertex3f(offset, -offset, -offset);

	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(offset, offset, -offset);
	glVertex3f(offset, offset, offset);
	glVertex3f(offset, -offset, offset);
	glVertex3f(offset, -offset, -offset);

	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex3f(-offset, offset, -offset);
	glVertex3f(-offset, offset, offset);
	glVertex3f(-offset, -offset, offset);
	glVertex3f(-offset, -offset, -offset);

	glEnd();
}


void DrawPyramid(float offset)
{
	glBegin(GL_TRIANGLES);

	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, offset, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-offset, -offset, offset);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(offset, -offset, offset);


	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, offset, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(offset, -offset, offset);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(offset, -offset, -offset);


	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, offset, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(offset, -offset, -offset);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-offset, -offset, -offset);

	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, offset, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-offset, -offset, -offset);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-offset, -offset, offset);

	glEnd();
}

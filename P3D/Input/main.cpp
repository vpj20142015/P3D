#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include <stdio.h>
#include <stdlib.h>

float angle = 0.0;
float angleVertical = 0;
float red = 1.0, blue = 1.0, green = 1.0;


void changeSize(int w, int h)
{
	float ratio;

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0) h = 1;

	ratio = 1.0 * w / h;

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Set the correct perspective.
	gluPerspective(45, ratio, 1, 1000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 5.0,
		0.0, 0.0, -1.0,
		0.0, 1.0, 0.0);
}


void renderScene(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	glRotatef(angle, 0.0, 1.0, 0.0);
	glRotatef(angleVertical, 1, 0, 0);
	/*glBegin(GL_TRIANGLES);
	glColor3f(1.0, 1.0, 1.0);
	glVertex3f(-0.5, -0.5, 0.0);
	glColor3f(red, green, blue);
	glVertex3f(0.5, 1.0, 0.0);
	glColor3f(1.0, 1.0, 1.0);
	glVertex3f(1.0, -0.5, 0.0);
	glEnd();*/

	glutWireSphere(1, 10, 10);

	glPopMatrix();

	//angle += 0.02;

	glutSwapBuffers();
}

void mouseInput(int button, int state, int x, int y)
{
	if (button == 0)
	{
		printf("%d, %d\n", x, y);
	}
}

void mousePosition(int x, int y)
{
	printf("%d, %d", x, y);
}

void processNormalKeys(unsigned char key, int x, int y)
{
	key = toupper(key);

	if (key == 27) exit(0);

	if (key == 'A')
	{
		printf("tecla a \n");
		angle -= 0.2f;

	}
	if (key == 'S')
	{
		printf("tecla s\n");
		angleVertical += 0.2f;
	}
	if (key == 'D')
	{
		printf("tecla d\n");
		angle += 0.2f;
	}
	if (key == 'W')
	{
		printf("tecla w\n");
		angleVertical -= 0.2f;
	}
}


void processSpecialKeys(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_F1: red = 1.0; green = 0.0; blue = 0.0;
		glColor3f(red, green, blue);
		break;
	case GLUT_KEY_F2: red = 0.0; green = 1.0; blue = 0.0;
		glColor3f(red, green, blue);
		break;
	case GLUT_KEY_F3: red = 0.0; green = 0.0; blue = 1.0;
		glColor3f(red, green, blue);
		break;
	}
}


void main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(320, 320);
	glutCreateWindow("Programa-06");
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);

	//adding here the setting of keyboard processing
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);
	//gere mouse clicks
	glutMouseFunc(mouseInput);
	//mouse position
	glutPassiveMotionFunc(mousePosition);

	glutMainLoop();
}

#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"

void renderScene(void){
	int i;
	char str[] = "Isto é um teste!";

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-10.0, 10.0, -10.0, 10.0);
	glRasterPos2d(0.0f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);

	for (i = 0; i < strlen(str); i++) glutBitmapCharacter(GLUT_BITMAP_8_BY_13, str[i]);
	glFlush();
}

int main(int argc, char **argv)
{
	// Por inicialização aqui
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Texto");
	//glutFullScreen();

	// Por registo de funções aqui
	glutDisplayFunc(renderScene);

	// Entrar no ciclo do GLUT aqui
	glutMainLoop();
	return 0;
}
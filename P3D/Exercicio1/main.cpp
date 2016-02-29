#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include <iostream>

void changeSize(int w, int h)
{
	float ratio;
	// Prevenir a divisão por zero, se a janela for muito pequena
	if(h == 0) h = 1;
	ratio = 1.0 * w / h;
	// Definir o viewporT de toda a janela
	glViewport(0, 0, w, h);
	// Efetuar o reset do sistema de coordenadas, antes de o utilizar
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Definir a perspectiva pretendida
	gluPerspective(45, ratio, 1, 1000);
	//glOrtho(-2.0, 2.0,-2.0, 2.0,-10.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
}

void renderScene(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0); //clear black

	glLoadIdentity();
	gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	//Colocar aqui os comandos de desenho de objetos
	glutWireTeapot(1);

	glutSwapBuffers();
}

int main(int argc, char **argv)
{
	printf("Program started.\n");

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 20);
	glutInitWindowSize(800, 600);
	glutCreateWindow("OpenGL First Window");

	// Por registo de funções aqui
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	// Alguns settings para OpenGL
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glutMainLoop();

	return 0;
}
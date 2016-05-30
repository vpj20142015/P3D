#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE

#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include "glm.h"
#include <stdio.h>
#include <windows.h>

//Inicializa as propriedades do material, fonte de luz, z-buffer, etc...
void init(void)
{
	GLfloat light_ambient[] = { 0.6, 0.2, 0.2, 1.0 }; // Vermelho "acinzentado"
	GLfloat light_diffuse[] = { 1.0, 0.0, 0.0, 1.0 }; // Vermelho
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 }; // Branco
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 }; // Luz direccional (posicionada no infinito)
	GLfloat mat_ambient_and_diffuse[] = { 1.0, 1.0, 1.0, 1.0 }; // Reflecte 100% todas as componentes de cor
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 }; // Reflecte 100% todas as componentes de cor
	GLfloat mat_shininess[] = { 128.0 }; // Expoente especular. [0 (mais brilho), 128 (menos brilho)]

	// Permitir iluminação e activar fonte de luz #0
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// Configurar cor da fonte de luz
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	// Configurar a posição da fonte de luz
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	// Configurar material
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambient_and_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	// Outros...
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
}


void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glutSolidSphere(1.0, 100, 100);
	//glutWireSphere(1.0, 30, 30);
	//glutSolidSphere(1.0, 30, 30);

	glFlush();
}


void reshape(int w, int h)
{
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h)
		glOrtho(-1.5, 1.5, -1.5*(GLfloat)h / (GLfloat)w, 1.5*(GLfloat)h / (GLfloat)w, -10.0, 10.0);
	else
		glOrtho(-1.5*(GLfloat)w / (GLfloat)h, 1.5*(GLfloat)w / (GLfloat)h, -1.5, 1.5, -10.0, 10.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	}
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Programa-13");

	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);

	glutMainLoop();

	return 0;
}
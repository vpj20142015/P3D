#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE

//Nevoeiro.

#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include "glm.h"
#include <stdio.h>
#include <math.h>
#include <windows.h>

// Protótipos de funções
void display(void);
void reshape(GLsizei w, GLsizei h);


int myDL;
void funcmyDL(void);


void init(void)
{
	GLfloat fogColor[] = { 0.5, 0.5, 0.5, 1.0 }; // Nevoeiro cinzento

	// Define técnica de shading: GL_FLAT, GL_SMOOTH
	glShadeModel(GL_SMOOTH);

	// Activa o teste de profundidade
	glEnable(GL_DEPTH_TEST);

	// Activar o nevoeiro
	glEnable(GL_FOG);

	// Define a cor do nevoeiro
	glFogfv(GL_FOG_COLOR, fogColor);

	// Define nevoeiro linear
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogf(GL_FOG_START, 2.0f);
	glFogf(GL_FOG_END, 20.0f);

	// Define nevoeiro exponencial (1 - e^(-0.04d))
	/*glFogi(GL_FOG_MODE, GL_EXP);
	glFogf(GL_FOG_DENSITY, 0.1f);*/

	// Define nevoeiro exponencial ao quadrado (1 - e^(-(0.04d)^2))
	/*glFogi(GL_FOG_MODE, GL_EXP2);
	glFogf(GL_FOG_DENSITY, 0.04f);*/

	// Compila o modelo
	funcmyDL();
}


void display(void)
{
	glClearColor(0.5, 0.5, 0.5, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	gluLookAt(0.0, 2.0, 0.0,
		0.0, 0.0, -10.0,
		0.0, 1.0, 0.0);

	// Floor
	glCallList(myDL);

	glutSwapBuffers();

	glFlush();
}


void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 0.5, 200.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -5.0);
}


void funcmyDL(void)
{
	int x, z;

	myDL = glGenLists(1);

	glNewList(myDL, GL_COMPILE);
	// Floor
	for (x = -100; x <= 100; x += 2)
	{
		for (z = -100; z <= 100; z += 2)
		{
			glColor3f(1.0, 1.0, 1.0);
			glBegin(GL_QUADS);
			glNormal3f(0.0, 1.0, 0.0);
			glVertex3f(x, 0.0f, z);				// Top Left
			glVertex3f(x + 1.0f, 0.0f, z);		// Top Right
			glVertex3f(x + 1.0f, 0.0f, z - 1.0f);	// Bottom Right
			glVertex3f(x, 0.0f, z - 1.0f);		// Bottom Left
			glEnd();
		}
		for (z = -99; z <= 100; z += 2)
		{
			glColor3f(0.2, 0.2, 0.2);
			glBegin(GL_QUADS);
			glNormal3f(0.0f, 1.0f, 0.0f);
			glVertex3f(x, 0.0f, z);				// Top Left
			glVertex3f(x + 1.0f, 0.0f, z);		// Top Right
			glVertex3f(x + 1.0f, 0.0f, z - 1.0f);	// Bottom Right
			glVertex3f(x, 0.0f, z - 1.0f);		// Bottom Left
			glEnd();
		}
	}
	for (x = -99; x <= 100; x += 2)
	{
		for (z = -99; z <= 100; z += 2)
		{
			glColor3f(1.0, 1.0, 1.0);
			glBegin(GL_QUADS);
			glNormal3f(0.0, 1.0, 0.0);
			glVertex3f(x, 0.0f, z);				// Top Left
			glVertex3f(x + 1.0f, 0.0f, z);		// Top Right
			glVertex3f(x + 1.0f, 0.0f, z - 1.0f);	// Bottom Right
			glVertex3f(x, 0.0f, z - 1.0f);		// Bottom Left
			glEnd();
		}
		for (z = -100; z <= 100; z += 2)
		{
			glColor3f(0.0, 0.0, 1.0);
			glBegin(GL_QUADS);
			glNormal3f(0.0f, 1.0f, 0.0f);
			glVertex3f(x, 0.0f, z);				// Top Left
			glVertex3f(x + 1.0f, 0.0f, z);		// Top Right
			glVertex3f(x + 1.0f, 0.0f, z - 1.0f);	// Bottom Right
			glVertex3f(x, 0.0f, z - 1.0f);		// Bottom Left
			glEnd();
		}
	}
	glEndList();
}


int main(int argc, char** argv)
{
	// Inicialização do GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);

	glutGameModeString("1366x768:32");
	if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE))
	{
		glutEnterGameMode();
	}
	else  // Cria Janela Normal
	{
		glutInitWindowPosition(0, 0);
		glutInitWindowSize(800, 600);
		glutCreateWindow("Programa-15");
	}

	// Inicializações
	init();

	// Registar funções de callback
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(display);

	// Ciclo infinito do GLUT
	glutMainLoop();

	return 0;
}

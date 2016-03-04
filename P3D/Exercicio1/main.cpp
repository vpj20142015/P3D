#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include <iostream>

float angle = 0.0f;


void changeSize(int w, int h)
{
	float ratio;
	// Prevenir a divisão por zero, se a janela for muito pequena
	if (h == 0) h = 1;
	ratio = 1.0 * w / h;
	// Definir o viewpor de toda a janela
	glViewport(0, 0, w, h);
	// Efetuar o reset do sistema de coordenadas, antes de o utilizar
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Definir a perspectiva pretendida
	gluPerspective(45, ratio, 1, 1000);
	//glOrtho(-2.0, 2.0, -2.0, 2.0, -10.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
}
void renderScene(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	//rodar objeto
	angle += 0.5f;
	glRotatef(angle, 0.0, 1.0, 0.0);
	// Colocar aqui os comandos de desenho de objetos
	glutWireTeapot(1);
	// Troca os buffers, mostrando o que acabou de ser desenhado
	glutSwapBuffers();
}
int main(int argc, char **argv)
{
	// Por inicialização aqui
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Programa-03");
	glutFullScreen();
	// Por registo de funções aqui
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	// Alguns settings para OpenGL
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	// Entrar no ciclo do GLUT aqui
	glutMainLoop();
	return 0;
}
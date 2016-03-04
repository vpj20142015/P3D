#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include <iostream>

int win_w; //width 
int win_h; //height
float ratio;
float angle = 0.0f;

void changeSize(int w, int h)
{
	win_w = w;
	win_h = h;
	
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

	//rodar objeto
	angle += 0.5f;
	
	//Top Left
	glViewport(0, win_h / 2, win_w / 2, win_h / 2);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, ratio, 1, 1000);
	//glOrtho(-3.0, 3.0, -3.0, 3.0, 1.0, 50.0);
	gluLookAt(0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(angle, 0.0, 1.0, 0.0);
	glutWireTeapot(1);

	//Top Right
	glViewport(win_w / 2, win_h / 2, win_w / 2, win_h / 2);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, ratio, 1, 1000);
	//glOrtho(-3.0, 3.0, -3.0, 3.0, 1.0, 50.0);
	gluLookAt(5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(angle, 0.0, 1.0, 0.0);
	glutWireTeapot(1);

	//Bottom Left
	glViewport(0, 0, win_w / 2, win_h / 2);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, ratio, 1, 1000);
	//glOrtho(-3.0, 3.0, -3.0, 3.0, 1.0, 50.0);
	gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(angle, 0.0, 1.0, 0.0);
	glutWireTeapot(1);

	//Bottom Right
	glViewport(win_w / 2, 0, win_w / 2, win_h / 2);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, ratio, 1, 1000);
	//glOrtho(-3.0, 3.0, -3.0, 3.0, 1.0, 50.0);
	gluLookAt(0.0, 2.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(angle, 0.0, 1.0, 0.0);
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
	glutInitWindowSize(600, 600);
	glutCreateWindow("QuatroViewPorts");
	//glutFullScreen();

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
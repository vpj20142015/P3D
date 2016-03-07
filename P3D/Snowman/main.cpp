#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include <iostream>

int win_w; //width 
int win_h; //height
float ratio;

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

void drawSnowMan(){

	glColor3ub(rand() % 255, rand() % 255, rand() % 255);
	//Barriga
	glTranslatef(0.0, 0.75, 0.0);
	glutWireSphere(0.75, 20.0, 20.0);

	//Cabeça
	glTranslatef(0.0, 1.0, 0.0);
	glutWireSphere(0.25, 20.0, 20.0);

	//desenha olhos
	glPushMatrix();
	glColor3f(0.3f, 0.3f, 0.3f);
	glTranslatef(0.05f, 0.1f, 0.18f);
	glutSolidSphere(0.05f, 10, 10);
	glTranslatef(-0.1f, 0.0f, 0.0f);
	glutSolidSphere(0.05f, 10, 10);
	glPopMatrix();

	//nariz
	glColor3f(1.0, 0.0, 0.0);
	glTranslatef(0.0, 0.0, 0.25);
	glRotatef(0, 0.0, 1.0, 0.0);
	glutSolidCone(0.05, 0.5, 10, 2);
}

void drawSnowmanMatrix(){
	glColor3f(1.0, 1.0, 1.0);
	for (int i = 0; i < 20; i+= 2){
		for (int j = 0; j < 20; j+= 2){
			glLoadIdentity();
			glPushMatrix();
			glTranslatef(i, 0, j);
			drawSnowMan();
			glPopMatrix();
		}
	}
}

void renderScene(void)
{

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glColor3f(1.0, 1.0, 1.0);

	//Top Left
	glViewport(0, 0, win_w, win_h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, ratio, 1, 1000);
	gluLookAt(20.0, 12.0, 35.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0);
	glMatrixMode(GL_MODELVIEW);

	drawSnowmanMatrix();
	
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
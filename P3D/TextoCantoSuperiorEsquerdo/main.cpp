#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"

void renderScene(void){
	int i, width, height;
	char str[] = "Isto é um teste!";

	width = glutGet(GLUT_WINDOW_WIDTH);
	height = glutGet(GLUT_WINDOW_HEIGHT);
	glViewport(0.0, 0.0, width, height);


	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	
	

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//Magic
	gluOrtho2D(0.0, (double)width, 0.0, (double)height);
	glScalef(1.0f, -1.0f, 1.0f); //Inverte a escala em y (o y aponta agora para baixo)
	glTranslatef(0.0f, (double)-height, 0.0f); //Move todo o y para cima (no sentido -y)

	glRasterPos2d(10.0f, 20.0f);
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
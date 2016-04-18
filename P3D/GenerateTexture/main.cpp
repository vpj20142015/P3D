/***********************************PROGRAMA-22*********************************/
//Texturas (Cria uma imagem vermelha).

#include "Dependencies\freeglut\freeglut.h"
#include <stdlib.h>


void showImage(unsigned char* pcData, int iWidth, int iHeight)
{
	GLuint texture;

	glEnable(GL_TEXTURE_2D);

	// allocate a texture name
	glGenTextures(1, &texture);

	// select our current texture
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, 3, 500, 500, 0, GL_RGB, GL_UNSIGNED_BYTE, pcData);

	glBegin(GL_QUADS);
	glTexCoord2f(-1.0f, -1.0f);
	glVertex2f(-1.0f, -1.0f);
	glTexCoord2f(1.0f, -1.0f);
	glVertex2f(1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(1.0f, 1.0f);
	glTexCoord2f(-1.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);
	glEnd();

	glDeleteTextures(1, &texture);

	glFlush();
}

void display(void)
{
	unsigned char *image = (unsigned char *)malloc(500 * 500 * 3);
	int i;

	for (i = 0; i<500 * 500 * 3; i += 3)
	{
		image[i] = 255;
		image[i + 1] = 0;
		image[i + 2] = 0;
	}

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	showImage(image, 500, 500);

	glFlush();

	free(image);
}

void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (w <= h)
	{
		glOrtho(-1.5, 1.5, -1.5*(GLfloat)h / (GLfloat)w, 1.5*(GLfloat)h / (GLfloat)w, -10.0, 10.0);
	}
	else
	{
		glOrtho(-1.5*(GLfloat)w / (GLfloat)h, 1.5*(GLfloat)w / (GLfloat)h, -1.5, 1.5, -10.0, 10.0);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, char** argv)
{
	// Por inicialização aqui
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Programa-22");

	// Por registo de funções aqui
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	// Entrar no ciclo do GLUT aqui
	glutMainLoop();

	return 0;
}

/*************************************FIM***************************************/
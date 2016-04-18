#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE

#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include "glm.h"
#include <stdio.h>
#include <windows.h>

GLMmodel* pmodel = NULL;


void loadmodel(void)
{
	if (pmodel == NULL)
	{
		pmodel = glmReadOBJ("models/f-16.obj");
		if (pmodel == NULL) { exit(0); }
		else
		{
			glmUnitize(pmodel);
			glmLinearTexture(pmodel);
			glmScale(pmodel, 1.0);
			glmFacetNormals(pmodel);
			glmVertexNormals(pmodel, 90.0);
		}
	}
}


void changeSize(int w, int h)
{
	float ratio;

	if (h == 0) h = 1;
	ratio = 1.0f * w / h;

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, ratio, 1, 1000);

	glMatrixMode(GL_MODELVIEW);
}


void renderScene(void)
{
	glClearColor(0.8, 0.8, 0.8, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(2.0, 2.0, 2.0,
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.0);

	// Renderização do modelo 3D
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);

	glutSwapBuffers();
}


int main(int argc, char **argv)
{
	// Por inicialização aqui
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(680, 480);
	glutCreateWindow("OpenGL Lighting");

	// Por registo de funções aqui
	glutDisplayFunc(renderScene);
	//glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);

	// Alguns settings para OpenGL
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Carrega o modelo
	loadmodel();

	// Entrar no ciclo do GLUT aqui
	glutMainLoop();

	return 0;
}
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"

void RenderScene(void)
{
	int i, widht, height;
	char str[] = "isto e um teste";
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLineWidth(2.0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);

	widht = glutGet(GLUT_WINDOW_WIDTH);
	height = glutGet(GLUT_WINDOW_HEIGHT);
	glViewport(0, 0, widht, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (double)widht, 0.0, (double)height);

	glTranslatef(0.0f, (float)height, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0, (float)-glutStrokeWidth(GLUT_STROKE_ROMAN, str[0]), 0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glScalef(0.125, 0.125, 0.125);
	for (i = 0; i < strlen(str); i++)glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	glFlush();
}


int main(int argc, char **argv)
{
	// Por inicialização aqui
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(400, 480);
	glutInitWindowSize(320, 320);
	glutCreateWindow("Stroke Character");

	glutDisplayFunc(RenderScene);

	glutMainLoop();
	return 0;
}
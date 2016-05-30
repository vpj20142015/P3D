#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE

#include "Dependencies\freeglut\freeglut.h"
#include <stdio.h>
#include <windows.h>
#include "tga.h"

// Protótipos de funções
void init(void);
void initDL(void);
void pressNormalKeys(unsigned char key, int x, int y);
void display(void);
void reshape(GLsizei w, GLsizei h);
void funcmyDL(void);
void load_cube_images(void);


// Variáveis globais
tgaInfo *im[6];
GLuint texture[6];
int myDL;
float cubeyangle = 0.0;


void init(void)
{
	// Define técnica de shading: GL_FLAT, GL_SMOOTH
	glShadeModel(GL_SMOOTH);

	// Activa o teste de profundidade
	glEnable(GL_DEPTH_TEST);

	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void applylights(void)
{
	// Define a posição de light0
	GLfloat light0_position[] = { 0.0f, 3.0f, 0.0f, 1.0f };
	// Define a posição de direcção de light1
	GLfloat spot_position[] = { 0.0f, 3.0f, -10.0f, 1.0f };
	GLfloat spot_direction[] = { 0.0f, -1.0f, 0.0f };

	// Aplica a light0
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

	// Aplica a light1
	glLightfv(GL_LIGHT1, GL_POSITION, spot_position);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_direction);

	glDisable(GL_LIGHTING);


	//// Desenha uma esfera que sinaliza a posição da light1
	//glPushMatrix();
	//glColor3f(1.0, 1.0, 1.0);
	//glTranslatef(0.0f, 3.0f, -10.0f);
	//glutSolidSphere(0.1, 20, 20);
	//glPopMatrix();

	glEnable(GL_LIGHTING);
}


void initDL(void)
{
	// Compila o modelo
	funcmyDL();
}


void pressNormalKeys(unsigned char key, int x, int y)
{
	if (key == 27) // ESC
	{
		exit(0);
	}
}


void display(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	gluLookAt(1.0f, 1.0f, 5.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);

	glRotatef(cubeyangle, 1.0f, 1.0f, 0.0f);
	cubeyangle += 0.5f;
	if (cubeyangle > 360.0) cubeyangle -= 360.0;

	applylights();

	glEnable(GL_TEXTURE_2D);
	// Cubo
	glCallList(myDL);
	glDisable(GL_TEXTURE_2D);

	glutSwapBuffers();

	glFlush();
}

void initLights(void)
{
	// Define a luz ambiente global
	GLfloat global_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	// Define a luz light0. Existem 8 fontes de luz no total.
	GLfloat light0_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat light0_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat light0_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	// Define a luz light1. Existem 8 fontes de luz no total.
	GLfloat light1_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat light1_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat light1_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat spot_angle = 45.0f;
	GLfloat spot_exp = 12.0f; // Maior valor = maior concentração de luz no centro

	// Fonte de luz ambiente
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

	// Fonte de luz posicional
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.1);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.05);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.05);

	// Fonte de luz cónica
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, spot_angle);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, spot_exp);

	// Activa a utilização de iluminação
	glEnable(GL_LIGHTING);
	// Activa a fonte de luz light0
	glEnable(GL_LIGHT0);
	// Activa a fonte de luz light1
	glEnable(GL_LIGHT1);
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
	myDL = glGenLists(1);

	glNewList(myDL, GL_COMPILE);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	// Z+ Face
	// Selecciona textura
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
	glEnd();
	// Z- Face
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
	glEnd();
	// X+ Face
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glBegin(GL_QUADS);
	glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
	glEnd();
	// X- Face
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glBegin(GL_QUADS);
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
	glEnd();
	// Y+ Face
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
	glEnd();
	// Y- Face
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glEnd();

	glEndList();
}


void load_cube_images(void)
{
	char *impathfile[6] = { "cm_front.tga", "cm_back.tga", "cm_right.tga", "cm_left.tga", "cm_top.tga", "cm_bottom.tga" };
	int i;

	// Carrega as imagens de textura
	for (i = 0; i<6; i++)
	{
		im[i] = tgaLoad(impathfile[i]);

		printf("IMAGE INFO: %s\nstatus: %d\ntype: %d\npixelDepth: %d\nsize%d x %d\n", impathfile[i], im[i]->status, im[i]->type, im[i]->pixelDepth, im[i]->width, im[i]->height); fflush(stdout);
	}

	// Cria nomes de texturas
	glGenTextures(6, texture);

	for (i = 0; i<6; i++)
	{
		// Selecciona uma textura
		glBindTexture(GL_TEXTURE_2D, texture[i]);

		// Selecciona a mistura da textura com a cor
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		// Cria textura de mipmaps
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, im[i]->width, im[i]->height, GL_RGB, GL_UNSIGNED_BYTE, im[i]->imageData);
		// Se não tem mipmaps
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, im[i]->width, im[i]->height, 0, GL_RGB, GL_UNSIGNED_BYTE, im[i]->imageData);
	}

	// Destroi as imagens
	for (i = 0; i<6; i++) tgaDestroy(im[i]);
}


int main(int argc, char** argv)
{
	// Inicialização do GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);

	// Cria a janela de visualização
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Programa-23");

	// Inicializações
	init();
	initLights();
	load_cube_images();
	initDL();

	// Registar funções de callback
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(display);
	glutKeyboardFunc(pressNormalKeys);

	// Ciclo infinito do GLUT
	glutMainLoop();

	return 0;
}

/*************************************FIM***************************************/
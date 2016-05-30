#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE

#ifndef GL_MULTISAMPLE_ARB
#define GL_MULTISAMPLE_ARB  0x809D
#endif

#include "Dependencies\freeglut\freeglut.h"
#include <stdio.h>
#include <windows.h>
#include "tga.h"

/* In case your <GL/gl.h> does not advertise EXT_texture_cube_map... */
#ifndef GL_EXT_texture_cube_map
# define GL_NORMAL_MAP_EXT                   0x8511
# define GL_REFLECTION_MAP_EXT               0x8512
# define GL_TEXTURE_CUBE_MAP_EXT             0x8513
# define GL_TEXTURE_BINDING_CUBE_MAP_EXT     0x8514
# define GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT  0x8515
# define GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT  0x8516
# define GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT  0x8517
# define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT  0x8518
# define GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT  0x8519
# define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT  0x851A
# define GL_PROXY_TEXTURE_CUBE_MAP_EXT       0x851B
# define GL_MAX_CUBE_MAP_TEXTURE_SIZE_EXT    0x851C
#endif


static GLenum faceTarget[6] = {
	GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT
};


// Protótipos de funções
void init(void);
void initDL(void);
void pressNormalKeys(unsigned char key, int x, int y);
void display(void);
void reshape(GLsizei w, GLsizei h);
void funcmyDL(void);
void load_bkg_image(void);
void load_cube_map_images(void);


// Variáveis globais
tgaInfo *imbkg;
tgaInfo *im[6];
GLuint texturebkg;
GLuint texture1, texture2;
int myDL;
float cubeyangle = 0.0;


void init(void)
{
	// Define técnica de shading: GL_FLAT, GL_SMOOTH
	glShadeModel(GL_SMOOTH);

	// Activa o teste de profundidade
	glEnable(GL_MULTISAMPLE_ARB);


	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

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

	gluLookAt(0.0f, 0.0f, 5.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	// Background
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texturebkg);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-7.8f, -6.0f, -5.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(7.8f, -6.0f, -5.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(7.8f, 6.0f, -5.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-7.8f, 6.0f, -5.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glPushMatrix();
	glTranslatef(-1.5f, 0.0f, 0.0f);
	glRotatef(cubeyangle, 1.0f, 1.0f, 0.0f);
	// Cube
	glBindTexture(GL_TEXTURE_CUBE_MAP_EXT, texture1);
	glEnable(GL_TEXTURE_CUBE_MAP_EXT);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glCallList(myDL);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_CUBE_MAP_EXT);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1.5f, 0.0f, 0.0f);
	glRotatef(cubeyangle + 90.0, 1.0f, 1.0f, 0.0f);
	// Cube
	glBindTexture(GL_TEXTURE_CUBE_MAP_EXT, texture2);
	glEnable(GL_TEXTURE_CUBE_MAP_EXT);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glCallList(myDL);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_CUBE_MAP_EXT);
	glPopMatrix();

	// Actualiza rotação dos objectos
	cubeyangle += 0.05f;
	if (cubeyangle > 360.0) cubeyangle -= 360.0;

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
	myDL = glGenLists(1);

	glNewList(myDL, GL_COMPILE);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	//glutSolidCube(1.0);
	//glutSolidSphere(1.0, 100, 100);
	glutSolidTorus(0.5, 1.0, 100, 100);
	//glutSolidTeapot(1.0);

	glEndList();
}


void load_bkg_image(void)
{
	char *impathfile = "back.tga";

	// Carrega a imagem de textura
	imbkg = tgaLoad(impathfile);

	// allocate a texture names
	glGenTextures(1, &texturebkg);

	// select our current texture
	glBindTexture(GL_TEXTURE_2D, texturebkg);

	// select modulate to mix texture with color for shading
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// build our texture mipmaps
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, imbkg->width, imbkg->height, GL_RGB, GL_UNSIGNED_BYTE, imbkg->imageData);

	// Destroi a imagem
	tgaDestroy(imbkg);
}


void load_cube_map_images(void)
{
	char *impathfile[6] = { "xpos.tga", "xneg.tga", "ypos.tga", "yneg.tga", "zpos.tga", "zneg.tga" };
	int i;

	// Carrega as imagens de textura
	for (i = 0; i<6; i++)
	{
		im[i] = tgaLoad(impathfile[i]);

		printf("IMAGE INFO: %s\nstatus: %d\ntype: %d\npixelDepth: %d\nsize%d x %d\n", impathfile[i], im[i]->status, im[i]->type, im[i]->pixelDepth, im[i]->width, im[i]->height); fflush(stdout);
	}

	// allocate a texture name
	glGenTextures(1, &texture1);

	// select our current texture
	glBindTexture(GL_TEXTURE_CUBE_MAP_EXT, texture1);

	// Carrega as imagens para as várias faces da textura
	for (i = 0; i<6; i++)
	{
		glTexImage2D(faceTarget[i], 0, GL_RGBA, im[i]->width, im[i]->height, 0, GL_RGB, GL_UNSIGNED_BYTE, im[i]->imageData);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Destroi as imagens
	for (i = 0; i<6; i++) tgaDestroy(im[i]);
}


void load_cube_map2_images(void)
{
	char *impathfile[6] = { "cm_front.tga", "cm_back.tga", "cm_right.tga", "cm_left.tga", "cm_top.tga", "cm_bottom.tga" };
	int i;

	// Carrega as imagens de textura
	for (i = 0; i<6; i++)
	{
		im[i] = tgaLoad(impathfile[i]);

		printf("IMAGE INFO: %s\nstatus: %d\ntype: %d\npixelDepth: %d\nsize%d x %d\n", impathfile[i], im[i]->status, im[i]->type, im[i]->pixelDepth, im[i]->width, im[i]->height); fflush(stdout);
	}

	// allocate a texture name
	glGenTextures(1, &texture2);

	// select our current texture
	glBindTexture(GL_TEXTURE_CUBE_MAP_EXT, texture2);

	// Carrega as imagens para as várias faces da textura
	for (i = 0; i<6; i++)
	{
		glTexImage2D(faceTarget[i], 0, GL_RGBA, im[i]->width, im[i]->height, 0, GL_RGB, GL_UNSIGNED_BYTE, im[i]->imageData);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Destroi as imagens
	for (i = 0; i<6; i++) tgaDestroy(im[i]);
}


int main(int argc, char** argv)
{
	// Inicialização do GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);

	// Cria a janela de visualização
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Programa-24");

	// Inicializações
	init();
	load_bkg_image();
	load_cube_map_images();
	load_cube_map2_images();
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
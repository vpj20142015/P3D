#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE

//Com Display Lists.
//Execute o programa com _DMESH definido
//Execute o programa sem a definição de _DMESH
//Altere o shade model entre GL_SMOOTH e GL_FLAT
//Altere o polygon mode entre GL_FILL e GL_LINE

#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include "glm.h"
#include <stdio.h>
#include <math.h>
#include <windows.h>

//#define _DMESH
#define MAX(a, b) (a > b ? a : b)


struct
{
	int w, h;
	int posx, posy;
} OGLWINDOW;

struct
{
	int frame, time, timebase;
	char stringfps[255];
} OGLFRAME;

struct
{
	float angle, anglev; // Ângulo relativo ao eixo dos Z's, no plano ZX
	float deltaAngle, deltaAnglev, deltaMove;
	float x, y, z; // Posição da câmara (centro-de-massa)
	float lx, ly, lz; // Vector do sentido de deslocamento da câmara
	float upx, upy, upz;
	float deltaSpeed;
	float speed; // Constante de velocidade de deslocamento
} OGLCAMERA;

int OGLFONT = (int)GLUT_BITMAP_8_BY_13;


// Protótipos de funções
void display(void);
void reshape(GLsizei w, GLsizei h);
void pressNormalKeys(unsigned char key, int x, int y);
void releaseNormalKeys(unsigned char key, int x, int y);
void pressSpecialKey(int key, int x, int y);
void releaseSpecialKey(int key, int x, int y);
void getframerate(void);


int myDL;
void funcmyDL(void);


void init(void)
{
	// Define técnica de shading: GL_FLAT, GL_SMOOTH
	glShadeModel(GL_FLAT);

	glPolygonMode(GL_FRONT, GL_LINE); // GL_LINE, GL_POINT, GL_FILL

	// Activa o teste de profundidade
	glEnable(GL_DEPTH_TEST);

	glClearDepth(0.0);

	glDepthFunc(GL_LEQUAL);

	// Compila o modelo
	funcmyDL();
}


void initCamera(void)
{
	OGLCAMERA.angle = 0.0f;
	OGLCAMERA.anglev = 0.0f;
	OGLCAMERA.deltaAngle = 0.0f;
	OGLCAMERA.deltaAnglev = 0.0f;
	OGLCAMERA.deltaMove = 0.0f;
	OGLCAMERA.x = 0.0f;
	OGLCAMERA.y = 1.75f;
	OGLCAMERA.z = 5.0f;
	OGLCAMERA.lx = 0.0f;
	OGLCAMERA.ly = 0.0f;
	OGLCAMERA.lz = -1.0f;
	OGLCAMERA.upx = 0.0f;
	OGLCAMERA.upy = 1.0f;
	OGLCAMERA.upz = 0.0f;
	OGLCAMERA.speed = 0.1f;
	OGLCAMERA.deltaSpeed = 0.0f;
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

	// Desenha uma esfera que sinaliza a posição da light0
	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);
	glTranslatef(0.0f, 3.0f, 0.0f);
	glutSolidSphere(0.1, 20, 20);
	glPopMatrix();

	// Desenha uma esfera que sinaliza a posição da light1
	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);
	glTranslatef(0.0f, 3.0f, -10.0f);
	glutSolidSphere(0.1, 20, 20);
	glPopMatrix();

	glEnable(GL_LIGHTING);
}


void applymaterial(int type)
{
	// Define as propriedades dos materiais
	// Type: 0 (Branco); 1 (Amarelo); 2 (Ciano); 3 (Branco-Emissor)
	GLfloat mat_ambient[4][4] = { { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } };
	GLfloat mat_diffuse[4][4] = { { 0.5f, 0.5f, 0.5f, 1.0f }, { 0.5f, 0.5f, 0.0f, 1.0f }, { 0.0f, 0.5f, 0.5f, 1.0f }, { 0.5f, 0.5f, 0.5f, 1.0f } };
	GLfloat mat_specular[4][4] = { { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } };
	GLfloat mat_emission[4][4] = { { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } };
	GLfloat mat_shininess[4][1] = { { 20.0f }, { 20.0f }, { 20.0f }, { 20.0f } };

	if ((type >= 0) && (type < 4))
	{
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient[type]); // GL_FRONT, GL_FRONT_AND_BACK , GL_BACK, 
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse[type]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular[type]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_emission[type]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess[type]);
	}
}


void pressNormalKeys(unsigned char key, int x, int y)
{
	if (key == 27) // ESC
	{
		exit(0);
	}
	else if (key == '1')
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
	else if (key == '2')
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else if (key == '3')
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else if ((key == 'A') || (key == 'a'))
	{
		OGLCAMERA.deltaAnglev = 0.02f;
	}
	else if ((key == 'Z') || (key == 'z'))
	{
		OGLCAMERA.deltaAnglev = -0.02f;
	}
	else if ((key == 'S') || (key == 's'))
	{
		OGLCAMERA.deltaSpeed = 0.01f;
	}
	else if ((key == 'X') || (key == 'x'))
	{
		OGLCAMERA.deltaSpeed = -0.01f;
	}
	else if ((key == 'N') || (key == 'n'))
	{
		glutLeaveGameMode();

		OGLWINDOW.w = 640;
		OGLWINDOW.h = 480;
		OGLWINDOW.posx = 0;
		OGLWINDOW.posy = 0;

		glutInitWindowPosition(OGLWINDOW.posx, OGLWINDOW.posy);
		glutInitWindowSize(OGLWINDOW.w, OGLWINDOW.w);
		glutCreateWindow("Programa-14");

		// Inicializações
		init();
		initLights();

		// Registar funções de callback
		glutDisplayFunc(display);
		glutReshapeFunc(reshape);
		glutIdleFunc(display);
		glutIgnoreKeyRepeat(1);
		glutKeyboardFunc(pressNormalKeys);
		glutKeyboardUpFunc(releaseNormalKeys);
		glutSpecialFunc(pressSpecialKey);
		glutSpecialUpFunc(releaseSpecialKey);
	}
	else if ((key == 'F') || (key == 'f'))
	{
		glutGameModeString("1366x768:32");

		if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE))
		{
			OGLWINDOW.w = 1366;
			OGLWINDOW.h = 1024;
			OGLWINDOW.posx = 0;
			OGLWINDOW.posy = 0;

			glutEnterGameMode();
		}

		// Inicializações
		init();
		initLights();

		// Registar funções de callback
		glutDisplayFunc(display);
		glutReshapeFunc(reshape);
		glutIdleFunc(display);
		glutIgnoreKeyRepeat(1);
		glutKeyboardFunc(pressNormalKeys);
		glutKeyboardUpFunc(releaseNormalKeys);
		glutSpecialFunc(pressSpecialKey);
		glutSpecialUpFunc(releaseSpecialKey);
	}
}


void releaseNormalKeys(unsigned char key, int x, int y)
{
	if ((key == 'A') || (key == 'a'))
	{
		if (OGLCAMERA.deltaAnglev > 0.0f) OGLCAMERA.deltaAnglev = 0.0f;
	}
	else if ((key == 'Z') || (key == 'z'))
	{
		if (OGLCAMERA.deltaAnglev < 0.0f) OGLCAMERA.deltaAnglev = 0.0f;
	}
	if ((key == 'S') || (key == 's'))
	{
		if (OGLCAMERA.deltaSpeed > 0.0f) OGLCAMERA.deltaSpeed = 0.0f;
	}
	else if ((key == 'X') || (key == 'x'))
	{
		if (OGLCAMERA.deltaSpeed < 0.0f) OGLCAMERA.deltaSpeed = 0.0f;
	}
}


void pressSpecialKey(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		OGLCAMERA.deltaAngle = -0.02f;
		break;
	case GLUT_KEY_RIGHT:
		OGLCAMERA.deltaAngle = 0.02f;
		break;
	case GLUT_KEY_UP:
		OGLCAMERA.deltaMove = 1;
		break;
	case GLUT_KEY_DOWN:
		OGLCAMERA.deltaMove = -1;
		break;
	default:
		break;
	}
}


void releaseSpecialKey(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		if (OGLCAMERA.deltaAngle < 0.0f) OGLCAMERA.deltaAngle = 0.0f;
		break;
	case GLUT_KEY_RIGHT:
		if (OGLCAMERA.deltaAngle > 0.0f) OGLCAMERA.deltaAngle = 0.0f;
		break;
	case GLUT_KEY_UP:
		if (OGLCAMERA.deltaMove > 0.0f) OGLCAMERA.deltaMove = 0.0f;
		break;
	case GLUT_KEY_DOWN:
		if (OGLCAMERA.deltaMove < 0.0f) OGLCAMERA.deltaMove = 0.0f;
		break;
	default:
		break;
	}
}


void updateCameraPosition(void)
{
	if (OGLCAMERA.deltaSpeed)
	{
		OGLCAMERA.speed += OGLCAMERA.deltaSpeed;
		if (OGLCAMERA.speed < 0.0f) OGLCAMERA.speed = 0.0f;
	}

	if (OGLCAMERA.deltaMove)
	{
		// x = x0 + (Amplitude * Direcção) * Velocidade
		OGLCAMERA.x = OGLCAMERA.x + OGLCAMERA.deltaMove * (OGLCAMERA.lx) * OGLCAMERA.speed;
		OGLCAMERA.y = OGLCAMERA.y + OGLCAMERA.deltaMove * (OGLCAMERA.ly) * OGLCAMERA.speed;
		OGLCAMERA.z = OGLCAMERA.z + OGLCAMERA.deltaMove * (OGLCAMERA.lz) * OGLCAMERA.speed;
	}

	if (OGLCAMERA.deltaAngle || OGLCAMERA.deltaAnglev)
	{
		OGLCAMERA.angle += OGLCAMERA.deltaAngle;
		OGLCAMERA.anglev += OGLCAMERA.deltaAnglev;
		OGLCAMERA.lx = sin(OGLCAMERA.angle) * cos(OGLCAMERA.anglev);
		OGLCAMERA.ly = sin(OGLCAMERA.anglev);
		OGLCAMERA.lz = -cos(OGLCAMERA.angle) * cos(OGLCAMERA.anglev);
	}

	if (cos(OGLCAMERA.anglev) < 0.0f)
	{
		OGLCAMERA.upy = -1.0f;
	}
	else
	{
		OGLCAMERA.upy = 1.0f;
	}
}


void renderBitmapString(float x, float y, void *font, char *string)
{
	char *c;

	glDisable(GL_LIGHTING);

	glColor3f(1.0f, 0.0f, 0.0f);

	// set position to start drawing fonts
	glRasterPos2f(x, y);

	// loop all the characters in the string
	for (c = string; *c != '\0'; c++)
	{
		glutBitmapCharacter(font, *c);
	}

	glEnable(GL_LIGHTING);
}


void setOrthographicProjection(void)
{
	// switch to projection mode
	glMatrixMode(GL_PROJECTION);
	// save previous matrix which contains the 
	//settings for the perspective projection
	glPushMatrix();
	// reset matrix
	glLoadIdentity();
	// set a 2D orthographic projection (invert the y axis, down is positive)
	gluOrtho2D(0, OGLWINDOW.w, OGLWINDOW.h / 2, 0);
	glMatrixMode(GL_MODELVIEW);
}


void resetPerspectiveProjection(void)
{
	// set the current matrix to GL_PROJECTION
	glMatrixMode(GL_PROJECTION);
	// restore previous settings
	glPopMatrix();
	// get back to GL_MODELVIEW matrix
	glMatrixMode(GL_MODELVIEW);
}


void draw_mesh(int xmin, int xmax, int zmin, int zmax)
{
	int x, z;
	int xstep, zstep;
	float xtmp, ztmp;

	xstep = 5;
	zstep = 5;

	glBegin(GL_TRIANGLES);
	for (x = 0; x<xstep; x++)
	{
		xtmp = (float)(xmax - xmin) / (float)xstep;

		for (z = 0; z<zstep; z++)
		{
			ztmp = (float)(zmax - zmin) / (float)zstep;

			glNormal3f(0.0f, 1.0f, 0.0f);
			glVertex3f(xmin + (float)x*(float)xtmp, 0.0f, zmin + (float)z*(float)ztmp);
			glVertex3f(xmin + (float)(x + 1)*(float)xtmp, 0.0f, zmin + (float)z*(float)ztmp);
			glVertex3f(xmin + (float)x*(float)xtmp, 0.0f, zmin + (float)(z + 1)*(float)ztmp);

			glNormal3f(0.0f, 1.0f, 0.0f);
			glVertex3f(xmin + (float)(x + 1)*(float)xtmp, 0.0f, zmin + (float)z*(float)ztmp);
			glVertex3f(xmin + (float)(x + 1)*(float)xtmp, 0.0f, zmin + (float)(z + 1)*(float)ztmp);
			glVertex3f(xmin + (float)x*(float)xtmp, 0.0f, zmin + (float)(z + 1)*(float)ztmp);
		}
	}
	glEnd();
}


void display(void)
{
	char infotext[255];

	updateCameraPosition();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	gluLookAt(OGLCAMERA.x, OGLCAMERA.y, OGLCAMERA.z,
		OGLCAMERA.x + OGLCAMERA.lx, OGLCAMERA.y + OGLCAMERA.ly, OGLCAMERA.z + OGLCAMERA.lz,
		OGLCAMERA.upx, OGLCAMERA.upy, OGLCAMERA.upz);

	applylights();

	// Floor
	glCallList(myDL);

	setOrthographicProjection();
	glPushMatrix();
	glLoadIdentity();
	sprintf(infotext, "X:%.1f Y:%.1f Z:%.1f", OGLCAMERA.x, OGLCAMERA.y, OGLCAMERA.z);
	renderBitmapString(30, 15, (void *)OGLFONT, infotext);
	sprintf(infotext, "Angle:%.2f Anglev:%.2f", OGLCAMERA.angle, OGLCAMERA.anglev);
	renderBitmapString(30, 25, (void *)OGLFONT, infotext);
	sprintf(infotext, "lx:%.2f ly:%.2f lz:%.2f", OGLCAMERA.lx, OGLCAMERA.ly, OGLCAMERA.lz);
	renderBitmapString(30, 35, (void *)OGLFONT, infotext);
	sprintf(infotext, "SPEED:%.2f", OGLCAMERA.speed);
	renderBitmapString(30, 45, (void *)OGLFONT, infotext);
	glPopMatrix();
	resetPerspectiveProjection();

	setOrthographicProjection();
	glPushMatrix();
	glLoadIdentity();
	getframerate();
	renderBitmapString(MAX(OGLWINDOW.w - 100, 0), 15, (void *)OGLFONT, OGLFRAME.stringfps);
	glPopMatrix();
	resetPerspectiveProjection();

	glutSwapBuffers();

	glFlush();
}


void reshape(GLsizei w, GLsizei h)
{
	if (h == 0) h = 1;

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 0.5, 200.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -5.0);
}


void getframerate(void)
{
	OGLFRAME.frame++;

	OGLFRAME.time = glutGet(GLUT_ELAPSED_TIME);

	if ((OGLFRAME.time - OGLFRAME.timebase) > 1000)
	{
		sprintf(OGLFRAME.stringfps, "FPS: %4.2f", OGLFRAME.frame * 1000.0 / (OGLFRAME.time - OGLFRAME.timebase));
		OGLFRAME.timebase = OGLFRAME.time;
		OGLFRAME.frame = 0;
	}
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
			applymaterial(0);
#ifdef _DMESH
			draw_mesh(x, x + 1.0f, z, z - 1.0f);
#else
			glBegin(GL_QUADS);
			glNormal3f(0.0, 1.0, 0.0);
			glVertex3f(x, 0.0f, z);				// Top Left
			glVertex3f(x + 1.0f, 0.0f, z);		// Top Right
			glVertex3f(x + 1.0f, 0.0f, z - 1.0f);	// Bottom Right
			glVertex3f(x, 0.0f, z - 1.0f);		// Bottom Left
			glEnd();
#endif
		}
		for (z = -99; z <= 100; z += 2)
		{
			applymaterial(1);
#ifdef _DMESH
			draw_mesh(x, x + 1.0f, z, z - 1.0f);
#else
			glBegin(GL_QUADS);
			glNormal3f(0.0f, 1.0f, 0.0f);
			glVertex3f(x, 0.0f, z);				// Top Left
			glVertex3f(x + 1.0f, 0.0f, z);		// Top Right
			glVertex3f(x + 1.0f, 0.0f, z - 1.0f);	// Bottom Right
			glVertex3f(x, 0.0f, z - 1.0f);		// Bottom Left
			glEnd();
#endif
		}
	}
	for (x = -99; x <= 100; x += 2)
	{
		for (z = -99; z <= 100; z += 2)
		{
			applymaterial(0);
#ifdef _DMESH
			draw_mesh(x, x + 1.0f, z, z - 1.0f);
#else
			glBegin(GL_QUADS);
			glNormal3f(0.0, 1.0, 0.0);
			glVertex3f(x, 0.0f, z);				// Top Left
			glVertex3f(x + 1.0f, 0.0f, z);		// Top Right
			glVertex3f(x + 1.0f, 0.0f, z - 1.0f);	// Bottom Right
			glVertex3f(x, 0.0f, z - 1.0f);		// Bottom Left
			glEnd();
#endif
		}
		for (z = -100; z <= 100; z += 2)
		{
			applymaterial(2);
#ifdef _DMESH
			draw_mesh(x, x + 1.0f, z, z - 1.0f);
#else
			glBegin(GL_QUADS);
			glNormal3f(0.0f, 1.0f, 0.0f);
			glVertex3f(x, 0.0f, z);				// Top Left
			glVertex3f(x + 1.0f, 0.0f, z);		// Top Right
			glVertex3f(x + 1.0f, 0.0f, z - 1.0f);	// Bottom Right
			glVertex3f(x, 0.0f, z - 1.0f);		// Bottom Left
			glEnd();
#endif
		}
	}

	glBegin(GL_QUADS);
	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-100.0f, 20.0f, -100.0f);				// Top Left
	glVertex3f(100.0f, 20.0f, -100.0f);		// Top Right
	glVertex3f(100.0f, 0.0f, -100.0f);	// Bottom Right
	glVertex3f(-100.0f, 0.0f, -100.0f);		// Bottom Left
	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-100.0f, 20.0f, 100.0f);				// Top Left
	glVertex3f(100.0f, 20.0f, 100.0f);		// Top Right
	glVertex3f(100.0f, 0.0f, 100.0f);	// Bottom Right
	glVertex3f(-100.0f, 0.0f, 100.0f);		// Bottom Left
	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(100.0f, 20.0f, -100.0f);				// Top Left
	glVertex3f(100.0f, 20.0f, 100.0f);		// Top Right
	glVertex3f(100.0f, 0.0f, 100.0f);	// Bottom Right
	glVertex3f(100.0f, 0.0f, -100.0f);		// Bottom Left
	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-100.0f, 20.0f, -100.0f);				// Top Left
	glVertex3f(-100.0f, 20.0f, 100.0f);		// Top Right
	glVertex3f(-100.0f, 0.0f, 100.0f);	// Bottom Right
	glVertex3f(-100.0f, 0.0f, -100.0f);		// Bottom Left
	glEnd();

	

	glEndList();
}


int main(int argc, char** argv)
{
	// Inicialização do GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);

	glutGameModeString("1366x768:32@60");
	if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE))
	{
		OGLWINDOW.w = 1366;
		OGLWINDOW.h = 1024;
		OGLWINDOW.posx = 0;
		OGLWINDOW.posy = 0;

		glutEnterGameMode();
	}
	else  // Cria Janela Normal
	{
		OGLWINDOW.w = 640;
		OGLWINDOW.h = 480;
		OGLWINDOW.posx = 0;
		OGLWINDOW.posy = 0;

		glutInitWindowPosition(OGLWINDOW.posx, OGLWINDOW.posy);
		glutInitWindowSize(OGLWINDOW.w, OGLWINDOW.w);
		glutCreateWindow("Programa-15");
	}

	// Inicializações
	init();
	initCamera();
	initLights();

	// Registar funções de callback
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(display);
	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(pressNormalKeys);
	glutKeyboardUpFunc(releaseNormalKeys);
	glutSpecialFunc(pressSpecialKey);
	glutSpecialUpFunc(releaseSpecialKey);

	// Ciclo infinito do GLUT
	glutMainLoop();

	return 0;
}
#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/video/video.hpp>
#include <opencv2/video/background_segm.hpp>
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include <math.h>
#include "tga.h"

#define _DMESH

using namespace cv;
using namespace std;

Mat frameOriginal, frameHSV, frameFiltered, frameFlipped, fgMaskMOG, controlFlipped, tempimage, tempimage2;

Ptr<BackgroundSubtractor> pMOG; //MOG Background subtractor

int iLowH = 0;
int iHighH = 179;

int iLowS = 133;
int iHighS = 250;

int iLowV = 180;
int iHighV = 255;

bool frameCapturedSuccessfully = false;
VideoCapture cap(CV_CAP_ANY); //capture the video from web cam
int width = 640;
int height = 480;

Point circleCenter = Point(0, 0);
int circleRadius = 2;

int myDL;

//Modos existentes: position tracking e augmented reality
int demoModes = 2;
//Modo current
int demoMode = 1;

tgaInfo *im;
GLuint textureEarth, textureMoon;
GLUquadric *mysolid;
GLfloat spin = 0.05;
float planetCenterX = 0, planetCenterY = 0;
float raioOrbita = 0.1;
float periodoOrbital = 1.0;
float moonOrbitIterator = 0;
GLuint textures[2];

//Usado para implementar um rolling moving average de modo a limpar o sinal 
float newValuesWeight = 1.0;
float accumulatorX = 0, accumulatorY = 0, accumulatorZ = 0;

void load_tga_image(std::string nome, GLuint texture, bool transparency)
{
	std::string impathfile = "textures/" + nome + ".tga";

	std::vector<char> writable(impathfile.begin(), impathfile.end());
	writable.push_back('\0');

	// Carrega a imagem de textura
	im = tgaLoad(&writable[0]);
	//printf("IMAGE INFO: %s\nstatus: %d\ntype: %d\npixelDepth: %d\nsize%d x %d\n", impathfile, im->status, im->type, im->pixelDepth, im->width, im->height);

	// Seleciona a textura atual
	glBindTexture(GL_TEXTURE_2D, texture);

	// set up quadric object and turn on FILL draw style for it
	mysolid = gluNewQuadric();
	gluQuadricDrawStyle(mysolid, GLU_FILL);

	// turn on texture coordinate generator for the quadric
	gluQuadricTexture(mysolid, GL_TRUE);

	// select modulate to mix texture with color for shading
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // MIPMAP
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// build our texture mipmaps
	if (!transparency){
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, im->width, im->height, GL_RGB, GL_UNSIGNED_BYTE, im->imageData); // MIPMAP
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, im->width, im->height, 0, GL_RGB, GL_UNSIGNED_BYTE, im->imageData);
	}
	else{
		//Textura com transpar�ncia (an�is de saturno)
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, im->width, im->height, GL_RGB, GL_UNSIGNED_BYTE, im->imageData); // MIPMAP
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, im->width, im->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, im->imageData);
	}


	// Destroi a imagem
	tgaDestroy(im);
}

void draw_mesh(int xmin, int xmax, int zmin, int zmax)
{
	int x, z;
	int xstep, zstep;
	float xtmp, ztmp;

	xstep = 5;
	zstep = 5;

	int y = -5;

	glBegin(GL_TRIANGLES);
	for (x = 0; x<xstep; x++)
	{
		xtmp = (float)(xmax - xmin) / (float)xstep;

		for (z = 0; z<zstep; z++)
		{
			ztmp = (float)(zmax - zmin) / (float)zstep;

			glNormal3f(0.0f, 1.0f, 0.0f);
			glVertex3f(xmin + (float)x*(float)xtmp, y, zmin + (float)z*(float)ztmp);
			glVertex3f(xmin + (float)(x + 1)*(float)xtmp, y, zmin + (float)z*(float)ztmp);
			glVertex3f(xmin + (float)x*(float)xtmp, y, zmin + (float)(z + 1)*(float)ztmp);

			glNormal3f(0.0f, 1.0f, 0.0f);
			glVertex3f(xmin + (float)(x + 1)*(float)xtmp, y, zmin + (float)z*(float)ztmp);
			glVertex3f(xmin + (float)(x + 1)*(float)xtmp, y, zmin + (float)(z + 1)*(float)ztmp);
			glVertex3f(xmin + (float)x*(float)xtmp, y, zmin + (float)(z + 1)*(float)ztmp);
		}
	}
	glEnd();
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

void floorAndWallsDL(void)
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

void initLights(void)
{
	// Define a luz ambiente global
	GLfloat global_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	// Define a luz light0. Existem 8 fontes de luz no total.
	GLfloat light0_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat light0_diffuse[] = { 0.6f, 0.6f, 0.6f, 1.0f };
	GLfloat light0_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	// Define a luz light1. Existem 8 fontes de luz no total.
	GLfloat light1_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat light1_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat light1_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat spot_angle = 45.0f;
	GLfloat spot_exp = 12.0f; // Maior valor = maior concentra��o de luz no centro

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

	// Fonte de luz c�nica
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, spot_angle);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, spot_exp);

	// Activa a utiliza��o de ilumina��o
	glEnable(GL_LIGHTING);
	// Activa a fonte de luz light0
	glEnable(GL_LIGHT0);
	// Activa a fonte de luz light1
	glEnable(GL_LIGHT1);
}

void applylights(void)
{
	// Define a posi��o de light0
	GLfloat light0_position[] = { 0.0f, 3.0f, 0.0f, 1.0f };
	// Define a posi��o de direc��o de light1
	GLfloat spot_position[] = { 0.0f, 3.0f, -5.0f, 1.0f };
	GLfloat spot_direction[] = { 0.0f, -1.0f, 0.0f };

	// Aplica a light0
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

	// Aplica a light1
	glLightfv(GL_LIGHT1, GL_POSITION, spot_position);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_direction);

	glEnable(GL_LIGHTING);
}

void init(void)
{

	glEnable(GL_CULL_FACE_MODE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	// Define t�cnica de shading: GL_FLAT, GL_SMOOTH
	glShadeModel(GL_SMOOTH);

	glPolygonMode(GL_FRONT, GL_FILL); // GL_LINE, GL_POINT, GL_FILL

	// Compila o modelo
	floorAndWallsDL();
}

void drawAxes(float length)
{
	glPushAttrib(GL_POLYGON_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);

	glDisable(GL_LIGHTING);

	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(length, 0, 0);

	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, length, 0);

	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, length);
	glEnd();

	glPopAttrib();

}

void HoughDetection(Mat& src_gray, Mat& src_display)
{

	int largest_area = 0;
	int largest_contour_index = 0;
	RNG rng(12345);
	Scalar color;

	vector<vector<Point>> contours; // Vector for storing contour
	vector<Vec4i> hierarchy;

	findContours(src_gray, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE); // Find the contours in the image

	if (contours.size() > 0){

		//Foram encontrados contornos

		//cout << contours.size() << endl;

		for (int i = 0; i < contours.size(); i++) // iterate through each contour. 
		{
			double a = contourArea(contours[i], false);  //  Find the area of contour
			if (a>largest_area){
				largest_area = a;
				largest_contour_index = i;                //Store the index of largest contour
			}

		}

		//cout << largest_contour_index << endl << endl;

		vector<vector<Point> > contours_poly(contours.size());
		vector<Point2f>center(contours.size());
		vector<float>radius(contours.size());

		approxPolyDP(Mat(contours[largest_contour_index]), contours_poly[largest_contour_index], 3, true);
		minEnclosingCircle((Mat)contours_poly[largest_contour_index], center[largest_contour_index], radius[largest_contour_index]);

		//Encontramos um circulo
		/*for (int i = 0; i< contours.size(); i++)
		{
		drawContours(src_display, contours_poly, i, Scalar(0, 0, 0), 1, 8, vector<Vec4i>(), 0, Point());
		}*/
		if ((int)radius[largest_contour_index] > 15)
		{
			/*circle(src_display, center[largest_contour_index], (int)radius[largest_contour_index], Scalar(255, 0, 0), 2, 8, 0);
			circle(src_display, center[largest_contour_index], 5, Scalar(255, 0, 0), -1);*/

			switch (demoMode)
			{
			case 0:{
				newValuesWeight = 0.4;
				//Positional Tracking
				accumulatorX = (newValuesWeight * center[largest_contour_index].x) + (1.0 - newValuesWeight) * accumulatorX;
				accumulatorY = (newValuesWeight * center[largest_contour_index].y) + (1.0 - newValuesWeight) * accumulatorY;
				accumulatorZ = (newValuesWeight * (int)radius[largest_contour_index]) + (1.0 - newValuesWeight) * accumulatorZ;
				circleCenter = Point(accumulatorX, accumulatorY);
				circleRadius = accumulatorZ;
				break;
			}
			case 1:{
				//Realidade aumentada, planeta por cima da bola
				//X e Y acompanham instantaneamente, limpamos o ruido do raio do planeta
				newValuesWeight = 1.0;
				accumulatorX = (newValuesWeight * center[largest_contour_index].x) + (1.0 - newValuesWeight) * accumulatorX;
				accumulatorY = (newValuesWeight * center[largest_contour_index].y) + (1.0 - newValuesWeight) * accumulatorY;
				circleCenter = Point(accumulatorX, accumulatorY);
				newValuesWeight = 0.8;
				accumulatorZ = (newValuesWeight * (int)radius[largest_contour_index]) + (1.0 - newValuesWeight) * accumulatorZ;
				circleRadius = accumulatorZ;
				break;
			}
			default:
				break;
			}

		}
	}

	///HoughDetection - Slow and unreliable

	//// will hold the results of the detection
	//std::vector<Vec3f> circles;
	//// runs the actual detection
	//HoughCircles(src_gray, circles, CV_HOUGH_GRADIENT, 1, src_gray.rows / 16, cannyThreshold, accumulatorThreshold, 5, 800);
	//for (size_t i = 0; i < circles.size(); i++)
	//{
	//	Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
	//	int radius = cvRound(circles[i][2]);
	//	// circle center
	//	circle(src_display, center, 3, Scalar(0, 255, 0), -1, 8, 0);
	//	// circle outline
	//	circle(src_display, center, radius, Scalar(0, 0, 255), 3, 4, 0);
	//}

	//Flip and show the control window
	//flip(src_gray, controlFlipped, 0);
	

}

void OrangeFilter(Mat& source, Mat& destination){
	inRange(source, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), destination); //Threshold the image

	//morphological opening (remove small objects from the foreground)
	erode(destination, destination, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	dilate(destination, destination, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

	//morphological closing (fill small holes in the foreground)
	dilate(destination, destination, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	erode(destination, destination, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

}

double round(double d)
{
	return floor(d + 0.5);
}

float RangeAToRangeB(float input, float input_start, float input_end, float output_start, float output_end, float divisor){
	double slope = 1.0 * (output_end - output_start) / (input_end - input_start);
	return (output_start + slope * (input - input_start)) / divisor;
}

void display()
{
	// clear the window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// read a new frame from video
	if (frameCapturedSuccessfully) //if not success, break loop
	{

		cvtColor(frameOriginal, frameHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

		//Filtrar para deixar apenas coisas avermelhadas
		OrangeFilter(frameHSV, frameFiltered);

		//Retirar o background
		pMOG->operator()(frameFiltered, fgMaskMOG);

		//Blur para suavizar as fronteiras
		GaussianBlur(fgMaskMOG, frameFiltered, Size(9, 9), 4, 4);

		flip(frameFiltered, controlFlipped, 1);
		imshow("Control", controlFlipped);

		//runs the detection, and update the display
		HoughDetection(frameFiltered, frameOriginal);

	}
	else{
		cout << "No frame captured!" << endl;
	}

	//////////////////////////////////////////////////////////////////////////////////
	// Here, set up new parameters to render a scene viewed from the camera.

	//set viewport
	//glViewport(0, 0, frameFlipped.size().width, frameFlipped.size().height);
	glViewport(0, 0, width, height);


	float zoomRange = 0;
	float x = 0, y = 0, z = 0;
	float raioOrbitaAtual = 0;
	switch (demoMode)
	{
	case 0:
		/////////////////////////////////////////////////////////////////////////////////
		// Drawing routine

		//now that the camera params have been set, draw your 3D shapes
		//first, save the current matrix

		glDisable(GL_TEXTURE_2D);

		//set projection matrix using intrinsic camera params
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		//FPV, zoom afeta FOV
		gluPerspective(60 - circleRadius / 5, width / height, 0.1, 100);

		//you will have to set modelview matrix using extrinsic camera params
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);

		glPushMatrix();

		zoomRange = RangeAToRangeB((float)circleRadius, 20, 150, 0, 10, 1);

		//move to the position where you want the 3D object to go
		glTranslatef(RangeAToRangeB((float)circleCenter.x, 0.0, (float)width, -width / 2.0, width / 2.0, 100.0),
			RangeAToRangeB((float)circleCenter.y, 0.0, (float)height, -height / 2.0, height / 2.0, 50.0),
			zoomRange);


		// Floor
		glCallList(myDL);

		drawAxes(1.0);

		applylights();

		glTranslatef(-2, 0, 0);

		glutSolidTeapot(0.5);

		glTranslatef(4, 0, 0);

		glutSolidTeacup(0.5);

		glPopMatrix();
		break;
	case 1:

		glClear(GL_COLOR_BUFFER_BIT);

		glDisable(GL_TEXTURE_2D);

		flip(frameOriginal, tempimage, 0);
		flip(tempimage, tempimage2, 1);
		glDrawPixels(tempimage2.size().width, tempimage2.size().height, GL_BGR, GL_UNSIGNED_BYTE, tempimage2.ptr());

		glClear(GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho((float)width / (float)height, (float)-width / (float)height, -1, 1, -100, 100);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glEnable(GL_TEXTURE_2D);

		glColor4f(1.0, 1.0, 1.0, 1.0);

		glBindTexture(GL_TEXTURE_2D, textures[0]);

		applymaterial(0);

		applylights();

		// Draw sphere
		glPushMatrix();

		planetCenterX = RangeAToRangeB((float)circleCenter.x, 0.0, (float)width, -width / 2.0, width / 2.0, width / 2.65);
		planetCenterY = -RangeAToRangeB((float)circleCenter.y, 0.0, (float)height, -height / 2.0, height / 2.0, height / 2.0);

		//move to the position where you want the 3D object to go
		glTranslatef(planetCenterX, planetCenterY, 0);
		glRotatef(-spin, 0.0, 1.0, 0.0);
		//Endireitar os planetas
		glRotatef(-90.0, 1.0, 0.0, 0.0);
		gluSphere(mysolid, circleRadius / (height / 2.0), 100, 100);

		glPopMatrix();
		glPushMatrix();

		glBindTexture(GL_TEXTURE_2D, textures[1]);

		applymaterial(0);

		applylights();

		raioOrbitaAtual = raioOrbita + (circleRadius / 100.0);
		//C�lculo do movimento de transla��o do planeta em volta do sol
		x = planetCenterX + raioOrbitaAtual * sin(moonOrbitIterator / 180.0 * 3.14);
		y = planetCenterY;// *cos(moonOrbitIterator / 180.0 * 3.14);
		z = circleRadius / (height / 2.0) + raioOrbitaAtual * cos(moonOrbitIterator / 180.0 * 3.14);

		//Esta linha define a velocidade orbital
		moonOrbitIterator += 3.14 * 2 / (periodoOrbital);
		if (moonOrbitIterator >= 360)
		{
			//Demos uma volta completa ao planeta (um ano)
			moonOrbitIterator = 0;
		}

		glTranslatef(
			x,
			y,
			z);

		//Endireitar os planetas
		glRotatef(-90.0, 1.0, 0.0, 0.0);

		//Rota��o sobre si pr�prio
		//glRotatef(_rotacaoAtual, 0.0, 1.0, 0.0);

		//Endireitar os planetas
		//glRotatef(-90.0, 1.0, 0.0, 0.0);

		gluSphere(mysolid, (circleRadius / (height / 2.0)) / 2.0, 64, 64);

		glPopMatrix();

		spin = spin + 2;
		if (spin > 360.0) spin = spin - 360.0;

		break;
	default:
		break;
	}

	// show the rendering on the screen
	glutSwapBuffers();

	// post the next redisplay
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	// set OpenGL viewport (drawable area)
	glViewport(0, 0, width, height);
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{

	}
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'q':
		// quit when q is pressed
		exit(0);
		break;
	case 'm' :
		demoMode += 1;
		if (demoMode >= demoModes){
			demoMode = 0;
		}
		accumulatorX = 0;
		accumulatorY = 0;
		accumulatorZ = 0;
		break;
	default:
		break;
	}
}

void idle()
{
	// grab a frame from the camera
	frameCapturedSuccessfully = cap.read(frameOriginal);
}

int main(int argc, char** argv)
{
	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the web cam" << endl;
		return -1;
	}

	pMOG = new BackgroundSubtractorMOG(); //MOG approach

	namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"

	//Create trackbars in "Control" window
	cvCreateTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
	cvCreateTrackbar("HighH", "Control", &iHighH, 179);

	cvCreateTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
	cvCreateTrackbar("HighS", "Control", &iHighS, 255);

	cvCreateTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
	cvCreateTrackbar("HighV", "Control", &iHighV, 255);

	// initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(width, height);
	glutCreateWindow("OpenGL / OpenCV Example");

	// Inicializa��es
	init();
	initLights();
	glGenTextures(2, textures);
	load_tga_image("earth", textures[0], false);
	load_tga_image("moon", textures[1], false);

	// set up GUI callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	// start GUI loop
	glutMainLoop();

	return 0;
}
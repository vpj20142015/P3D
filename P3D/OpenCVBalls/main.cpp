#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/video/video.hpp>
#include <opencv2/video/background_segm.hpp>
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include <math.h>

#define _DMESH

using namespace cv;
using namespace std;

// windows and trackbars name
const std::string usage = "Usage : tutorial_HoughCircle_Demo <path_to_input_image>\n";

// initial and max values of the parameters of interests.
const int cannyThresholdInitialValue = 255;
const int accumulatorThresholdInitialValue = 35;
const int maxAccumulatorThreshold = 200;
const int maxCannyThreshold = 255;

Mat frameOriginal, frameHSV, frameFiltered, frameFlipped, fgMaskMOG, controlFlipped, tempimage, tempimage2;

Ptr<BackgroundSubtractor> pMOG; //MOG Background subtractor

int iLowH = 0;
int iHighH = 179;

int iLowS = 133;
int iHighS = 250;

int iLowV = 120;
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
int demoMode = 0;

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

void init(void)
{

	glEnable(GL_CULL_FACE_MODE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	// Define técnica de shading: GL_FLAT, GL_SMOOTH
	glShadeModel(GL_SMOOTH);

	glPolygonMode(GL_FRONT, GL_FILL); // GL_LINE, GL_POINT, GL_FILL

	// Compila o modelo
	floorAndWallsDL();
}


// a useful function for displaying your coordinate system
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

		//DEBUG - desenhar contornos
		/*for (int i = 0; i< contours.size(); i++)
		{
		drawContours(src_display, contours_poly, i, Scalar(0, 0, 0), 1, 8, vector<Vec4i>(), 0, Point());
		}*/
		if ((int)radius[largest_contour_index] > 15)
		{
			circle(src_display, center[largest_contour_index], (int)radius[largest_contour_index], Scalar(0, 0, 255), 2, 8, 0);
			circle(src_display, center[largest_contour_index], 5, Scalar(255, 0, 0), -1);

			circleCenter = center[largest_contour_index];
			circleRadius = (int)radius[largest_contour_index];
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

	//set projection matrix using intrinsic camera params
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//gluPerspective is arbitrarily set, you will have to determine these values based
	//on the intrinsic camera parameters
	gluPerspective(60 - circleRadius / 5, width / height, 0.1, 100);

	//you will have to set modelview matrix using extrinsic camera params
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);

	float zoomRange = 0;
	switch (demoMode)
	{
	case 0:
		/////////////////////////////////////////////////////////////////////////////////
		// Drawing routine

		//now that the camera params have been set, draw your 3D shapes
		//first, save the current matrix
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
		flip(frameOriginal, tempimage, 0);
		flip(tempimage, tempimage2, 1);
		glDrawPixels(tempimage2.size().width, tempimage2.size().height, GL_BGR, GL_UNSIGNED_BYTE, tempimage2.ptr());
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
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(width, height);
	glutCreateWindow("OpenGL / OpenCV Example");

	// Inicializações
	init();
	initLights();

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
// Standard C
#include <stdio.h>
#include <stdlib.h>
// OpenCV
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>


#define MAXFACES 10


CvHaarClassifierCascade *cascade_f[1];
CvMemStorage			*storage;
IplImage *imgfaces[MAXFACES];


void detectFaces(IplImage *img);


void detectFaces(IplImage *img)
{
	CvRect *r;
	int i;

	IplImage* gray = cvCreateImage(cvSize(img->width, img->height), 8, 1);

	cvCvtColor(img, gray, CV_BGR2GRAY);
	cvEqualizeHist(gray, gray);

	/* detect faces */
	CvSeq *faces = cvHaarDetectObjects(gray, cascade_f[0], storage, 1.1, 2/*Aumentar este valor, diminui os FP*/, 0/*CV_HAAR_DO_CANNY_PRUNING*/, cvSize(30/*40*/, 30/*40*/));

	for (i = 0; i<MAXFACES; i++)
	{
		if (imgfaces[i]) cvReleaseImage(&imgfaces[i]);
	}

	/* return if not found */
	if (faces->total == 0) return;

	/* draw a rectangle for each face found */
	for (i = 0; i < (faces ? faces->total : 0); i++)
	{
		r = (CvRect*)cvGetSeqElem(faces, i);

		if (i < MAXFACES)
		{
			cvSetImageROI(img, cvRect(r->x, r->y, r->width, r->height));
			imgfaces[i] = cvCreateImage(cvSize(128, 128), img->depth, img->nChannels);
			cvResize(img, imgfaces[i], CV_INTER_LINEAR);
			cvResetImageROI(img);
		}

		cvRectangle(img, cvPoint(r->x, r->y), cvPoint(r->x + r->width, r->y + r->height), CV_RGB(255, 0, 0), 1, 8, 0);
	}

	/* reset buffer for the next object detection */
	cvClearMemStorage(storage);

	cvReleaseImage(&gray);
}


int main(void)
{
	char *file1_0 = "haarcascade_frontalface_alt.xml";
	char windownames[MAXFACES][8] = { "FACE#1", "FACE#2", "FACE#3", "FACE#4", "FACE#5", "FACE#6", "FACE#7", "FACE#8", "FACE#9", "FACE#10" };
	CvCapture *capture = 0;
	IplImage *frame = 0;
	int key = 0;
	int i;

	/* load the face classifier */
	cascade_f[0] = (CvHaarClassifierCascade*)cvLoad(file1_0, 0, 0, 0);

	/* setup memory storage, needed by the object detector */
	storage = cvCreateMemStorage(0);

	/* initialize camera */
	capture = cvCaptureFromCAM(CV_CAP_ANY);

	/* always check */
	if (!capture) {
		fprintf(stderr, "Cannot open initialize webcam!\n");
		return 1;
	}

	/*cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 640);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 480);*/

	/* create a window for the video */
	cvNamedWindow("Face", CV_WINDOW_AUTOSIZE);

	while (key != 'q') {
		/* get a frame */
		frame = cvQueryFrame(capture);

		/* always check */
		if (frame){
			/* always check */
			assert(cascade_f[0] && storage && frame);

			/* detect faces and display image */
			detectFaces(frame);

			/* detect eyes and display image */
			//detectEyes(frame);

			/* display current frame */
			cvShowImage("Face", frame);

			/* display faces */
			for (i = 0; i<MAXFACES; i++)
			{
				if (imgfaces[i])
				{
					/* create a window for the face#N (do nothing if already exists) */
					cvNamedWindow(windownames[i], CV_WINDOW_AUTOSIZE);

					cvShowImage(windownames[i], imgfaces[i]);
				}
				else
				{
					cvDestroyWindow(windownames[i]);
				}
			}
		}

		/* exit if user press 'q' */
		key = cvWaitKey(1);
	}

	/* free memory */
	cvDestroyWindow("Face");
	for (i = 0; i<MAXFACES; i++) cvDestroyWindow(windownames[i]);
	cvReleaseCapture(&capture);

	return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#define BS 11     // Block Size

int main(int argc, char* argv[])
{
	CvCapture *capture = 0;
	IplImage *frame = NULL; // frame
	IplImage *grayA = NULL; // gray
	IplImage *grayB = NULL; // prevgray
	IplImage *velx = NULL;
	IplImage *vely = NULL;
	int key = 0;

	/* initialize camera */
	capture = cvCaptureFromCAM(CV_CAP_ANY);
	/*cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 320);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 280);*/

	/* always check */
	if (!capture) {
		fprintf(stderr, "Cannot open initialize webcam!\n");
		return 1;
	}

	while (key != 'q') {
		/* get a frame */
		frame = cvQueryFrame(capture);

		/* always check */
		if (frame)
		{
			if (!grayA) grayA = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
			if (!grayB) grayB = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
			if (!velx) velx = cvCreateImage(cvGetSize(frame), IPL_DEPTH_32F, 1);
			if (!vely) vely = cvCreateImage(cvGetSize(frame), IPL_DEPTH_32F, 1);
			cvCvtColor(frame, grayA, CV_BGR2GRAY);

			//cvCalcOpticalFlowLK(grayB, grayA, cvSize(BS, BS), velx, vely);
			CvTermCriteria IterCriteria;
			IterCriteria.type = CV_TERMCRIT_ITER;
			IterCriteria.max_iter = 5;
			// For OpenCV 2.4.9 please use: #include "opencv2/legacy/legacy.hpp"
			cvCalcOpticalFlowHS(grayB, grayA, 1, velx, vely, 0.1, IterCriteria);

			cvNamedWindow("HorFlowBM", CV_WINDOW_AUTOSIZE);
			cvShowImage("HorFlowBM", velx);
			cvNamedWindow("VerFlowBM", CV_WINDOW_AUTOSIZE);
			cvShowImage("VerFlowBM", vely);

			for (int i = 0; i<frame->height; i += 5)
			{
				for (int j = 0; j<frame->width; j += 5)
				{
					int dx = (int)cvGetReal2D(velx, i, j);
					int dy = (int)cvGetReal2D(vely, i, j);
					cvLine(frame, cvPoint(j, i), cvPoint(j + dx, i + dy), CV_RGB(255, 255, 255), 1, 8, 0);
				}
			}

			cvNamedWindow("OpticalFlow", CV_WINDOW_AUTOSIZE);
			cvShowImage("OpticalFlow", frame);

			cvCopyImage(grayA, grayB);
		}

		/* exit if user press 'q' */
		key = cvWaitKey(1);
	}

	/* free memory */
	cvReleaseCapture(&capture);
	if (!frame) cvReleaseImage(&frame);
	if (!grayA) cvReleaseImage(&grayA);
	if (!grayB) cvReleaseImage(&grayB);
	if (!velx) cvReleaseImage(&velx);
	if (!vely) cvReleaseImage(&vely);
	cvDestroyWindow("OpticalFlow");

	return 0;
}
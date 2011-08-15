#include "devices/OmniCamera.h"
#include <iostream>
#include "opencv2/opencv.hpp"
#include "..\include\devices\Psd.h"
#include "devices/Wheel.h"

using namespace Recogbot;
using namespace std;
<<<<<<< HEAD

void main(void) {

	Wheel::Wheel();
	system("pause");
	return;

	PSD psd;
	return;
=======
#include "devices/Psd.h"

void main(void) {
>>>>>>> chiminoh/master
	OmniCamera omnicam;
	cout << omnicam.connect();
	IplImage *frame=cvCreateImage(cvSize(omnicam.getWidth(),omnicam.getHeight()),8,3);
	cvNamedWindow("w");
	while(omnicam.isConnected()) {
		omnicam.grab(frame);
		cvShowImage("w",frame);
		cvWaitKey(10);
	}
}
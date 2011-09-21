#include "devices/OmniCamera.h"
#include "utils/DB.h"
#include <iostream>
#include "opencv2/opencv.hpp"

using namespace Recogbot;
using namespace std;

void main(void) {

	OmniCamera omnicam;
	DB db;
	db.createFolder("omni_1");
	db.locateFolder("omni_1");
	cout << omnicam.connect();
	IplImage *frame=cvCreateImage(cvSize(omnicam.getWidth(),omnicam.getHeight()),8,3);
	cvNamedWindow("w");
	while(omnicam.isConnected()) {
		omnicam.grab(frame);
		db.saveImg(frame);
		cout << "frame " << db.getCnt() << endl;
		cvShowImage("w",frame);
		cvWaitKey(10);
	}
}

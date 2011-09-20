/*
   Any Test !! OKOK~!!
*/
#include <cstdio>
#include "devices/PSDSensor.h"
#include "utils/DB.h"
using namespace Recogbot;

void main(void)
{
	DB db;
	db.createFolder("korea");
	db.locateFolder("korea");
	IplImage *img=cvCreateImage(cvSize(10,10),8,3);
	db.saveImg(img);
	db.saveImg(img);
	db.saveImg(img);
	/*return;
	PSDSensor psdtest;
	int i;
	psdtest.getPsdData();
	for(i = 0 ; i<7; i++)
		printf("%d\n", psdtest.psdData.psdvalue[i]);*/
	
	
}

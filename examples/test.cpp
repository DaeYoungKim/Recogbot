/*
   Any Test !! OKOK~!!
*/
#include <cstdio>
#include "devices/PSDSensor.h"
using namespace Recogbot;

void main(void)
{
	PSDSensor psdtest;
	int i;
	psdtest.getPsdData();
	for(i = 0 ; i<7; i++)
		printf("%d\n", psdtest.psdData.psdvalue[i]);
	
	
}

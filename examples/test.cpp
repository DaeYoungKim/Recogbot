/*
   Any Test !! OKOK~!!
*/
#include <cstdio>
#include "devices/PSDSensor.h"
#include <devices/MotorDriver.h>
#include "devices/DeviceSwig.h"
#include "devices/Network.h"
using namespace Recogbot;

void main(void)
{
	PSDSensor psdtest;
	int i;
	psdtest.getPsdData();
	for(i = 0 ; i<7; i++)
		printf("%d\n", psdtest.psdData.psdvalue[i]);
	
	
}
/*
void main(void) {
	//Recogbot::PSD psd;
	//Recogbot::Wheel wheel;
	//Recogbot::DeviceSwig devSwig;

	MotorDriver motorDriver;

	while(true){
		int command;
		printf("command> ");
		scanf("%d", &command);
		switch(command){
		case 1 :
			motorDriver.moveForward(0.2);
			break;
		case 2 :
			motorDriver.moveBackward(0.2);
			break;
		case 3 :
			motorDriver.rotateLeft(0.2);
			break;
		case 4 :
			motorDriver.rotatetRight(0.2);			
			break;
		case 5 :
			motorDriver.driveManual(0.1, 0.1);			
			break;
		case 6 :
			motorDriver.loadVelocityValue();
			printf("sv : %f, rv : %f\n", motorDriver.strightVelocity, motorDriver.rotationVelocity);
			break;
		case 7 :
			motorDriver.stop();
		}
	}

	if(Network::enable()) printf("success loading config file!\n");
	else printf("%s \n", "faild to load...\n");
	
	printf("state : %d\n", Network::getNetworkState());
	
	Network::disable();
	printf("isConnection(0 or 1) : %d\n", Network::isConnected());
}
*/


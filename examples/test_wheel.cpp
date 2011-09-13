#include <cstdio>
#include <devices/MotorDriver.h>
using namespace Recogbot;

void main(void) {
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
}
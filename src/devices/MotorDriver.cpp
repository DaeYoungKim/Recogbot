#include "devices/MotorDriver.h"
#include "devices/Network.h"
#include <cstdio>


namespace Recogbot {
	MotorDriver::MotorDriver(){
		
		condition = true;

		if( !(Network::isConnected()) ) Network::enable();         // checking network
		if( Network::isConnected ){
			
			MWrite_EnableMotor( SERVO_ENABLE, SERVO_ENABLE);     // enable motor driver
			MWrite_OperationMode(VELOCITY_MODE, VELOCITY_MODE);  // set velocity mode
			RWrite_2W_KinematicsDrive(0.0, 0.0, MPS);            // init velocity = 0.0 m/s
			
			printf("network, motor condition is ok!\n");
		}else{
			condition = false; 
			printf("failed networking \n");
		}
	}

	MotorDriver::~MotorDriver(){
		MWrite_EnableMotor( SERVO_DISABLE, SERVO_DISABLE);
	}

	int MotorDriver::driveManual(float sv, float rv){
		if(condition) RWrite_2W_KinematicsDrive(sv, rv, MPS);
		else return false;
	}
	
	int MotorDriver::moveForward(float velocity){
		if(condition) RWrite_TranslationVelocity(velocity);
		else return false;
	}
	
	int MotorDriver::moveBackward(float velocity){
		if(condition) RWrite_TranslationVelocity(-velocity);
		else return false;
	}
	
	int MotorDriver::rotateLeft(float velocity){
		if(condition) RWrite_RotationVelocity(velocity);
		else return false;
	}
	
	int MotorDriver::rotatetRight(float velocity){
		if(condition) RWrite_RotationVelocity(-velocity);
		else return false;
	}
	
	int MotorDriver::loadVelocityValue(){
		if(condition) RRead_2W_KinematicsDrive(strightVelocity, rotationVelocity, MPS);
		else return false;
	}
		
	int MotorDriver::stop(){
		if(condition) RWrite_StopDrive();
		else return false;
	}
}

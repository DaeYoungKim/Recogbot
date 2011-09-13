/*
	CNU, IIMIL
	Yong-Cheol Lee

	Created in June, 2011 by Yong-Cheol Lee (budlbaram@nate.com)
*/

#ifndef RECOGBOT_DEVICES_MotorDriver_H
#define RECOGBOT_DEVICES_MotorDriver_H

#include "NRLAB02Defv01.h"

namespace Recogbot {
	class MotorDriver {
	private:
		bool condition;             // condition means failed networking

	public:
		MotorDriver();
		~MotorDriver();
		
		float strightVelocity;
		float rotationVelocity;

									// All Velocity Veriable Range : 0 ~ 0.35, moving mode : m/s
		int driveManual(float sv, float rv);
		int moveForward(float velocity);
		int moveBackward(float velocity);
		int rotateLeft(float velocity);
		int rotatetRight(float velocity);
		int loadVelocityValue();
		
		int stop();
	};
}
#endif
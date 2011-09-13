/*
	CNU, IIMIL
	SuneMun Jue

	Created in Aug, 2011 by SuneMun Jue (tjdansld87.naver.com)
*/

#ifndef RECOGBOT_DEVICES_PSD_H
#define RECOGBOT_DEVICES_PSD_H
#include "NRLAB02Defv01.h"

namespace Recogbot {
	class PSDSensor {
	private:
		bool isCondition;
	public:
		PSD psdData;
		PSDSensor();
		bool getPsdData();
		~PSDSensor();
	};
}
#endif

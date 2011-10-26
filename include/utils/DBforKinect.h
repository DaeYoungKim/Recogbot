/*
	CNU, IIMIL
	Chi-Min Oh

	Created in September, 2011 by Chi-Min Oh (sapeyes@image.chonnam.ac.kr)
*/

#ifndef RECOGBOT_UTILS_DBFORKINECT_H
#define RECOGBOT_UTILS_DBFORKINECT_H
#include <string>
#include "opencv2/opencv.hpp"
#include "devices/KinectPlayer.h"
#include "utils/DB.h"

namespace Recogbot {
	class DBforKinect : public DB {
	public:

		struct PlayerArray {
			Vector4 position;
			Vector4 skeletonPositions[NUI_SKELETON_POSITION_COUNT];
			unsigned userNum;
		};

		DBforKinect(char *folderName="none"){ DB(foldername); }

		void savePlayer(KinectPlayer& kp, const char* tag);

		void readPlayer(KinectPlayer *kp, const char* tag);

	};
}
#endif
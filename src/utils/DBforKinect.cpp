#include "utils/DBforKinect.h"
#include <cstdio>
#include <direct.h>
#include <assert.h>

namespace Recogbot {
	void DBforKinect::savePlayer(KinectPlayer& kp, const char* tag) {
		PlayerArray pa;
		
		pa.userNum = kp.getUserNum();
		pa.position = kp.getPosition();
		memcpy(pa.skeletonPositions, kp.getSkeletonPositions(), sizeof(Vector4)*NUI_SKELETON_POSITION_COUNT);
		/*Vector4 *skeleton = kp.getSkeletonPositions();
		for(int i=0; i<NUI_SKELETON_POSITION_COUNT; i++) 
			pa.skeletonPositions[i] = skeleton[i];
		*/
		sprintf(_text, "../db/%s/%05d_%s.skeleton.txt", _folderName.c_str(), _cnt, tag);
		FILE *pf = fopen(_text,"w");
		Vector4 *skeleton = kp.getSkeletonPositions();
		for(int i=0; i<NUI_SKELETON_POSITION_COUNT; i++)
			fprintf(pf,"%f %f %f\n",skeleton[i].x,skeleton[i].y,skeleton[i].z);
		fclose(pf);
		saveArray(&pa,tag,sizeof(pa),1);
		saveImg(kp.getColorMask(),"cmask");
		saveImg(kp.getDepthMask(),"dmask");
	}

	void DBforKinect::readPlayer(KinectPlayer *kp, const char* tag) {
		PlayerArray pa;
		readArray(&pa,tag,sizeof(PlayerArray),1);
		IplImage *cmask = readGrayImg("cmask");
		IplImage *dmask = readGrayImg("dmask");
		kp->init(pa.userNum,dmask,cmask,pa.position,pa.skeletonPositions);
		/*for(int i=0; i<NUI_SKELETON_POSITION_COUNT; i++) {
			cout << i << ": x " << pa.skeletonPositions[i].x << ", y " << pa.skeletonPositions[i].y<< ", z " << pa.skeletonPositions[i].z<< endl;
		}*/
		cvReleaseImage(&cmask);
		cvReleaseImage(&dmask);
	}
}
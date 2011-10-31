/*
	Pose recognition class using SVM based on Kinect

	CNU, IIMIL
	Chi-Min Oh

	Created in October, 2011 by Chi-Min Oh (sapeyes@image.chonnam.ac.kr)
*/

#ifndef RECOGBOT_PROCESS_KINECTPOSERECOGNIZER_H
#define RECOGBOT_PROCESS_KINECTPOSERECOGNIZER_H
#include <devices/kinect.h>
#include <utils/svm.h>

namespace Recogbot {
	class KinectPoseRecognizer {
	public:
		KinectPoseRecognizer();

		void init(int numOfPoses, int numOfFeatureDimensions=60);

		int numOfPoses() {return _numOfPoses;}

		/**
		* 
		* @return ( -1 : none, 0 and others : pose )
		**/
		int recognizePose(Kinect &kinect);

	private:
		int _numOfPoses;

		int _numOfFeatureDimensions;

		SVM_Classifier *_classifiers;

		double *_features;
	};
}
#endif
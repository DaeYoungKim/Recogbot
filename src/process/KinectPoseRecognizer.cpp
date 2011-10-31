#include "process/KinectPoseRecognizer.h"
#include <cstdio>
namespace Recogbot {

	// 관절에서 특징벡터 얻기. (영상 특징은 아직 없음)
	void transform(Vector4 *positions, double* features, unsigned feature_dimension) {
		double *feature_runner=features;
		for(int i=0; i<NUI_SKELETON_POSITION_COUNT; i++) {
			*feature_runner++ = positions[i].x;
			*feature_runner++ = positions[i].y;
			*feature_runner++ = positions[i].z;		
		}
	}

	KinectPoseRecognizer::KinectPoseRecognizer() :_classifiers(0), _features(0) {
		
	}

	void KinectPoseRecognizer::init(int numOfPoses, int numOfFeatureDimensions) {
		_numOfPoses = numOfPoses;
		
		if(_classifiers)
			delete [] _classifiers;
		_classifiers = 0;

		_classifiers = new SVM_Classifier[numOfPoses];

		if(_features)
			delete [] _features;
		_features = 0;

		_features = new double[numOfFeatureDimensions];
		memset(_features,0,numOfFeatureDimensions*sizeof(double));

		char text[256];
		for(int i=0; i<numOfPoses; i++) {
			sprintf(text,"../db/svm/%dsv_model.txt",i);
			_classifiers[i].load_param(text,numOfFeatureDimensions);
		}
	}



	/**
	* 
	* @return ( -1 : none, 0 and others : pose )
	**/
	int KinectPoseRecognizer::recognizePose(Kinect &kinect) {
		int index = kinect.findNearestPlayerNum();
		int max = -10E10;
		int max_index = -1;
		
		if(index != -1 && kinect.getPlayer(index).isInitialized()) {
			KinectPlayer player = kinect.getPlayer(index);
					
			Vector4 *positions = player.getLocalizedSkeletonPositions(); //상대좌표
			transform(positions, _features, _numOfFeatureDimensions); //특징추출
					
			for(int i=0; i<_numOfPoses; i++) {
				double val = _classifiers[i].classify(_features);
				if(val >max) {
					max = val;
					max_index = i;
				}
			}
		}

		return max_index;
	}
}
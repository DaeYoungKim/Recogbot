/*
	CNU, IIMIL
	Chi-Min Oh

	Created in Sept., 2011 by Chi-Min Oh (sapeyes@image.chonnam.ac.kr)

	Note 
	 - Resolution : Depth 320*240, Color 640*480 (fixed) (Sept. 27, 2011)
	 - KinectPlayer class has been created to capsulize all about a person. 
*/

#ifndef RECOGBOT_DEVICES_KINECTPLAYER_H
#define RECOGBOT_DEVICES_KINECTPLAYER_H

#include <windows.h>
#include "MSR_NuiApi.h"
#include "opencv2/opencv.hpp"
#include <vector>
#include <ostream>

namespace Recogbot {

	class KinectPlayer {
	public:
		KinectPlayer();

		KinectPlayer(const KinectPlayer &kp);

		~KinectPlayer();
		
		void init(unsigned userNum, IplImage* depthMask, IplImage* colorMask, Vector4 position, Vector4 skeletonPositions[NUI_SKELETON_POSITION_COUNT]);

		void uninit() { _initialized = false; }

		unsigned getUserNum() const { return _userNum; }

		Vector4 getPosition() const { return _position; }

		IplImage* getDepthMask() const { return _depthMask; }

		IplImage* getColorMask() const { return _colorMask; }

		Vector4* getSkeletonPositions() { return _skeletonPositions; }

		Vector4 getSkeletonPosition(int index) { return _skeletonPositions[index]; }

		Vector4 getSkeletonDepthPosition(int index);

		Vector4 getSkeletonColorPosition(int index);

		Vector4* getLocalizedSkeletonPositions();

		friend ostream& operator <<(ostream& os, const KinectPlayer& ks);

		/**
		* It only works for 320*240 image
		**/
		void drawSkeletonInDepthCoordinates(IplImage *img, CvScalar segmentColor=CV_RGB(255,255,0), CvScalar jointColor=CV_RGB(255,0,0));

		/**
		* It only works for 640*480 image
		**/
		void drawSkeletonInColorCoordinates(IplImage *img, CvScalar segmentColor=CV_RGB(255,255,0), CvScalar jointColor=CV_RGB(255,0,0));

		bool isInitialized() { return _initialized; }
		
	private:

		unsigned _userNum;

		Vector4 _position;
		
		Vector4 _skeletonPositions[NUI_SKELETON_POSITION_COUNT];

		IplImage *_depthMask;

		IplImage *_colorMask;

		bool _initialized;

		Vector4 vectorizeXfromC(Vector4 x, Vector4 c);
	};

	ostream& operator <<(ostream& os, const KinectPlayer& ks);
}

#endif
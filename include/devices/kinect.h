/*
	CNU, IIMIL
	Chi-Min Oh

	Created in June, 2011 by Chi-Min Oh (sapeyes@image.chonnam.ac.kr)	
*/

#ifndef RECOGBOT_DEVICES_KINECT_H
#define RECOGBOT_DEVICES_KINECT_H

#include <windows.h>
#include "MSR_NuiApi.h"
#include "opencv2/opencv.hpp"
#include <vector>
#include <ostream>
#include "devices/KinectPlayer.h"

namespace Recogbot {

	class Kinect {
	public:
		Kinect();
		~Kinect();
	
		bool init();
		void uninit();

	
		void videoImage(IplImage *img);

		void depthImage(IplImage *img);

		void depth8BitImage(IplImage *img);

		void depthPlayerImage(IplImage *img);

		int numOfPlayers();

		void skeletonImage(IplImage img);

		KinectPlayer getPlayer(int index);

		/**
		* Find nearest person if there is any skeleton.
		* @return nearest Kinect Player or null if no one plays with kinect.
		**/
		unsigned findNearestPlayerNum();


		/*void colorImageOfPlayer(int num, IplImage img);

		void depthImageOfPlayer(int num, IplImage img);

		void calibratedImageOfPlayer(int num, IplImage img);

		void skeletonImageOfPlayer(int num, IplImage img);

		

		void getPlayerSkeleton(int num);*/

		long getCameraElevationAngle();

		void setCameraElevationAngle(long angle);

		CRITICAL_SECTION _csSkeleton;


	private:


		static DWORD WINAPI nuiProcessThread(LPVOID pParam);

		
		CRITICAL_SECTION _csVideo;
		CRITICAL_SECTION _csDepth;
		

		HANDLE _events[4];

		const NUI_IMAGE_FRAME * _pVideoFrame;
		const NUI_IMAGE_FRAME * _pDepthFrame;
		NUI_SKELETON_FRAME _skeletonFrame;

		HANDLE _videoHandle;
		HANDLE _depthHandle;
		HANDLE _nextVideoFrameEvent;
		HANDLE _nextDepthFrameEvent;
		HANDLE _nextSkeletonEvent;
		HANDLE _nuiProcessStop;

		IplImage *_videoImg;
		IplImage *_depthImg;
		IplImage *_depthPlayerImg;

		IplImage *_depthMask[NUI_SKELETON_COUNT];
		IplImage *_colorMask[NUI_SKELETON_COUNT];
		
		//static WINAPI HRESULT eventHandlerFunc(LPVOID arg);

		bool _calibrated;

		bool _initialized;

		int _cnt_players;		

		//vector<KinectPlayer> _players;

		KinectPlayer _players[NUI_SKELETON_COUNT];

	};
}
#endif
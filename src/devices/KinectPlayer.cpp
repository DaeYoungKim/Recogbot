#include "devices/kinectplayer.h"
#include <cstdio>
namespace Recogbot {

	void Nui_DrawSkeletonSegment( CvPoint *points, IplImage *img, CvScalar color, int numJoints,  ... )
	{
		va_list vl;
		va_start(vl,numJoints);
		CvPoint segmentPositions[NUI_SKELETON_POSITION_COUNT];
		for (int iJoint = 0; iJoint < numJoints; iJoint++)
		{
			NUI_SKELETON_POSITION_INDEX jointIndex = va_arg(vl,NUI_SKELETON_POSITION_INDEX);
			segmentPositions[iJoint].x = points[jointIndex].x;
			segmentPositions[iJoint].y = points[jointIndex].y;
		}
		
		for(int i=0; i<numJoints-1; i++)
		{
			cvLine(img,segmentPositions[i],segmentPositions[i+1],color,2);
		}
		
		va_end(vl);
	}

	KinectPlayer::KinectPlayer(): _userNum(0), _depthMask(0), _colorMask(0), _initialized(false) {
		_depthMask = cvCreateImage(cvSize(320,240),8,1); 
		_colorMask = cvCreateImage(cvSize(640,480),8,1);
	}

	KinectPlayer::KinectPlayer(const KinectPlayer &kp): _userNum(0), _depthMask(0), _colorMask(0), _initialized(false) {
		_depthMask = cvCreateImage(cvSize(320,240),8,1); 
		_colorMask = cvCreateImage(cvSize(640,480),8,1);
		if(const_cast<KinectPlayer*>(&kp)->isInitialized())
			init(kp.getUserNum(), kp.getDepthMask(), kp.getColorMask(), kp.getPosition(), 
			const_cast<KinectPlayer*>(&kp)->getSkeletonPositions());
	}

	KinectPlayer::~KinectPlayer() {
		cvReleaseImage(&_depthMask);
		cvReleaseImage(&_colorMask);
	}

	void KinectPlayer::init(unsigned userNum, IplImage* depthMask, IplImage* colorMask, Vector4 position, Vector4 skeletonPositions[NUI_SKELETON_POSITION_COUNT]) {
		_userNum = userNum;
		cvCopy(depthMask,_depthMask);
		cvCopy(colorMask,_colorMask);
		_position = position;
		for(int i=0; i<NUI_SKELETON_POSITION_COUNT; i++)
			_skeletonPositions[i] = skeletonPositions[i];
		_initialized = true;
	}

	Vector4 KinectPlayer::getSkeletonDepthPosition(int index) {
		Vector4 v;
		float fx=0,fy=0;
		NuiTransformSkeletonToDepthImageF(_skeletonPositions[index],&fx,&fy);
		v.x = (int) ( fx * 320 + 0.5f );
		v.y = (int) ( fy * 240 + 0.5f );
		return v;
	}

	Vector4 KinectPlayer::getSkeletonColorPosition(int index) {
		Vector4 v1,v2;
		float fx=0,fy=0;
		long x=0,y=0,z=0;
		long x_color=0,y_color=0;
		NuiTransformSkeletonToDepthImageF(_skeletonPositions[index],&fx,&fy);
		v1.x = (int) ( fx * 320 + 0.5f );
		v1.y = (int) ( fy * 240 + 0.5f );
		v1.z = (USHORT) _skeletonPositions[index].z;
		NuiImageGetColorPixelCoordinatesFromDepthPixel(NUI_IMAGE_RESOLUTION_640x480, 0, x, y, z, &x_color, &y_color);			
		v2.x=x_color;
		v2.y=y_color;
		return v2;
	}

	void KinectPlayer::drawSkeletonInDepthCoordinates(IplImage *img, CvScalar segmentColor, CvScalar jointColor) {
		CvPoint points[NUI_SKELETON_POSITION_COUNT];

		float fx=0,fy=0;
		for(int i=0; i<NUI_SKELETON_POSITION_COUNT; i++) {
			NuiTransformSkeletonToDepthImageF(_skeletonPositions[i],&fx,&fy);
			points[i].x = (int) ( fx * 320 + 0.5f );
			points[i].y = (int) ( fy * 240 + 0.5f );
		}

		Nui_DrawSkeletonSegment(points,img,segmentColor,4,NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_SPINE, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_HEAD);
		Nui_DrawSkeletonSegment(points,img,segmentColor,5,NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_LEFT, NUI_SKELETON_POSITION_ELBOW_LEFT, NUI_SKELETON_POSITION_WRIST_LEFT, NUI_SKELETON_POSITION_HAND_LEFT);
		Nui_DrawSkeletonSegment(points,img,segmentColor,5,NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_RIGHT, NUI_SKELETON_POSITION_ELBOW_RIGHT, NUI_SKELETON_POSITION_WRIST_RIGHT, NUI_SKELETON_POSITION_HAND_RIGHT);
		Nui_DrawSkeletonSegment(points,img,segmentColor,5,NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_HIP_LEFT, NUI_SKELETON_POSITION_KNEE_LEFT, NUI_SKELETON_POSITION_ANKLE_LEFT, NUI_SKELETON_POSITION_FOOT_LEFT);
		Nui_DrawSkeletonSegment(points,img,segmentColor,5,NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_HIP_RIGHT, NUI_SKELETON_POSITION_KNEE_RIGHT, NUI_SKELETON_POSITION_ANKLE_RIGHT, NUI_SKELETON_POSITION_FOOT_RIGHT);
		
		for (int i = 0; i < NUI_SKELETON_POSITION_COUNT ; i++)
			cvCircle(img,points[i],3,jointColor,4);
	}

	void KinectPlayer::drawSkeletonInColorCoordinates(IplImage *img, CvScalar segmentColor, CvScalar jointColor) {
		CvPoint points[NUI_SKELETON_POSITION_COUNT];

		float fx=0,fy=0;
		long x=0,y=0,z=0;
		long x_color=0,y_color=0;
		for(int i=0; i<NUI_SKELETON_POSITION_COUNT; i++) {
			NuiTransformSkeletonToDepthImageF(_skeletonPositions[i], &fx, &fy);
			x = (int) ( fx * 320 + 0.5f );
			y = (int) ( fy * 240 + 0.5f );
			z = (USHORT) _skeletonPositions[i].z;

			NuiImageGetColorPixelCoordinatesFromDepthPixel(NUI_IMAGE_RESOLUTION_640x480, 0, x, y, z, &x_color, &y_color);			
			points[i].x=x_color;
			points[i].y=y_color;
		}

		Nui_DrawSkeletonSegment(points,img,segmentColor,4,NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_SPINE, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_HEAD);
		Nui_DrawSkeletonSegment(points,img,segmentColor,5,NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_LEFT, NUI_SKELETON_POSITION_ELBOW_LEFT, NUI_SKELETON_POSITION_WRIST_LEFT, NUI_SKELETON_POSITION_HAND_LEFT);
		Nui_DrawSkeletonSegment(points,img,segmentColor,5,NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_RIGHT, NUI_SKELETON_POSITION_ELBOW_RIGHT, NUI_SKELETON_POSITION_WRIST_RIGHT, NUI_SKELETON_POSITION_HAND_RIGHT);
		Nui_DrawSkeletonSegment(points,img,segmentColor,5,NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_HIP_LEFT, NUI_SKELETON_POSITION_KNEE_LEFT, NUI_SKELETON_POSITION_ANKLE_LEFT, NUI_SKELETON_POSITION_FOOT_LEFT);
		Nui_DrawSkeletonSegment(points,img,segmentColor,5,NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_HIP_RIGHT, NUI_SKELETON_POSITION_KNEE_RIGHT, NUI_SKELETON_POSITION_ANKLE_RIGHT, NUI_SKELETON_POSITION_FOOT_RIGHT);
		
		for (int i = 0; i < NUI_SKELETON_POSITION_COUNT ; i++)
			cvCircle(img,points[i],3,jointColor,4);
	}

	Vector4 KinectPlayer::vectorizeXfromC(Vector4 x, Vector4 c) {
		x.x -= c.x; x.y -= c.y; x.z -= c.z;
		float norm = sqrt(x.x*x.x + x.y*x.y + x.z*x.z);
		x.x /= norm; x.y /= norm; x.z /= norm; 
		return x;
	}

	Vector4* KinectPlayer::getLocalizedSkeletonPositions() {
		Vector4 *localized = new Vector4[NUI_SKELETON_POSITION_COUNT];
		Vector4 *p = _skeletonPositions;
		
		localized[NUI_SKELETON_POSITION_HIP_CENTER].x=0.0;
		localized[NUI_SKELETON_POSITION_HIP_CENTER].y=0.0;
		localized[NUI_SKELETON_POSITION_HIP_CENTER].z=0.0;
		localized[NUI_SKELETON_POSITION_SPINE] = vectorizeXfromC(p[NUI_SKELETON_POSITION_SPINE],p[NUI_SKELETON_POSITION_HIP_CENTER]);
		localized[NUI_SKELETON_POSITION_SHOULDER_CENTER] = vectorizeXfromC(p[NUI_SKELETON_POSITION_SHOULDER_CENTER],p[NUI_SKELETON_POSITION_SPINE]);
		localized[NUI_SKELETON_POSITION_HEAD] = vectorizeXfromC(p[NUI_SKELETON_POSITION_HEAD],p[NUI_SKELETON_POSITION_SHOULDER_CENTER]);

		localized[NUI_SKELETON_POSITION_SHOULDER_LEFT] = vectorizeXfromC(p[NUI_SKELETON_POSITION_SHOULDER_LEFT],p[NUI_SKELETON_POSITION_SHOULDER_CENTER]);
		localized[NUI_SKELETON_POSITION_ELBOW_LEFT] = vectorizeXfromC(p[NUI_SKELETON_POSITION_ELBOW_LEFT],p[NUI_SKELETON_POSITION_SHOULDER_LEFT]);
		localized[NUI_SKELETON_POSITION_WRIST_LEFT] = vectorizeXfromC(p[NUI_SKELETON_POSITION_WRIST_LEFT],p[NUI_SKELETON_POSITION_ELBOW_LEFT]);
		localized[NUI_SKELETON_POSITION_HAND_LEFT] = vectorizeXfromC(p[NUI_SKELETON_POSITION_HAND_LEFT],p[NUI_SKELETON_POSITION_WRIST_LEFT]);

		localized[NUI_SKELETON_POSITION_SHOULDER_RIGHT] = vectorizeXfromC(p[NUI_SKELETON_POSITION_SHOULDER_RIGHT],p[NUI_SKELETON_POSITION_SHOULDER_CENTER]);
		localized[NUI_SKELETON_POSITION_ELBOW_RIGHT] = vectorizeXfromC(p[NUI_SKELETON_POSITION_ELBOW_RIGHT],p[NUI_SKELETON_POSITION_SHOULDER_RIGHT]);
		localized[NUI_SKELETON_POSITION_WRIST_RIGHT] = vectorizeXfromC(p[NUI_SKELETON_POSITION_WRIST_RIGHT],p[NUI_SKELETON_POSITION_ELBOW_RIGHT]);
		localized[NUI_SKELETON_POSITION_HAND_RIGHT] = vectorizeXfromC(p[NUI_SKELETON_POSITION_HAND_RIGHT],p[NUI_SKELETON_POSITION_WRIST_RIGHT]);

		localized[NUI_SKELETON_POSITION_HIP_LEFT] = vectorizeXfromC(p[NUI_SKELETON_POSITION_HIP_LEFT],p[NUI_SKELETON_POSITION_HIP_CENTER]);
		localized[NUI_SKELETON_POSITION_KNEE_LEFT] = vectorizeXfromC(p[NUI_SKELETON_POSITION_KNEE_LEFT],p[NUI_SKELETON_POSITION_HIP_LEFT]);
		localized[NUI_SKELETON_POSITION_ANKLE_LEFT] = vectorizeXfromC(p[NUI_SKELETON_POSITION_ANKLE_LEFT],p[NUI_SKELETON_POSITION_KNEE_LEFT]);
		localized[NUI_SKELETON_POSITION_FOOT_LEFT] = vectorizeXfromC(p[NUI_SKELETON_POSITION_FOOT_LEFT],p[NUI_SKELETON_POSITION_ANKLE_LEFT]);

		localized[NUI_SKELETON_POSITION_HIP_RIGHT] = vectorizeXfromC(p[NUI_SKELETON_POSITION_HIP_RIGHT],p[NUI_SKELETON_POSITION_HIP_CENTER]);
		localized[NUI_SKELETON_POSITION_KNEE_RIGHT] = vectorizeXfromC(p[NUI_SKELETON_POSITION_KNEE_RIGHT],p[NUI_SKELETON_POSITION_HIP_RIGHT]);
		localized[NUI_SKELETON_POSITION_ANKLE_RIGHT] = vectorizeXfromC(p[NUI_SKELETON_POSITION_ANKLE_RIGHT],p[NUI_SKELETON_POSITION_KNEE_RIGHT]);
		localized[NUI_SKELETON_POSITION_FOOT_RIGHT] = vectorizeXfromC(p[NUI_SKELETON_POSITION_FOOT_RIGHT],p[NUI_SKELETON_POSITION_ANKLE_RIGHT]);


		return localized;

	}

	ostream& operator <<(ostream& os,  const KinectPlayer& ks) {
		Vector4 v = ks.getPosition();
		Vector4* positions = const_cast<KinectPlayer*>(&ks)->getSkeletonPositions();
		Vector4 head = positions[NUI_SKELETON_POSITION_HEAD];
		os << " Player[" << ks.getUserNum() << "] at (" << v.x << "," << v.y << "," << v.z << ")"<<endl;
		os << "  -- Head : (" << head.x << "," << head.y << "," << head.z << ")"<<endl;
		return os;
	}
}
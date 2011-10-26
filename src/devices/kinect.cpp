#include "devices/kinect.h"
#include <cstdio>
namespace Recogbot {

	RGBQUAD Nui_ShortToQuad_Depth( USHORT s ) {
		USHORT RealDepth = (s & 0xfff8) >> 3;
		USHORT Player = s & 7;

		// transform 13-bit depth information into an 8-bit intensity appropriate
		// for display (we disregard information in most significant bit)
		BYTE l = 255 - (BYTE)(256*RealDepth/0x0fff);

		RGBQUAD q;
		q.rgbRed = q.rgbBlue = q.rgbGreen = 0;

		switch( Player ) {
		case 0:
			q.rgbRed = l / 2;
			q.rgbBlue = l / 2;
			q.rgbGreen = l / 2;
			break;
		case 1:
			q.rgbRed = l;
			break;
		case 2:
			q.rgbGreen = l;
			break;
		case 3:
			q.rgbRed = l / 4;
			q.rgbGreen = l;
			q.rgbBlue = l;
			break;
		case 4:
			q.rgbRed = l;
			q.rgbGreen = l;
			q.rgbBlue = l / 4;
			break;
		case 5:
			q.rgbRed = l;
			q.rgbGreen = l / 4;
			q.rgbBlue = l;
			break;
		case 6:
			q.rgbRed = l / 2;
			q.rgbGreen = l / 2;
			q.rgbBlue = l;
			break;
		case 7:
			q.rgbRed = 255 - ( l / 2 );
			q.rgbGreen = 255 - ( l / 2 );
			q.rgbBlue = 255 - ( l / 2 );
		}

		return q;
	}

	RGBQUAD Nui_ShortToQuad_Depth_Masks( int x, int y, USHORT s, IplImage** depthMask, IplImage** colorMask ) {
		USHORT RealDepth = (s & 0xfff8) >> 3;
		USHORT Player = s & 7;

		// transform 13-bit depth information into an 8-bit intensity appropriate
		// for display (we disregard information in most significant bit)
		BYTE l = 255 - (BYTE)(256*RealDepth/0x0fff);

		RGBQUAD q;
		q.rgbRed = q.rgbBlue = q.rgbGreen = 0;

		long x_color=0,y_color=0;
		if(Player!=0) {
			depthMask[Player-1]->imageData[y*320+x] = 255;
			NuiImageGetColorPixelCoordinatesFromDepthPixel(NUI_IMAGE_RESOLUTION_640x480, 0, x, y, (s & 0xfff8), &x_color, &y_color);
			colorMask[Player-1]->imageData[y_color*640+x_color] = 255;
		}

		switch( Player ) {
		case 0:  // background
			q.rgbRed = l / 2;
			q.rgbBlue = l / 2;
			q.rgbGreen = l / 2;
			break;
		case 1: // player 1
			q.rgbRed = l;
			break;
		case 2: // player 2
			q.rgbGreen = l;
			break;
		case 3: // player 3
			q.rgbRed = l / 4;
			q.rgbGreen = l;
			q.rgbBlue = l;
			break;
		case 4: // player 4
			q.rgbRed = l;
			q.rgbGreen = l;
			q.rgbBlue = l / 4;
			break;
		case 5: // player 5
			q.rgbRed = l;
			q.rgbGreen = l / 4;
			q.rgbBlue = l;
			break;
		case 6: // player 6
			q.rgbRed = l / 2;
			q.rgbGreen = l / 2;
			q.rgbBlue = l;
			break;
		case 7: // player 7
			q.rgbRed = 255 - ( l / 2 );
			q.rgbGreen = 255 - ( l / 2 );
			q.rgbBlue = 255 - ( l / 2 );
		}

		return q;
	}

	Kinect::Kinect() 
	: _initialized(false),
	  _calibrated(false),
	  _pVideoFrame(NULL),
	  _pDepthFrame(NULL),
	  _cnt_players(0) {
		_nextVideoFrameEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
		_nextDepthFrameEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
		_nextSkeletonEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
		_nuiProcessStop=CreateEvent(NULL,FALSE,FALSE,NULL);
		InitializeCriticalSection(&_csVideo);
		InitializeCriticalSection(&_csDepth);
		InitializeCriticalSection(&_csSkeleton);
		_videoImg = cvCreateImage(cvSize(640,480),8,3);
		_depthImg = cvCreateImage(cvSize(320,240),16,1);		
		_depthPlayerImg = cvCreateImage(cvSize(320,240),8,3);	
		for(int i=0; i<NUI_SKELETON_COUNT; i++) {
			_depthMask[i] = cvCreateImage(cvSize(320,240),8,1);		
			_colorMask[i] = cvCreateImage(cvSize(640,480),8,1);		
		}
	}

	Kinect::~Kinect() {
		uninit();
		DeleteCriticalSection(&_csVideo);
		DeleteCriticalSection(&_csDepth);
		DeleteCriticalSection(&_csSkeleton);
		CloseHandle(_nextVideoFrameEvent);
		CloseHandle(_nextDepthFrameEvent);
		CloseHandle(_nextSkeletonEvent);
		CloseHandle(_nuiProcessStop);
		cvReleaseImage(&_videoImg);
		cvReleaseImage(&_depthImg);
		cvReleaseImage(&_depthPlayerImg);
		for(int i=0; i<NUI_SKELETON_COUNT; i++) {
			cvReleaseImage(&_depthMask[i]);
			cvReleaseImage(&_colorMask[i]);	
		}
		
	}

	bool Kinect::init() {
		HRESULT hr;
		hr = NuiInitialize( NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_SKELETON | NUI_INITIALIZE_FLAG_USES_COLOR);
		
		if(hr<0)
			return false;

		hr = NuiSkeletonTrackingEnable( _nextSkeletonEvent, 0 );

		if(hr<0)
			return false;

		hr = NuiImageStreamOpen(
			NUI_IMAGE_TYPE_COLOR,
			NUI_IMAGE_RESOLUTION_640x480,
			0,
			2,
			_nextVideoFrameEvent,
			&_videoHandle );

		if(hr<0)
			return false;

		hr = NuiImageStreamOpen(
			NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX,
			NUI_IMAGE_RESOLUTION_320x240,
			0,
			2,
			_nextDepthFrameEvent,
			&_depthHandle );

		if(hr<0)
			return false;

		_events[0] = _nuiProcessStop;
		_events[1] = _nextDepthFrameEvent;
		_events[2] = _nextVideoFrameEvent;
		_events[3] = _nextSkeletonEvent;

		CreateThread(NULL,0,nuiProcessThread,this,0,NULL);

		_initialized = true;
		return true;
	}

	void Kinect::uninit() {
		NuiSkeletonTrackingDisable();
		NuiImageStreamReleaseFrame(_depthHandle,_pDepthFrame);
		NuiImageStreamReleaseFrame(_videoHandle,_pVideoFrame);
		NuiShutdown();
		_initialized=false;
	}

	KinectPlayer Kinect::getPlayer(int index) {
		assert(index>=0 && index <NUI_SKELETON_COUNT);
		return _players[index];		
	}

	DWORD WINAPI Kinect::nuiProcessThread(LPVOID pParam) {
		Kinect *pthis = (Kinect *)pParam;
		HRESULT hr;
		KINECT_LOCKED_RECT LockedRect;
		
		while(1) {
			int nEventIdx=WaitForMultipleObjects(sizeof(pthis->_events)/sizeof(pthis->_events[0]),pthis->_events,FALSE,100);
			
			if(nEventIdx==0)
				break;

			switch(nEventIdx){			
				case 1: // color event
					hr = NuiImageStreamGetNextFrame(
						pthis->_depthHandle,
						0,
						&pthis->_pDepthFrame);

					pthis->_pDepthFrame->pFrameTexture->LockRect(0, &LockedRect, NULL, 0);

					EnterCriticalSection(&pthis->_csVideo);
					
					for(int i=0; i<NUI_SKELETON_COUNT; i++){
						cvZero(pthis->_depthMask[i]);
						cvZero(pthis->_colorMask[i]);
					}

					if(LockedRect.Pitch !=0) {
						BYTE* pBuffer = (BYTE*) LockedRect.pBits;
						memcpy(pthis->_depthImg->imageData,pBuffer, (pthis->_depthImg->widthStep)*(pthis->_depthImg->height));
					}

					if( LockedRect.Pitch != 0 ) 
					{
						BYTE *pBuffer = (BYTE*) LockedRect.pBits;
						USHORT *pBufferRun = (USHORT*) pBuffer;
		
						for( int y = 0 ; y < 240 ; y++ )
						{
							for( int x = 0 ; x < 320 ; x++ )
							{
								RGBQUAD quad = Nui_ShortToQuad_Depth_Masks( x,y,*pBufferRun,pthis->_depthMask,pthis->_colorMask );
								pBufferRun++;
								pthis->_depthPlayerImg->imageData[y*pthis->_depthPlayerImg->widthStep+x*pthis->_depthPlayerImg->nChannels+0] = quad.rgbBlue;
								pthis->_depthPlayerImg->imageData[y*pthis->_depthPlayerImg->widthStep+x*pthis->_depthPlayerImg->nChannels+1] = quad.rgbGreen;
								pthis->_depthPlayerImg->imageData[y*pthis->_depthPlayerImg->widthStep+x*pthis->_depthPlayerImg->nChannels+2] = quad.rgbRed;				
							}
						}				
					}
					
					LeaveCriticalSection(&pthis->_csVideo);
					pthis->_pDepthFrame->pFrameTexture->UnlockRect(0);
					NuiImageStreamReleaseFrame(pthis->_depthHandle,pthis->_pDepthFrame);
					
					break;

				case 2: // depth event
					hr = NuiImageStreamGetNextFrame(
						pthis->_videoHandle,
						0,
						&pthis->_pVideoFrame);

					pthis->_pVideoFrame->pFrameTexture->LockRect(0, &LockedRect, NULL, 0);

					EnterCriticalSection(&pthis->_csDepth);

					if( LockedRect.Pitch != 0 ) {
						BYTE* pBuffer = (BYTE*) LockedRect.pBits;

						for( int y = 0 ; y < 480 ; y++ ) {
							for( int x = 0 ; x < 640 ; x++)	 {
								pthis->_videoImg->imageData[y*pthis->_videoImg->widthStep+x*pthis->_videoImg->nChannels+0] = *pBuffer++;
								pthis->_videoImg->imageData[y*pthis->_videoImg->widthStep+x*pthis->_videoImg->nChannels+1] = *pBuffer++;
								pthis->_videoImg->imageData[y*pthis->_videoImg->widthStep+x*pthis->_videoImg->nChannels+2] = *pBuffer++;
								pBuffer++;
							}
						}
					}
					LeaveCriticalSection(&pthis->_csDepth);
					pthis->_pVideoFrame->pFrameTexture->UnlockRect(0);
					NuiImageStreamReleaseFrame(pthis->_videoHandle,pthis->_pVideoFrame);
					break;

				case 3: // 스켈레톤 이벤트
					hr = NuiSkeletonGetNextFrame( 0, &pthis->_skeletonFrame );
					NuiTransformSmooth(&pthis->_skeletonFrame,NULL);
					
					EnterCriticalSection(&pthis->_csSkeleton);
					
					// 스켈레톤으로 추적된 사람 수 0으로 초기화
					pthis->_cnt_players=0;
					
					int cnt_players = 0; // 스켈레톤 카운트 변수
					int num = 0; // 사용자 번호 변수
					float fx=0,fy=0; 
					int cx=0,cy=0; // 스켈레톤 깊이 영상 중심위치
					
					for(int i=0; i<NUI_SKELETON_COUNT; i++) {
						if(pthis->_skeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED){
							
							++cnt_players; // 스켈레톤 수 증가
							
							// 스켈레톤의 사람 번호 찾기.. (왜 이게 sdk에서 지원 안하는지 이해가 안가네..)
							num = pthis->_players[i].getUserNum();
							unsigned count[NUI_SKELETON_COUNT]; //총 6명이니..
							memset(count,0,sizeof(int)*NUI_SKELETON_COUNT);
							
							for(int k=0; k<NUI_SKELETON_POSITION_COUNT; k++) {
								NuiTransformSkeletonToDepthImageF(pthis->_skeletonFrame.SkeletonData[i].SkeletonPositions[k],&fx,&fy);
								cx = (int) (320*fx+0.5f);
								cy = (int) (240*fy+0.5f);
														
								for(int j=0; j<NUI_SKELETON_COUNT; j++) 
									if( cx>=0 && cx<320 && cy>=0 && cy <=240 )
										if(pthis->_depthMask[j]->imageData[cx+cy*320] != 0) 										
											count[j]++;									
							}

							int max =0;
							int max_index =0;
							for(int j=0; j<6; j++)
								if(count[j]>max)
									max_index=j;
							num = max_index;

							pthis->_players[i].init(num, pthis->_depthMask[num], pthis->_colorMask[num], 
								pthis->_skeletonFrame.SkeletonData[i].Position, 
								pthis->_skeletonFrame.SkeletonData[i].SkeletonPositions );							
						}
						else {
							pthis->_players[i].uninit();
						}
					}
										
					pthis->_cnt_players=cnt_players;
					LeaveCriticalSection(&pthis->_csSkeleton);
					break;
			}
		}
		return 0;
	}

	void Kinect::videoImage(IplImage *img) {
		if(_videoImg->widthStep == img->widthStep && _videoImg->height == img->height 
			&& _videoImg->nChannels == img->nChannels && _videoImg->depth == img->depth) {
			EnterCriticalSection(&_csVideo);
			cvCopy(_videoImg,img);
			LeaveCriticalSection(&_csVideo);
		}
	}

	void Kinect::depthImage(IplImage *img) {
		if(_depthImg->widthStep == img->widthStep && _depthImg->height == img->height 
			&& _depthImg->nChannels == img->nChannels && _depthImg->depth == img->depth) {
			EnterCriticalSection(&_csDepth);
			cvCopy(_depthImg,img);
			LeaveCriticalSection(&_csDepth);
		}
	}

	void Kinect::depth8BitImage(IplImage *img) {
		if(_depthImg->width == img->width && _depthImg->height == img->height && img->depth == 8) {
			EnterCriticalSection(&_csDepth);
			cvNormalize(_depthImg,img,0.0,255.0,CV_MINMAX,NULL);
			LeaveCriticalSection(&_csDepth);
		}
	}

	void Kinect::depthPlayerImage(IplImage *img) {
		if(_depthPlayerImg->width == img->width && _depthPlayerImg->height == img->height && img->depth == 8) {
			EnterCriticalSection(&_csDepth);
			cvCopy(_depthPlayerImg,img);
			LeaveCriticalSection(&_csDepth);
		}
	}

	int Kinect::numOfPlayers() {
		return _cnt_players;
	}

	long Kinect::getCameraElevationAngle(){
		long angle;
		NuiCameraElevationGetAngle(&angle);
		return angle;
	}

	void Kinect::setCameraElevationAngle(long angle){
		NuiCameraElevationSetAngle(angle);		
	}

	unsigned Kinect::findNearestPlayerNum() {
		float min = 10E5;
		int min_index = -1;
		for(int i=0; i<NUI_SKELETON_COUNT; i++) {
			if(_players[i].isInitialized() && _players[i].getPosition().z < min)
				min_index = i;
		}
		return min_index;
	}
	

}
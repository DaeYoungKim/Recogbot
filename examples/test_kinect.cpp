#include <iostream>
#include <opencv2/opencv.hpp>
#include <devices/kinect.h>
#include <utils/DB.h>
#include <utils/DBforKinect.h>
#include <utils/svm.h>

using namespace std;
using namespace Recogbot;

// 관절에서 특징벡터 얻기. (영상 특징은 아직 없음)
void transform(Vector4 *positions, double* features, unsigned feature_dimension) {
	double *feature_runner=features;
	for(int i=0; i<NUI_SKELETON_POSITION_COUNT; i++) {
		*feature_runner++ = positions[i].x;
		*feature_runner++ = positions[i].y;
		*feature_runner++ = positions[i].z;		
	}
}

void main(void) {

	unsigned char menu =0;  

	unsigned total_n_pose = 12;

	unsigned pose_n_frames = 100;

	unsigned feature_dimension = 60;  // 특징 차원 수

	double *features = new double[feature_dimension];  // 특징벡터

	float gamma[12] = {0.1, 0.1, 0.1, 0.1, 
						0.1, 0.1, 0.1, 0.1,
						0.1, 0.1, 0.1, 0.1};

	Kinect kinect;
	if(!kinect.init()) {
		cout << "Kinect is not working";
		return;
	}

	long angle = kinect.getCameraElevationAngle();

	IplImage *frame = cvCreateImage(cvSize(640,480),8,3);
	IplImage *frame_masked = cvCreateImage(cvSize(640,480),8,3);
	IplImage *depth8 = cvCreateImage(cvSize(320,240),8,1);
	IplImage *depth8_masked = cvCreateImage(cvSize(320,240),8,1);
	IplImage *depthPlayer = cvCreateImage(cvSize(320,240),8,3);
	char text[256],text2[256];;

	while(1) {
		cvDestroyAllWindows();
		cout << " 1 : 키넥트 연결 확인 " << endl;
		cout << " 2 : 포즈 저장하기 " << endl;
		cout << " 3 : 포즈 확인하기 " << endl;
		cout << " 4 : SVM 학습 및 검증 집합 만들기 " << endl;
		cout << " 5 : SVM 학습하기 " << endl;
		cout << " 6 : SVM 검증집합 인식결과 " << endl;
		cout << " 7 : 실시간 포즈 인식 " << endl;
		cout << " u : 카메라 각도 올리기" << endl;
		cout << " d : 카메라 각도 내리기" << endl;
		cout << " 종료 : ctrl+c"<<endl;
		cout << " > ";
		cin >> menu;
		getchar();

		/* 
			키넥트 영상 보기
		*/
		if(menu == '1') {
			cout << "메뉴복귀 : esc 키 누르기 (opencv 윈도우 활성화된 상태에서) " << endl;
			int key = 0;
			while(1) {
				kinect.videoImage(frame);
				kinect.depth8BitImage(depth8);
				kinect.depthPlayerImage(depthPlayer);

				//cout << "Number of Players : " << kinect.numOfPlayers() << endl;
				for(int i=0; i<NUI_SKELETON_COUNT; i++)
					if(kinect.getPlayer(i).isInitialized())
						kinect.getPlayer(i).drawSkeletonInDepthCoordinates(depthPlayer);
				cvShowImage("depth",depth8);
				cvShowImage("depthPlayer",depthPlayer);
				cvShowImage("color",frame);
				key = cvWaitKey(10);
				if(key==27 || key=='q' || key=='Q')
					break;
			}
		}	
		
		/*
			포즈 저장하기
		*/
		else if(menu == '2') {

			int key = 0;
			
			cout << " 몇개의 포즈를 찍으시겠습니까 ? > " ;
			cin >> total_n_pose;

			cout << " 포즈당 몇 프레임을 찍으시겠습니까 ? > " ;
			cin >> pose_n_frames;


			DBforKinect db;

				
			for(int i=0; i<total_n_pose; i++) {
				key =0;
				while(key != 1) {
					cout << i << "번째 포즈 저장 시작 [1 엔터] " << endl;
					cin >> key;
				}
				Sleep(5000);
				sprintf(text,"%d_pose",i);
				db.createFolder(text);
				db.locateFolder(text);
				db.initCnt();

				for(int j=0; j<pose_n_frames; ) {
					
					kinect.videoImage(frame);
					kinect.depth8BitImage(depth8);
					kinect.depthPlayerImage(depthPlayer);
					cvShowImage("players",depthPlayer);
				
					int index = kinect.findNearestPlayerNum();

					if(index != -1 && kinect.getPlayer(index).isInitialized()) {
						cout << i << "pose , " << j << " frame " << endl;
						KinectPlayer player = kinect.getPlayer(index);
						cvShowImage("player",player.getDepthMask());
						sprintf(text,"player",i);
						db.savePlayer(player,text);
						db.saveImg(frame,"color");
						db.saveImg(depth8,"depth");
						db.increaseCnt();
						++j;
						player.drawSkeletonInColorCoordinates(frame);
						cvShowImage("check",frame);
						cvWaitKey(10);
					}					
					key = cvWaitKey(10);
					if(key==27 || key=='q' || key=='Q')
						break;
				}
				cout << i<< "번째 포즈를 저장 완료" << endl;
				cvDestroyAllWindows();
			}
			cout << " 모든 포즈를 저장하였습니다. " << endl;
			
		}

		/*
			포즈 저장된 것 확인
		*/
		else if(menu == '3') {
			int key = 0;
			
			cout << " 몇개의 포즈를 찍으셨습니까 ? > " ;
			cin >> total_n_pose;

			cout << " 포즈당 몇 프레임 확인 하시겠습니까 ? > " ;
			cin >> pose_n_frames;

			DBforKinect db;
			cout << " 저장된 포즈를 다시 확인합니다. " << endl;
			db.initCnt();

			for(int i=0; i<total_n_pose; i++) {
				cout << " : " <<i << " 번째 포즈 확인 하기 [엔터] ";
				getchar();
				sprintf(text,"%d_pose",i);
				db.locateFolder(text);	
				db.initCnt();
				for(int j=0; j<pose_n_frames;j++ ) {
					cout << j << "th frame" << endl;
					IplImage *color_img = db.readImg("color");
					IplImage *depth_img = db.readImg("depth");

					KinectPlayer player;
					db.readPlayer(&player,"player");
					player.drawSkeletonInColorCoordinates(color_img);
					cvShowImage("player",color_img);
					cvShowImage("cmask",player.getColorMask());
					cvShowImage("dmask",player.getDepthMask());
					db.increaseCnt();
					key = cvWaitKey(10);
					if(key==27 || key=='q' || key=='Q')
						break;	
					cvReleaseImage(&color_img);
					cvReleaseImage(&depth_img);
				}
				cvDestroyAllWindows();
			}
			cout << " 모두 확인하였습니다. [엔터]" << endl;
			getchar();
		}


		/*
			svm 학습 데이터 만들기
		*/
		else if(menu == '4') {
		
			int key = 0;
			
			cout << " 저장된 포즈 수  > " ;
			cin >> total_n_pose;

			cout << " 포즈 당 프레임 수  > " ;
			cin >> pose_n_frames;

			int n_training = (int)pose_n_frames*0.7;
			int n_test = total_n_pose - n_training;

			DBforKinect db;
			db.initCnt();
			db.createFolder("svm");

			// 학습 집합 만들기
			for(int i=0; i<total_n_pose; i++) {
				cout << " : " <<i << " 번째 포즈 학습 데이터 생성중  "<<endl;

				sprintf(text,"../db/svm/%dpose_features.txt",i);
				FILE *pf = fopen(text,"w");
				
				for(int j=0; j<total_n_pose; j++) {
					sprintf(text,"%d_pose",j);
					db.locateFolder(text);
					db.initCnt();
					for(int k=0; k<n_training; k++) {
						//cout << i << "pose's "<< j << "pose feature at "<< k << "th frame" << endl;

						IplImage *color_img = db.readImg("color");
						IplImage *depth_img = db.readImg("depth");
						IplImage *color_mask = db.readImg("cmask");
						IplImage *depth_mask = db.readImg("dmask");

						KinectPlayer player;
						db.readPlayer(&player,"player");
						
						Vector4 *positions = player.getLocalizedSkeletonPositions(); //상대좌표
						transform(positions,features,feature_dimension); //특징추출

						//Vector4* positions = player.getSkeletonPositions(); //절대좌표
						//transform(positions,features,feature_dimension); //특징추출

						if(j==i)
							fprintf(pf,"+1 ");
						else
							fprintf(pf,"-1 ");

						for(int f=0; f<feature_dimension; f++){
							fprintf(pf, " %d:%f", f+1, features[f]);
						}
						fprintf(pf,"\n");

						player.drawSkeletonInColorCoordinates(color_img);
						cvShowImage("player",color_img);
						cvWaitKey(10);
						cvReleaseImage(&color_img);
						cvReleaseImage(&depth_img);
						cvReleaseImage(&color_mask);
						cvReleaseImage(&depth_mask);
						db.increaseCnt();
					}
				}

				fclose(pf);
			}

			// 테스트 집합 만들기 
			for(int i=0; i<total_n_pose; i++) {
				cout << " : " <<i << " 번째 포즈 테스트 데이터 만드는 중   "<<endl;

				sprintf(text,"../db/svm/%dpose_features_test.txt",i);
				FILE *pf = fopen(text,"w");
				
				for(int j=0; j<total_n_pose; j++) {
					sprintf(text,"%d_pose",j);
					db.locateFolder(text);
					db.initCnt();
					for(int k=n_training; k<pose_n_frames; k++) {

						IplImage *color_img = db.readImg("color");
						IplImage *depth_img = db.readImg("depth");
						IplImage *color_mask = db.readImg("cmask");
						IplImage *depth_mask = db.readImg("dmask");

						KinectPlayer player;
						db.readPlayer(&player,"player");
						
						Vector4 *positions = player.getLocalizedSkeletonPositions(); //상대좌표
						transform(positions,features,feature_dimension); //특징추출

						//Vector4* positions = player.getSkeletonPositions(); //절대좌표
						//transform(positions,features,feature_dimension); //특징추출

						if(j==i)
							fprintf(pf,"+1 ");
						else
							fprintf(pf,"-1 ");

						for(int f=0; f<feature_dimension; f++){
							fprintf(pf, " %d:%f", f+1, features[f]);
						}
						fprintf(pf,"\n");

						player.drawSkeletonInColorCoordinates(color_img);
						cvShowImage("player",color_img);
						cvWaitKey(10);
						cvReleaseImage(&color_img);
						cvReleaseImage(&depth_img);
						cvReleaseImage(&color_mask);
						cvReleaseImage(&depth_mask);
						db.increaseCnt();
					}
				}

				fclose(pf);
			}
			cout << " 학습 및 테스트 데이터 구축 완료 [엔터]" << endl;
			getchar();
		}

		/*
			SVM 학습하기
		*/
		else if(menu == '5') {
			int key = 0;
			int type = 0;
			cout << " 학습할 포즈 수  > " ;
			cin >> total_n_pose;

			while(type != 1 && type != 2) {
				cout << " 선택하세요. 선형[1] 비선형[2] > " ;
				cin >> type;
			}

			if(type == 1) {
				for(int i=0; i<total_n_pose; i++) {
					sprintf(text,"%d",i);
					ShellExecute(NULL, "open", "learning_linear.bat", text, NULL, SW_SHOW);  
				}
			}
			else {
				for(int i=0; i<total_n_pose; i++) {
					sprintf(text,"%d %f",i,gamma[i]);
					ShellExecute(NULL, "open", "learning.bat", text, NULL, SW_SHOW);  
				}
			}
		}

		/*
			SVM 검증집합 성능 체크
		*/
		else if(menu == '6') {
			int key = 0;
			int type = 0;
			cout << " 학습한 포즈 수  > " ;
			cin >> total_n_pose;

			for(int i=0; i<total_n_pose; i++) {
				sprintf(text,"%d",i);
				ShellExecute(NULL, "open", "test.bat", text, NULL, SW_SHOW);  
			}
		}

		/*
			실시간 포즈 인식
		*/
		else if(menu == '7') {
			cout << "메뉴복귀 : esc 키 누르기 (opencv 윈도우 활성화된 상태에서) " << endl;

			int key = 0;
			
			cout << " 인식 포즈 개수 ? > " ;
			cin >> total_n_pose;

			SVM_Classifier *svm = new SVM_Classifier[total_n_pose];
			for(int i=0; i<total_n_pose; i++) {
				sprintf(text,"../db/svm/%dsv_model.txt",i);
				svm[i].load_param(text,feature_dimension);
			}

			CvFont font;
			cvInitFont(&font,CV_FONT_HERSHEY_PLAIN, 1.1,1.1,0,2);
			
			while(1) {
				
				kinect.videoImage(frame);
				kinect.depth8BitImage(depth8);
				kinect.depthPlayerImage(depthPlayer);
				
				int index = kinect.findNearestPlayerNum();

				if(index != -1 && kinect.getPlayer(index).isInitialized()) {
					KinectPlayer player = kinect.getPlayer(index);
					
					player.drawSkeletonInDepthCoordinates(depthPlayer);
					
					Vector4 *positions = player.getLocalizedSkeletonPositions(); //상대좌표
					transform(positions,features,feature_dimension); //특징추출
					
					int max =-10E10;
					int max_index=-1;
					for(int i=0; i<total_n_pose; i++) {
						double val = svm[i].classify(features);
						if(val >max) {
							max = val;
							max_index = i;
						}
						
					}
					printf("pose %d\n",max_index);
					Vector4 v = player.getSkeletonDepthPosition(NUI_SKELETON_POSITION_HEAD);
					sprintf(text,"%d pose", max_index);
					cvPutText (depthPlayer,text,cvPoint(v.x-50,v.y-30), &font, cvScalar(255,255,0));
				}					
				cvShowImage("players",depthPlayer);
				key = cvWaitKey(10);
				if(key==27 || key=='q' || key=='Q') {
					delete [] svm;
					break;
				}
			}
		}

		else if(menu == 'u') {
			kinect.setCameraElevationAngle(angle+=5);
		}

		else if(menu == 'd') {
			kinect.setCameraElevationAngle(angle-=5);
		}

		else if(menu == 'q')
			break;
	}



}
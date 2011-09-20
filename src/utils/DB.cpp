#include "utils/DB.h"
#include <cstdio>
#include <direct.h>
#include <assert.h>
namespace Recogbot {

	DB::DB() {
		_cntImg=0;
		_folderName="none";
	}

	void DB::createFolder(const char* name){
		
		sprintf(_text,"../db/%s",name);
		mkdir(_text);
	}

	void DB::locateFolder(const char* name){
		_folderName=name;
	}

	void DB::saveImg(IplImage* img){
		assert(strcmp(_folderName.c_str(),"none")!=0);
		assert(img!=0);

		createFolder(_folderName.c_str());
			
		sprintf(_text,"../db/%s/%05d.jpg",_folderName.c_str(),_cntImg++);
		cvSaveImage(_text,img);
	}

	void DB::saveImgWithCnt(IplImage* img, unsigned cnt){
		assert(img!=0);

		createFolder(_folderName.c_str());

		sprintf(_text,"../db/%s/%05d.jpg",_folderName.c_str(),cnt);
		cvSaveImage(_text,img);
	}

	IplImage* DB::readImg(){
		sprintf(_text,"../db/%s/%05d.jpg",_folderName.c_str(),_cntImg++);
		return cvLoadImage(_text);
	}

	IplImage* DB::readImgwithCnt(unsigned cnt){
		sprintf(_text,"../db/%s/%05d.jpg",_folderName.c_str(),cnt++);
		return cvLoadImage(_text);
	}
}
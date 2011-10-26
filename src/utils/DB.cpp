#include "utils/DB.h"
#include <cstdio>
#include <direct.h>
#include <assert.h>
namespace Recogbot {

	DB::DB(char *folderName) {
		_cnt=0;
		_folderName=folderName;
	}

	void DB::createFolder(const char* name){
		
		sprintf(_text,"../db/%s",name);
		mkdir("../db");
		mkdir(_text);
	}

	void DB::locateFolder(const char* name){
		_folderName=name;
	}

	void DB::saveImg(IplImage* img, const char* tag){
		assert(strcmp(_folderName.c_str(),"none")!=0);
		assert(img!=0);

		createFolder(_folderName.c_str());
			
		sprintf(_text,"../db/%s/%05d_%s.jpg",_folderName.c_str(),_cnt, tag);
		cvSaveImage(_text,img);
	}

	void DB::saveImgWithCnt(IplImage* img, const char* tag, unsigned cnt){
		assert(img!=0);

		createFolder(_folderName.c_str());

		sprintf(_text,"../db/%s/%05d_%s.jpg",_folderName.c_str(),_cnt, tag);
		cvSaveImage(_text,img);
		_cnt = cnt;
	}

	IplImage* DB::readImg(const char* tag){
		sprintf(_text,"../db/%s/%05d_%s.jpg",_folderName.c_str(),_cnt, tag);
		return cvLoadImage(_text);
	}

	IplImage* DB::readGrayImg(const char* tag){
		sprintf(_text,"../db/%s/%05d_%s.jpg",_folderName.c_str(),_cnt, tag);
		return cvLoadImage(_text,0);
	}

	IplImage* DB::readImgwithCnt(const char* tag, unsigned cnt){
		sprintf(_text, "../db/%s/%05d_%s.jpg", _folderName.c_str(), cnt, tag);
		return cvLoadImage(_text);
	}

	void DB::saveArray(void *arrPtr, const char* tag, size_t nBytes, size_t nElements) {
		sprintf(_text, "../db/%s/%05d_%s.arr", _folderName.c_str(), _cnt, tag );
		FILE* pf = fopen(_text,"wb");
		fwrite(arrPtr, nBytes, nElements, pf);
		fclose(pf);
	}

	void DB::saveArray(void *arrPtr, const char* tag, size_t nBytes, size_t nElements, unsigned cnt) {
		sprintf(_text, "../db/%s/%05d_%s.arr", _folderName.c_str(), cnt, tag);
		FILE* pf = fopen(_text, "wb");
		fwrite(arrPtr, nBytes, nElements, pf);
		fclose(pf);
	}

	void DB::readArray(void *arrPtr, const char* tag, size_t nBytes, size_t nElements) {
		sprintf(_text, "../db/%s/%05d_%s.arr", _folderName.c_str(), _cnt, tag);
		FILE* pf = fopen(_text, "rb");
		fread(arrPtr, nBytes, nElements, pf);
		fclose(pf);
	}

	void DB::readArray(void *arrPtr, const char* tag, size_t nBytes, size_t nElements, unsigned cnt) {
		sprintf(_text, "../db/%s/%05d_%s.arr", _folderName.c_str(), cnt, tag);
		FILE* pf = fopen(_text, "rb");
		fread(arrPtr, nBytes, nElements, pf);
		fclose(pf);
	}

	void DB::setCnt(unsigned val) {	_cnt = val; }

	void DB::initCnt() { _cnt = 0; }

	unsigned DB::getCnt() { return _cnt; }

	void DB::increaseCnt() { ++_cnt; }

	void DB::decreaseCnt() { --_cnt; }
}
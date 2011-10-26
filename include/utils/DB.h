/*
	CNU, IIMIL
	Chi-Min Oh

	Created in September, 2011 by Chi-Min Oh (sapeyes@image.chonnam.ac.kr)
*/

#ifndef RECOGBOT_UTILS_DB_H
#define RECOGBOT_UTILS_DB_H
#include <string>
#include "opencv2/opencv.hpp"

namespace Recogbot {
	class DB {
	public:
		DB(char *folderName="none");

		void createFolder(const char* name);

		void locateFolder(const char* name);

		void saveImg(IplImage* img, const char* tag);

		void saveImgWithCnt(IplImage* img, const char* tag, unsigned cnt);

		IplImage* readImg(const char* tag);

		IplImage* readImgwithCnt(const char* tag, unsigned cnt);

		IplImage* readGrayImg(const char* tag);

		void saveArray(void *arrPtr, const char* tag, size_t nBytes, size_t nElements);

		void saveArray(void *arrPtr, const char* tag, size_t nBytes, size_t nElements, unsigned cnt);

		void readArray(void *arrPtr, const char* tag, size_t nBytes, size_t nElements);

		void readArray(void *arrPtr, const char* tag, size_t nBytes, size_t nElements, unsigned cnt);

		void setCnt(unsigned val);

		void initCnt();

		unsigned getCnt();

		void increaseCnt();

		void decreaseCnt();

	protected:

		unsigned _cnt;

		std::string _folderName;

		char _text[256];
	};

	
}
#endif
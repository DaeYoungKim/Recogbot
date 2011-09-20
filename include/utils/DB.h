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
		DB();

		void createFolder(const char* name);

		void locateFolder(const char* name);

		void saveImg(IplImage* img);

		void saveImgWithCnt(IplImage* img, unsigned cnt);

		IplImage* readImg();

		IplImage* readImgwithCnt(unsigned cnt);

		void setCnt(unsigned val);

		void initCnt();

	private:
		unsigned _cntImg;
		std::string _folderName;
		char _text[256];
	};
}
#endif
#pragma once
#include <stdlib.h>     

#include <gl/glut.h>
#include <cv.h>
#include <cxcore.h>
#include <windows.h>
class OpenglWindow
{
public:
	OpenglWindow(HINSTANCE instance, HWND hWnd, int w, int h);
	~OpenglWindow(void);


	HWND m_hWnd;
	HWND m_hWndDlg;
	HWND m_hWndPic;
	int m_width;
	int m_height;
	bool bIsThreadRunning;
	GLuint m_texture;
	GLUquadricObj *m_newcy;
	
	CCamera objCamera; 
	CArticulatedObject *p_aobj;
	IplImage* lab[3];

	void DoGesture(string gesture);

	static void DrawCylinder(double x, double y, double z,double width, double height, double angle);


private:
	static OpenglWindow *pInstance;
	static BOOL CALLBACK DialogProc1(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static void CALLBACK ThreadFunc(void *arg);

};


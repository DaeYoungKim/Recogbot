#include "OpenglWindow.h"
#include "resource1.h"
#include <process.h>
#include <highgui.h>
void EnableOpenGL (HWND hWnd, HDC *hDC, HGLRC *hRC)
{
	PIXELFORMATDESCRIPTOR pfd;
	int iFormat;

	/* get the device context (DC) */
	*hDC = GetDC (hWnd);

	/* set the pixel format for the DC */
	ZeroMemory (&pfd, sizeof (pfd));
	pfd.nSize = sizeof (pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | 
		PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	iFormat = ChoosePixelFormat (*hDC, &pfd);
	SetPixelFormat (*hDC, iFormat, &pfd);

	/* create and enable the render context (RC) */
	*hRC = wglCreateContext( *hDC );
	wglMakeCurrent( *hDC, *hRC );

}

void DisableOpenGL (HWND hWnd, HDC hDC, HGLRC hRC)
{
	wglMakeCurrent (NULL, NULL);
	wglDeleteContext (hRC);
	ReleaseDC (hWnd, hDC);
}



OpenglWindow *OpenglWindow::pInstance = 0;

OpenglWindow::OpenglWindow(HINSTANCE instance, HWND hWnd, int w, int h) 
{
	m_hWnd = hWnd;
	m_width = w;
	m_height = h;
	pInstance = this;

	if(!IsWindow(m_hWndDlg))
		m_hWndDlg = CreateDialog(instance,MAKEINTRESOURCE(IDD_DIALOG1),hWnd,(DLGPROC)DialogProc1);

	ShowWindow(m_hWndDlg,SW_SHOW);
	SendMessage(m_hWndDlg,WM_SIZE,w,h);
	RECT r;
	m_hWndPic = GetDlgItem(m_hWndDlg,IDC_STATIC_PIC);
	GetWindowRect(m_hWnd,&r);
	if(hWnd==0)
		SetWindowPos(m_hWndDlg, m_hWndDlg, 0,0,w+6,h+31,SWP_NOZORDER);
	else
		SetWindowPos(m_hWndDlg, m_hWndDlg, r.left+20,r.top+18,w+6,h+31,SWP_NOZORDER);
	SetWindowPos(m_hWndPic, m_hWndPic, 0, 0,w,h,SWP_NOZORDER);
	bIsThreadRunning=true;
	CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadFunc,0,0,0);

	m_newcy =gluNewQuadric();
	//m_bHasImage = false;
	p_aobj = NULL;
	objCamera.Position_Camera(0, 15.0f, 100.0f,	0, 15.0f, 0,   0, 1, 0);
//	m_img = cvCreateImage(cvSize(640,480),8,3);
	lab[0] = cvLoadImage("1.jpg");
	lab[1] = cvLoadImage("2.jpg");
	lab[2] = cvLoadImage("3.jpg");
	cvFlip(lab[0],lab[0]);
	cvFlip(lab[1],lab[1]);
	cvFlip(lab[2],lab[2]);
}

OpenglWindow::~OpenglWindow(void)
{
	cvReleaseImage(&lab[0]);
	cvReleaseImage(&lab[1]);
	cvReleaseImage(&lab[2]);
}

BOOL CALLBACK OpenglWindow::DialogProc1(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static RECT rt;

	switch(uMsg)
	 {
		case WM_INITDIALOG :
			return TRUE;
		
		case WM_SIZE :
			GetClientRect(hWndDlg, &rt);
			InvalidateRect(hWndDlg, NULL, TRUE);
			return 0;

		case WM_CLOSE:
			pInstance->bIsThreadRunning = false;
			DestroyWindow(hWndDlg);
			return 0;

	 }
	
	 return FALSE;
}


void InitGL(int cx, int cy, int w, int h, int thredid)
{
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)w/(GLfloat)h,0.1f,300.0f);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	

	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really Nice Perspective    Calculations
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	//glEnable(GL_TEXTURE_2D);
}

void Draw_Grid()
{															

	for(float i = -500; i <= 500; i += 5)
	{
		glBegin(GL_LINES);
			glColor3ub(150, 190, 150);						
			glVertex3f(-500, 0, i);					
			glVertex3f(500, 0, i);
			glVertex3f(i, 0,-500);							
			glVertex3f(i, 0, 500);
		glEnd();
	}
}

void CALLBACK OpenglWindow::ThreadFunc(void *arg)
{
	int w = pInstance->m_width;
	int h = pInstance->m_height;
	HGLRC hRC;
	HDC hDC;
	EnableOpenGL(pInstance->m_hWndPic, &hDC, &hRC);
	RECT rect;
	
	GetClientRect(pInstance->m_hWndPic,&rect);
	InitGL(rect.right, rect.bottom,rect.right-rect.left,rect.bottom-rect.top,0);

	GLfloat ambientLight[] = { 0.9f, 0.0f, 0.0f, 1.0f };     // <1>    
	GLfloat diffuseLight[] = { 0.9f, 0.0f, 0.0f, 1.0f };       // <2>    
	GLfloat lightPos[] = { w/2, h/2, 350, 1.0f };    // <3>    

	glGenTextures(1, &pInstance->m_texture);
	glBindTexture(GL_TEXTURE_2D, pInstance->m_texture);
	glEnable(GL_TEXTURE_2D);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);


	for(;;)
	{	
		if(!pInstance->bIsThreadRunning)
			break;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Screen And Depth    Buffer
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity(); // Reset The Current Modelview Matrix

		gluLookAt(pInstance->objCamera.mPos.x,  pInstance->objCamera.mPos.y,  pInstance->objCamera.mPos.z,	
			  pInstance->objCamera.mView.x, pInstance->objCamera.mView.y, pInstance->objCamera.mView.z,	
			  pInstance->objCamera.mUp.x,   pInstance->objCamera.mUp.y,   pInstance->objCamera.mUp.z);


		Draw_Grid();

		
		//glPushMatrix();
		//glTranslatef(0,-10.0f,0);
		glEnable(GL_TEXTURE_2D);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,pInstance->lab[0]->width,pInstance->lab[0]->height,0,GL_BGR_EXT,GL_UNSIGNED_BYTE,pInstance->lab[0]->imageData);
		glBegin(GL_POLYGON);						
			//glColor3f(1.0f,0.0f,0.0f);			
			glTexCoord2f(0.0f, 1.0f);glVertex3f( 57.5f, 30.0f, 0.0f);		
			glTexCoord2f(0.0f, 0.0f);glVertex3f( 57.5f, 0.0f, 0.0f);		
			glTexCoord2f(1.0f, 0.0f);glVertex3f( 57.5f, 0.0f, 115.0f);		
			glTexCoord2f(1.0f, 1.0f);glVertex3f( 57.5f, 30.0f, 115.0f);					
		glEnd();
		glFlush();	
		glDisable(GL_TEXTURE_2D);

		glEnable(GL_TEXTURE_2D);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,pInstance->lab[1]->width,pInstance->lab[1]->height,0,GL_BGR_EXT,GL_UNSIGNED_BYTE,pInstance->lab[1]->imageData);
		glBegin(GL_POLYGON);						
			//glColor3f(0.0f,1.0f,0.0f);			
			glTexCoord2f(0.0f, 1.0f);glVertex3f( -57.5f, 30.0f, 0.0f);		
			glTexCoord2f(0.0f, 0.0f);glVertex3f( -57.5f, 0.0f, 0.0f);		
			glTexCoord2f(1.0f, 0.0f);glVertex3f( 57.5f, 0.0f, 0.0f);		
			glTexCoord2f(1.0f, 1.0f);glVertex3f( 57.5f, 30.0f, 0.0f);		
		glEnd();
		glFlush();	
		glDisable(GL_TEXTURE_2D);

		glEnable(GL_TEXTURE_2D);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,pInstance->lab[2]->width,pInstance->lab[2]->height,0,GL_BGR_EXT,GL_UNSIGNED_BYTE,pInstance->lab[2]->imageData);
		glBegin(GL_POLYGON);						
			//glColor3f(0.0f,0.0f,1.0f);			
			glTexCoord2f(0.0f, 1.0f);glVertex3f( -57.5f, 30.0f, 115.0f);		
			glTexCoord2f(0.0f, 0.0f);glVertex3f( -57.5f, 0.0f, 115.0f);					
			glTexCoord2f(1.0f, 0.0f);glVertex3f( -57.5f, 0.0f, 0.0f);					
			glTexCoord2f(1.0f, 1.0f);glVertex3f( -57.5f, 30.0f, 0.0f);					
		glEnd();
		glFlush();	
		glDisable(GL_TEXTURE_2D);
		                                 
	

		

		//glEnable(GL_LIGHTING);                                     // <4>    
		//glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);            // <5>    
		//glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);            // <6>    
		//glLightfv(GL_LIGHT0, GL_POSITION, lightPos);               // <7>    
		//glEnable(GL_LIGHT0);      

		/*if(pInstance->p_aobj)
		{
			for(int i=0; i<=Part_Arm_RightDown; i++)
			{
				double x = pInstance->p_aobj->parts[i].x;
				double y = pInstance->p_aobj->parts[i].y;
				double w = pInstance->p_aobj->parts[i].sx * pInstance->p_aobj->parts[i].w;
				double h = pInstance->p_aobj->parts[i].sy * pInstance->p_aobj->parts[i].h;
				double angle = pInstance->p_aobj->parts[i].rz*57.2957795-90 ;
				switch(i)
				{
				case Part_Head:
					y -= h;
					break;
				case Part_Arm_LeftUp:
				case Part_Arm_LeftDown:
					x -= w/2;
					break;
				case Part_Arm_RightUp:
				case Part_Arm_RightDown:
					x += w/2;

				
				}
				DrawCylinder(x,y,0,w,h,angle);
			}		
		}*/
		

		SwapBuffers (hDC);
		Sleep (10);

	}

	DisableOpenGL(pInstance->m_hWndDlg, hDC, hRC);
}

void OpenglWindow::DrawCylinder(double x, double y, double z,double width, double height, double angle)
{
	glPushMatrix();
	glTranslated(x,y,z);
	glRotated(-angle+90,0.0,0.0,1.0);
	glRotated(90,1.0,0.0,0.0);
	gluQuadricDrawStyle(pInstance->m_newcy, GLU_FILL); // 선(glut의 wire)으로 하려면 GLU_LINE
	gluCylinder(pInstance->m_newcy,width/2,width/2,height,32,4); // 원기둥
	glPopMatrix();
	//delete newcy;
}
//
//void OpenglWindow::DrawImage(IplImage *img)
//{
//	pInstance->m_bHasImage = true;
//	pInstance->m_img = img;
//}

void OpenglWindow::DoGesture(string gesture)
{
	if(gesture.compare("Turn Right")==0)
		objCamera.Rotate_View(CAMERASPEED);
	else if(gesture.compare("Turn Left")==0)
		objCamera.Rotate_View(-CAMERASPEED);
	else if(gesture.compare("Go Forward")==0)
		objCamera.Move_Camera(CAMERASPEED);
}
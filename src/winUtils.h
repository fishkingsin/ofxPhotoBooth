#ifndef _WIN_UTILS
#define _WIN_UTILS

#include "ofMain.h"

static string	windowTitle;
static int		storedWindowX;
static int		storedWindowY;
static int		storedWindowW;
static int		storedWindowH;
static bool		bCustomFullscreen = false;

//--------------------------------------
static void ofBeginCustomFullscreen(int x, int y, int width, int height){

	if (ofGetWindowMode() == OF_GAME_MODE) return;

	#ifdef TARGET_WIN32

	    HWND vWnd  = FindWindow(NULL,  "ofxPhotoBooth");

		if(!bCustomFullscreen){

			RECT r;

			if(GetWindowRect(vWnd, &r)){
				//more accurate as it takes windows elements into account
				storedWindowX = r.left;
				storedWindowY = r.top;
				storedWindowW = r.right - r.left;
				storedWindowH = r.bottom - r.top;
			}else{
				//fallback
				storedWindowX = 0;
				storedWindowY = 0;
				storedWindowW = ofGetWidth();
				storedWindowH = ofGetHeight();
			}

		}

	    long windowStyle = GetWindowLong(vWnd, GWL_STYLE);

	    windowStyle &= ~WS_OVERLAPPEDWINDOW;
	    windowStyle |= WS_POPUP;

	   	SetWindowLong(vWnd, GWL_STYLE, windowStyle);

		SetWindowPos(vWnd, HWND_TOPMOST, x, y, width, height, SWP_FRAMECHANGED);
		//SetWindowPos( vWnd, HWND_TOPMOST, x, y, width, height, SWP_NOREPOSITION | SWP_NOSIZE );
		bCustomFullscreen = true;

	#endif
}

//--------------------------------------
static void  ofEndCustomFullscreen(){

	if (ofGetWindowMode() == OF_GAME_MODE) return;

	#ifdef TARGET_WIN32

		if(bCustomFullscreen){
			HWND vWnd  = FindWindow(NULL, "ofxPhotoBooth");

		    long windowStyle = GetWindowLong(vWnd, GWL_STYLE);
			windowStyle  |= WS_OVERLAPPEDWINDOW;
		    windowStyle  &= ~WS_POPUP;


			SetWindowLong(vWnd, GWL_STYLE, windowStyle);
			SetWindowPos(vWnd, HWND_TOP,storedWindowX, storedWindowY, storedWindowW, storedWindowH, SWP_FRAMECHANGED);
			bCustomFullscreen = false;
		}

	#endif
}

static void ofSetWindowAlwaysOnTop() {
#ifdef TARGET_WIN32

    //get its handle "GLUT" = class name "ogl" = window
    HWND hwnd = FindWindow( NULL, "PhotoBooth" );

    SetWindowPos( hwnd, HWND_TOPMOST, NULL, NULL, NULL, NULL, SWP_NOREPOSITION | SWP_NOSIZE );
#endif

#ifdef TARGET_OSX
    // What goes here??
#endif

#ifdef TARGET_LINUX
    // What goes here?
#endif
}


#endif


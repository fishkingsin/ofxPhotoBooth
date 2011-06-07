#include "ofMain.h"
#include "ofAppGlutWindow.h"
#include "testApp.h"

#include <stdio.h>
#include <stdlib.h>
//#define TEST_LOG
//========================================================================


int main( )
{
    /*HINSTANCE hInst = ShellExecute(0,
                                       "open",                      // Operation to perform
                                       "./ofxAppMonitor.exe",  // Application name
                                       "",           // Additional parameters
                                       0,                           // Default directory
                                       SW_MINIMIZE  );

            hInst = ShellExecute(0,
                                       "open",                      // Operation to perform
                                       "./flosc_2.0.4c-fullXML.jar",  // Application name
                                       "",           // Additional parameters
                                       0,                           // Default directory
                                       SW_MINIMIZE  );

            hInst = ShellExecute(0,
                                       "open",                      // Operation to perform
                                       "./CoreControllUI/CoreControllUI.exe",  // Application name
                                       "",           // Additional parameters
                                       0,                           // Default directory
                                       SW_SHOW  );*/
#ifdef TEST_LOG
    /*create a new text file for logging with data and time as the name*/

    /* alright first we need to get time and date so our logs can be ordered*/
    char dateStr [9];
    char timeStr [9];

    /*time stuff */
    time_t rawtime;
    struct tm * timeinfo;
    char fileName [80];

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    strftime (fileName,80,"logs/log_%B_%d_%y_%H_%M_%S.txt",timeinfo);

    FILE *stream ;
    if ((stream = freopen(fileName, "w", stdout)) == NULL)
    {
        ofLog(OF_LOG_ERROR,"Can not open %s",fileName);
    }
    //exit(-1);
#endif


    // this kicks off the running of my app
    // can be OF_WINDOW or OF_FULLSCREEN
    // pass in width and height too:

#ifdef USE_OFXFENSTER
    ofAppGlutWindow window;
	ofSetupOpenGL(&window, 1024,768, OF_WINDOW);
    testApp* app = new testApp();
    ofxFenster fenster;
    fenster.init(app,"PhotoBooth Fenster");
    ofRunApp(app);
#else
    ofSetupOpenGL(640,480, OF_WINDOW);			// <-------- setup the GL context
    ofRunApp(new testApp());
#endif


#ifdef TEST_LOG
    stream = freopen("CON", "w", stdout);
#endif


}


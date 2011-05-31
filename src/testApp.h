#ifndef _TEST_APP
#define _TEST_APP

//note 20101007
//add svaeJPG in ofxImage
//ofxFOBTextureSaver changed save jpg / png opertation
#include "ofMain.h"
//#include "ofxFlashCommunication.h"
#ifdef USE_AR
#define MY_MODELL_HELPER
#include "ARHelper.h"
#ifdef MY_MODELL_HELPER
#include "myModelHandler.h"
#else
#include "ModelHandler.h"
#endif
#endif

#include "ofx3DModelLoader.h"
#ifdef TARGET_WIN32
#ifdef USE_GSTREAMER
#include "ofVideoPlayerGst.h"
#endif//USE_GSTREAMER
#else if TARGET_OSX
//#include "ofxQTKitVideoTexture.h"
#endif //TARGET_WIN32

//#include "ofxVBO.h"

#include "global.h"



#define MAXPATH 1024
//#define DEBUG
//#define  INSTRUCTION
#include "CameraHandler.h"
#include "myNetworkHandler.h"
#include "ofxLogtime.h"
#include "ofxAutoShutdown.h"

#include "CountDown.h"

#include "HeartBeatThread.h"

#define COPY_TEST

#include "winUtils.h"
class testApp : public ofBaseApp , public ofxThread
{

public:
    ~testApp();
    void setup();
    void update();
    void draw();
    void drawTwoImage(ofBaseDraws *baseDraw);

    void keyPressed  (int key);
    void keyReleased(int key);
    void mousePressed(int x, int y, int button);
    void windowResized(int w, int h);
    void threadedFunction();


//events
    void fadeIn(ofEventArgs &e);

    void networkMessage(OSCEventArgs &e);
    void afterSave(ofEventArgs &e);
    void onComposImage(ofEventArgs &e);
//utils
    void setUpInstructionVideo();
    void clearStorage();
    void clearLog();
    void sendMail(string addr);
    void LiveView();

    bool bWideScreen;
#ifdef USE_AR
    void arModelActived(ofAREventArgs &e);
    void arModelDeactived(ofAREventArgs &e);
    void initModel();
    void reloadModel(int numModel, int *__id, string *__fn);

    ARHelper *arhelper;

    vector <int> nID;
    vector <string> nModel;
    bool useAR;
#ifdef MY_MODELL_HELPER
    myModelHandler *modelHandler;
#else
    ModelHandler *modelHandler;
#endif

#endif

    bool bChangeModel,bComposPhoto;

    ofImage loading;
    ofImage intro;
    ofxFadableBase fadable;

#ifdef INSTRUCTION
#ifdef TARGET_WIN32

#ifdef USE_GSTREAMER
    ofVideoPlayerGst player;
#else
    ofVideoPlayer player;
#endif //USE_GSTREAMER

#else if TARGET_OSX
    ofVideoPlayer player;


#endif //TARGET_WIN32
#endif
    ofxTimer timer;

    ofxXmlSettings xml;

    bool bSetup;
    int vw,vh;
    int x,y;
    float tol;
    float timeout;
    float maskA;
    bool bLoaded;
    bool bText;

#ifdef INSTRUCTION
    ofxShader shaderBGSub;

    ofTexture myTexture,myTexture2;

    ofImage secondImage;
#endif
    ofTrueTypeFont myFont;
//------------------------------------------FBO saver;
    ofxFBOTextureSaver fboSaver;
    ofxFBOTextureSaver fboSaverL;
    int appWidth,appHeight;
    int fboWidth,fboHeight,fboX,fboY;
    bool bCapture;

    CameraHandler camera;
    string cap_path,output_path;

    myNetworkHandler network;

    ofxXmlSettings oscXML;

    string				tmpLocalHost;
    int					tmpPort;
    int					tmpFlashPort;

    string bg_fn,fg_fn;
    bool                bNETWORKMode;
    bool bChangeBG,bChangeFG;
    bool fgLoaded;
    ofImage fgImg;

    //indicator
    bool bShow4R;
    bool bShow6R;
    bool binProgress;
    ofImage _4R;
    ofImage _6R;
    ofImage nowLoading;

    //captures
    bool bShow;
    string lastCaptureFile;
    string sendImagesFile;
    string target_photo_path;
    ofxImage lastCapImage,tempCap;
    bool canLoadImage;
    int MAX_CAP_FILE;
    bool bMirrorMaster;
    float offsetX,offsetY,LiveViewScale;
    bool bLogTime;
    ofxAutoShutdown autoshutdown;
    ofImage *downloadedImage;


    bool bPause;
    ofSerial serial;
    ofxTimer idel;
    void setPause(bool _pause);
    void onTimesUp(ofEventArgs &args);
    void resetIdel();
    int previewX,previewY,previewW,previewH;
    int cWidth,cHeight;
    bool bFullPath;
//counter
    CountDown countDown;
    void Shot(ofEventArgs &arg);
    void Counting(ofEventArgs &arg);
    bool bLoadImport;
    string importName;
    int cropArea[2];

    int cropAlpha;
    ofImage screenSaver;

//font
    ofTrueTypeFont	helvetica;


// log
    int selectLogLevel;
    int currentLogLevel;
    string LogName[6];

    //HeartBeatThread
    HeartBeatThread heartbeat;

};

#endif

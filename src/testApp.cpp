#include "testApp.h"
#include "PBModel.h"
#ifdef TARGET_WIN32
#include <windows.h>
#endif
#include <stdio.h>
#ifdef TARGET_OSX
#include <mach-o/dyld.h>
#include <libgen.h>
#endif
static bool bDebug;
static bool bFlipFinal;
static void copyImage(const char *_src, const char *_des)
{
#ifdef TARGET_WIN32
    char cmd[256];
    sprintf(cmd,"copy %s %s",_src,_des);

    for (int i = 0 ; i < 256 ; i++)
    {
        if (cmd[i]=='/') cmd[i]='\\';
    }

    system(cmd);
#else
    uint32_t bufsize = 4096;
    char exepath[4096];
    int result = _NSGetExecutablePath(exepath, &bufsize);
    if (result == 0)   // _NSGetExecutablePath() worked
    {
        char command[4096];
        sprintf(command, "/bin/cp %s %s", _src,_des);
        result = system(command);
        if (result == -1)
        {
            printf("Error command: %s\n",command);
            // error checking here
        }
    }
#endif
}

static string today_str;
static string target_photo_path;
//--------------------------------------------------------------
testApp::~testApp()
{
    heartbeat.stopThread();
    ofLog(OF_LOG_VERBOSE,"testApp::~testApp()");
}
//--------------------------------------------------------------
void testApp::setup()
{


    if ( xml.loadFile("config.xml"))
    {
        ofLog(OF_LOG_VERBOSE,"config.xml loaded");
    }
    else
    {
        ofLog(OF_LOG_ERROR,"Could not Found arconfig.xml");
    }
    //heart beat

    heartbeat.startThread(true,false);

    bComposPhoto = false;
    bChangeModel = false;
    binProgress = false;

    downloadedImage = NULL;
    _4R.loadImage("images/_4R.png");
    _6R.loadImage("images/_6R.png");
    nowLoading.loadImage("images/nowLoading.png");
    myFont.loadFont("fonts/verdana.ttf", 24);
    helvetica.loadFont("fonts/Helvetica.Ttf", 24);

    screenSaver.loadImage("images/ScreenSaver.png");
    ofBackground(0,0,0);
    lastCaptureFile = "unknow_barcode";


    cWidth = xml.getAttribute("SETTINGS:CCANVAS","width",1280);
    cHeight = xml.getAttribute("SETTINGS:CCANVAS","height",720);

    bDebug = false;

    currentLogLevel = selectLogLevel = xml.getValue("SETTINGS:DEBUG",0);
    LogName[0] = "OF_LOG_VERBOSE";
    LogName[1] = "OF_LOG_NOTICE";
    LogName[2] = "OF_LOG_WARNING";
    LogName[3] = "OF_LOG_ERROR";
    LogName[4] = "OF_LOG_FATAL_ERROR";
    LogName[5] = "OF_LOG_SILENT";
    switch (currentLogLevel)
    {
    case 0:
        ofSetLogLevel(OF_LOG_VERBOSE);
        bDebug = true;
        break;
    case 1:
        ofSetLogLevel(OF_LOG_NOTICE);
        break;
    case 2:
        ofSetLogLevel(OF_LOG_WARNING);
        break;
    case 3:
        ofSetLogLevel(OF_LOG_ERROR);
        break;
    case 4:
        ofSetLogLevel(OF_LOG_ERROR);
        break;
    case 5:
        ofSetLogLevel(OF_LOG_SILENT);
        break;

    }

    ofHideCursor();
    //=======================
    //delete captures storage
    MAX_CAP_FILE = xml.getValue("SETTINGS:MAX_CAPTURES",200);
    sendImagesFile  = xml.getValue("SETTINGS:IMAGE_NAME","image.jpg");

    //=======================
    clearLog();
    if (xml.getValue("SETTINGS:ALWAYSONTOP",false))ofSetWindowAlwaysOnTop();
    int width = xml.getAttribute("SETTINGS:CANVAS","width",1920);
    int height = xml.getAttribute("SETTINGS:CANVAS","height",1080);
    int camW = xml.getAttribute("SETTINGS:CAMERA","width",640);
    int camH = xml.getAttribute("SETTINGS:CAMERA","height",480);
#ifdef USE_AR
    useAR = xml.getValue("SETTINGS:AR",false);
#endif
    x = xml.getAttribute("SETTINGS:CANVAS","x",0);
    y = xml.getAttribute("SETTINGS:CANVAS","y",0);
    //path setup
    cap_path = xml.getAttribute("SETTINGS:CAMERA","capture_path","captures/");

    //make time stemp folder
    today_str = ofToString(ofGetYear())+"_"+ofToString(ofGetMonth())+"_"+ofToString(ofGetDay());

    char cmd[256];
    sprintf(cmd,"mkdir %s%s",ofToDataPath(cap_path,true).c_str(),today_str.c_str());
#ifdef TARGET_WIN32
    for (int i = 0 ; i < 256 ; i++)
    {
        if (cmd[i]=='/') cmd[i]='\\';
    }
#endif
    printf(cmd);
    system(cmd);


    output_path = xml.getAttribute("SETTINGS:PHOTOSAVER","output_path","output/");

    countDown.setup(xml.getValue("SETTINGS:COUNTER",1));
    ofAddListener(countDown.COUNTER_REACHED,this,&testApp::Shot);
    ofAddListener(countDown.COUNTER_COUNTING,this,&testApp::Counting);
    clearStorage();
    ofSetWindowShape(width,height);
    //ofSetFullscreen(xml.getValue("SETTINGS:FULLSCREEN",false));

    ofSetWindowPosition(x,y);
    ofSetVerticalSync(true);
    ofEnableSmoothing();
    ofEnableAlphaBlending();
    ofSetWindowTitle("ofxPhotoBooth");

    //SET WINDOWS borderless

    glShadeModel (GL_SMOOTH);
    gui.addTitle("PhotoBooth");
    // overriding default theme
    gui.config->toggleHeight = 18;
    gui.config->buttonHeight = 18;
    gui.config->sliderTextHeight = 22;
    gui.config->titleHeight = 18;
    gui.config->fullActiveColor = 0x005500;
    gui.addComboBox("LogLevel", selectLogLevel, 6, LogName);
    gui.addSlider("timeout",timeout,0.00001f,10.0f);// = xml.getAttribute("SETTINGS:SCREENSAVER","timeout",1);
    gui.addSlider("LiveViewScale",LiveViewScale,0.00001f,10.0f);// = xml.getAttribute("SETTINGS:SCREENSAVER","timeout",1);
    gui.addSlider("offsetX",offsetX,-cWidth/2,cWidth/2);
    gui.addSlider("offsetY",offsetY,-cHeight/2,cHeight/2);
    gui.addSlider("maskA",maskA,1,255);
    gui.addButton("bCapture",bCapture);
    gui.addToggle("Mirror Master",bMirrorMaster);
    gui.addToggle("bPause",bPause);
    gui.addToggle("bLogTime",bLogTime);

    gui.addSlider("cropArea1",cropArea[0],0,1000);
    gui.addSlider("cropArea2",cropArea[1],0,1000);

    gui.addSlider("cropAlpha",cropAlpha,0,255);
    gui.addToggle("Show4RIndicator",bShow4R).setNewColumn(true);
    gui.addToggle("Show6RIndicator",bShow6R);
    gui.addToggle("bFullPath",bFullPath);

    gui.addToggle("bFlipFinal",bFlipFinal);

    setUpInstructionVideo();





    bWideScreen = false;
    if (((float)width/(float)height)>(4.0f/3.0f))
    {
        bWideScreen = true;
    }


    autoshutdown.setup();



    camera.setup(camW,camH);
    camera.setDisplaySize(camW,camH);
    camera.setStartTime(xml.getAttribute("SETTINGS:CAMERA","starttime",500));


    fboWidth = xml.getAttribute("SETTINGS:FBOSAVER","width",cWidth);
    fboHeight = xml.getAttribute("SETTINGS:FBOSAVER","height",cHeight);
    fboX = xml.getAttribute("SETTINGS:FBOSAVER","x",0);
    fboY = xml.getAttribute("SETTINGS:FBOSAVER","y",0);
    previewX = xml.getAttribute("SETTINGS:PREVIEW","x",0);
    previewY = xml.getAttribute("SETTINGS:PREVIEW","y",0);
    previewW = xml.getAttribute("SETTINGS:PREVIEW","w",640);
    previewH = xml.getAttribute("SETTINGS:PREVIEW","h",480);

    fboSaver.allocate(fboWidth,fboHeight,true);
    int photW = xml.getAttribute("SETTINGS:PHOTOSAVER","width",2592);
    int photH = xml.getAttribute("SETTINGS:PHOTOSAVER","height",1728);


    //*****************

    chromakeyShader *shader = camera.getShader();
    shader->setTexId(GL_TEXTURE3,GL_TEXTURE4);
    shader->setup(photW,photH);


    fboSaverL.allocate(photW,photH,false);
    //*********************

    //serial
    serial.setup(//com.c_str(), baud
        xml.getAttribute("SETTINGS:SERIAL","comport","COM3"),
        xml.getAttribute("SETTINGS:SERIAL","baud",19200));
    //idel timer------------------------
    int idelTime = xml.getValue("SETTINGS:IDEL",-1);
    if (idelTime>0)
    {
        idel.setup(idelTime,false);
        idel.startTimer();
        ofAddListener(idel.TIMER_REACHED,this,&testApp::onTimesUp);
    }
    //idel timer------------------------end
    //Network------------------------
    //NETWORK SETTINGS

    network.setup();
    bChangeBG=bShow=false;
    //Network------------------------END

    bCapture = false;
    bSetup = false;

    loading.loadImage("images/loading.png");
    intro.loadImage("images/intro.png");


    fadable.setAlpha(1);
    fadable.setFadeSeconds(1);
    heartbeat.addGui();
    gui.loadFromXML();
    bPause = false;
    fadable.fadeOut();
    timer.setup(60*timeout,false);

    ofAddListener(timer.TIMER_REACHED,this,&testApp::fadeIn);
    ofAddListener(network.MESSAGE_AVAILABLE,this,&testApp::networkMessage);
    ofAddListener(fboSaver.imageSaver.SAVE_DONE,this,&testApp::afterSave);
    //******************************************************
    void* pointer = camera.getCanonCamera();
    if (pointer!=NULL)
        ofAddListener(camera.getCanonCamera()->IMAGE_DOWNLOADED,this,&testApp::onComposImage);
    //******************************************************



    startThread(true,false);
    keyPressed('0');
    camera.chageBackground("images/background/bg_number1.jpg");
    bgImg.loadImage("images/background/bg_number1.jpg");
    //keyPressed('5');
    tempCap.allocate(cWidth,cHeight,OF_IMAGE_COLOR_ALPHA);
    bLoadImport = false;

#ifdef USE_OFXFENSTER
    fenster->setFPS(70);
	fenster->setBounds(1280, 0, fboWidth, fboHeight);


#endif
}
void testApp::threadedFunction()
{
    while ( isThreadRunning() != 0 )
    {
        if ( lock() )
        {
            unlock();
            ofSleepMillis(1 * 1000);
        }

    }
}
//--------------------------------------------------------------
void testApp::update()
{
    heartbeat.isAlive();
    if (selectLogLevel!=currentLogLevel)
    {
        currentLogLevel=selectLogLevel;
        switch (currentLogLevel)
        {
        case 0:
            ofSetLogLevel(OF_LOG_VERBOSE);
            break;
        case 1:
            ofSetLogLevel(OF_LOG_NOTICE);
            break;
        case 2:
            ofSetLogLevel(OF_LOG_WARNING);
            break;
        case 3:
            ofSetLogLevel(OF_LOG_ERROR);
            break;
        case 4:
            ofSetLogLevel(OF_LOG_ERROR);
            break;
        case 5:
            ofSetLogLevel(OF_LOG_SILENT);
            break;

        }
    }
    /*if(bLoadImport)
     {
     ofLog(OF_LOG_VERBOSE,"Staert Load Import image");
     bLoadImport = false;
     if(downloadedImage!=NULL && downloadedImage!=camera.getDownloadedImage())
     {

     downloadedImage->~ofImage();
     downloadedImage=NULL;
     ofLog(OF_LOG_VERBOSE,"downloadedImage empty");
     }
     ofImage * temp = new ofImage();
     ofLog(OF_LOG_VERBOSE,"new downloadedImage");
     bool bLoadedImg = temp->loadImage(importName);
     if(!bLoadedImg)
     {
     ofLog(OF_LOG_ERROR,"downloadedImage fail to load %s",importName.c_str());
     }
     bComposPhoto = true;
     downloadedImage = temp;
     }*/
    if (bLogTime)ofLog(OF_LOG_VERBOSE,"log time %s\n",logtime.LogTime());

    if (ofGetFrameNum()==300)
    {
        ofBeginCustomFullscreen(x, ofGetScreenHeight(), cWidth, cHeight);
    }
    if (ofGetFrameNum()==100)
    {

        if (xml.getValue("SETTINGS:FULLSCREEN",false))
        {
            keyPressed('f');
        }
#ifdef USE_AR
        if (!bSetup && useAR)
        {
            arhelper = new ARHelper();
#ifdef MY_MODELL_HELPER
            modelHandler = new myModelHandler();
#else
            modelHandler = new ModelHandler();
#endif


            arhelper->setModelHandler(modelHandler);

            bSetup = arhelper->setup(camera.getWidth(),camera.getHeight());

            initModel();

            //minimap.enabled = true;

            //bSetup = true;

        }
#endif

    }
    if (bChangeBG)
    {
        ofBeginCustomFullscreen(x, y, cWidth, cHeight);
        if (bg_fn!="")
        {
            camera.chageBackground("images/background/"+bg_fn);
            bgImg.loadImage("images/background/"+bg_fn);
        }

        resetIdel();
        setPause(false);
        bChangeBG = false;
    }
    if (bChangeFG)
    {

        if (fg_fn!="")
        {
            ofDisableArbTex();
            fgLoaded=fgImg.loadImage("images/foreground/"+fg_fn);
            ofEnableArbTex();
        }
        bChangeFG = false;
    }
    if (!bPause)
    {
        camera.update();

#ifdef USE_AR
        if (camera.getPixels()!=NULL && useAR && bSetup)
        {
            //pass camera pixel to ar
            if (!bChangeModel)arhelper->update(camera.getPixels(),camera.getWidth(),camera.getHeight());
        }
#endif
        //        printf("arhelper->update(camera.getPixels(),camera.getWidth(),camera.getHeight());");
        ofSoundUpdate();
        fadable.updateFade();
#ifdef USE_AR
        if (useAR)
            if (bSetup && arhelper->isDetected())
            {

                timer.setup(timeout*60,false);
                timer.startTimer();
            }
#endif
        if (fadable.getAlpha()>0.9f)
        {
            if (bLoaded)
            {
#ifdef INSTRUCTION
                player.update();

                unsigned char *imgPixels = player.getPixels();
                myTexture.loadData(imgPixels, vw,vh, GL_RGB);

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, myTexture.getTextureData().textureID);


                // go back to default texture unit !!
                glActiveTexture(GL_TEXTURE0);
#endif//#ifdef INSTRUCTION
            }
        }

        if (canLoadImage)
        {
            ofDisableArbTex();

            //bShow
            bShow=lastCapImage.loadImage(cap_path+lastCaptureFile+".jpg");
            if (!bShow)
            {
                ofLog(OF_LOG_VERBOSE,"unable To Load "+cap_path+lastCaptureFile+".jpg");
            }
            ofEnableArbTex();

            canLoadImage = false;
            network.sendMessage("Captured");
            //bShow=  true;
        }
        //system("./yucolabPhotoBooth.exe > log`date +%Y%m%d%H%m%S`.txt");
        LiveView();
#ifdef USE_AR
        if (bChangeModel)
        {

            ofLog(OF_LOG_VERBOSE,"%s ModelHandler changeModels(nID,nModel)",logtime.LogTime());
            modelHandler->changeModels(nID,nModel);
            bChangeModel = false;
        }
#endif
        if (bComposPhoto)
        {
            ofLog(OF_LOG_VERBOSE,"testApp Composing Image");

            chromakeyShader *shader = camera.getShader();


            fboSaverL.clear(0,0,0,1);
            fboSaverL.begin();

            ofPushStyle();
            ofSetColor(255,255,255);
            //ofRect(0,0,fboSaverL.getWidth(),fboSaverL.getHeight());


            if (shader!=NULL)shader->setShaderActive(true);
            downloadedImage->draw(0,0,fboSaverL.getWidth(),fboSaverL.getHeight());
            if (shader!=NULL)shader->setShaderActive(false);
#ifdef USE_AR
            if (useAR)arhelper->draw(fboSaverL.getWidth(),fboSaverL.getHeight(),true);
#endif
            string output_file_name = output_path+lastCaptureFile+".jpg";
            ofLog(OF_LOG_VERBOSE,"fboSaverL: save image to "+output_file_name);
            canLoadImage = true;
            fboSaverL.saveImage(output_file_name);

            //copy file by batch
            float lastTime = ofGetElapsedTimef();
            ofLog(OF_LOG_VERBOSE,"Before Save time: %f",lastTime);
#ifdef COPY_TEST
#ifdef TARGET_WIN32
//            char cmd[256];
//
//            sprintf(cmd,"copy %s %s",ofToDataPath(output_file_name,true).c_str(),ofToDataPath(cap_path+sendImagesFile,true).c_str());
//
//            for (int i = 0 ; i < 256 ; i++)
//            {
//                if (cmd[i]=='/') cmd[i]='\\';
//            }
//
//            system(cmd);
            copyImage(ofToDataPath(output_file_name,true).c_str(),ofToDataPath(cap_path+sendImagesFile,true).c_str());
#endif
#else
            fboSaverL.saveImage(cap_path+sendImagesFile );
#endif

            char buf[256];
            sprintf(buf,"output/%s%s/.jpg",output_path.c_str(),today_str.c_str(),logtime.LogTime());

#ifdef COPY_TEST
#ifdef TARGET_WIN32
//            sprintf(cmd,"copy %s %s",ofToDataPath(output_file_name,true).c_str(),ofToDataPath(buf,true).c_str());
//
//            for (int i = 0 ; i < 256 ; i++)
//            {
//                if (cmd[i]=='/') cmd[i]='\\';
//            }
//
//            system(cmd);
            copyImage(ofToDataPath(output_file_name,true).c_str(),ofToDataPath(buf,true).c_str());

#endif
#else
            fboSaverL.saveImage(buf);
#endif
            ofLog(OF_LOG_VERBOSE,"Tie  diff: %f",ofGetElapsedTimef()-lastTime);

            fboSaverL.end();
            bShow = true;
            bComposPhoto = false;
            binProgress = false;
            ofLog(OF_LOG_VERBOSE,"testApp Composing Image DONE");

            ofPopStyle();

        }
    }
    countDown.rect.updateFade();
    //ofSetWindowAlwaysOnTop();

}

//--------------------------------------------------------------
void testApp::draw()
{


    if (!bShow)
    {
        glPushMatrix();
        glTranslatef(fboX,fboY,0 );
        if (bMirrorMaster)
        {
            glTranslatef(cWidth/2,cWidth/2,0);
            glRotatef(180,0,1,0);
            glTranslatef(-cWidth/2,-cWidth/2,0);
        }
        fboSaver.draw(0,0,cWidth,cHeight);
        glPopMatrix();

        fboSaver.draw(previewX,previewY,previewW,previewH);
    }


    if (bPause)
    {
        ofEnableAlphaBlending();
        //        screenSaver.draw(previewX,previewY,previewW,previewH);
        ofDrawBitmapString("Press Open Camera",previewX,previewY);
        //        screenSaver.draw(fboX,fboY,cWidth,cHeight);
        drawTwoImage(&screenSaver);
        ofDisableAlphaBlending();
    }

    glPushMatrix();
    if (bShow)
    {
        glPushMatrix();
        //glTranslatef((cWidth-fboWidth)/2.0f,(cHeight-fboHeight)/2.0f,0);
        ofSetColor(0xffffff);
        if (!camera.isCanonCamera())
        {
            drawTwoImage(&lastCapImage);
            //            lastCapImage.draw(fboX,fboY,cWidth,cHeight);
            //            lastCapImage.draw(previewX,previewY,previewW,previewH);
        }
        else
        {
            //            fboSaverL.draw(fboX,fboY,cWidth,cHeight);
            //            fboSaverL.draw(previewX,previewY,previewW,previewH);
            drawTwoImage(&fboSaverL);
        }
        ofDrawBitmapString("Press Restart\n+file: "+cap_path+lastCaptureFile+".jpg",previewX+10,previewY+30);
        glPopMatrix();
    }

    glPopMatrix();
    camera.drawColorImage();
    {
        ofPushStyle();
        ofEnableAlphaBlending();
        ofSetColor(255,255,255);
        if (!bShow)
        {
            if (bShow4R)
            {
                //            _4R.draw(fboX,fboY,cWidth,cHeight);
                //            _4R.draw(previewX,previewY,previewW,previewH);
                drawTwoImage(&_4R);

            }
            if (bShow6R)
            {
                //            _6R.draw(fboX,fboY,cWidth,cHeight);
                //            _6R.draw(previewX,previewY,previewW,previewH);
                drawTwoImage(&_6R);
            }
        }
        ofDisableAlphaBlending();
        ofPopStyle();
    }
    if (binProgress)
    {
        ofEnableAlphaBlending();
        drawTwoImage(&nowLoading);
        ofDisableAlphaBlending();

    }
    gui.draw();
    countDown.draw(fboX,fboY,cWidth,cHeight);
    countDown.draw(previewX,previewY,previewW,previewH);
    //    drawTwoImage(&countDown);


}
void testApp::LiveView()
{


    fboSaver.begin();
    ofBackground(0,0,0);
    glPushMatrix();
    if (bFlipFinal && bCapture)
    {


        glTranslatef(fboWidth/2.0f,fboHeight/2.0f,0);
        glRotatef(180,0,1,0);
        glTranslatef(-fboWidth/2.0f,-fboHeight/2.0f,0);
    }
    {
        ofPushStyle();
        ofSetColor(50,50,50);
        ofRect(0,0,fboWidth,fboHeight);
        ofPopStyle();
        glPushMatrix();
        //        glTranslatef(50,0,0);

        bgImg.draw(0,0,fboWidth,fboHeight);
        ofEnableAlphaBlending();
        camera.draw(0,0,fboWidth,fboHeight);
        ofDisableAlphaBlending();
        glPopMatrix();
        ofPushStyle();
        ofEnableAlphaBlending();
        ofSetColor(0,0,0,cropAlpha);
        ofRect(0,0,cropArea[0],fboHeight);
        ofRect(fboWidth-cropArea[0],0,cropArea[0],fboHeight);

        ofRect(0,0,cropArea[1],fboHeight);
        ofRect(fboWidth-cropArea[1],0,cropArea[1],fboHeight);


        ofDisableAlphaBlending();
        ofPopStyle();

#ifdef USE_AR
        if (bSetup && useAR)
        {


            //if(!bChangeModel)
            arhelper->draw(fboSaver.getWidth(),fboSaver.getHeight());
            if (bText)
            {


                glPushMatrix();
                ofSetColor(255 , 255, 255 );
                string msg = "";;
                myFont.drawString(msg, (cWidth/2)-myFont.stringWidth(msg)/2,cHeight/2);
                glPopMatrix();
            }
            ofSetupScreen();
            glDisable(GL_DEPTH_TEST);
            if (fadable.getAlpha()>0)
            {
                ofPushStyle();
                ofEnableAlphaBlending();
                ofSetColor(0,0,0,maskA*fadable.getAlpha());
                //ofRect(0,0,cWidth,cHeight);



                if (bLoaded)
                {
#ifdef INSTRUCTION
                    int cx = cWidth/2;
                    int cy = cHeight/2;
                    ofSetColor(255,255,255);

                    shaderBGSub.setShaderActive(true);
                    shaderBGSub.setUniform("tex1", 1);
                    shaderBGSub.setUniform("tex2", 2);
                    shaderBGSub.setUniform("tol", tol);
                    player.draw(cx-vw/2,cy-vh/2);


                    myTexture2.unbind();
                    myTexture.unbind();

                    shaderBGSub.setShaderActive(false);
                    glDisable(GL_DEPTH_TEST);
#endif //#ifdef INSTRUCTION
                }

                ofDisableAlphaBlending();
                ofPopStyle();
            }
            ofSetColor(0xffffff);


        }
        else if (useAR)
        {
            ofEnableAlphaBlending();
            ofSetColor(255,255,255,255*sinf(ofGetElapsedTimef())+125);
            loading.draw(0,0);
            ofDisableAlphaBlending();
            ofDrawBitmapString("Loading ... ",fboWidth/2,fboHeight/2);
        }
#endif

        if (fgLoaded && bCapture)
        {
            ofEnableAlphaBlending();
            fgImg.draw((fboWidth-cWidth)/2.0f,(fboHeight-cHeight)/2.0f,
                       cWidth,cHeight);
            ofDisableAlphaBlending();
        }
        else if (fgLoaded)
        {
            glPushMatrix();
            //            glTranslatef(cWidth/2,cHeight/2,0);
            //            glRotatef(180,0,1,0);
            //            glTranslatef(-cWidth/2,-cHeight/2,0);
            ofEnableAlphaBlending();
            fgImg.draw((fboWidth-cWidth)/2.0f,(fboHeight-cHeight)/2.0f,
                       cWidth,cHeight);
            ofDisableAlphaBlending();
            glPopMatrix();


        }

        if (bCapture)
        {

            //            char buf[256];
            //            sprintf(buf,"%i_%i_%i_%i_%i_%i.jpg",
            //                    ofGetYear(),
            //                    ofGetMonth(),
            //                    ofGetDay(),
            //                    ofGetHours(),
            //                    ofGetMinutes(),
            //                    ofGetSeconds());
            //            lastCaptureFile = buf;
            //        fboSaver.getImageSaver()->resize(cWidth,((cWidth/3.0f)*2.0f));
            if (!camera.isCanonCamera())//saver image for small while canon is not available
            {
                ofLog(OF_LOG_VERBOSE,"%s Capture small image ",logtime.LogTime());
                string saveName = cap_path+lastCaptureFile+".jpg";
                string sendFileName = cap_path+sendImagesFile;

                /*if(bFlipFinal)
                {
                    ofxFBOTextureSaver tempFBO;
                    tempFBO.allocate(fboWidth,fboHeight,true);
                    tempFBO.begin();
                        glPushMatrix();
                        glTranslatef(fboWidth/2.0f,fboHeight/2.0f,0);
                        glRotatef(180,0,1,0);
                            glTranslatef(-fboWidth/2.0f,-fboHeight/2.0f,0);
                            fboSaver.draw(0,0,fboWidth,fboHeight);
                        glPopMatrix();
                    tempFBO.end();
                    tempFBO.saveImage(saveName,fboWidth,fboHeight);
                }
                else
                {*/
                fboSaver.saveImage(saveName,fboWidth,fboHeight);//need to get fbo pixel in GL_RGB and order RGBPixel
                exif.readExif(saveName);
                //}
                float lastTime = ofGetElapsedTimef();
                ofLog(OF_LOG_VERBOSE,"Before Save time: %f",lastTime);
#ifdef COPY_TEST
//#ifdef TARGET_WIN32
//                char cmd[256];
//
//                sprintf(cmd,"copy %s %s",ofToDataPath(saveName,true).c_str(),ofToDataPath(sendFileName,true).c_str());
//
//                for (int i = 0 ; i < 256 ; i++)
//                {
//                    if (cmd[i]=='/') cmd[i]='\\';
//                }
//
//                system(cmd);
//#else
//
                copyImage(ofToDataPath(saveName,true).c_str(),ofToDataPath(sendFileName,true).c_str());

//#endif
#else
                fboSaver.saveImage(cap_path+sendImagesFile ,fboWidth,fboHeight);
#endif
                char buf[256];
                sprintf(buf,"%s%s/%s.jpg",cap_path.c_str(),today_str.c_str(),logtime.LogTime());
                ofLog(OF_LOG_VERBOSE,"Saving timestemp file %s",buf);

#ifdef COPY_TEST
//#ifdef TARGET_WIN32
//                sprintf(cmd,"copy %s %s",ofToDataPath(saveName,true).c_str(),ofToDataPath(buf,true).c_str());
//
//                for (int i = 0 ; i < 256 ; i++)
//                {
//                    if (cmd[i]=='/') cmd[i]='\\';
//                }
//
//                system(cmd);
//#else
                copyImage(ofToDataPath(saveName,true).c_str(),ofToDataPath(buf,true).c_str());


//#endif
#else
                fboSaver.saveImage(buf);

#endif
                target_photo_path = buf;

                ofLog(OF_LOG_VERBOSE,"Time  diff: %f",ofGetElapsedTimef()-lastTime);
                canLoadImage = true;
                binProgress = false;
            }
            //network.sendMessage("filename",lastCaptureFile+".jpg");
            //network.sendMessage("barcode",lastCaptureFile);
            //bool bLoad = false;
            bCapture = false;



        }

    }

    if (canLoadImage)
    {

        helvetica.drawString("Loading...",fboSaver.getWidth()/2,fboSaver.getHeight()/2);
    }


    glPopMatrix();


    fboSaver.end();




}
//--------------------------------------------------------------
void testApp::keyPressed(int key)
{

    if (bDebug)
    {
        switch (key)
        {
        case OF_KEY_RETURN:

            if (bNETWORKMode)
            {
                //Start sending OSC
                network.sendMessage("Hello Flash");
            }
            break;
        case 's':
            camera.videoSettings();
            //if (useAR)arhelper->videoSettings();
            break;
        case 'd':
#ifdef USE_AR
            if (useAR)arhelper->toggleDebug();
#endif
            break;
        case 'f':

            ofBeginCustomFullscreen(x, y, cWidth, cHeight);
            break;
        case 'F':

            ofEndCustomFullscreen();
            break;
        case ' ':
            gui.toggleDraw();
            if (gui.isOn())
            {
                ofShowCursor();
                //ofSetWindowPosition(0,0);
                ofEndCustomFullscreen();
            }
            else
            {
                ofHideCursor();
                ofBeginCustomFullscreen(x, y, cWidth, cHeight);
            }
            break;
        case '[':
            gui.nextPage();
            break;
        case ']':
            gui.prevPage();
            break;
        case '`':
            gui.nextPageWithBlank();
            break;
        case 'l':
            gui.setAlignRight(true);
            break;
        case 'L':
            gui.setAlignRight(false);
            break;
            /* case '=':
             scale+=scale*0.01f;
             for (int i = 0 ; i < numModels ; i ++)
             {
             models[i]->setScale(scale,scale,scale);
             }
             break;
             case '-':
             scale-=scale*0.01f;
             for (int i = 0 ; i < numModels ; i ++)
             {
             models[i]->setScale(scale,scale,scale);
             }
             break;
             case 'l':
             glEnable (GL_LIGHTING);
             break;
             case 'k':
             glDisable (GL_LIGHTING);
             break;*/
        case 'm':
#ifdef USE_AR
            //if (useAR)arhelper->toggleMirror();
#endif
            break;
            /*case 't':
             bMove = !bMove;
             unsigned delay = 0;
             unsigned duration = 1000;
             int target = 100;
             if (bMove)tweenlinear.setParameters(7,easinglinear,ofxTween::easeOut,0,target,duration,delay);
             else tweenlinear.setParameters(7,easinglinear,ofxTween::easeIn,target,0,duration,delay);
             break;*/

        case 'c':
            countDown.start();

            break;
        case'0':
        case'1':
        case'2':
        case'3':
        case'4':
            ofDisableArbTex();
            fgLoaded=fgImg.loadImage("images/foreground/fg_number"+ofToString(key-'0')+".png");
            ofEnableArbTex();
            break;
        case'5':
        case'6':
        case'7':
        case'8':
        case'9':
            camera.chageBackground("images/background/bg_number"+ofToString(key-'0')+".jpg");
            bgImg.loadImage("images/background/bg_number"+ofToString(key-'0')+".jpg");
            break;
        case OF_KEY_BACKSPACE:
            bShow = false;
            //fboSaverL.clear(0,0,0,1);
            break;
        case 'M':
            sendMail("mail:james.kong@yucolab.com");
            break;

        case OF_KEY_F1:
#ifdef USE_AR
            if (useAR)arhelper->printMatrix();
#endif
            break;
        case OF_KEY_F2:

            //ofEndCustomFullscreen();
            ofBeginCustomFullscreen(0, ofGetHeight(), cWidth, cHeight);
            break;
        case OF_KEY_F3:
            //ofEndCustomFullscreen();
            ofBeginCustomFullscreen(x, y, cWidth, cHeight);


            break;
        case 27:
            //if (useAR)modelHandler->removeModelCollection();
            ofLog(OF_LOG_VERBOSE,"keyPress ESC");

            break;
        }
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key)
{


}


//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{
    camera.mousePressed( x, y, button);
}


//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{

}

void testApp::setPause(bool _pause)
{
    bPause = _pause;
    if (bPause)
    {

        if (camera.getCanonCamera())
        {
            camera.getCanonCamera()->endLiveView();
        }
    }
}
//-------------------------------------time out
void testApp::onTimesUp(ofEventArgs &args)
{
    ofLog(OF_LOG_VERBOSE,"Idel Time reached");
    serial.writeByte('a');
    setPause(true);
}
//-------------------------------------
void testApp::resetIdel()
{
    idel.stopTimer();
    idel.startTimer();
}
void testApp::Shot(ofEventArgs &arg)
{
    binProgress = true;
    bCapture = true;
    camera.takePhoto();
    CanonCameraWrapper* canon = camera.getCanonCamera();
#ifdef USE_AR
    if (useAR)    arhelper->saveARData();
#endif
    if (canon!=NULL)
    {

    }
}
void testApp::Counting(ofEventArgs &arg)
{
    network.sendMessage("Counter");
}
void testApp::drawTwoImage(ofBaseDraws *baseDraw)
{

    baseDraw->draw(fboX,fboY,cWidth,cHeight);
    baseDraw->draw(previewX,previewY,previewW,previewH);

}
#ifdef USE_OFXFENSTER

void testApp::fensterDraw()
{
    fboSaver.draw(0,0);
}
void testApp::fensterUpdate()
{
    if(ofGetFrameNum()==300)
    {
        #ifdef TARGET_WIN32

	    HWND vWnd  = FindWindow(NULL,  "PhotoBooth Fenster");


	    long windowStyle = GetWindowLong(vWnd, GWL_STYLE);

	    windowStyle &= ~WS_OVERLAPPEDWINDOW;
	    windowStyle |= WS_POPUP;

	   	SetWindowLong(vWnd, GWL_STYLE, windowStyle);

		SetWindowPos(vWnd, HWND_TOP, 1280, 0, fboWidth, fboHeight, SWP_FRAMECHANGED);


	#endif
    }
}
#else

#endif

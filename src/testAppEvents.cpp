#include "testApp.h"
void testApp::fadeIn(ofEventArgs &e)
{

    fadable.fadeIn();

    if (bLoaded)
    {
#ifdef INSTRUCTION
#ifdef TARGET_WIN32
        player.setFrame(1);
#endif
        player.setPaused(false);
#endif
    }
    timer.stopTimer();
    //minimap.setCandidate(-1);
    //minimap.enabled = false;
    //minimap.enabled = false;
    //ofLog(OF_LOG_VERBOSE,"testApp Timer Event --- fade In screen saver");

}

#ifdef USE_AR
//--------------------------------------------------------------
void testApp::arModelActived(ofAREventArgs &e)//armodels active
{
    //minimap.enableAppEvents();
    fadable.fadeOut();
#ifdef INSTRUCTION
    if (bLoaded)player.setPaused(true);
#endif
    //minimap.setCandidate(e.markerId);
    //minimap.enabled = true;
    timer.setTimer(60*timeout);
    timer.startTimer();
//    ofLog(OF_LOG_VERBOSE,"testApp AR Marker Event --- fade Out screen saver");
    bText = false;

}
//--------------------------------------------------------------
void testApp::arModelDeactived(ofAREventArgs &e)//armodel deactive
{
    //minimap.enabled = false;
    bText = true;
}
//--------------------------------------------------------------
#endif
void testApp::networkMessage(OSCEventArgs &e)
{
    ofLog(OF_LOG_VERBOSE,"networkMessage Events");
    oscXML.loadFromBuffer(e.arg);
    if (oscXML.pushTag("OSCPACKET"))
    {
        string address = oscXML.getAttribute("MESSAGE","NAME"," ");
        if (oscXML.pushTag("MESSAGE"))
        {
            int numTag = oscXML.getNumTags("ARGUMENT");
#ifdef DEBUG
            for (int i = 0 ; i < numTag ; i++)
            {


                string type = oscXML.getAttribute("ARGUMENT","TYPE"," ",i);

                if (type=="s")
                {

                    printf("OSC value s = %s\n",oscXML.getAttribute("ARGUMENT","VALUE"," ",i).c_str());
                }
                if (type=="i")
                {

                    printf("OSC value i = %i\n",oscXML.getAttribute("ARGUMENT","VALUE",0,i));
                }
                if (type=="f")
                {

                    printf("OSC value f = %f\n",oscXML.getAttribute("ARGUMENT","VALUE",0.0f,i));
                }

            }
#endif
#ifdef USE_AR
            if (address=="model")
            {
                nID.clear();
                nModel.clear();


                for (int i = 0 ; i < numTag ; i++)
                {
                    nID.push_back(oscXML.getAttribute("ARGUMENT","VALUE",0,i++));

                    nModel.push_back(oscXML.getAttribute("ARGUMENT","VALUE"," ",i));
                }


                bChangeModel = true;
            }

            else if (address=="background" && numTag==1)
#else
            if (address=="background" && numTag==1)
#endif
            {

                bg_fn = oscXML.getAttribute("ARGUMENT","VALUE","",0);
                bChangeBG = true;
                size_t p = bg_fn.find(".jpg");
                string temp = bg_fn.substr(0,p);
                selectedFrame = combox->getIndexForTitle(temp);
                /*if(bg_fn=="bg_number1.jpg")
                {
                    selectedFrame = 0;
                }
                else if(bg_fn=="bg_number2.jpg")
                {
                    selectedFrame = 1;
                }else if(bg_fn=="bg_number3.jpg")
                {
                    selectedFrame = 2;
                }else if(bg_fn=="bg_number4.jpg")
                {
                    selectedFrame = 3;
                }else if(bg_fn=="bg_number5.jpg")
                {
                    selectedFrame = 4;
                }*/

            }
            else if (address=="foreground" && numTag==1)
            {

                fg_fn = oscXML.getAttribute("ARGUMENT","VALUE","",0);
                bChangeFG = true;

            }
            else if (address=="barcode" && numTag==1)
            {

                lastCaptureFile = oscXML.getAttribute("ARGUMENT","VALUE"," ",0);

            }
            else if (address=="shoot" && numTag==1)
            {
                keyPressed('c');
            }
            else if (address=="restart")
            {
                bShow = false;
                //if(bHidden)ofBeginCustomFullscreen(0, ofGetScreenHeight(), cWidth, cHeight);
            }
            else if (address=="restart_bg")
            {
                bShow = false;
                if(bHidden)ofBeginCustomFullscreen(0, ofGetScreenHeight(), cWidth, cHeight);
            }
            else if (address=="reset_live_view")
            {
                setPause(false);
            }
            else if (address=="calibrate" && numTag==1)
            {
                camera.toggleCapBG();
            }
            else if (address=="keycolor" && numTag==1)
            {
                string type = oscXML.getAttribute("ARGUMENT","TYPE","i",0);
                int _keyColor = 0 ;
                if (type=="i")
                {
                    _keyColor = oscXML.getAttribute("ARGUMENT","VALUE",0,0);
                }
                camera.toggleKeyColor(_keyColor);
            }
            else if (address=="bShow6R" && numTag==1)
            {
                string type = oscXML.getAttribute("ARGUMENT","TYPE","i",0);

                if (type=="i")
                {
                    bShow6R = oscXML.getAttribute("ARGUMENT","VALUE",0,0);
                }

            }
            else if (address=="finish" && numTag==1)
            {
                setPause(false);
                bShow = false;
                network.sendMessage("barcode",lastCaptureFile);
                if(target_photo_path!="")
                {
					network.sendMessage("image_path",ofToDataPath(target_photo_path,bFullPath).c_str());
                    network.sendMessage("title",bg_fn);
                }
                if(bHidden)ofBeginCustomFullscreen(0, ofGetScreenHeight(), cWidth, cHeight);
            }
            else if (address=="file" && numTag==1)
            {
                string type = oscXML.getAttribute("ARGUMENT","TYPE"," ",0);
                if (type=="s")
                {
                    importName = oscXML.getAttribute("ARGUMENT","VALUE"," ",0);
                    ofLog(OF_LOG_NOTICE,"OSC value s = %s\n",importName.c_str());
                    /*ofImage * temp = new ofImage();

                    temp->loadImage(_filename);
                    bComposPhoto = true;
                    downloadedImage = temp;*/
                    if (importName!=" ")
                    {
                        bLoadImport = true;
                    }

                }

            }

            oscXML.popTag();
        }
        oscXML.popTag();
    }

//    size_t emailFound = e.arg.find("mail");
//    size_t changeBGfound = e.arg.find("bg_number");
//    size_t changeFGfound = e.arg.find("fg_number");
//    size_t changeModelfound = e.arg.find("model");
//    if (e.arg=="take_photo")
//    {
//        bCapture = true;
//        camera.takePhoto();
//        char buf[256];
//
//        ofLog(OF_LOG_VERBOSE,"%s : TakePhoto",logtime.LogTime());
//    }
//    else if ( e.arg=="restart")
//    {
//        bShow = false;
//        ofLog(OF_LOG_VERBOSE,"%s : Restart",logtime.LogTime());
//        //network.sendMessage("restarted");
//    }
//    else if (e.arg=="calibrate")
//    {
//        ofLog(OF_LOG_VERBOSE,"%s : Calibrating",logtime.LogTime());
//        camera.toggleCapBG();
//    }
//    else if (lastCaptureFile!="" && emailFound!=string::npos)
//    {
//        sendMail(e.arg);
//    }
//    else if (changeBGfound!=string::npos)
//    {
//        char buf[256];
//
//        ofLog(OF_LOG_VERBOSE,logtime.LogTime());
//        bChangeBG = true;
//    }
//    else if (changeFGfound!=string::npos)
//    {
//        char buf[256];
//
//        ofLog(OF_LOG_VERBOSE,logtime.LogTime());
//        bChangeFG = true;
//    }
//    else if (changeModelfound!=string::npos)
//    {
//        char buf[256];
//
//        ofLog(OF_LOG_VERBOSE,"Preasing Model Event Message %s",logtime.LogTime());
//        bChangeFG = true;
//        if (bSetup && useAR)
//        {
//#ifdef MY_MODELL_HELPER
//            modelHandler->praseMessage(e.arg);
//#else
//
//#endif
//
//        }
//    }
//    else
//    {
//        char buf[256];
//        ofLog(OF_LOG_VERBOSE,logtime.LogTime());
//        network.sendMessage("What the fuck you are saying?");
//    }
}
//--------------------------------------------------------------
void testApp::afterSave(ofEventArgs &e)
{
    ofLog(OF_LOG_VERBOSE,"Image Saver done");
    //network.sendMessage("barcode","data/"+cap_path+lastCaptureFile);

}
//--------------------------------------------------------------
void testApp::onComposImage(ofEventArgs &e)
{
    ofLog(OF_LOG_VERBOSE,"testApp event downloaded photo onComposImage");
    /* chromakeyShader keyingShader;
      keyingShader.setup(width, height);

     keyingShader.setTolParams(tol);
     keyingShader.setReferenceImage(&tmp);
     keyingShader.setKeyColor(grabbedColor);*/
    /*    chromakeyShader *shader = camera.getShader();
        fboSaverL.begin();

        ofPushStyle();
        ofSetColor(255,255,255);
        ofRect(0,0,fboSaverL.getWidth(),fboSaverL.getHeight());
        ofPopStyle();
        downloadedImage = camera.getDownloadedImage();
        shader->setShaderActive(true);
        downloadedImage->draw(0,0,fboSaverL.getWidth(),fboSaverL.getHeight());
        shader->setShaderActive(false);
        arhelper->draw(fboSaverL.getWidth(),fboSaverL.getHeight(),true);
        fboSaverL.saveImage("capture/"+lastCaptureFile+".jpg");
        fboSaverL.end();*/
    bComposPhoto = true;
    downloadedImage = camera.getDownloadedImage();

}

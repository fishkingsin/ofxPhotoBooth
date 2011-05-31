#include "testApp.h"
#include "PBModel.h"
void testApp::setUpInstructionVideo()
{
#ifdef INSTRUCTION
    bLoaded = player.loadMovie(xml.getAttribute("SETTINGS:SCREENSAVER","file","movies/instruction.mov"));

    secondImage.loadImage("images/green.jpg");


    shaderBGSub.loadShader("shaders/mix");



    if (bLoaded)
    {
        ofDisableArbTex();
        player.stop();
        player.setLoopState(OF_LOOP_NORMAL);
        vw = player.getWidth();
        vh = player.getHeight();
        secondImage.resize(vw,vh);


        unsigned char *imgPixels = player.getPixels();

        myTexture.allocate(vw,vh,GL_RGB);
        myTexture.loadData(imgPixels, vw,vh, GL_RGB);


        unsigned char *imgPixels2 = secondImage.getPixels();
        myTexture2.allocate( vw , vh , GL_RGB);
        myTexture2.loadData(imgPixels2, vw , vh , GL_RGB);




        ///// multitextureLoad stuff
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, myTexture.getTextureData().textureID);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, myTexture2.getTextureData().textureID);


        // go back to default texture unit !!
        glActiveTexture(GL_TEXTURE0);

        ofEnableArbTex();


        gui.addSlider("Shader Tolorence",tol,0.0f,1.0f);
    }
    else
    {

        //std::exit(0);
        ofLog(OF_LOG_FATAL_ERROR,"Can not Load model instruction movie!");
    }
#endif//#ifdef INSTRUCTION
}

#ifdef USE_AR
void testApp::initModel()
{
    if (xml.pushTag("SETTINGS"))
    {
        int tags = xml.getNumTags("MODELS");
        for (int i = 0 ; i <  tags ; i++)
        {
            int id = xml.getAttribute("MODELS","id",0,i);
            float scale = xml.getAttribute("MODELS","scale",1.0f,i);
            string fn = xml.getAttribute("MODELS","foldername","",i);
            PBModel *pbModel = new PBModel(id,scale,fn);
            pbModel->UseGui(true);
            pbModel->setup();
            pbModel->setStartTime(xml.getAttribute("MODELS","starttime",1,i));
            glutSwapBuffers();
            modelHandler->addModelCollection((ARModelCollections*)pbModel);
        }


        for (int  i = 0 ; i < modelHandler->getModel().size();i++)
        {
            ARModelCollections* armodel = modelHandler->getModel()[i];
            ofAddListener(armodel->MODEL_ACTIVE,this,&testApp::arModelActived);
            ofAddListener(armodel->MODEL_DEACTIVE,this,&testApp::arModelDeactived);
        }

        xml.popTag();
    }

    gui.loadFromXML();
}
void testApp::reloadModel(int numModel, int *__id, string *__fn)
{
    gui.saveToXML();
    modelHandler->removeModelCollection();

    for (int i = 0 ; i <  numModel ; i++)
    {
        int id = __id[i];//xml.getAttribute("MODELS","id",0,i);

        string fn = __fn[i];//xml.getAttribute("MODELS","foldername","",i);
        PBModel *pbModel = new PBModel(id,1,fn);
        pbModel->UseGui(false);
        pbModel->setup();

        glutSwapBuffers();
        modelHandler->addModelCollection((ARModelCollections*)pbModel);
    }


    for (int  i = 0 ; i < modelHandler->getModel().size();i++)
    {
        ARModelCollections* armodel = modelHandler->getModel()[i];
        ofAddListener(armodel->MODEL_ACTIVE,this,&testApp::arModelActived);
        ofAddListener(armodel->MODEL_DEACTIVE,this,&testApp::arModelDeactived);
    }


    gui.loadFromXML();
}
#endif
void testApp::clearStorage()
{

    ofxDirList dir;
    dir.allowExt("png");
    dir.allowExt("jpg");
    dir.setVerbose(true);
    int num = dir.listDir(cap_path);
    ofLog(OF_LOG_VERBOSE,"Total "+ofToString(num)+" Files in "+cap_path);

    if (num>MAX_CAP_FILE)
    {
        for (int i =0 ; i< num-MAX_CAP_FILE; i++)
        {
            if ( remove( dir.getPath(i).c_str()) == -1 )
                ofLog(OF_LOG_ERROR, "Error deleting "+dir.getPath(i) );
            else
                ofLog(OF_LOG_NOTICE, "Deleted "+dir.getPath(i) );
        }
    }
}
void testApp::clearLog()
{

    string folder("log");
    ofxDirList dir;
    dir.allowExt("txt");

    dir.setVerbose(true);
    int num = dir.listDir("log");
    ofLog(OF_LOG_VERBOSE,"Total "+ofToString(num)+" Files in "+folder);

    if (num>MAX_CAP_FILE)
    {
        for (int i =0 ; i< num-MAX_CAP_FILE; i++)
        {
            if ( remove( dir.getPath(i).c_str()) == -1 )
                ofLog(OF_LOG_ERROR, "Error deleting "+dir.getPath(i)+folder );
            else
                ofLog(OF_LOG_NOTICE, "Deleted "+dir.getPath(i)+folder );
        }
    }
}
void testApp::sendMail(string msg)
{

    string addr = msg.substr (5,msg.length()-5);
    char cmd[MAXPATH];
    char buf[256];
    sprintf(buf,"%i_%i_%i_%i_%i_%i\0",
            ofGetYear(),
            ofGetMonth(),
            ofGetDay(),
            ofGetHours(),
            ofGetMinutes(),
            ofGetSeconds());
    ofLog(OF_LOG_VERBOSE,"Send mail at %s",buf);
    string myPath = ofToDataPath(cap_path+sendImagesFile,true);
    std::replace( myPath.begin(), myPath.end(), '/', '\\' );
    sprintf(cmd," blat -bodyF body.txt -8bitmime -t %s -attach %s -sf subject.txt -debug",
            addr.c_str(),
            myPath.c_str()
           );
    ofLog(OF_LOG_VERBOSE,cmd);
    system(cmd);
}

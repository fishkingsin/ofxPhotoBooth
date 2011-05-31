#include "PBModel.h"

PBModel::PBModel(int _id,float _scale, string _folder)
{
    ARModelCollections::ARModelCollections();
    id = _id;
    scale = _scale;//0.1;

    folder = _folder;
    model_path = _model_path+folder;
    image_path = _image_path+folder;
    z = 0;
    numModelImages = 0;
    color = ofRandom(0x000000,0xffffff);
}

PBModel::~PBModel()
{
    saveSetting();
    models.~vector();
    screen_images.~vector();
    model_images.~vector();
    if (imagepoints!=NULL)free(imagepoints);
    if (modelPnts!=NULL)free(modelPnts);
}
void PBModel::saveSetting()
{
    if (bUseGui)
    {
        config = new ofxXmlSettings();
        config->addValue("Scale",scale);
        config->addValue("tweenScale",tweenScale);
        config->addValue("Image_Size",iseize);
        config->addValue("Easing",targetEasing);
        config->addValue("Model_Z",z);
        config->addValue("RoationX",rotX);
        config->addValue("RoationY",rotY);
        config->addValue("RoationZ",rotZ);
        config->addValue("Alpha",alpha);
        config->addValue("debug",debug);
        config->addValue("cullFace",cullFace);

        config->saveFile(folder+".xml");

    }
}
void PBModel::selfSetup()
{
    z=rotX=rotY=rotZ=alpha=debug=0;
    if (bUseGui)
    {
        gui.addSlider("Model_Z",z,-500,500);
        gui.addSlider("RoationX",rotX,0,360);
        gui.addSlider("RoationY",rotY,0,360);
        gui.addSlider("RoationZ",rotZ,0,360);
        gui.addSlider("Alpha",alpha,0,255);
        gui.addSlider("cullFace",cullFace,0,1);
        gui.addToggle("debug",debug);
        gui.addToggle("bDepthTest",bDepthTest);
        gui.addToggle("bCullFace",bCullFace);


    }
    else
    {
        if (config!=NULL)
        {
            z = config->getValue("Model_Z",0);
            rotX = config->getValue("RoationX",0);
            rotY = config->getValue("RoationY",0);
            rotZ = config->getValue("RoationZ",0);
            alpha = config->getValue("Alpha",0);
            debug = config->getValue("debug",0);
            cullFace = config->getValue("cullFace",0);
        }
    }


    ofxXmlSettings xml;
    if ( xml.loadFile("arconfig.xml"))
    {
        ofLog(OF_LOG_VERBOSE,"arconfig.xml loaded");
    }
    else
    {
        ofLog(OF_LOG_ERROR,"Could not Found arconfig.xml");
    }
    int m_range = xml.getValue("SETTINGS:MODEL_POSITION_RANDGE",200);
    int i_range = xml.getValue("SETTINGS:IMAGE_POSITION_RANDGE",200);


    //gui.addPage(folder+"_models");
    modelPnts = new ofPoint[numModels];
    for (int i = 0 ; i < numModels ; i ++)
    {
        if (bUseGui)
        {
            gui.addSlider("ModelPoint_x"+ofToString(i),modelPnts[i].x,-m_range,m_range);
            gui.addSlider("ModelPoint_y"+ofToString(i),modelPnts[i].y,-m_range,m_range);
        }
    }

    //gui.addPage(folder+"_Images");
    imagepoints = new ofPoint[numModelImages];

    //gui.addSlider("pointZ",pointZ,-100,100);
    for (int i = 0 ; i < numModelImages ; i ++)
    {
        if (bUseGui)
        {
            gui.addSlider("ImagePoint_x"+ofToString(i),imagepoints[i].x,-i_range,i_range);
            gui.addSlider("ImagePoint_y"+ofToString(i),imagepoints[i].y,-i_range,i_range);
        }
    }
    if (bUseGui) gui.loadFromXML();
    //system("pause");

}
void PBModel::selfUpdate()
{

    for (int i = 0 ; i < numModels ; i ++)
    {
        models[i]->setARModelPosition3D(ofxVec3f(modelPnts[i].x,modelPnts[i].y,tweenScale*scale*(i)*(tweenexpo.update()/numModels)));
    }
}

#ifdef MY_MODELL_HELPER
/*void PBModel::setModelMatrix(int  _id ,float *_m , float *project)
{
//printf("void PBModel::setModelMatrix(int  _id ,float *_m , float *project)");





    {
        for (int i = 0 ; i < 16 ; i++)
            m[i] = _m[i];
        matrixUpdated = true;
        inactiveCount = 0;
    }
//printf("void PBModel::setModelMatrix(int  _id ,float *_m , float *project)-------------------------");
}*/

#endif
void PBModel::fadeIn()
{
    unsigned duration = 5000;
    unsigned delay = 5;
    int target = 1;

    tweenexpo.setParameters(7,easingexpo,ofxTween::easeOut,0,target,duration,delay);

}
void PBModel::fadeOut()
{
    unsigned delay = 1;
    unsigned duration = 5000;
    int target = 0;
    exitCount = 0;

    tweenexpo.setParameters(7,easingexpo,ofxTween::easeIn,target,0,duration,delay);

}
void PBModel::drawModel()
{


    if(bDepthTest)glEnable(GL_DEPTH_TEST);
    if(bCullFace)glEnable(GL_CULL_FACE);

    if(cullFace==0)glCullFace(GL_BACK);
    else if(cullFace==1)glCullFace(GL_FRONT);

//glActiveTexture(GL_TEXTURE0);

    glPushMatrix();

    glTranslatef(0,0,z);
    glRotatef(-90,0,0,1);
    glRotatef(rotX,1,0,0);
    glRotatef(rotY,0,1,0);
    glRotatef(rotZ,0,0,1);

    for (int i = 0 ; i < numModels ; i ++)
    {
        models[i]->draw();
    }

    if (debug)
    {
        if (numModels==0)
        {
            ofDrawBitmapString("I have no model at all",0,0);
        }
        ofDrawBitmapString("Tween Value: "+ofToString(tweenexpo.update()),0,10);
    }
    if(numModels==0)
    {
        ofPushStyle();
        ofSetColor(color);
        ofxBox(0,0,100);
        ofPopStyle();
    }

    glPopMatrix();
    if(bDepthTest)glDisable(GL_DEPTH_TEST);
    if(bCullFace)glDisable(GL_CULL_FACE);

    //ofSetupScreen();

};
void PBModel::draw2D()
{
    //if (currentFrame-lastFrame<timeOut && ofGetElapsedTimef()>startGameTime)
    {
        ofSetColor(255,255,255);
        ofEnableAlphaBlending();

        //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
        float e = tweenexpo.update();

        if (e>0.9)
        {
            ofSetColor(255,255,255,255*e);
            for (int i = 0 ; i < numModelImages ; i ++)
            {
                int w = model_images[i]->ofImage::getWidth();
                int h = model_images[i]->ofImage::getHeight();
                int x = model_images[i]->ofxMSAInteractiveObject::x;
                int y = model_images[i]->ofxMSAInteractiveObject::y;
                glPushMatrix();
                glTranslatef(0,0,model_images[i]->ofxVec3f::z);
                model_images[i]->ofImage::draw(x-w/2,y-h/2,w,h);
                glPopMatrix();
            }
            ofDisableAlphaBlending();
        }
        ofEnableAlphaBlending();
        ofPushStyle();
        ofSetColor(255,255,255,255*e);
        for (int i = 0 ; i < numScreenImages ; i ++)
        {
            screen_images[i]->draw(0,0);
        }
        ofPopStyle();
        ofDisableAlphaBlending();
    }
}
void PBModel::findScreenCoord( const GLdouble *proj, const GLint *view)
{
    GLdouble ret_x, ret_y, ret_z;
    float x = 0;
    float y = 0;
    float z = 0;
    if ( models.size()>0)
        for (int i = 0 ; i < numModelImages ; i ++)
        {
            gluProject( models[i%numModels]->getPos3d()->x + imagepoints[i].x,
                        models[i%numModels]->getPos3d()->y + imagepoints[i].y,
                        models[i%numModels]->getPos3d()->z +z,
                        targetMatrix,//get the modle matrix from modelset to calc each model screen coord
                        proj,
                        view, &ret_x, &ret_y, &ret_z);
            x = ret_x;
            y = ofGetHeight()-ret_y;
            z = ret_z;
            // models[i]->setARModelPosition2D(ofxVec2f(x,y));
            model_images[i]->setPos(x,y);
            model_images[i]->set(x,y,z);
        }
    else
        for (int i = 0 ; i < numModelImages ; i ++)
        {
            model_images[i]->setPos(0,0);
            model_images[i]->set(0,0,0);
        }
}

void PBModel::setId(int _id)
{
    id =id;
}
string PBModel::getModelName()
{
    return folder;

}
void PBModel::changeModel(string _name)
{
    while(!models.empty())
    {
        ARModel* tempItem = models.back();
        tempItem->~ARModel();
        models.pop_back();
    }
    if ( config->loadFile(folder+".xml"))
    {
        ofLog(OF_LOG_VERBOSE,folder+".xml loaded");

    }
    else
    {
        config->~ofxXmlSettings();
        config = NULL;
        ofLog(OF_LOG_ERROR,folder+".xml loaded ERROR \n *************USE DEFAULT VALUE");
        return;
    }
    if (config!=NULL)
    {
        scale = config->getValue("Scale",0);
        tweenScale = config->getValue("tweenScale",0);
        iseize = config->getValue("Image_Size",0);
        targetEasing = config->getValue("Easing",0);
    }
    else{
        scale = 1;
        tweenScale = 1;
        iseize = 1;
        targetEasing = 1;
    }
    ofxDirList dir;
    dir.allowExt("3DS");
    dir.allowExt("3ds");
    int num  = dir.listDir(model_path);
    ARModel* model;

    for (int i = 0 ; i < num ; i ++)
    {

        model = new ARModel();
        ofDisableArbTex();
        model->loadModel(ofToDataPath(dir.getPath(i)).c_str(), 1);
        ofEnableArbTex();
        model->setup();
        model->setScale(scale,scale,scale);
        models.push_back(model);
        ofLog(OF_LOG_VERBOSE,"Loading Model "+dir.getPath(i)+"Successed");

    }
}

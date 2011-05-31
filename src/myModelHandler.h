
#pragma once

#include "ModelHandler.h"
#include "ofxLog.h"

#define NUM 4

class myModelHandler : public ModelHandler
{
public:
    void update();
    void draw();
    void draw2D();
    void changeModels(vector<int>&id,vector<string>&name);
    void praseMessage(string input);
   // void updateModelMatrix(int _i , float *m , float *project);
protected:
private:
//    map<int,ARModelCollections*> modelMap;

};



#ifndef PBMODEL_H
#define PBMODEL_H

#include "ARModelCollections.h"

class PBModel : public ARModelCollections
{
public:
    PBModel(int _id,float _scale, string _folder);
    ~PBModel();
    void saveSetting();
    void selfSetup();
void selfUpdate();
#ifdef MY_MODELL_HELPER

   // void setModelMatrix(int  _id ,float *_m , float *project);
    #endif
    void drawModel();
    void draw2D();
    void findScreenCoord( const GLdouble *proj, const GLint *view);
    void fadeIn();
    void fadeOut();

    void setId(int _id);
    string getModelName();
    void changeModel(string _name);
protected:
private:

    ofPoint *imagepoints;
    ofPoint *modelPnts;

    int z;
    int alpha;
    int cullFace;
    int color;

    bool debug;
    bool bDepthTest;
    bool bCullFace;

};

#endif // PBModel_H

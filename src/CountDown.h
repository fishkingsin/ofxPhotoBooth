#ifndef COUNTDOWN_H
#define COUNTDOWN_H
#include "ofMain.h"
#include "ofxTimer.h"
#include "ofxDirList.h"
#include "ofxTween.h"
#include "ofxSimpleGuiToo.h"
#include "ofxFadable.h"
#include "myNetworkHandler.h"
class CountDown //: public ofBaseDraws
{
public:
    ofEvent <ofEventArgs> COUNTER_REACHED;
    ofEvent <ofEventArgs> COUNTER_COUNTING;
    CountDown();
    virtual ~CountDown();
    void setup(int _second);
    void update(ofEventArgs &args);
    void draw(float x,float y){};
    void draw(float x ,float y, float w, float h);//bool blips);


	float getHeight(){return 0;}
	float getWidth(){return 0;}

	void setAnchorPercent(float xPct, float yPct){};
    void setAnchorPoint(float x, float y){};
	void resetAnchor(){};
    void start();
    void stop();



    ofxTimer mTimer;
    int mCount;
    ofImage *countImage;
    int num ;
    bool bShow;
    float second;
//    ofSoundPlayer  shutter;
    float scale;
    bool bFlash;
    ofxFadableRect rect;


};

#endif // COUNTDOWN_H

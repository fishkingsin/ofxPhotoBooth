#include "CountDown.h"

CountDown::CountDown()
{
    //ctor
    mCount = -1;;
    countImage = NULL;
    num = 0;
    bShow = false;
    scale = 2;
    bFlash = false;
    rect.setColor(0,0,0);
    rect.setAlpha(0);
    rect.fadeOut();
    rect.setFadeSeconds(1);
}

CountDown::~CountDown()
{
    //dtor
}
void CountDown::setup(int _second)
{
    ofxDirList dir;
    dir.allowExt("png");
    num  = dir.listDir("images/counter");
    countImage = new ofImage[num];
    for (int i = 0 ; i < num ; i++)
    {
        countImage[i].loadImage(dir.getPath(i));
    }
    mTimer.setup(_second,true);
    stop();
    ofAddListener(mTimer.TIMER_REACHED,this,&CountDown::update);
    second = _second;
//    shutter.loadSound("sounds/camera.wav");

    //gui.addPage("CountDown");
    gui.addSlider("scale",scale,0,10);

}
void CountDown::update(ofEventArgs &args)
{
    if (mCount==num-1)
    {
        static ofEventArgs timerEventArgs;
        ofNotifyEvent(COUNTER_REACHED, timerEventArgs, this);
    }
    if (mCount==num-1)
    {
//        shutter.play();
        stop();
        rect.fadeIn();


    }
    else{
        //play count down sound?TBC
        static ofEventArgs timerEventArgs;
        ofNotifyEvent(COUNTER_COUNTING, timerEventArgs, this);
    }

    mCount++;

    if (mCount==num)
    {

        rect.setAlpha(1);
        rect.fadeOut();
    }





    mCount%=num;
}
void CountDown::draw(float x ,float y, float w, float h)//bool blips)
{
    if (bShow)
    {
        ofPushStyle();
        ofEnableAlphaBlending();
        ofFill();
        //for (int i = 0 ; i < num ; i++)
        if (mCount>-1)
        {
            int centerW = w/2;
            int centerH = h/2;
            ofImage *temp;
            /*if (mCount==num-2 && blips)
            {
                temp = &countImage[num-1];
            }
            else
            {*/
            temp = &countImage[mCount];
            //}
            float _w = temp->width*scale;
            float _h = temp->height*scale;

            int tW = _w/2;
            int tH = _h/2;
            int _x = centerW-tW;
            int _y = centerH-tH;

            temp->draw(x+_x,y+_y,_w,_h);

        }

        ofDisableAlphaBlending();
        ofPopStyle();
    }

        ofPushStyle();
        ofEnableAlphaBlending();
        ofFill();
        rect.updateFade();
        rect.draw(0,0,ofGetWidth(),ofGetHeight());
        ofDisableAlphaBlending();
        ofPopStyle();


}
void CountDown::start()
{
    mCount = 0;
    mTimer.startTimer();
    bShow = true;
}
void CountDown::stop()
{
    mTimer.stopTimer();
    bShow = false;
}

#ifndef HEARTBEATTHREAD_H_INCLUDED
#define HEARTBEATTHREAD_H_INCLUDED

#include "ofxThread.h"
#include "ofxNetwork.h"
#include "ofxLogTime.h"
class HeartBeatThread:  public ofxThread
{
public :
    HeartBeatThread()
    {
        mMessage="HeartBeat";
        //create the socket and set to send to 127.0.0.1:3335
        udpConnection.Create();
        udpConnection.Connect("127.0.0.1",3335);
        udpConnection.SetNonBlocking(true);

    }
    void addGui()
    {
        gui.addPage("HeartBeat");
        gui.addToggle("HeartBeat_bVerbose",bVerbose);
    }

    void isAlive()
    {
        alive = true;
    }
private:

    void threadedFunction()
    {
        while ( ofxThread::isThreadRunning() != 0 )
        {

            if ( ofxThread::lock() )
            {


                if (alive )
                {


                    sendMessage(mMessage);

                    alive = false;
                }
                char udpMessage[100000];
                udpConnection.Receive(udpMessage,100000);
                string message=udpMessage;
                if (message!="")
                {
                   if(bVerbose) ofLog(OF_LOG_VERBOSE,"Heart Beat recevie : "+message);
                   if(bVerbose)
                   {
                       sendMessage(mMessage+" response");
                       ofLog(OF_LOG_VERBOSE,"Heart Beat response : "+mMessage);
                   }
                }

                ofSleepMillis(1000);
                ofxThread::unlock();

            }
        }
    }
    void sendMessage(string msg)
    {
        int sent = udpConnection.Send(msg.c_str(),msg.length());
                    switch (sent)
                    {
                    case SOCKET_TIMEOUT:
                        if(bVerbose)ofLog(OF_LOG_ERROR,"Heart beat Send Error - SOCKET_TIMEOUT");
                        break;
                    case NO_TIMEOUT:
                        if(bVerbose)ofLog(OF_LOG_ERROR,"Heart beat Send Error - NO_TIMEOUT");
                        break;
                    case -1:
                        if(bVerbose)ofLog(OF_LOG_ERROR,"Heart beat Send Error - UNKNOW");
                        break;
                    default:
                        if(bVerbose)ofLog(OF_LOG_VERBOSE,"%s HeartBeat Sent",logtime.LogTime());
                        break;
                    }
    }

    bool alive;

    ofxUDPManager udpConnection;
    string mMessage;
    bool bVerbose;
};

#endif // HEARTBEATTHREAD_H_INCLUDED

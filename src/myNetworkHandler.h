#ifndef MYNETWORKHANDLER_H_INCLUDED
#define MYNETWORKHANDLER_H_INCLUDED

#include "NetworkHandler.h"

class myNetworkHandler :public NetworkHandler
{
public :
    void setup()
    {
        NetworkHandler::setup();
//        bLoaded = false;
//        bShow = false;
//        bTakePhoto = false;


    }
    void sendMessage(string message)
    {
#ifdef USE_FLASHCOMMUNICATION
#else
        ofxOscMessage m;
        m.setAddress( "/openframeworks" );
        ofLog(OF_LOG_VERBOSE,"myNetworkHandler sending message: "+message);
        m.addStringArg(message);
        sender.sendMessage( m );
#endif
    };
    void sendMessage(string name,int value)
    {
#ifdef USE_FLASHCOMMUNICATION
#else
        ofxOscMessage m;
        m.setAddress(name );
        ofLog(OF_LOG_VERBOSE,"myNetworkHandler sending value: "+ofToString(value));
        m.addIntArg(value);
        sender.sendMessage( m );
#endif
    };
    void sendMessage(string name,string value)
    {
#ifdef USE_FLASHCOMMUNICATION
#else
        ofxOscMessage m;
        m.setAddress(name );
        ofLog(OF_LOG_VERBOSE,"myNetworkHandler sending value: "+value);
        m.addStringArg(value);
        sender.sendMessage( m );
#endif
    };
//    string getBackground()
//    {
//        string tmp(fn);
//        fn = "";
//        return tmp;
//    }
//    string getForeground()
//    {
//        string tmp(fn);
//        fn = "";
//        return tmp;
//    }

//    bool bLoaded;
//    bool bTakePhoto;
//    bool bShow;

//    ofImage bg;
//    string fn;

private:
    void CommandSearch(string _command)
    {
        ofLog(OF_LOG_VERBOSE, _command);
        static OSCEventArgs e;
        e.arg = _command;
        ofNotifyEvent(MESSAGE_AVAILABLE, e, this);
//        size_t changeBGfound = _command.find("bg_number");
//        size_t changeFGfound = _command.find("fg_number");
//        size_t takePhotofound = _command.find("take_photo");
//        size_t changeModelfound = _command.find("model");//potoco "model:mode_name:markerid"
//        vector<string>splitCommand;
//        size_t end =  _command.find_first_of()("-");
//        size_t start = 0;
//        while(delim!=string::npos)
//        {
//            splitCommand.push_back(_command.substr(start,delim));
//
//        }
//
//
//        //size_t emailSent = _command.find("emailSent");
//        if (changeBGfound!=string::npos)
//        {
//            fn = "images/background/"+_command+".jpg";
//            static OSCEventArgs e;
//            e.arg = _command;
//            ofNotifyEvent(MESSAGE_AVAILABLE, e, this);
//            //bLoaded = bg.loadImage("images/background/"+_command+".jpg");
//            if (bLoaded)
//            {
//#ifdef USE_FLASHCOMMUNICATION
//#else
//                string addr = "/openframeworks";
//                string arg = e.arg;
//                ofxOscMessage m;
//                m.setAddress(  addr );
//                m.addStringArg( arg );
//                sender.sendMessage( m );
//                ofLog(OF_LOG_VERBOSE,"NetworkHandler sent "+addr+" " +arg);
//#endif
//            }
//        }
//        else if (changeFGfound!=string::npos)
//        {
//            fn = "images/foreground/"+_command+".png";
//            static OSCEventArgs e;
//            e.arg = _command;
//            ofNotifyEvent(MESSAGE_AVAILABLE, e, this);
//            if (bLoaded)
//            {
//#ifdef USE_FLASHCOMMUNICATION
//#else
//                string addr = "/openframeworks";
//                string arg = e.arg;
//                ofxOscMessage m;
//                m.setAddress(  addr );
//                m.addStringArg( arg );
//                sender.sendMessage( m );
//                ofLog(OF_LOG_VERBOSE,"NetworkHandler sent "+addr+" " +arg);
//#endif
//            }
//        }
//        else if (takePhotofound!=string::npos)
//        {
//            bTakePhoto = true;
//            ofLog(OF_LOG_VERBOSE,"Networkhandler recieve take photo");
//            static OSCEventArgs e;
//            e.arg = _command;
//            ofNotifyEvent(MESSAGE_AVAILABLE, e, this);
//
//        }
//        else if (changeModelfound!=string::npos)
//        {
//
//            static OSCEventArgs e;
//            e.arg = _command;
//            ofNotifyEvent(MESSAGE_AVAILABLE, e, this);
//        }
//        /*else if (emailSent)
//        {
//            bLoaded = false;
//            ofLog(OF_LOG_VERBOSE,"Networkhandler recieve email sent");
//            static OSCEventArgs e;
//            e.arg = _command;
//            ofNotifyEvent(MESSAGE_AVAILABLE, e, this);
//
//        }*/
//        else
//        {
//            static OSCEventArgs e;
//            e.arg = _command;
//            ofNotifyEvent(MESSAGE_AVAILABLE, e, this);
//        }

    }
};

#endif // MYNETWORKHANDLER_H_INCLUDED




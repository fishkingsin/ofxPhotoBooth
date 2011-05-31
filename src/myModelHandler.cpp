#include "myModelHandler.h"
#include "PBModel.h"
void myModelHandler::update()
{

    for (int i = 0 ; i<numModels ; i++)
    {
        models[i]->update();

    }


}

void myModelHandler::draw()
{
    for (int i = 0 ; i<numModels ; i++)
    {
        models[i]->draw();
    }
}


void myModelHandler::draw2D()
{
    for (int i = 0 ; i<numModels ; i++)
    {
        models[i]->draw2D();
    }
}
void myModelHandler::changeModels(vector<int> &id,vector<string> &name)
{

    int changeZise = id.size();
    for (int i = 0 ; i < id.size() ;i++)
    {
        OFXLOG("ID :");
        OFXLOG(id[i]);

    }
    for (int i = 0 ; i < name.size() ;i++)
    {
        OFXLOG("Name :");
        OFXLOG(name[i]);
    }
    for (int i = 0 ; i<models.size() ; i++)
    {
        if (changeZise>i)
        {
            ofLog(OF_LOG_VERBOSE,"input id %i model id %i",id[i],models[i]->getId());
            if (id[i]==models[i]->getId())

            {

                ofLog(OF_LOG_VERBOSE,"Change Model %i %s",id[i],name[i].c_str());
                models[i]->changeModel(name[i]);
            }
        }
        else
            models[i]->changeModel(" ");
    }
}
//void myModelHandler::praseMessage(string input)
//{
//    //model:1,budda;2.tart;4,budda;3.tart
//    string tag("model:");
//
//    size_t  pos = input.find(tag);    // position of "live" in str
//    string str3 = input.substr (pos+tag.length());
//    printf("myModelHandler i am going to phrase this :%s\n",input.c_str());
//
//    string word;
//    stringstream stream(str3);
//    while ( getline(stream, word, ';') )
//    {
//        string str4;
//        stringstream stream2(word);
//        int count = 0 ;
//        int id = -1;
//        string input_name;
//        while ( getline(stream2, str4, ',') )
//        {
//
//            switch (count)
//            {
//            case 0:
//                id = atoi(str4.c_str());
//                break;
//            case 1:
//                input_name = str4;
//                printf("id: %i - name: %s\n",id,input_name.c_str());
//                changeModel(id,input_name);
//                break;
//            default:
//                return;
//                break;
//            }
//            count++;
//        }
//        /*char * pch;
//        char buf[word.length()];
//        char* delim = ",";
//        sprintf(buf,word.c_str());
//        pch = strtok (buf,delim);
//        int count = 0;
//        int id = -1;
//        string name;
//        while (pch != NULL)
//        {
//
//            pch = strtok (NULL, delim);
//
//            switch (count)
//            {
//            case 0:
//                id = atoi(pch);
//                printf ("%i\n",id);
//                break;
//            case 1:
////                name = string(pch);
////                printf("%s\n",name);
//                //changeModel(id,pch);
//                break;
//            default:
//                return;
//                break;
//            }
//
//            count++;
//        }*/
//
//    }
//
//
//}
//--------------------------------------------------------------
//void myModelHandler::updateModelMatrix(int _i , float *m , float *project)
//{
//    for (int i = 0 ; i<NUM ; i++)
//    {
//        if (_i==i)
//        {
//            models[i]->setModelMatrix(_i,m , project);
////            PBModel* temp = (PBModel*)modelMap[i];
////            if (temp)temp->setModelMatrix(_i,m , project);
////            else return;
//        }
//    }
//
//}

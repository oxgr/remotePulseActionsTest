#ifndef _THREADED_OBJECT
#define _THREADED_OBJECT

#include "ofMain.h"


class threadedCurl : public ofThread{
    
public:
    
    
    bool bIsNewCurl;
    bool bIsGet;
    string computerID;
    string theHttpAddress;
    string messages;
    ofxJSONElement jsonResult;
    
    int bpmThread;
    bool touchThread;
    string statusThread;
    
    // threaded fucntions that share data need to use lock (mutex)
    // and unlock in order to write to that data
    // otherwise it's possible to get crashes.
    //
    // also no opengl specific stuff will work in a thread...
    // threads can't create textures, or draw stuff on the screen
    // since opengl is single thread safe
    
    void setup(string compId, string httpAddress){
        computerID = compId;
        theHttpAddress = httpAddress;
    }
    
    
    void start(){
        bIsGet = true;
        startThread();   // blocking, verbose
    }
    
    void startPost(int bpm, bool touch, string status, string mes){
        bpmThread = bpm;
        touchThread = touch;
        statusThread = status;
        messages = mes;
        bIsGet = false;
        startThread();   // blocking, verbose
    }
    
    void stop(){
        stopThread();
    }
    
    //--------------------------
    void threadedFunction(){
        /*
         curl -s -S
         From the man page:
         -s Silent or quiet mode. Don't show progress meter or error messages. Makes Curl mute.
         -S When used with -s it makes curl show an error message if it fails.
         */
        
        while(isThreadRunning()){
            if( lock() ){
                if (bIsGet){
                    string command = "curl -s -S '" + theHttpAddress + "'";
                    //ofLog()<< command;
                    string capture = ofSystem(command);
                    //ofLog()<< capture;
                    jsonResult.parse(capture);
                    bIsNewCurl = true;
                }else{
                    // do post
                    string command = "curl -s -S -X POST -H 'Content-Type: application/json' -d '{ \"compid\":\"" + computerID+"\",\"message\" :\""+messages+"\", \"bpm\":"+ ofToString(bpmThread) +", \"touch\":\""+ ofToString(touchThread)+"\",\"status\":\""+statusThread+"\",\"time-stamp\":\"" + ofGetTimestampString() +"\", \"time\":"+ofToString(ofGetUnixTime())+" }' " + theHttpAddress;
                    string capture = ofSystem(command);
                    //ofLog()<< capture;
                }
                unlock();
                stop();
                
            }
        }
    }
    
    
};

#endif

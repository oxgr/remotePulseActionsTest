//
//  webNetwork.h
//  remotePulse
//
//  Created by Stephan Schulz on 2020-01-17.
//

#ifndef webNetwork_h
#define webNetwork_h

#include "ofEvents.h"
#include "socketHandler.h"

class webNetwork{
private:
    
public:
    
    socketHandler socket_object;
    
    bool bDebug;
    
    ofxPanel gui_web;
    ofParameter<bool> bEnableWeb;
    bool old_bEnableWeb;
    
    float other_runtime_fade = 255;
    string other_runtimeStr;
    string old_other_runtimeStr;
    string other_serialStr;
    
    void setup(){
        
          socket_object.setup();
        ofAddListener(socketHandler::messageReceived , this, &webNetwork::receivedMessage);
        
        gui_web.setup();
        gui_web.setName("internet");
        gui_web.setHeaderBackgroundColor(ofColor(255,0,0));
        
        gui_web.add(bEnableWeb.set("enableWeb", false));
        gui_web.add(socket_object.gui_socket);
        gui_web.loadFromFile("GUIs/gui_web.xml");
        
        socket_object.init();
    }
    
    void exit(){
         socket_object.appExit();
    }
    
    void receivedMessage(ofxJSONElement & e){
        //ofLog()<<"author: " + e.at(0) +" text: "+ e.at(1);
    }
    
    void update(){
            socket_object.update();
    }
    
    void draw(int _x, int _y){
        
        socket_object.draw(_x,_y);
    }
    
    void drawAliveMsg(int _x, int _y){
        ofPushMatrix();
        ofTranslate(_x,_y);
        other_runtime_fade -= 1;
        other_runtime_fade = MAX(10,other_runtime_fade);
        
        ofSetColor(0,other_runtime_fade);
        ofDrawBitmapString("other runtime:"+other_runtimeStr,0, 0);
        ofSetColor(0);
        ofDrawBitmapString("other runtime:"+other_runtimeStr,0, 0);
        
        ofSetColor(0,other_runtime_fade);
        ofDrawBitmapString("other alive:"+other_serialStr,0, 20);
        ofSetColor(0);
        ofDrawBitmapString("other alive:"+other_serialStr,0, 20);
        
        ofPopMatrix();
        
    }
    
    void checkGui(){
        
        if(bEnableWeb != old_bEnableWeb){
            old_bEnableWeb = bEnableWeb;
            //            if(bEnableOSC == false) sendMode(0);
        }
        
    }
    
    string getComputerID(){
        return socket_object.computerID;
    }
};

#endif /* webNetwork_h */

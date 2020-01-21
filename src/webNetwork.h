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
    
     ofParameter<int> webSendInterval;
     int sendTimer;
    
    ofParameter <bool> bPeriodicResend;
    float resendTimer;
    string lastTouchID = "";
    int lastTouchValue;
    
    bool gotBPM = false;
    bool gotTouch = false;
    
    int rxBPM;
    int old_rxBPM;
    ofParameter<string> rxBPM_str;
    
    int txBPM;
    ofParameter<string> txBPM_str;
    
    bool rxTouch;
    bool txTouch;
    ofParameter<string> rxTouch_str;
    ofParameter<string> txTouch_str;
    
    bool wantToSendData = false;
    string txAppAlive;
    
    string sendToID;
    
    void setup(){
        
        socket_object.setup();
        ofAddListener(socketHandler::messageReceived , this, &webNetwork::receivedMessage);
        
        sendToID = socket_object.other_computerID;
        
        gui_web.setup();
        gui_web.setName("internet");
        gui_web.setHeaderBackgroundColor(ofColor(255,0,0));
        
        gui_web.add(bEnableWeb.set("enableWeb", true));
        
        gui_web.add(webSendInterval.set("webSendIntervalMil", 10,0,100));
        
        gui_web.add(rxTouch_str.set("other touch received", ""));
        gui_web.add(rxBPM_str.set("other bpm received", ""));
        
        gui_web.add(txTouch_str.set("me touch sent", ""));
        gui_web.add(txBPM_str.set("me bpm sent", ""));
        
        gui_web.add(bPeriodicResend.set("periodicResend", false));
        
        gui_web.add(socket_object.gui_socket);
        gui_web.loadFromFile("GUIs/gui_web.xml");
        
        socket_object.init();
    }
    
    void exit(){
         socket_object.appExit();
    }
    
    void update(){
            socket_object.update();
        
        if(bEnableWeb){
            
             //TODO: if messageBuffer.size() == 0   sendTimer = ofGetElapsedTimeMillis();
            if(ofGetElapsedTimeMillis() - sendTimer > webSendInterval){
                sendTimer = ofGetElapsedTimeMillis();
             
                if(wantToSendData == true){
                    wantToSendData = false;
                    ofLog()<<"wantToSendData == true";
                    ofLog()<<"txBPM "<<txBPM<<" txTouch "<<txTouch<<" txAppAlive "<<txAppAlive;
                     socket_object.sendValues(txBPM,txTouch,txAppAlive);
                }
                //TODO: add sending
               
            }
            
             // let's make sure we send the touch state, since it is responsible for stopping action
//            if(lastTouchID != "" && ofGetElapsedTimef() - resendTimer > 10){
//                resendTimer = ofGetElapsedTimef();
//            ofLog()<<"resend touch every x seconds";
//                addTouchMessage(lastTouchID,lastTouchValue);
//            }

        }
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
    
    void addBPMMessage(string _sendToID, int _value){
        
         wantToSendData = true;
        
//        ofxOscMessage myMessage;
//        
//        myMessage.setAddress("/BPM");
//        myMessage.addStringArg(_ip); 
//        myMessage.addIntArg(_value);
//        messageBuffer.push_back(myMessage);
        
//        if(bDebug) 
            ofLog()<<ofGetTimestampString()<<" addBPMMessage() msg: "<<_sendToID<<" value "<<_value;

        txBPM = _value;
        txBPM_str = ofToString(_value);
    }
    
    void addTouchMessage(string _sendToID, bool _value){
        
        wantToSendData = true;
        
        lastTouchID = _sendToID;
        lastTouchValue = _value;
        
//        ofxOscMessage myMessage;
//
//        myMessage.setAddress("/Touch");
//        myMessage.addStringArg(_ip); 
//        myMessage.addIntArg(_value);
//        messageBuffer.push_back(myMessage);
        
//        if(bDebug)
            ofLog()<<ofGetTimestampString()<<" addTouchMessage() msg: "<<_sendToID<<" value "<<_value;

        txTouch = _value;
        txTouch_str = ofToString(_value);
        
        resendTimer = ofGetElapsedTimef();
        
    }
    
    void addAliveMessage(string _sendToID, string _runtimeStr, int _serialAliveStr){
       
        
        wantToSendData = true;
        
        if(bDebug) ofLog()<<"addAliveMessage () "<<_sendToID<<" _runtimeStr "<<_runtimeStr<<" _serialAliveStr "<<_serialAliveStr;
        
        txAppAlive = _runtimeStr + "*" + ofToString(_serialAliveStr);

    }
    
    void checkGui(){
        
        if(bEnableWeb != old_bEnableWeb){
            old_bEnableWeb = bEnableWeb;
            //            if(bEnableOSC == false) sendMode(0);
        }
        
        //int bpm, bool touch, string status
//        socket_object.sendValues(25,true,"1:13:19-101");
        //socket_object.sendToSocket("send test mes");
    }
    
    string getComputerID(){
        return socket_object.me_computerID;
    }
    
    void receivedMessage(ofxJSONElement & e){
        
       
        int temp_bpm = e["bpm"].asInt();
         bool temp_touch = ofToBool(e["touch"].asString());
        string temp_status = e["status"].asString();
        
        ofLog()<< "changed element: " << e["compid"].asString();
        ofLog()<< "changed element: " << e["message"].asString();
        ofLog()<< "changed element: " << temp_bpm;
        ofLog()<< "changed element: " << temp_touch;
        ofLog()<< "changed element: " << temp_status;
        ofLog()<< "  element: " << e["time-stamp"].asString();
        // unix time
        ofLog()<< "changed element: " << ofToString(e["time"].asInt());
        
        if(rxBPM != temp_bpm){
            rxBPM = temp_bpm;
            rxBPM_str = ofToString(rxBPM);
            gotBPM = true;
        }
        
        if(rxTouch != temp_touch){
            rxTouch = temp_touch; 
            rxTouch_str = ofToString(rxTouch);
            gotTouch= true;
        }
        
        vector<string> split_str = ofSplitString(temp_status, "*");
        if(split_str.size() >= 2){
            if(other_runtimeStr != split_str[0] || other_serialStr != split_str[1]){
                other_runtimeStr = split_str[0];
                other_serialStr = split_str[1];
                other_runtime_fade = 255;
            }
        }
        //time
        
        //ofLog()<<"author: " + e.at(0) +" text: "+ e.at(1);
    }
};

#endif /* webNetwork_h */

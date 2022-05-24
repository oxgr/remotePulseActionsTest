//
//  webNetwork.h
//  remotePulse
//
//  Created by Stephan Schulz on 2020-01-17.
//

#ifndef webNetwork_h
#define webNetwork_h

#include "ofEvents.h"
//#include "socketHandler.h"
#include "ofxMQTT.h"

//https://remote-pulse.cloud.shiftr.io

class webNetwork{
private:
    
public:
    
    //    socketHandler socket_object;
    ofxMQTT mqtt_client;
    
    bool bDebug;
    
    ofxPanel gui_web;
    ofParameter<bool> bEnableWeb;
    bool old_bEnableWeb;
    
    float other_runtime_fade = 255;
    string other_runtimeStr;
    //    string old_other_runtimeStr;
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
    
    //    string sendToID;
    string other_computerID;
    string me_computerID;
    
    float lastMsgTimer = 0;
    
    string serialAliveStr, runtimeStr;
    
    void setup(){
        
        //        socket_object.setup();
        //        ofAddListener(socketHandler::messageReceived , this, &webNetwork::receivedMessage);
        
        loadIdFile("id.txt");
        
        //begin(string hostname, int port)
        mqtt_client.begin("remote-pulse.cloud.shiftr.io", 1883);
        //connect(string clientId, string username, string password)
        bool success = mqtt_client.connect(me_computerID, "remote-pulse", "AKAcwIpazHPzJ0VQ");
        //mqtt://remote-pulse:AKAcwIpazHPzJ0VQ@remote-pulse.cloud.shiftr.io
        ofLog()<<"success "<<success;
        
        ofAddListener(mqtt_client.onOnline, this, &webNetwork::onOnline);
        ofAddListener(mqtt_client.onOffline, this, &webNetwork::onOffline);
        ofAddListener(mqtt_client.onMessage, this, &webNetwork::onMessage);
        
       
        
        //        sendToID = socket_object.other_computerID;
        
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
        
//        gui_web.add(socket_object.gui_socket);
        gui_web.loadFromFile("GUIs/gui_web.xml");
        
        txTouch_str = "0";
        //        socket_object.init();
    }
    
    void exit(){
        mqtt_client.disconnect();
    }
    
    void update(){
        mqtt_client.update();
        
        if(bEnableWeb){
            
            //TODO: if messageBuffer.size() == 0   sendTimer = ofGetElapsedTimeMillis();
            if(ofGetElapsedTimeMillis() - sendTimer > webSendInterval){
                sendTimer = ofGetElapsedTimeMillis();
                
                if(wantToSendData == true){
                    wantToSendData = false;
                    ofLog()<<"wantToSendData == true";
                    ofLog()<<"txBPM "<<txBPM<<" txTouch "<<txTouch<<" txAppAlive "<<txAppAlive;
//                    socket_object.sendValues(txBPM,txTouch,txAppAlive);
                    string new_msg;
                    new_msg += "pc~"+me_computerID+",";
                    new_msg += "bpm~"+ofToString(txBPM)+",";
                    new_msg += "touch~"+txTouch_str.get()+",";
                    new_msg += "time~"+runtimeStr+",";
                    new_msg += "serial~"+serialAliveStr;
                    mqtt_client.publish("remotePulse",new_msg);
                }
                //TODO: add sending
                
            }
            
            // let's make sure we send the touch state, since it is responsible for stopping action
            //            if(lastTouchID != "" && ofGetElapsedTimef() - resendTimer > 10){
            //                resendTimer = ofGetElapsedTimef();
            //            ofLog()<<"resend touch every x seconds";
            //                addTouchMessage(lastTouchID,lastTouchValue);
            //            }
            
            if(rxTouch == 1 && ofGetElapsedTimef() - lastMsgTimer > 30){
                lastMsgTimer = ofGetElapsedTimef();
                gotTouch = true;
                rxTouch = 0;
                ofLog()<<" lastMsgTimer > 30 && last rxTouch == 1. have not received anything for a long time via web! force unTouch";
            }
        }
    }
    
    void draw(int _x, int _y){
        
//        socket_object.draw(_x,_y);
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
    
    void addAliveMessage(string _sendToID, string _runtimeStr, int _serialAliveCounter){
        
        
        wantToSendData = true;
        
        if(bDebug) ofLog()<<"addAliveMessage () "<<_sendToID<<" _runtimeStr "<<_runtimeStr<<" _serialAliveCounter "<<_serialAliveCounter;
        
        txAppAlive = _runtimeStr + "*" + ofToString(_serialAliveCounter);
        serialAliveStr = ofToString(_serialAliveCounter);
        runtimeStr = _runtimeStr;
    
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
            return me_computerID;
        }
    
    void onOnline(){
        ofLog() << "online";
        
        //subscribe(string topic, int qos)
        mqtt_client.subscribe("remotePulse");
        
        string isOnline = (mqtt_client.connected() ? "true" : "false");
        //        ofSendMessage("connected:"+isOnline);
    }
    
    void onOffline(){
        ofLog() << "offline";
    }
    
    void onMessage(ofxMQTTMessage &msg){
        ofLog() << "message: " << msg.topic << " - " << msg.payload;
        //        ofSendMessage(msg.topic + ":" +msg.payload);
        if(msg.topic == "remotePulse"){
            vector<string> splitPayload = ofSplitString(msg.payload,",");
            
            if(splitPayload.size() == 5){
                
                vector<string> splitComputer = ofSplitString(splitPayload[0],"~");
                string temp_otherID = splitComputer[1]; 
                
                if(temp_otherID == other_computerID){
                    vector<string> splitBPM = ofSplitString(splitPayload[1],"~");
                    int temp_bpm = ofToInt(splitBPM[1]); 
                    
                    vector<string> splitTouch = ofSplitString(splitPayload[2],"~");
                    bool temp_touch = ofToBool(splitTouch[1]);
                    
                    vector<string> splitRuntime = ofSplitString(splitPayload[3],"~");
                    string temp_runtimeStr = splitRuntime[1];
                    
                    vector<string> splitSerial = ofSplitString(splitPayload[4],"~");
                    string temp_serialStr = splitSerial[1];
                    
                    if(rxBPM != temp_bpm){
                        rxBPM = temp_bpm;
                        rxBPM_str = ofToString(rxBPM);
                        gotBPM = true;
                        lastMsgTimer = ofGetElapsedTimef();
                    }
                    
                    if(rxTouch != temp_touch){
                        rxTouch = temp_touch; 
                        rxTouch_str = ofToString(rxTouch);
                        gotTouch= true;
                        lastMsgTimer = ofGetElapsedTimef();
                    }
                    
                    if(other_runtimeStr != temp_runtimeStr || other_serialStr != temp_serialStr){
                        other_runtimeStr = temp_runtimeStr;
                        other_serialStr = temp_serialStr;
                        other_runtime_fade = 255;
                        lastMsgTimer = ofGetElapsedTimef();
                    }
                    
                }//end if(temp_otherID == other_computerID)
            }//end if(splitPayload.size() == 5)
        }//end  if(msg.topic == "remotePulse")
        
        
    }
    
    /*
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
     lastMsgTimer = ofGetElapsedTimef();
     }
     
     if(rxTouch != temp_touch){
     rxTouch = temp_touch; 
     rxTouch_str = ofToString(rxTouch);
     gotTouch= true;
     lastMsgTimer = ofGetElapsedTimef();
     }
     
     vector<string> split_str = ofSplitString(temp_status, "*");
     if(split_str.size() >= 2){
     if(other_runtimeStr != split_str[0] || other_serialStr != split_str[1]){
     other_runtimeStr = split_str[0];
     other_serialStr = split_str[1];
     other_runtime_fade = 255;
     lastMsgTimer = ofGetElapsedTimef();
     }
     }
     //time
     
     //ofLog()<<"author: " + e.at(0) +" text: "+ e.at(1);
     }
     */
    
    void loadIdFile(string _fileName){
        ofBuffer buff = ofBufferFromFile(_fileName);
        
        if(buff.size()) {
            for (ofBuffer::Line it = buff.getLines().begin(), end = buff.getLines().end(); it != end; ++it) {
                string line = *it;
                
                
                if(line.empty() == false) {
                    vector<string> split_str = ofSplitString(line, ":");
                    
                    if(split_str[0] == "me") me_computerID = split_str[1];
                    else  if(split_str[0] == "other") other_computerID = split_str[1];
                    
                }
            }
        } else{
            ofLog()<<"!!! no good info in id.txt !!!";
            me_computerID = "x";
            other_computerID = "y";
        }
        ofLog()<< "me_computerID: "+ me_computerID;
        ofLog()<< "other_computerID: "+ other_computerID;
        //        sendToID = 
    }
};

#endif /* webNetwork_h */

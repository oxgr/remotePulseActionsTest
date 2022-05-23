//
//  circle.cpp
//  simpleEventsExample
//
//  Created by Roy Macdonald on 12/25/12.
//  Copyright (c) 2012 micasa. All rights reserved.
//

#include "socketHandler.h"

// the static event, or any static variable, must be initialized outside of the class definition.
ofEvent<ofxJSONElement> socketHandler::messageReceived = ofEvent<ofxJSONElement>();

void socketHandler::setup(){
    
 
     ofBuffer buff = ofBufferFromFile("id.txt");
    
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
    
    // ofSetLogLevel(OF_LOG_VERBOSE);
    stateChangedExternal = false;
    
    
    //isAliveInit = false;
    //isAlive.addListener(this, &socketHandler::isAliveChanged);
//    triggerSend.addListener(this,&socketHandler::triggerSendPressed);
    
    
//    gui_socket.setup();
    gui_socket.setName("socket");
//    gui_socket.setPosition(20,20);
    
    
//    gui_socket.add(showText.set("showText",true));
//    gui_socket.add(textPosX.set("textPosX", 50,0,ofGetWidth()));
//    gui_socket.add(textPosY.set("textPosY",50,0,ofGetHeight()));
    gui_socket.add(itvlSecCheckWeb.set("checkDatabase",10,1,30));
    //gui_socket.add(itvlSecCheckSoc.set("itvlSecCheckSoc",10,1,30));
    //gui_socket.add(hidePing.set("hidePing",true));
    
    
    //gui_socket.add(isConnected.set("socket_connection:","false"));
    //gui_socket.add(isConnectedWeb.set("web_connection:","false"));
    //gui_socket.add(isAlive.set("isAlive","false"));
    gui_socket.add(textField.set("", "test"));
    gui_socket.add(triggerSend.set("Send",false));
    triggerSend.addListener(this,&socketHandler::triggerSendPressed);
//    gui_socket.loadFromFile("settings/gui_sockets.xml");
    
    //parameters_socket.add(<#ofAbstractParameter &p#>, <#Args &parameters...#>)
    //ofLog()<<"setting up";
//    textField = "app started";
    isConn = false;
    autoTimerWeb = 0;
    autoTimerSoc  = 0;
    //itvlSecCheckWeb = 10;
    
    //isConnWeb = checkInternet();    isConnWeb = true;
    isConnectedWeb = ofToString(isConnWeb);
    isLastCall = false;
    
    //theHttpAddress = "http://localhost:3001/";
//    theHttpAddress = "http://pulseapi.nfshost.com/";
    theHttpAddress = "https://remote-pulse-server.glitch.me";
    //pulseapi.nfshost.com
    
 threadGet.setup(me_computerID, theHttpAddress);
}

void socketHandler::init(){
 textField = "app started";   
    triggerSend = true;
}
int socketHandler::findID(){
    return 0;
}

void socketHandler::update(){

    //getDatabase()
    
    
    // only poll if thread is available
    if(((ofGetElapsedTimeMillis()-autoTimerWeb) > itvlSecCheckWeb*1000) && (!threadGet.isThreadRunning()) ){
        // tempMessage = getDatabase();
        
        autoTimerWeb = ofGetElapsedTimeMillis();
        
        // check if the database has changed
        threadGet.start();
        
    }
    
    // send message if there are items in the deque
    if((threadGet.isThreadRunning() == false) && deqToSend.size()> 0 ){
        threadGet.startPost(deqToSend.at(0).bpm, deqToSend.at(0).touch, deqToSend.at(0).status, deqToSend.at(0).message);
        deqToSend.pop_front();
    }
    
    
    /*
     if(bNewitemSent && (ofGetElapsedTimeMillis()-startItemWait > 5000)){
     bNewitemSent = false;
     // word wasn't echoed back so connection is bad
     //terminateConnection();
     }*/
    
    // if there is a
    if((threadGet.isThreadRunning() == false) && threadGet.bIsNewCurl){
        threadGet.bIsNewCurl = false;
        processDatabase(threadGet.jsonResult);
        
    }
}



void socketHandler::draw(int _x, int _y){
    
    ofPushMatrix();
    ofTranslate(_x,_y);
//    if(showText){
        int h = 15;
    int temp_y = 0;
    string mes;
    mes += " me_ID: " +me_computerID + " other_ID: " +other_computerID;
    ofDrawBitmapString(mes, 0, temp_y);
    temp_y+=h;
    temp_y+=h;
      int spaceing = 15*5;
        for(int i=0; i<tempMessage.size(); i++){
            //tempMessage[i]
            mes = tempMessage[i]["compid"].asString() ;
            //mes += tempMessage[i]["_id"].asString();
            mes += " @ " + convertTime(tempMessage[i]["time"].asString());
            mes += "\n mess - " +tempMessage[i]["message"].asString();
            mes += "\n bpm - " +tempMessage[i]["bpm"].asString();
            mes += "\n touch - " +tempMessage[i]["touch"].asString();
            mes += "\n status - " +tempMessage[i]["status"].asString();
//            ofDrawBitmapString(mes, 0, temp_y+=h);
              ofDrawBitmapString(mes, 0, temp_y + i*spaceing);
        }

    
    ofPopMatrix();
}

string socketHandler::convertTime(string num){
    //num.erase(num.length()-4,3);
    time_t t = ofToDouble(num);
    struct tm *tm = localtime(&t);
    char date[20];
    strftime(date, sizeof(date), "%h %d %H:%M", tm);
    return date;
}

void socketHandler::sendValues(int bpm, bool touch, string status){
    mostRecentVals.bpm = bpm;
    mostRecentVals.touch = touch;
    mostRecentVals.status = status;
    mostRecentVals.isSent = false;
    deqToSend.push_back(mostRecentVals);
    
    /*
     threadGet.startPost(bpm,touch,status, textField);
     bpmT = bpm;
     touchT = touch;
     statusT =status;
     */
}

void socketHandler::triggerSendPressed(bool & triggerSend){
    mostRecentVals.message = textField;
    mostRecentVals.isSent = false;
    deqToSend.push_back(mostRecentVals);
    triggerSend = false;
    
    /*
     threadGet.startPost(bpmT,touchT,statusT, textField);
     triggerSend = false;
     */ 
}


void socketHandler::checkDeque(){
    if(messages.size() > 10){
        messages.pop_back();
    }
}


void socketHandler::processDatabase(ofxJSONElement elem){
    
    // if init should set temp to old right away
    
    if (initListener){
        //olDB = elem;
        olDB.clear();
        for(int i=0; i<elem.size(); i++){
            olDB.push_back(ofToString(elem[i]));
        }
        initListener = false;
    }
    
    
    // compare past and present database and fire an event if there was a change in one of them and say which ones.
    // check if old and new DB are still the same size.
    
    if(elem.size() == olDB.size()){
        
        for(int i=0; i<elem.size(); i++){
            //ofLog()<<"pintID";
            //ofLog()<<elem[i]["compid"].asString();
            //ofLog()<<computerID;
            if(elem[i]["compid"].asString() == me_computerID){
                
                // don't bother comparing bc came from home computer
                
            }else{
                string tempNow = ofToString(elem[i]);
                string tempOld = olDB.at(i);
                if (tempNow != tempOld){
                    //ofLog()<<elem[i]["message"];
                    ofxJSONElement temp = elem[i];
                    //ofLog()<<temp["message"];
                    ofNotifyEvent(messageReceived, temp);
                }
            }
        }
        
        //ofNotifyEvent(messageReceived, tempToSend);
    }else{
        // a computer was added or deleted or this init.
    }
    
    olDB.clear();
    for(int i=0; i<elem.size(); i++){
        olDB.push_back(ofToString(elem[i]));
    }
    
    tempMessage = elem;
}




void socketHandler::appExit(){
    //client.send("ofisOn_false");
    //client.close();
    //terminateConnection();
    isLastCall = false;
//    gui_socket.saveToFile("settings/gui_sockets.xml");
    //pingTest.waitForThread();
}

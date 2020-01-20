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
                computerID = line;
                break;
            }
        }
    } else{
        computerID = "x";
    }
    ofLog()<< "load: "+ computerID;
    
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
    theHttpAddress = "http://pulseapi.nfshost.com/";
    //pulseapi.nfshost.com
    
 threadGet.setup(computerID, theHttpAddress);
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
    
    
    if(((ofGetElapsedTimeMillis()-autoTimerWeb) > itvlSecCheckWeb*1000) ){
       // tempMessage = getDatabase();
        
       
        autoTimerWeb = ofGetElapsedTimeMillis();
        //checkInternet();
//        tempMessage = getDatabase();
           threadGet.start();
    }
    
    if(bNewitemSent && (ofGetElapsedTimeMillis()-startItemWait > 5000)){
        bNewitemSent = false;
        // word wasn't echoed back so connection is bad
        //terminateConnection();
    }
    
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
     mes += " computerID " +computerID;
    ofDrawBitmapString(mes, 0, temp_y+=h);
        for(int i=0; i<tempMessage.size(); i++){
            //tempMessage[i]
            mes = tempMessage[i]["compid"].asString() ;
            //mes += tempMessage[i]["_id"].asString();
            mes += " @ " + convertTime(tempMessage[i]["time"].asString());
            mes += " - " +tempMessage[i]["message"].asString();
            ofDrawBitmapString(mes, 0, temp_y+=h);
        }
        
        /*
        for(int i=0; i<messages.size(); i++){
            string mes = messages.at(i)["author"].asString() + " : ";
            mes += " @ " + convertTime(messages.at(i)["time"].asString());
            mes += " - " +messages.at(i)["text"].asString();
            //ofDrawString(mes, x, );
            //ofDrawBitmapStringHighlight(<#std::string text#>, <#int x#>, <#int y#>)
            //ofColor c = ofColor::fromHex(0xFF0000);
            string hexTemp = messages.at(i)["color"].asString();
            hexTemp.erase(0,1);
            hexTemp = "0x" + hexTemp;
            //int hexVal = ofToInt();
            ofColor c = ofColor::fromHex(ofHexToInt(hexTemp));
            
            //ofDrawBitmapStringHighlight(mes, textPosX, textPosY+i*15);
            
            ofSetColor(c);
            ofDrawBitmapString(mes, textPosX, textPosY+i*spaceing);
            
            //ofDrawRectangle( textPosX-spaceing, (textPosY+i*spaceing)-spaceing, spaceing, spaceing);
        }
         */
//    }
    
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

void socketHandler::triggerSendPressed(bool & triggerSend){
    ofLog()<< "sending "<<textField;
    //sendToSocket(textField);
    //client.send(textField);
    //sendToDBPut(textField);
//    sendToDBPost(textField);
    
    threadGet.startPost(textField); 
    
    triggerSend = false;
    /*
    if( dbId == "na"){
        sendToDBPost(textField);
    }else{
        sendToDBPut(textField);
    }*/
    
   //sendToDB2(textField);
    
}


//--------------------------------------------------------------

/*
 void isAliveChanged(bool & isAlive){
 //ofLog()<<"changed";
 //if(isAliveInit & !stateChangedExternal){
 if(isAlive != isAliveOld){
 client.send("ofisAlive_"+ ofToString(isAlive));
 isAliveOld = isAlive;
 }
 
 }
 */

//--------------------------------------------------------------


/*
void socketHandler::sendToSocket(string mess){
    client.send(mess);
    bNewitemSent = true;
    startItemWait = ofGetElapsedTimeMillis();
    lastItem = mess;
}
*/


void socketHandler::checkDeque(){
    if(messages.size() > 10){
        messages.pop_back();
    }
}


void socketHandler::processDatabase(ofxJSONElement elem){
    
    // if init should set temp to old right away
    
    
    dbId = "na";
    // check if the computer ID is already represented
    for(int i=0; i<elem.size(); i++){
        if(elem[i]["comp-id"].asString() == computerID){
            dbId = elem[i]["_id"].asString();
            break;
        }
    }
    // compare past and present database and fire an event if there was a change in one of them and say which ones.
    // check if old and new DB are still the same size.
    
    if(elem.size() == oldDb.size()){
        
        for(int i=0; i<elem.size(); i++){
            if(elem[i]["comp-id"].asString() == computerID){
                // don't bother comparing bc came from home computer
                
            }else{
                //if()temp[i]
                /*
                 if (temp[i].asString() != oldDb[i].asString()){
                 // something has changed send it to the event.
                 ofxJSONElement toSend = temp[i];
                 ofNotifyEvent(messageReceived, toSend);
                 }
                 */
                
            }
        }
        
        //ofNotifyEvent(messageReceived, tempToSend);
    }else{
        // a computer was added or deleted or this init.
    }
    
    tempMessage = elem;
    oldDb = elem;
}

//void socketHandler::sendToDBPost(string messages){
//    string command2 = "curl -X POST -H 'Content-Type: application/json' -d '{ \"title\": \"Pizza\", \"price\": 10.5 }' http://localhost:3001/";
//    string command = "curl -X POST -H 'Content-Type: application/json' -d '{ \"compid\":\"" + computerID+"\",\"message\" :\""+messages+"\","+ +"\"time-stamp\":\"" + ofGetTimestampString() +"\", \"time\":"+ofToString(ofGetUnixTime())+" }' " + theHttpAddress;
//    command += " 2>&1 >/dev/null";
//    ofLog()<< command;
//    ofLog()<< command2;
//    string capture = ofSystem(command);
//    ofLog()<< capture;
//}

//void socketHandler::sendToDBPut(string messages){
//    //dbId =
//    string command = "curl -X PUT -H 'Content-Type: application/json' -d '{ \"compid\":\"" + computerID+"\",\"message\":\""+messages+"\",\"time-stamp\":\"" + ofGetTimestampString() +"\", \"time\":"+ofToString(ofGetUnixTime())+"}' "+theHttpAddress+computerID;
//    //string command = "curl -X POST -H 'Content-Type: application/json' -d '{ \"comp-id\":\"" + computerID+"\",\"message\" :\""+messages+"\","+ +"\"time-stamp\":\"" + ofGetTimestampString() +"\", \"time\":"+ofToString(ofGetUnixTime())+" }' " + theHttpAddress;
//    command += " 2>&1 >/dev/null";
//    ofLog()<< command;
//    string capture = ofSystem(command);
//    ofLog()<< capture;
//}


//ofxJSONElement socketHandler::getDatabase(){
//    ofxJSONElement temp;
//    string command = "curl '" + theHttpAddress + "'";
//    command += " 2>&1 >/dev/null";
//    ofLog()<< command;
//    string capture = ofSystem(command);
//    ofLog()<< capture;
//    temp.parse(capture);
//    // if init should set temp to old right away
//    
//    
//    dbId = "na";
//    // check if the computer ID is already represented
//    for(int i=0; i<temp.size(); i++){
//        if(temp[i]["comp-id"].asString() == computerID){
//            dbId = temp[i]["_id"].asString();
//            break;
//        }
//    }
//    // compare past and present database and fire an event if there was a change in one of them and say which ones.
//    // check if old and new DB are still the same size.
//    
//    if(temp.size() == oldDb.size()){
//        
//        for(int i=0; i<temp.size(); i++){
//            if(temp[i]["comp-id"].asString() == computerID){
//                // don't bother comparing bc came from home computer
//                
//            }else{
//                //if()temp[i]
//                /*
//                if (temp[i].asString() != oldDb[i].asString()){
//                    // something has changed send it to the event.
//                    ofxJSONElement toSend = temp[i];
//                    ofNotifyEvent(messageReceived, toSend);
//                }
//                 */
//            
//            }
//        }
//        
//        //ofNotifyEvent(messageReceived, tempToSend);
//    }else{
//    // a computer was added or deleted or this init.
//    }
//    tempMessage = temp;
//    
//    oldDb = temp;
//    return temp;
//}

void socketHandler::checkDatabase(){
    // compare past and present database and fire an event if there was a change in one of them and say which ones.
    // check if old and new DB are still the same size.
    
    
    
    
    
}

/*
//--------------------------------------------------------------
void socketHandler::onMessage( ofxLibwebsockets::Event& args ){
    //cout<<"got message "<<args.message<<endl;
    
    tempMessage.parse(args.message);
    bool isPing= false;
    //{"type":"message","data":{"time":1575577264125,"text":"ofisOn_true","author":"Computer-B","color":"#50514f"}}
    if ((tempMessage["type"].asString() == "message") && !isLastCall ){
        //messages.insert(const_iterator __position, <#const_reference __x#>)
        
        if(tempMessage["data"]["text"].asString().find("ping") != std::string::npos){
            isPing = true;
        }
        
        if((tempMessage["data"]["text"].asString() == lastItem) && (computerID == tempMessage["data"]["author"].asString() )){
            // it is from myself so ignore it
            // using it as a confirmation that the last message was received
            bNewitemSent = false;
        }else if(tempMessage["data"]["text"].asString().find(roger) != std::string::npos){
            // it is a roger do not have it trigger event.
        }else{
            // it is a new message that isn't just a roger.
            tempToSend.clear();
            tempToSend.push_back(tempMessage["data"]["author"].asString());
            tempToSend.push_back(tempMessage["data"]["text"].asString());
            ofNotifyEvent(messageReceived, tempToSend);
            // send out a roger to confirm message is received
            sendToSocket("roger - " + tempMessage["data"]["text"].asString());
        }
        if(isPing && hidePing){
            
        }else{
            messages.push_front(tempMessage["data"]);
        }
        
        checkDeque();
    }
    else if(tempMessage["type"].asString() == "history" ){
        for (int i=0; i<tempMessage["data"].size(); i++){
            messages.push_front(tempMessage["data"][i]);
            checkDeque();
        }
    }
    
    //ofLog()<< args.message;
}
*/



void socketHandler::appExit(){
    //client.send("ofisOn_false");
    //client.close();
    //terminateConnection();
    isLastCall = false;
//    gui_socket.saveToFile("settings/gui_sockets.xml");
    //pingTest.waitForThread();
}

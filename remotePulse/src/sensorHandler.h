//
//  sensorHandler.h
//  clouddisplaycontrol
//
//  Created by Caroline Record on 4/24/19.
//
//

#ifndef sensorHandler_h
#define sensorHandler_h

#include "ofxSerial.h"

/*
 alive
 touch:1
 alive
 BPM:70
 BPM:102
 BPM:105
 alive
 BPM:108
 BPM:112
 BPM:114
 BPM:70
 touch:0
 BPM:-1
 alive
 alive
 alive
 */

class SerialMessage
{
public:
    SerialMessage(): fade(0)
    {
    }
    SerialMessage(const std::string& _message,
                  const std::string& _exception,
                  int _fade):
    message(_message),
    exception(_exception),
    fade(_fade)
    {
    }
    std::string message;
    std::string exception;
    int fade;
};


class sensorHandler {
    
public:
    
    
//    oscNetwork * osc_object;
    
    vector<ofxIO::BufferedSerialDevice> devices;
    
    std::vector<SerialMessage> serialReceiveMessages;
    
    long serialSendTimer;
    std::vector<string> * serialSendBuffer;
    
    
    ofxPanel gui_sensor;
    ofParameter<float> smoothing;
    float old_smoothing;
    float lastSin, lastCos;
    
    //    ofParameter<int> fadeDuration;
    //    ofParameter<int> fadeMin;
    //    ofParameter<int> fadeMax;
    //    ofParameter<int> minPressTime;
    
    int bpm, touch;
    int old_bpm;
    bool gotTouch;
    bool gotBPM;
    ofColor bpmColor;
    
    bool bDeviceConnected;
    float lastConnectionTimer;
    
    vector<int> accumlatedReadings;
    vector<float> accumlatedDeg;
    int accumIndex;
    ofParameter<int> accumAmount;
    int old_accumAmount;
    
    float aliveTimer;
    int aliveCounter;
    
    bool bDebug;
    
    void setup(std::vector<string> & _serialTxBuffer){ //oscNetwork & _osc){
        
        serialSendBuffer = &_serialTxBuffer;
//        this->osc_object = &_osc;
        
        setupSerial();
        
        gui_sensor.setup();
        gui_sensor.setName("serialSensor");
        gui_sensor.setPosition(220,70);
        gui_sensor.setHeaderBackgroundColor(ofColor(255,0,0));
        gui_sensor.add(smoothing.set("smoothing",0.1,0.0001,1));
        gui_sensor.add(accumAmount.set("accumAmount",10,1,100));
        //        gui_button.add(restBright.set("restBright",100,0,255));
        //        gui_button.add(fadeDuration.set("fadeDuration",100,1,1500));
        //        gui_button.add(fadeMin.set("fadeMin",0,0,255));
        //        gui_button.add(fadeMax.set("fadeMax",255,0,255));
        gui_sensor.loadFromFile("gui_sensor.xml");
        
        //        setButtonOn(); 
        
        //        filter = ofxBiquadFilter1f(OFX_BIQUAD_TYPE_LOWPASS, 0.01, 0.7, 0.0);
        //  filter.setType(OFX_BIQUAD_TYPE_HIGHPASS);
        //         filter.setFc(smoothing);
        
        accumlatedReadings.resize(accumAmount);
        accumlatedDeg.resize(accumAmount);
        for(int i=0; i < accumlatedReadings.size(); i++){
            accumlatedReadings[i] = 0;
            accumlatedDeg[i] = 0;
        }
        accumIndex = 0;
        
        bDebug = false;
        aliveCounter = -1;
        aliveTimer = ofGetElapsedTimef();
        
        bpmColor = ofColor(255,255,255);
    }
    
    void exit(){
        for(auto oneDevice: devices){
            oneDevice.unregisterAllEvents(this);
        }
    }
    
    void setupSerial(){
        
        auto deviceDescriptors = ofx::IO::SerialDeviceUtils::listDevices();
        
        if (!deviceDescriptors.empty()) {
            
            ofLogNotice("sensorHandler::setup") << "Connected Devices: ";
            for (auto deviceDescriptor: deviceDescriptors){
                ofLogNotice("ofApp::setup") << "\t" << deviceDescriptor;
            }
            
            //check how many devices match to what we are looking for
            int matchingDeviceCount = 0;
            for (auto deviceDescriptor: deviceDescriptors){
#ifdef USE_SERIAL
                if(ofIsStringInString(deviceDescriptor.description(), "Teensy")){
#endif
                    matchingDeviceCount++;
#ifdef USE_SERIAL
                }
#endif
            }
            
            devices.resize(matchingDeviceCount);
            //            allButtons.resize(matchingDeviceCount);
            
            int cnt = 0;
            for (auto deviceDescriptor: deviceDescriptors){
#ifdef USE_SERIAL
                if(ofIsStringInString(deviceDescriptor.description(), "Teensy")){
#endif
                    // Choose a device based on correct deviceDescriptor and connect
                    bool success = devices[cnt].setup(deviceDescriptor, 115200); //9600); //115200);
                    
                    if (success){
                        devices[cnt].registerAllEvents(this);
                        ofLogNotice("ofApp::setup") << "Successfully setup " << deviceDescriptor;
                        //                        allButtons[cnt].myID = cnt;
                        bDeviceConnected = true;
                        cnt++;
                    } else {
                        ofLogNotice("ofApp::setup") << "Unable to setup " << deviceDescriptor;
                    }
#ifdef USE_SERIAL   
                }
#endif
            }
            
        }else{
            ofLogNotice("ofApp::setup") << "No devices connected.";
            bDeviceConnected = false;
        }
        
    }
    
    void update(){
        
        
        //        if(old_smoothing != smoothing){
        //            old_smoothing = smoothing;
        //            filter.setFc(smoothing);
        //        }
        //        
        //        filter.update(encoderPos);
        
      
        
    }
    
    void updateSerial(bool _ignoreBuffer){
        if(bDeviceConnected == false){
            
            if(ofGetElapsedTimef() - lastConnectionTimer > 10){
                lastConnectionTimer = ofGetElapsedTimef();
                
                for(auto oneDevice: devices){
                    oneDevice.unregisterAllEvents(this);
                }
                
                setupSerial();
            }
            
        }else{
            lastConnectionTimer = ofGetElapsedTimef();
            
            if(ofGetElapsedTimeMillis() - serialSendTimer > 10){
                
#ifdef USE_SERIAL
                
                if(serialSendBuffer->size() > 0){
                    serialSendTimer = ofGetElapsedTimeMillis();
                    
                    if(_ignoreBuffer){
                        if(bDebug) ofLog()<<"sendBytes() newest "<<serialSendBuffer->back();
                        ofx::IO::ByteBuffer textBuffer(serialSendBuffer->back());
                        devices[0].writeBytes(textBuffer);
                        devices[0].writeByte('\n');
                        serialSendBuffer->clear();
                    }else{
                        if(bDebug) ofLog()<<"sendBytes() "<<serialSendBuffer->front();
                        ofx::IO::ByteBuffer textBuffer(serialSendBuffer->front());
                        devices[0].writeBytes(textBuffer);
                        devices[0].writeByte('\n');
                        assert(!serialSendBuffer->empty());
                        serialSendBuffer->erase(serialSendBuffer->begin());
                    }
                    
                    
                }
#endif
            }
        }
    }
    void draw(int _x, int _y){
        ofPushMatrix();
        ofTranslate(_x,_y);
        drawSerialInfo();
        
        ofPopMatrix();
    }
   
    
    void drawSerialInfo(){
        //-----------print info about connected devices
        
        ofSetColor(255);
        std::stringstream ss;
        //        for(auto oneDevice: devices){
        //            ss << "Connected to: " << oneDevice.port() << std::endl;
        //        }
        int tempY = 0;
        ss <<"Serial Hands:"<<endl;
        if(bDeviceConnected == true){
            for(int i=0; i<devices.size(); i++){
                ss <<i<<" Connected to: " << devices[i].port() << std::endl;
            }
        }else{
            for(int i=0; i<devices.size(); i++){
                ss <<i<<" NOT connected: " << devices[i].port() << std::endl;
            }
            float temp_diff = 10 - ( ofGetElapsedTimef() - lastConnectionTimer );
            ss <<"try again in "<<ofToString(temp_diff,1)<< " sec "<< std::endl;
        }
        ofDrawBitmapString(ss.str(), ofVec2f(0, 0));
        
        tempY = 30;
        //----------serial alive info
        if(ofGetElapsedTimef() - aliveTimer <= 10){
            ofDrawBitmapString("local serial alive "+ofToString(aliveCounter), ofVec2f(0, tempY+=15));
        }else{
            ofDrawBitmapString("local serial NOT alive ", ofVec2f(0, tempY+=15));
        }
        
        ofSetColor(bpmColor);
        ofDrawBitmapString("last T: "+ofToString(touch)+" B: "+ofToString(bpm), ofVec2f(0, tempY+=15));
        
        
        //-----------print serial messages on screen
        int x = 0;
        int y = 80;
        
        int height = 20;
        
        auto iter = serialReceiveMessages.begin();
        
        // Cycle through each of our messages and delete those that have expired.
        while (iter != serialReceiveMessages.end()) {
            iter->fade -= 1;
            
            if (iter->fade < 0){
                iter = serialReceiveMessages.erase(iter);
            }else{
                ofSetColor(255, ofClamp(iter->fade, 0, 255));
                ofDrawBitmapString(iter->message, ofVec2f(x, y));
                
                y += height;
                
                if (!iter->exception.empty()){
                    ofSetColor(255, 0, 0, ofClamp(iter->fade, 0, 255));
                    ofDrawBitmapString(iter->exception, ofVec2f(x + height, y));
                    y += height;
                }
                
                ++iter;
            }
        }//end  while (iter != serialReceiveMessages.end()) 
        
        
    }
    void restBrightChanged (int & rBright){
        sendBytes("[" + ofToString(rBright) +"v]");
        
    }
    
    void fadeDurationChanged (int & fadeDuration){
        sendBytes("[" + ofToString(fadeDuration) +"d]");
    }
    
    void fadeMinChanged (int & fadeMin){
        sendBytes("[" + ofToString(fadeMin) +"n]");
    }
    
    void fadeMaxChanged (int & fadeMax){
        sendBytes("[" + ofToString(fadeMax) +"x]");
    }
    
    void setButtonOn(){
        //        sendBytes("[" + ofToString(restBright) +"v]");
    }
    
    void setButtonOff(){
        sendBytes("[" + ofToString(0) +"v]");
    }
    
    void triggerPulse(){
        sendBytes("[f]");
    }
    
    //void
    
    void sendBytes(string message){
#ifdef USE_SERIAL
        ofLog()<<"sendBytes() "<<message;
        //        ofx::IO::ByteBuffer textBuffer(message);
        //        devices[0].writeBytes(textBuffer);
        //        devices[0].writeByte('\n');
#endif
    }
    
    void onSerialBuffer(const ofxIO::SerialBufferEventArgs& args){
        // Buffers will show up here when the marker character is found.
        //        SerialMessage message(args.buffer().toString(), "", 500);
        //          SerialMessage message(args.buffer().toString(), "", 20);
        //        ofNotifyEvent(buttonPressed, message.message, this);
        //ofApp::update();
         if(bDebug == true)       ofLog()<<"received serial: " << args.buffer().toString();
        //        serialReceiveMessages.push_back(message);
        
        vector<string> splitStr = ofSplitString(args.buffer().toString(), ":");
        if(splitStr[0] == "bpm"){
            //            encoderPos = ofToInt(args.buffer().toString());
            //            encoderPos = ofToInt(splitStr[1]);
            //            old_raw_deg = raw_deg;
            //            raw_deg = ofMap(encoderPos,0,1024,0,360,true);
            //            float temp_diff = ofAngleDifferenceDegrees(old_raw_deg,raw_deg); //raw_deg - old_raw_deg;
            ////            ofLog()<<"encoderPos "<<encoderPos<<" raw_deg "<<raw_deg<<" temp_diff "<<temp_diff;
            //            
            //            accum_deg+= temp_diff;
            
            //            vector<string> splitStr2 = ofSplitString(splitStr[1], " ");
            if(bDebug) ofLog()<<"splitStr.size "<<splitStr.size();
            if(bDebug) ofLog()<<"splitStr[3] "<<splitStr[3]<<" splitStr[4] "<<splitStr[4];
            
            old_bpm = bpm;
            bpm = ofToInt(splitStr[4]);
            gotBPM = true;
            if(bpmColor.r > 250) bpmColor = ofColor(0);
            else bpmColor = ofColor(255);
            //                bpmColor = ofColor(ofRandom(255),ofRandom(255),ofRandom(255));
            //            }
        } else if(splitStr[0] == "a"){
            //alive
            aliveTimer = ofGetElapsedTimef();
            aliveCounter = ofToInt(splitStr[1]);
            
        } else if(splitStr[0] == "touch"){
            
            touch = ofToInt(splitStr[1]);
            gotTouch = true;
//            osc_object->addTouchMessage(osc_object->sendToIP, touch);
            
            
        }
        
        
        //        encoderPos = std::stoi(message);
        
        //        ofLog()<<ofGetTimestampString()<<" : "<<args.buffer().toString()<<" ,encoderPos "<<encoderPos;
        
        
        //        vector<string> split_message = ofSplitString(message.message,",");
        //        if(split_message.size() >= 2){
        //            
        //            int button_id = ofToInt(split_message[0]);
        //            ofLog()<<"button_id "<<button_id<<"  message.message "<< message.message<<" split_message[1] "<<split_message[1];
        //            if(split_message[1] == "buttonDown"){
        //                allButtons[button_id].old_isPressed = allButtons[button_id].isPressed;
        //                allButtons[button_id].isPressed = false;
        //                allButtons[button_id].releaseTimer = ofGetElapsedTimeMillis();
        //                allButtons[button_id].bJustGotRelased = false;
        //            }else  if(split_message[1] == "buttonUp"){
        //                allButtons[button_id].old_isPressed = allButtons[button_id].isPressed;
        //                allButtons[button_id].isPressed = true;
        //                allButtons[button_id].pressedTimer = ofGetElapsedTimeMillis();
        //                allButtons[button_id].bJustGotRelased = false;
        //            }
        //        }
        
        
    }
    
    void onSerialError(const ofxIO::SerialBufferErrorEventArgs& args){
        // Errors and their corresponding buffer (if any) will show up here.
        SerialMessage message(args.buffer().toString(),
                              args.exception().displayText(),
                              500);
        serialReceiveMessages.push_back(message);
        
        ofLog()<<"error";
        ofLog()<<args.buffer().toString();
        ofLog()<<args.exception().displayText();
        
        //on USB disconnecting:
        //macos has this error message: ofIsStringInString(args.exception().displayText(), "Device not configured")
        //RPI has this error message: Input/output error
        
//        [notice ] Exception: IO Exception (5): Input/output error, file /home/pi/openFrameworks/addons/ofxSerial/libs/serial/src/impl/unix.cc, line 488.

        if(ofIsStringInString(args.exception().displayText(), "Device not configured") ||
           ofIsStringInString(args.exception().displayText(), "Input/output error")){
            bDeviceConnected = false;
        }
    }
    
    
    
  
};


#endif /* sensorHandler_h */


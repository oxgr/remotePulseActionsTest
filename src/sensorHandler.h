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
    
    
    
    vector<ofxIO::BufferedSerialDevice> devices;
    
    std::vector<SerialMessage> serialMessages;
    
    
    ofxPanel gui_sensor;
    ofParameter<float> smoothing;
    float old_smoothing;
     float lastSin, lastCos;
    
    //    ofParameter<int> fadeDuration;
    //    ofParameter<int> fadeMin;
    //    ofParameter<int> fadeMax;
    //    ofParameter<int> minPressTime;
    
    int encoderPos;
    int old_encoderPos;
    float accum_deg;
    
//    float smoothed_encoderPos;
//    float old_smoothed_encoderPos;
    
    float raw_deg, old_raw_deg;
    float smoothed_deg, old_smoothed_deg;
    
    float old_smooth_dir;
    int dirSign,old_dirSign;
    float rotation_dir;
    //    ofxBiquadFilter1f filter;
    
    bool bDeviceConnected;
    float lastConnectionTimer;
    
    vector<int> accumlatedReadings;
    vector<float> accumlatedDeg;
    int accumIndex;
    ofParameter<int> accumAmount;
    int old_accumAmount;
    
    float aliveTimer;
    int aliveCounter;
    
    void setup(){
        
        setupSerial();
        
        gui_sensor.setup();
        gui_sensor.setName("serialSensor");
        gui_sensor.setPosition(220,10);
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
        
        aliveCounter = 0;
        aliveTimer = ofGetElapsedTimef();
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
                if(ofIsStringInString(deviceDescriptor.description(), "Teensyduino")){
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
                if(ofIsStringInString(deviceDescriptor.description(), "Teensyduino")){
#endif
                    // Choose a device based on correct deviceDescriptor and connect
                    bool success = devices[cnt].setup(deviceDescriptor, 9600); //115200);
                    
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
        }
        
        //        float dx = encoderPos - smoothed_encoderPos;
        //        smoothed_encoderPos += dx * smoothing;
        
        old_smoothed_deg = smoothed_deg;
        smoothed_deg = (smoothing * accum_deg) + ((1 - smoothing) * smoothed_deg);
             /*
        if(old_accumAmount != accumAmount){
            old_accumAmount = accumAmount;
            accumlatedReadings.resize(accumAmount);  
            accumlatedDeg.resize(accumAmount);
        }
        accumlatedReadings[accumIndex] = encoderPos;
       
        accumlatedDeg[accumIndex] = raw_deg;
        
       
        
        accumIndex ++;
        accumIndex %= accumAmount;
        
//       float deltaAngle = ofAngleDifferenceDegrees(raw_deg, smoothed_deg);
        float smooth_dir;
        if(raw_deg < smoothed_deg){
            smooth_dir = ofAngleDifferenceDegrees(raw_deg, smoothed_deg);
        }else{
            smooth_dir = ofAngleDifferenceDegrees(smoothed_deg, raw_deg);
        }
        
        smoothed_deg += smooth_dir * smoothing; // Alter the constant to change easing rate
        
        */
//        smoothed_deg = normaliseAngle(currAngle);
        
        /*
          old_smoothed_deg = smoothed_deg;
//        old_smooth_dir = smooth_dir;
        old_dirSign = dirSign;
        
        float smooth_dir;
        if(raw_deg < smoothed_deg){
            smooth_dir = ofAngleDifferenceDegrees(raw_deg, smoothed_deg);
        }else{
            smooth_dir = ofAngleDifferenceDegrees(smoothed_deg, raw_deg);
        }
        
        dirSign = ofSign(smooth_dir);
        
        ofLog()<<"smooth_dir "<<smooth_dir<<" dirSign "<<dirSign;
        if(old_dirSign != dirSign){
            if(ABS(smooth_dir) > 20){   
                dirSign = old_dirSign;
            }
        }
        //        }else{
        smoothed_deg = smoothed_deg + (smoothing * dirSign);
        */
        
        
//        }
//        smoothed_deg = meanAngle(accumlatedDeg);
//        smoothed_deg = AngAvrg(accumlatedDeg);
//        smoothed_deg = meanAngle2(accumlatedDeg);
        
//        float temp_deg = getMovingAvg(raw_deg, smoothing);
//        
//        float temp_dif = ofAngleDifferenceDegrees(raw_deg, temp_deg);
//        ofLog()<<"smooth temp_dif "<<temp_dif;
//        if(ABS(temp_dif) > 100){
//            ofLog()<<" > 180 r: "<<raw_deg<<" s: "<<temp_deg;
//            float temp_raw_deg = ofWrapDegrees(raw_deg - (temp_dif - 180) );
//            smoothed_deg = getMovingAvg(temp_raw_deg, smoothing);
//        }else{
//            smoothed_deg = temp_deg;
//        }
        
       
        
        //rotation_dir > 0 == cw 
        rotation_dir = ofAngleDifferenceDegrees(old_smoothed_deg, smoothed_deg);
//        rotation_dir = ofAngleDifferenceDegrees(old_raw_deg, raw_deg);

    }
    
    void draw(int _x, int _y){
        ofPushMatrix();
        ofTranslate(_x,_y);
        drawSerialInfo();
        
        ofPopMatrix();
    }
    void draw2(int _x, int _y){
        
        //----raw serial reading as triangle + text
        ofPushMatrix();
        ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
        ofRotateZDeg(raw_deg);
        ofSetColor(255);
        ofDrawTriangle(0, -40, -20, 0, 20, 0);
        ofDrawLine(0,0,0,-100);
        ofPopMatrix();
        
        ofPushMatrix();
        ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
        ofSetColor(255);
        ofDrawBitmapString("raw: "+ofToString(encoderPos) + " , deg " + ofToString(raw_deg,0),-20,40);
        ofPopMatrix();
        
        //----smoothed serial reading
        ofPushMatrix();
        ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
        ofRotateZDeg(smoothed_deg);
        ofSetColor(200);
        ofDrawTriangle(0, -40, -20, 0, 20, 0);
        ofDrawLine(0,0,0,-100);
        ofPopMatrix();
        
        ofPushMatrix();
        ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
        ofSetColor(255);
//        ofDrawBitmapString("smooth: "+ofToString(smoothed_encoderPos,0) + " , deg " + ofToString(smoothed_deg,0),-20,60);
        ofDrawBitmapString("smooth degree: " + ofToString(smoothed_deg,0),-20,60);

        ofPopMatrix();
        
        //-----accum deg
        ofPushMatrix();
        ofPushStyle();
        ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
        ofRotateZDeg(accum_deg);
        ofNoFill();
        ofSetColor(0,0,255);
        ofDrawTriangle(0, -40, -20, 0, 20, 0);
        ofDrawLine(0,0,0,-100);
        ofPopStyle();
        ofPopMatrix();
        
        ofPushMatrix();
        ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
        ofSetColor(255);
        ofDrawBitmapString("accum_deg: " + ofToString(accum_deg,0),-20,80);
        ofPopMatrix();
        
        //----center point
        ofPushMatrix();
        ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
        ofSetColor(255,0,0);
        ofDrawCircle(0,0,5);
        ofPopMatrix();
        
        //------direction arrow
        ofPushMatrix();
        ofTranslate(ofGetWidth()/2, ofGetHeight()/2+100);
        ofSetColor(255,255,0);
        ofVec3f arrowTailPoint (0,0,0);
        ofVec3f arrowHeadPoint (-20*rotation_dir, 0,0);
        ofSetLineWidth(3);
        ofDrawArrow(arrowTailPoint, arrowHeadPoint, 10.0);
        ofSetLineWidth(1);
        ofPopMatrix();
        
        drawSerialInfo();
    }
    
    void drawSerialInfo(){
        //-----------print info about connected devices
       
        
        std::stringstream ss;
        //        for(auto oneDevice: devices){
        //            ss << "Connected to: " << oneDevice.port() << std::endl;
        //        }
        ss <<"Serial buttons:"<<endl;
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
        
        
        //----------serial alive info
        if(ofGetElapsedTimef() - aliveTimer <= 10){
            ofDrawBitmapString("sensor alive "+ofToString(aliveCounter), ofVec2f(0, 50));
        }else{
            ofDrawBitmapString("sensor NOT alive ", ofVec2f(0, 50));
        }
        
        ofDrawBitmapString("rotation_dir "+ofToString(rotation_dir), ofVec2f(0, 80));
        
        
        //-----------print serial messages on screen
        int x = 0;
        int y = 80;
        
        int height = 20;
        
        auto iter = serialMessages.begin();
        
        // Cycle through each of our messages and delete those that have expired.
        while (iter != serialMessages.end()) {
            iter->fade -= 1;
            
            if (iter->fade < 0){
                iter = serialMessages.erase(iter);
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
        }//end  while (iter != serialMessages.end()) 
        
      
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
        //        ofLog()<<"received: " << message.message;
        //        serialMessages.push_back(message);
        
        vector<string> splitStr = ofSplitString(args.buffer().toString(), ":");
        if(splitStr[0] == "p"){
            //            encoderPos = ofToInt(args.buffer().toString());
            encoderPos = ofToInt(splitStr[1]);
            old_raw_deg = raw_deg;
            raw_deg = ofMap(encoderPos,0,1024,0,360,true);
            float temp_diff = ofAngleDifferenceDegrees(old_raw_deg,raw_deg); //raw_deg - old_raw_deg;
//            ofLog()<<"encoderPos "<<encoderPos<<" raw_deg "<<raw_deg<<" temp_diff "<<temp_diff;
            
            accum_deg+= temp_diff;

        } else if(splitStr[0] == "a"){
            aliveTimer = ofGetElapsedTimef();
            aliveCounter = ofToInt(splitStr[1]);
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
        serialMessages.push_back(message);
        
        ofLog()<<"error";
        ofLog()<<args.buffer().toString();
        ofLog()<<args.exception().displayText();
        
        if(ofIsStringInString(args.exception().displayText(), "Device not configured")){
            bDeviceConnected = false;
        }
    }
    
    
    
    //https://rosettacode.org/wiki/Averages/Mean_angle#C.2B.2B
    float meanAngle(vector<float> & c) {
        auto it = std::begin(c);
        auto end = std::end(c);
        
        float x = 0.0;
        float y = 0.0;
        float len = 0.0;
        while (it != end) {
            x += cos(*it * M_PI / 180);
            y += sin(*it * M_PI / 180);
            len++;
            
            it = std::next(it);
        }
        
        return atan2(y, x) * 180 / M_PI;
    }
    
    // angles have to be in the range [0, 360) and within 180° of each other.
    // n >= 1
    // returns the circular average of the angles int the range [0, 360).
    float meanAngle2(vector<float>& angles)
    {
        float average = angles[0];
        for (int i = 1; i<angles.size(); i++)
        {
            // average: (0, 360)
            float diff = angles[i]-average;
            // diff: (-540, 540)
            
            if (diff < -180)
                diff += 360;
            else if (diff >= 180)
                diff -= 360;
            // diff: (-180, 180)
            
            average += diff/(i+1);
            // average: (-180, 540)
            
            if (average < 0)
                average += 360;
            else if (average >= 360)
                average -= 360;
            // average: (0, 360)
        }
        return average;
    }
    
    float AngAvrg(const vector<float>& Ang)
    {
        vector<float>::const_iterator iter= Ang.begin();
        
        float fD   = *iter;
        float fSigD= *iter;
        
        while (++iter != Ang.end())
        {
            float fDelta= *iter - fD;
            
            if (fDelta < -180.) fD+= fDelta + 360.;
            else if (fDelta >  180.) fD+= fDelta - 360.;
            else                     fD+= fDelta       ;
            
            fSigD+= fD;
        }
        
        float fAvrg = fSigD / Ang.size();
        
        if (fAvrg >= 360.) return fAvrg -360.;
        if (fAvrg <  0.  ) return fAvrg +360.;
        return fAvrg      ;
    }
    
    float getMovingAvg(float _deg, float _smoothFactor){
        lastSin = _smoothFactor * lastSin + (1-_smoothFactor) * sin(ofDegToRad(_deg));
        lastCos = _smoothFactor * lastCos + (1-_smoothFactor) * cos(ofDegToRad(_deg));
        
        
        return ofRadToDeg(atan2(lastSin, lastCos));
    }
};


#endif /* sensorHandler_h */


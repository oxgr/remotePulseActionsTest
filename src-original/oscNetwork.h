

#ifndef oscNetwork_h
#define oscNetwork_h



#ifdef USE_OSC

#include "ofxOsc.h"

#include "ofParameterGroup.h"
#include "ofParameter.h"
#include "ofxDropdown.h"

//set computer to IP 188.0.1.250 with mask 255.255.255.0
//#define HOST "188.0.1.255" // "192.168.100.101"
//#define HOST "10.0.1.255" 
//#define PORT 8000
//
//#define PORT_RECEIVER 9000
#define NUM_MSG_STRINGS 45


class oscNetwork{
public:
    
    ofxOscSender sender;
    ofxOscReceiver receiver;
    
    
    bool bDebug;
    bool bPortraitMode;
    
    vector<ofxOscMessage> messageBuffer;
    int sendTimer;
    
    int current_msg_string;
    string msg_strings[NUM_MSG_STRINGS];
    float timers[NUM_MSG_STRINGS];
    
    
    //    ofParameterGroup parameters_oscManualControls;
    ofxPanel gui_osc;
    ofParameter <bool> bEnableOSC;
    bool old_bEnableOSC;
    ofParameter<int> oscSendPort;
    ofParameter<int> oscReceivePort;
    ofParameter<string> sendToIP;
    
    ofParameter<int> oscSendInterval;
    
    ofParameter <bool> bPeriodicResend;
    float resendTimer;
    string lastTouchIP = "";
    int lastTouchValue;
    
    int rxBPM;
    ofParameter<string> rxBPM_str;
    ofParameter<string> txBPM_str;
    
    int rxTouch;
    ofParameter<string> rxTouch_str;
    ofParameter<string> txTouch_str;
    //    bool initDone;
    //    float initTimer;
    
    
    float lastOscTimer;
    
    string myIP;
    string broadCastIP;
    //    int inputType;
    
    bool gotBPM = false;
    bool gotTouch = false;
    
    float other_runtime_fade = 255;
    string other_runtimeStr = "Anfang";
    //    string old_other_runtimeStr = "Anfang";
    string other_serialStr = "Anfang";
    
    float lastMsgTimer = 0;
    
//    int showState = -1;
//    ofParameter<int> showState = -1;
//    ofParameter<string> showState_str;
    
    unique_ptr<ofxIntDropdown> intDropdown;
    ofParameter<int> showState;
    
    string show_names[5] = {"neutral","start","stop","relaxed","emergency"};
    
    void setup(string _myIP, string _broadCastIP){
        
        myIP =_myIP;
        broadCastIP = _broadCastIP;
        
        bDebug = false;
        
        current_msg_string = 0;
        
        //        oscSendPort = 19924;
        //        oscReceivePort = 19923;
        
        
        gui_osc.setup();
        gui_osc.setName("osc");
        gui_osc.setHeaderBackgroundColor(ofColor(255,0,0));
        
        gui_osc.add(bEnableOSC.set("enableOSC", false));
        //        parameters_oscManualControls.add(bRunMode.set("run", false));
        //        
        gui_osc.add(oscSendPort.set("oscOutPort", 9924,9923,9924));
        gui_osc.add(oscReceivePort.set("oscInPort", 9923,9923,9924));
        gui_osc.add(sendToIP.set("sendToIP", "10.100.0.50"));
        
        gui_osc.add(oscSendInterval.set("oscSendInterval", 10,0,100));
        
        gui_osc.add(rxTouch_str.set("other touch received", ""));
        gui_osc.add(rxBPM_str.set("other bpm received", ""));
        
        gui_osc.add(txTouch_str.set("me touch sent", ""));
        gui_osc.add(txBPM_str.set("me bpm sent", ""));
        
        gui_osc.add(bPeriodicResend.set("periodicResend", false));
  
        showState.setName("app state");
        intDropdown = make_unique<ofxIntDropdown>(showState);
        intDropdown->enableCollapseOnSelection();
        intDropdown->disableMultipleSelection();
        intDropdown->add(0, "neutral");
        intDropdown->add(1, "start");
        intDropdown->add(2, "stop");
        intDropdown->add(3, "relaxed");
        intDropdown->add(4, "emergency");        
        gui_osc.add(intDropdown.get());
        
//        strOptions_A.setName("app state");
//        functionsDropdown_A = make_unique<ofxDropdown>(strOptions_A);
//        functionsDropdown_A->enableCollapseOnSelection();
//        functionsDropdown_A->disableMultipleSelection();
//        functionsDropdown_A->add("neutral")
//        functionsDropdown_A->add("start");
//        functionsDropdown_A->add("stop");
//        functionsDropdown_A->add("relaxed");
//        functionsDropdown_A->add("emergency");
//        
//        gui_osc.add(functionsDropdown_A.get());
        
//        gui_osc.add(showState.set("state index", 0,0,4));
//        gui_osc.add(showState_str.set("state name", ""));
        gui_osc.loadFromFile("GUIs/gui_osc.xml");
        
        showState = SHOW_NEUTRAL;
        //        inputType = -1;
    }
    
    void init(){
        bEnableOSC = true; //false;      
        
        ofLog()<<"set computer to manual IP to 188.0.1.250 with mask 255.255.255.0";
        //        ofxOscSenderSettings oscSenderSettings;
        //        oscSenderSettings.broadcast = true;
        //        oscSenderSettings.host = oscIP;
        //        oscSenderSettings.port = oscSendPort;
        //        sender.setup(oscSenderSettings);
        sender.setup(sendToIP, oscSendPort);
        
        receiver.setup(oscReceivePort);
        
        //        initDone = false;
        //        initTimer = ofGetElapsedTimef();
    }
    
    void exit(){
        //send resetPause at exit
        //        sendBundle("resetpause",-1,-1);
    }
    
    void addBPMMessage(string _ip, int _value){
        ofxOscMessage myMessage;
        
        myMessage.setAddress("/BPM");
        myMessage.addStringArg(_ip); 
        myMessage.addIntArg(_value);
        
        if(bDebug) ofLog()<<"addBPMMessage() send osc msg: "<<_ip<<" value "<<_value;
        
        messageBuffer.push_back(myMessage);
        
        txBPM_str = ofToString(_value);
    }
    
    void addTouchMessage(string _ip, int _value){
        
        lastTouchIP = _ip;
        lastTouchValue = _value;
        
        ofxOscMessage myMessage;
        
        myMessage.setAddress("/Touch");
        myMessage.addStringArg(_ip); 
        myMessage.addIntArg(_value);
        if(bDebug) ofLog()<<"addTouchMessage() send osc msg: "<<_ip<<" value "<<_value;
        
        messageBuffer.push_back(myMessage);
        
        txTouch_str = ofToString(_value);
        
        resendTimer = ofGetElapsedTimef();
        
    }
    
    
    void addAliveMessage(string _ip, string _runtimeStr, int _serialAliveStr){
        
        if(bDebug) ofLog()<<"addAliveMessage to _ip "<<_ip<<" _runtimeStr "<<_runtimeStr<<" _serialAliveStr "<<_serialAliveStr;
        ofxOscMessage myMessage;
        
        myMessage.setAddress("/appAlive");
        myMessage.addStringArg(_ip); 
        myMessage.addStringArg(_runtimeStr);
        myMessage.addIntArg(_serialAliveStr); 
        
        messageBuffer.push_back(myMessage);
    }
    void printStringAsInts(string str){
        for(size_t i = 0; i < str.size(); i++){
            cout << (int)str[i] << " , ";
        }
        cout << endl;
    }
    void update(string _myIP){
        
        checkGui();
        
        // hide old messages
        for(int i = 0; i < NUM_MSG_STRINGS; i++){
            if(timers[i] < ofGetElapsedTimef()){
                msg_strings[i] = "";
            }
        }
        
        
        if(bEnableOSC){
            
            //TODO: if messageBuffer.size() == 0   sendTimer = ofGetElapsedTimeMillis();
            if(ofGetElapsedTimeMillis() - sendTimer > oscSendInterval){
                sendTimer = ofGetElapsedTimeMillis();
                
                
                if(messageBuffer.size() > 0){
                    
                    if(bDebug) ofLog()<<"messageBuffer.front() "<<messageBuffer.front();
                    sender.sendMessage(messageBuffer.front(), true);
                    
                    //                    assert(!messageBuffer.empty());
                    //                    messageBuffer.front() = std::move(messageBuffer .back());
                    //                    messageBuffer.pop_back();
                    
                    assert(!messageBuffer.empty());
                    messageBuffer.erase(messageBuffer.begin());
                    
                    //TODO: remove first. maybe use deque instead of vector
                }
                
                
                //                if(last_vuValue != cur_vuValue || last_degreeValue != cur_degreeValue || last_dirValue != cur_dirValue){
                //                    lastOscTimer = ofGetElapsedTimef();
                //                    last_vuValue =  cur_vuValue;
                //                    last_degreeValue = cur_degreeValue;
                //                    last_dirValue = cur_dirValue;
                //                    
                //                    ofxOscMessage myMessage;
                //                    myMessage.setAddress("/Dial+VU");
                //                    myMessage.addStringArg(_myIP); 
                //                    myMessage.addFloatArg(cur_vuValue);
                //                    myMessage.addFloatArg(cur_degreeValue);
                //                    myMessage.addFloatArg(cur_dirValue);
                //                    myMessage.addIntArg(autoPlayerCount);
                //                    sender.sendMessage(myMessage, true);
                //                    
                //                }
            }
            
            // let's make sure we send the touch state, since it is responsible for stopping action
            if(lastTouchIP != "" && ofGetElapsedTimef() - resendTimer > 10){
                resendTimer = ofGetElapsedTimef();
                addTouchMessage(lastTouchIP,lastTouchValue);
            }
            
            if(ofGetElapsedTimef() - lastMsgTimer > 30){
                lastMsgTimer = ofGetElapsedTimef();
                gotTouch = true;
                rxTouch = 0;
                ofLog()<<" lastMsgTimer > 30. have not received anything for a long time via OSC! force unTouch";
            }
        }//end if(bEnableOSC)
        
        
        
        //MARK:--------check for waiting messages
        while(receiver.hasWaitingMessages()){
            // get the next message
            ofxOscMessage m;
            receiver.getNextMessage(m);
            
            // check for mouse moved message
            if(bDebug) ofLog()<<"osc receivere m.getAddress() "<<m.getAddress();
            
            if(m.getAddress() == "/status"){
                // both the arguments are int32's
                //                int temp_id = m.getArgAsInt32(0);
                //                int temp_status = m.getArgAsInt32(1);
                //                feedbackStatus[temp_id] = 1;
            }else if(m.getAddress() == "/BPM"){
                string temp_forWhom = m.getArgAsString(0);
                if(bDebug) ofLog()<<"osc for "<<temp_forWhom<<" bpm "<<m.getArgAsInt(1);
                
                if(temp_forWhom == myIP || temp_forWhom == broadCastIP){
                    //                if(temp_forWhom.compare(myIP) == true || temp_forWhom.compare(broadCastIP) == true){
                    rxBPM = m.getArgAsInt(1);
                    rxBPM_str = ofToString(rxBPM);
                    gotBPM = true;
                    lastMsgTimer = ofGetElapsedTimef();
                }       
                
            }else if(m.getAddress() == "/Touch"){
                string temp_forWhom = m.getArgAsString(0);
                if(bDebug){
                    ofLog()<<"myIP "<<myIP<<" osc for "<<temp_forWhom<<" bpm "<<m.getArgAsInt(1);
                }
                if(temp_forWhom == myIP || temp_forWhom == broadCastIP){
                    //                if(temp_forWhom.compare(myIP) == true || temp_forWhom.compare(broadCastIP) == true){
                    rxTouch = m.getArgAsInt(1);
                    rxTouch_str = ofToString(rxTouch);
                    gotTouch= true;
                    lastMsgTimer = ofGetElapsedTimef();
                }    
                
                // startShow (all lights and sound to normal)
                // stopShow (all lights and sound off)
                // relaxed (less light, less sound)
                // emergency (all lights on, no sound)
                
            }else if(m.getAddress() == "/startShow"){
                
                ofLog()<<"m.getAddress() /startShow ";
                showState = SHOW_START;
                if(bDebug) ofLog()<<"myIP "<<myIP;
                    
                rxTouch_str = "ofToString(m.getAddress())";
                lastMsgTimer = ofGetElapsedTimef();
                
            }else if(m.getAddress() == "/stopShow"){
                
                ofLog()<<"m.getAddress() /startShow ";
                showState = SHOW_STOP;
                if(bDebug) ofLog()<<"myIP "<<myIP;
                    
                rxTouch_str = "ofToString(m.getAddress())";
                lastMsgTimer = ofGetElapsedTimef();
                
            }else if(m.getAddress() == "/relaxedShow"){
                
                ofLog()<<"m.getAddress() /relaxedShow ";
                showState = SHOW_RELAXED;
                if(bDebug) ofLog()<<"myIP "<<myIP;
                    
                rxTouch_str = "ofToString(m.getAddress())";
                lastMsgTimer = ofGetElapsedTimef();
   
            }else if(m.getAddress() == "/emergencyShow"){
                
                ofLog()<<"m.getAddress() /emergencyShow ";
                showState = SHOW_EMERGENCY;
                if(bDebug) ofLog()<<"myIP "<<myIP;
                    
                rxTouch_str = "ofToString(m.getAddress())";
                lastMsgTimer = ofGetElapsedTimef();
                
                
            }else if(m.getAddress() == "/appAlive"){
                //MARK: /appAlive
                string temp_forWhom = m.getArgAsString(0);
                if(bDebug){
                    ofLog()<<"temp_forWhom "<<temp_forWhom<<" myIP "<<myIP;
                    ofLog()<<"temp_forWhom:" <<endl;
                    printStringAsInts(temp_forWhom);
                    
                    ofLog()<<"myIP:" <<endl;
                    printStringAsInts(myIP);
                    
                    //                    ofLog()<<"|"<<temp_forWhom<<"|";
                    //                    ofLog()<<"|"<<myIP<<"|";
                    //                    ofLog()<<_myIP<<"|";
                    ofLog()<<"runtimeStr "<<m.getArgAsString(1)<<" serialStr "<<m.getArgAsInt(2);
                    ofLog()<<"compare "<<(temp_forWhom == myIP ? "same" : "NOT");
                    ofLog()<<"ofIsStringInString "<<(ofIsStringInString(myIP,temp_forWhom) ? "same" : "NOT");
                    
                }
                //strcmp(param, "test") == 0
                //temp_forWhom == myIP
                
                //                if(temp_forWhom.compare(myIP) == true || temp_forWhom.compare(broadCastIP) == true){
                //                if(ofIsStringInString(myIP, temp_forWhom)){
                if(temp_forWhom == myIP || temp_forWhom == broadCastIP){
                    other_runtimeStr = m.getArgAsString(1);
                    other_serialStr = ofToString(m.getArgAsInt(2));
                    other_runtime_fade = 255;
                    lastMsgTimer = ofGetElapsedTimef();
                    if(bDebug)  ofLog()<<"in appAlive set other_runtimeStr to "<<other_runtimeStr;
                }         
                
            }else{
                // unrecognized message: display on the bottom of the screen
                string msg_string;
                msg_string = m.getAddress();
                msg_string += ": ";
                for(int i = 0; i < m.getNumArgs(); i++){
                    // get the argument type
                    msg_string += m.getArgTypeName(i);
                    msg_string += ":";
                    // display the argument - make sure we get the right type
                    if(m.getArgType(i) == OFXOSC_TYPE_INT32){
                        msg_string += ofToString(m.getArgAsInt32(i));
                    }
                    else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
                        msg_string += ofToString(m.getArgAsFloat(i));
                    }
                    else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
                        msg_string += m.getArgAsString(i);
                    }
                    else{
                        msg_string += "unknown";
                    }
                }
                // add to the list of strings to display
                msg_strings[current_msg_string] = msg_string;
                timers[current_msg_string] = ofGetElapsedTimef() + 5.0f;
                current_msg_string = (current_msg_string + 1) % NUM_MSG_STRINGS;
                ofLog()<<"OSC msg_string "<<msg_string;
                // clear the next line
                msg_strings[current_msg_string] = "";
            }
            
        }//end while(receiver.hasWaitingMessages()){
        //        if(bDebug)  ofLog()<<"oscNetwork update other_runtimeStr "<<other_runtimeStr;
    }
    
    void addSenderMsg(string _handle, int _valueA){
        ofxOscMessage myMessage;
        myMessage.setAddress(_handle);
        myMessage.addStringArg(myIP); 
        myMessage.addIntArg(_valueA);
        //        myMessage.addFloatArg(cur_degreeValue);
        //        myMessage.addFloatArg(cur_dirValue);
        //        myMessage.addIntArg(autoPlayerCount);
        sender.sendMessage(myMessage, true);
    }
    
    //--------------------------------------------------------------
    void draw(int _x, int _y){
        
        
        ofPushMatrix();
        ofTranslate(_x,_y);
        {
            
            if(bEnableOSC){
                
                //                ofPushMatrix();
                
                //                ofTranslate(ofGetWidth() - 400, 200);
                
                
                //                ofSetColor(219,235,243);
                int temp_y = 0;
                string buf;
                buf = "listening for osc messages on port " + ofToString(oscReceivePort);
                ofDrawBitmapString(buf, 0, temp_y +=15);
                // display instructions
                buf = "sending osc messages to " + sendToIP.get() + " " + ofToString(oscSendPort);
                ofDrawBitmapString(buf, 0,  temp_y +=15);
                float temp_diff = ofGetElapsedTimef() - lastOscTimer;
                buf = "since last send " + ofToString(temp_diff,2);
                ofDrawBitmapString(buf, 0, temp_y +=15); 
                
                for(int i = 0; i < NUM_MSG_STRINGS; i++){
                    ofDrawBitmapString(msg_strings[i], 0, temp_y +=15);
                }
                
                
                //                ofPopMatrix();
                //            }
                
            }
        }
        
        ofPopMatrix();
        
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
        ofDrawBitmapString("other serial alive:"+other_serialStr,0, 20);
        ofSetColor(0);
        ofDrawBitmapString("other serial alive:"+other_serialStr,0, 20);
        
        ofPopMatrix();
        
    }
    
    void checkGui(){
        
        if(bEnableOSC != old_bEnableOSC){
            old_bEnableOSC = bEnableOSC;
            //            if(bEnableOSC == false) sendMode(0);
        }
        
//        showState_str = show_names[showState];
    }
    
    
    
private:
    
};

#endif

#endif /* oscNetwork_h */
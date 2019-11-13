#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(30);
    
    ofSetFrameRate(60);
    ofBackground(20);
    
    //IP
    MX_IP = "10.0.1.191";
    US_IP = "10.0.1.192";
    
    broadCastIP = "10.0.1.255"; //not really broadcast. just an indicator if i should be listen to this message
    
    vector<string> ip_list = getLocalIPs();
    myIP = ip_list[0];
    
    for(int i=0; i<ip_list.size(); i++){
        ofLog()<<ip_list[i];
    }
    
    
    gui_main.setup();
    gui_main.setName("remotePulse");
    gui_main.setPosition(10,10);
    gui_main.setHeaderBackgroundColor(ofColor(255,0,0));
    //    gui_main.add(versionString.set("ver", ""));
    gui_main.add(bShowGui.set("showGui",true));
    gui_main.add(bEnableDMX.set("enableDMX",false));
    gui_main.loadFromFile("GUIs/gui_main.xml");
    
    //---dmx
    dmxValues.resize(MAX_DMX_CHANNELS+1);
    fill(dmxValues.begin(), dmxValues.end(), 0);
    
    //MARK: -------DMX setup-----------
#ifdef USE_DMX
    dmx.connect("serial-EN", MAX_DMX_CHANNELS ); // use the name
#endif
    dmxDeviceString = dmx.getDeviceString();
    
    
    //---serial
    dial_object.setup();
    dial_object.gui_dial.setPosition(10,200);
    
    //--osc
    osc_object.setup(myIP, broadCastIP);
    gui_osc.setup();
    gui_osc.setName("OSC");
    gui_osc.setPosition(430, 10);
    gui_osc.setHeaderBackgroundColor(ofColor(255,0,0));
    gui_osc.add(osc_object.parameters_oscManualControls);
    
    gui_osc.loadFromFile("GUIs/gui_osc.xml");
    osc_object.init();
    
    bShowGui = true;
    meBPM = 50;
    
    bpm_delayTimer.setPeriod(1); // to get it started
    
    test_delayTimer.setPeriod(10);
    
    beat1.load("sounds/oneBeat.wav");
    beat2.load("sounds/oneBeat2.wav");
    beat1.setVolume(1.0f);
    beat2.setVolume(1.0f);
    beat1.setMultiPlay(false);
    beat2.setMultiPlay(false);
    //    bpm_lerpTimer.setDuration(1);
}

void ofApp::exit(){
    osc_object.exit();
    
    dial_object.exit();
    dial_object.gui_dial.saveToFile("GUIs/gui_dial.xml");
}
//--------------------------------------------------------------
void ofApp::update(){
    ofSetWindowTitle(myIP+" | BPM-sender");
    
    if(ofGetElapsedTimef() - logTimer > 2){
        logTimer = ofGetElapsedTimef() ;
        
        if(ofIsStringInString(myIP, MX_IP.substr (0,6)) == true){
            
            ofBuffer buf;
            buf.set(ofGetTimestampString());
            ofBufferToFile("runLogs/heartbeat.txt", buf);
            
            osc_object.addAliveMessage(myIP,runtime_str);
            
        }else{
            ofLog()<<"wrong IP range. is"<<myIP<<" should be "<<MX_IP<<" or "<<US_IP;
        }
        
    }
    
    
    if(osc_object.gotBPM == true){
        otherBPM = osc_object.rxBPM;
        osc_object.gotBPM = false;
        
        //        float bpmMillis = otherBPM / 60.0;
        
        //        ofLog()<<"bpmMillis "<<bpmMillis;
        //        bpm_delayTimer.setPeriod(bpmMillis);
        //        bpm_lerpTimer.setDuration(bpmMillis);
    }
    
    
    //    meBPM += 1;
    //    if(meBPM > 200) meBPM = 50;
    
    if(test_delayTimer.tick()){
        meBPM = ofRandom(50,200);
    }
    
    if(old_meBPM != meBPM){
        old_meBPM = meBPM;
        osc_object.addBPMMessage(osc_object.sendToIP, meBPM);
    }
    
    
    
   
        if(old_otherBPM != otherBPM){
            old_otherBPM = otherBPM;
            bpmInSeconds = 60.0 / float(otherBPM);
            //            ofLog()<<"bpmInSeconds "<<bpmInSeconds;
            //            bpm_delayTimer.setPeriod(bpmInSeconds);
            //            bpm_lerpTimer.setDuration(bpmInSeconds);
            
        }

    
    
    if(bpm_delayTimer.tick() == true){
        
        bStart2ndBeat = true;
        if(beat1.isPlaying() == false) beat1.play();
        ofLog()<<ofGetElapsedTimef()<<" beat1.play()";
        
        if(old_bpmInSeconds != bpmInSeconds){
            old_bpmInSeconds = bpmInSeconds;
            ofLog()<<"BPM "<<otherBPM<<" in bpmInSeconds "<<bpmInSeconds;
            bpm_delayTimer.setPeriod(bpmInSeconds);
            bpm_lerpTimer.setDuration(bpmInSeconds);
        }
        
        bpm_lerpTimer.setToValue(0);
        bpm_lerpTimer.lerpToValue(1);
    }
    
    
//    ofLog()<<"bpm_lerpTimer.getProgress() "<<bpm_lerpTimer.getProgress()<<" bStart2ndBeat "<<bStart2ndBeat;
    if(bpm_lerpTimer.getProgress() > 0.15 && bStart2ndBeat == true){
        if(beat2.isPlaying() == false){
            beat2.play();
            ofLog()<<ofGetElapsedTimef()<<" beat2.play()";
            bStart2ndBeat = false;
        }
    }
    
    //    bpm_lerpTimer.lerpToValue(1);
    
    if(bpm_lerpTimer.getProgress() < 0.6){
        dmxValues[0] = ofMap(bpm_lerpTimer.getProgress(), 0, 0.6, 0, 255,true);
        dmxValues[1] = ofMap(bpm_lerpTimer.getProgress(), 0.15, 0.6, 0, 255,true);
    }else{
        dmxValues[0] = 0;
        dmxValues[1] = 0;
    }
    //    ofLog()<<"dmxValues[0] "<<int(dmxValues[0]);
    
    dial_object.update();
    if(osc_object.bEnableOSC == true)  osc_object.update(myIP);
    
    renderDMX();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    drawRuntime(ofGetWidth()/2, 10);
    
    ofPushMatrix();
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
    
    int tempY = 0;
    ofSetColor(255);
    ofDrawBitmapString("meBPM "+ofToString(meBPM),0,tempY+=15);
    ofDrawBitmapString("otherBPM "+ofToString(otherBPM),0,tempY+=15);
    osc_object.drawAliveMsg(0,tempY+=15);
    
    ofSetColor(255);
    stringstream msg;
    msg << "beat1.isPlaying: " << beat1.isPlaying() << endl;
    msg << "lerpTimer:"
    << " from " << bpm_lerpTimer.getStartValue()
    << " to " << bpm_lerpTimer.getTargetValue()
    << " : " << int(bpm_lerpTimer.getValue())
    << " (" << int(100*bpm_lerpTimer.getProgress()) << "%)" << endl;
    ofDrawBitmapString(msg.str(),0,tempY+=15);
    
    ofPopMatrix();
    
    dial_object.draw(250,10);
    
    
#ifdef USE_DMX
    drawDmxBar(5,ofGetHeight() - 50, 1 ,2); 
#endif
    
    if(bShowGui == true){
        gui_main.draw();
        gui_osc.draw();
        dial_object.gui_dial.draw();
    }
}

void ofApp::renderDMX() {
    
#ifdef USE_DMX
    for (int i = 0; i < dmxValues.size()-1; i++) {
        dmx.setLevel(i+1, dmxValues[i]);
    }
    
    if(bEnableDMX == true)    dmx.update();
#endif
    
}
void ofApp::drawRuntime(int _x, int _y){
    
    float myTime = ofGetElapsedTimef();
    int myMin = myTime/ 60;
    int myHours = myTime / 3600;
    myMin = myMin % 60;
    int mySec = fmod(myTime,60);
    
    int temp_y = 10;
    runtime_str = ofToString(myHours)+":"+ofToString(myMin)+":"+ofToString(mySec);
    ofDrawBitmapString(runtime_str, _x, _y);
    
}

void ofApp::saveGui(){
    //    gui_main.saveToFile("GUIs/gui_main.xml");
    
    
    dial_object.gui_dial.saveToFile("GUIs/gui_dial.xml");
    gui_osc.saveToFile("GUIs/gui_osc.xml");
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key == 'g'){
        
        bShowGui = !bShowGui;
        ofLog()<<"bShowGui "<<bShowGui;
        if(bShowGui == false){
            saveGui();
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    
}


vector<string> ofApp::getLocalIPs(){
    vector<string> result;
    
#ifdef TARGET_WIN32
    
    string commandResult = ofSystem("ipconfig");
    //ofLogVerbose() << commandResult;
    
    for (int pos = 0; pos >= 0; ){
        pos = commandResult.find("IPv4", pos);
        
        if (pos >= 0){
            pos = commandResult.find(":", pos) + 2;
            int pos2 = commandResult.find("\n", pos);
            
            string ip = commandResult.substr(pos, pos2 - pos);
            
            pos = pos2;
            
            if (ip.substr(0, 3) != "127"){ // let's skip loopback addresses
                result.push_back(ip);
                //ofLogVerbose() << ip;
            }
        }
    }
    
#else
    
    string commandResult = ofSystem("ifconfig");
    
    for(int pos = 0; pos >= 0; ){
        pos = commandResult.find("inet ", pos);
        
        if(pos >= 0){
            int pos2 = commandResult.find("netmask", pos);
            
            string ip = commandResult.substr(pos+5, pos2-pos-6);
            
            pos = pos2;
            
            if(ip.substr(0, 3) != "127"){ // let's skip loopback addresses
                result.push_back(ip);
                //ofLogVerbose() << ip;
            }
        }
    }
    
#endif
    
    return result;
}

void ofApp::drawDmxBar(int _x, int _y, int _groupSize, int _devices){
    
    ofPushMatrix();
    
    ofPushStyle();
    ofTranslate(_x, _y); //ofGetHeight()-30);
    
    //        ofPushMatrix();
    
    int yy = - 50;
    if(bEnableDMX == false)  ofDrawBitmapStringHighlight("!!DISABLED!!", 0, yy+=15);
    ofDrawBitmapStringHighlight("uses DMX USB", 0, yy+=15);
    ofDrawBitmapStringHighlight("dmx device = "+dmxDeviceString,0,yy+=15);
    
    
    int _w = 11;
    int groupCnt = 0;
    int temp_group = 0;
    int temp_x = 0;
    int temp_y = 0;
    
    //        ofLog()<<"_groupSize*_devices "<<(_groupSize*_devices);
    for(int i=1; i<=(_groupSize*_devices); i++){
        
#ifdef USE_DMX
        int temp_level = dmx.getLevel(i);
#else
        int temp_level = 255;
#endif
        ofFill();
        ofSetColor(temp_level);
        ofDrawRectangle(temp_x, temp_y, _w, _w);
        
        ofNoFill();
        ofSetColor(200);
        ofDrawRectangle(temp_x, temp_y, _w, _w);
        
        //            if(temp_level != 0){
        //                ofSetColor(200);
        //                if(i%2 == 0) ofDrawBitmapString(ofToString(i), temp_x+1, temp_y-15);
        //                else ofDrawBitmapString(ofToString(i), temp_x+1, temp_y-2);
        //            }
        
        //tint every 2nd group to better see grouping relation
        if(temp_group % 2 == 0){
            ofFill();
            ofSetColor(0,0,255,70);
            ofDrawRectangle(temp_x, temp_y, _w, _w);
        }
        
        temp_x = temp_x + _w;
        
        groupCnt++;
        if(groupCnt >= _groupSize){
            temp_group++;
            groupCnt = 0;
            
            //                temp_x = 0;
            //                temp_y += _w + 35;
        }
        
        
        if(temp_x > ofGetWidth() - _w -_x){
            temp_x = 0;
            temp_y += _w + 2;
        }
        
        
        ofPopStyle();
    }
    ofFill();
    ofPopMatrix();
    
    ofPushStyle();
    //draw dmx info o
    ofRectangle dmxBarRect(_x,_y,ofGetWidth(),_y-temp_y);
    ofPoint temp_mouse(ofGetMouseX(), ofGetMouseY());
    if(dmxBarRect.inside(temp_mouse)){
        ofRectangle dmxChanRect;
        
        temp_x = 0;
        temp_y = 0;
        for(int i=1; i<=(_groupSize*_devices); i++){
            
            dmxChanRect = ofRectangle(_x + temp_x, _y + temp_y, _w, _w);
            
            
            if(dmxChanRect.inside(temp_mouse)){
                int temp_level = dmx.getLevel(i);
                
                int temp_head = (i/_groupSize)+1;
                int temp_station = ((i/_groupSize) / 3)+1;
                ofDrawBitmapStringHighlight(ofToString(temp_station)+" :   "+ofToString(temp_head),temp_mouse.x,temp_mouse.y-45);
                ofDrawBitmapStringHighlight("chan "+ofToString(i),temp_mouse.x,temp_mouse.y-30);
                ofDrawBitmapStringHighlight("levl "+ofToString(temp_level),temp_mouse.x,temp_mouse.y-15);
                break;
            }
            temp_x = temp_x + _w;
            
            if(temp_x > ofGetWidth() - _w -_x){
                temp_x = 0;
                temp_y += _w + 2;
            }
        }//end  for(int i=1; i<=(_groupSize*_devices); i++)
    }//end  if(dmxBarRect.inside(temp_mouse))
    ofPopStyle();
}

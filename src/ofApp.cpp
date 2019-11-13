#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(30);
    
    ofSetFrameRate(60);
    ofBackground(127);
    
    //IP
    MX_IP = "10.0.1.191";
    US_IP = "10.0.1.192";
    
    broadCastIP = "10.0.1.255"; //not really broadcast. just an indicator if i should be listen to this message
    
    vector<string> ip_list = getLocalIPs();
    myIP = ip_list[0];
    
    for(int i=0; i<ip_list.size(); i++){
        ofLog()<<ip_list[i];
    }
    

    
    allHearts.resize(2);
    
    allHearts[0].setup("me",0,1,dmx);
    allHearts[1].setup("other",1,5,dmx);
    
    group_debug.setName("debug");
    group_debug.add(triggerFakeMe.set("triggerFake",false));
    group_debug.add(meTestTouched.set("meTouched",false));
    group_debug.add(meTestBPM.set("meTestBPM",60,0,200));
    
    group_debug.add(triggerFakeOther.set("triggerFake",false));
    group_debug.add(otherTestTouched.set("otherTouched",false));
    group_debug.add(otherTestBPM.set("otherTestBPM",60,0,200));
    
    gui_main.setup();
    gui_main.setName("remotePulse");
    gui_main.setPosition(10,50);
    gui_main.setHeaderBackgroundColor(ofColor(255,0,0));
    gui_main.add(versionString.set("ver", ""));
    gui_main.add(bEnableDMX.set("enableDMX",false));
    gui_main.add(bShowGui.set("showGui",true));
    gui_main.add(bDebug.set("debug",false));
    gui_main.add(group_debug);
    
    gui_main.add(allHearts[0].bUseSound.set("useAudio for "+allHearts[0].myLabel,false));
    gui_main.add(allHearts[1].bUseSound.set("useAudio for "+allHearts[1].myLabel,false));
    
    gui_main.add(beat2Offset.set("beat2Offset",0.2,0,1));
    gui_main.add(touchBrightness.set("touchBright",100,0,255));
    gui_main.add(firstMaxBrightness.set("firstBright",127,0,255));
    gui_main.add(secondMaxBrightness.set("secondBright",127,0,255));
    gui_main.add(firstVolume.set("firstVol",0.5,0,1));
    gui_main.add(secondVolume.set("secondVol",0.5,0,1)); 
 
    gui_main.loadFromFile("GUIs/gui_main.xml");
    gui_main.minimizeAll();
    
    allHearts[0].bUseSound = false;
    allHearts[1].bUseSound = true;
    
    string temp_versionString = version_object.getVersionString("ofApp.h");
    if(temp_versionString != ""){
        versionString = temp_versionString;
        gui_main.saveToFile("GUIs/gui_main.xml");
    }else{
        versionString = APP_VERSION; 
    }
    ofLog()<<"used versionString "<<versionString;
    
    bEnableDMX = true;
    
    //---dmx
    dmxValues.resize(MAX_DMX_CHANNELS+1);
    fill(dmxValues.begin(), dmxValues.end(), 0);
    
    //MARK: -------DMX setup-----------
#ifdef USE_DMX
    dmx.connect("serial-EN", MAX_DMX_CHANNELS ); // use the name
#endif
    dmxDeviceString = dmx.getDeviceString();
    
    
 
    
    //--osc
    osc_object.setup(myIP, broadCastIP);
    gui_osc.setup();
    gui_osc.setName("OSC");
    gui_osc.setPosition(430, 50);
    gui_osc.setHeaderBackgroundColor(ofColor(255,0,0));
    gui_osc.add(osc_object.parameters_oscManualControls);
    
    gui_osc.loadFromFile("GUIs/gui_osc.xml");
    osc_object.init();
    
    //---serial
    hands_object.setup();
    
    bShowGui = true;
    test_delayTimer.setPeriod(10);
    
}

void ofApp::exit(){
    osc_object.exit();
    
    hands_object.exit();
    //    hands_object.gui_sensor.saveToFile("GUIs/gui-sensor.xml");
}
//--------------------------------------------------------------
void ofApp::update(){
    ofSetWindowTitle(myIP+" | BPM-sender");
    
    checkGui();
    
    if(ofGetElapsedTimef() - logTimer > 2){
        logTimer = ofGetElapsedTimef() ;
        
        if(ofIsStringInString(myIP, MX_IP.substr (0,6)) == true){
            
            ofBuffer buf;
            buf.set(ofGetTimestampString());
            ofBufferToFile("runLogs/heartbeat.txt", buf);
            
            osc_object.addAliveMessage(osc_object.sendToIP,runtime_str, hands_object.aliveCounter);
            
        }else{
            //            ofLog()<<"wrong IP range. is"<<myIP<<" should be "<<MX_IP<<" or "<<US_IP;
        }
        
    }
    
    
    
    //--------------

    
    //Mark:--pass on OSC received BPM and touch
    if(osc_object.gotBPM == true){
        //        allHearts[1].bpm = osc_object.rxBPM;
        allHearts[1].setBPM(osc_object.rxBPM);
        osc_object.gotBPM = false;
    }
    if(osc_object.gotTouch == true){
        allHearts[1].setTouch(osc_object.rxTouch);
        osc_object.gotTouch = false;
    }
    
    //-----
    
    //    for(auto & aHeart : allHearts){
    //    for(int i=0; i<allHearts.size(); i++)
    allHearts[0].update(bEnableDMX, beat2Offset);
    allHearts[1].update(bEnableDMX, beat2Offset);
    //    }
    
    //send out to OSC my BPM
//    if(allHearts[0].haveNewBPM == true){
//        allHearts[0].haveNewBPM = false;
//        osc_object.addBPMMessage(osc_object.sendToIP, allHearts[0].bpm);
//    }
    
    //TODO: relay touched message from serial to OSC / other station
    
    //MARK:-----check serial and set local and remote
    hands_object.update();
    
    if(hands_object.gotBPM == true){
        allHearts[0].setBPM(hands_object.bpm);
        osc_object.addBPMMessage(osc_object.sendToIP, hands_object.bpm);
        hands_object.gotBPM = false;
    }
    if(hands_object.gotTouch == true){
        allHearts[0].setTouch(hands_object.touch);
        osc_object.addTouchMessage(osc_object.sendToIP, hands_object.touch);
        hands_object.gotTouch = false;
    }
    
    if(osc_object.bEnableOSC == true)  osc_object.update(myIP);
    
    //----debugging
    if(old_meTestTouched != meTestTouched){
        old_meTestTouched = meTestTouched;
        allHearts[0].setTouch(meTestTouched);
        osc_object.addTouchMessage(osc_object.sendToIP, meTestTouched);
    }
    if(triggerFakeMe == true && meTestTouched == true){
        triggerFakeMe = false;
        allHearts[0].setBPM(meTestBPM); //allHearts[0].bpm = meTestBPM;
        osc_object.addBPMMessage(osc_object.sendToIP, meTestBPM);
    }
    
    if(old_otherTestTouched != otherTestTouched){
        old_otherTestTouched = otherTestTouched;
        allHearts[1].setTouch(otherTestTouched);
    }
    if(triggerFakeOther == true  && otherTestTouched == true ){
        triggerFakeOther = false;
        allHearts[1].setBPM(otherTestBPM); //allHearts[0].bpm = meTestBPM;
    }
    
    renderDMX();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    drawRuntime(ofGetWidth()/2, 10);
    osc_object.drawAliveMsg(ofGetWidth()/2, 10+30);

    
    allHearts[0].draw(50, 400);
    allHearts[1].draw(350, 400); 
    
    hands_object.draw(10,10);
    
    
#ifdef USE_DMX
    drawDmxBar(5,ofGetHeight() - 50, 2 ,24); 
#endif
    
    if(bShowGui == true){
        gui_main.draw();
        gui_osc.draw();
        hands_object.gui_sensor.draw();
    }
}

void ofApp::renderDMX() {
    
#ifdef USE_DMX
    //    for (int i = 0; i < dmxValues.size()-1; i++) {
    //        dmx.setLevel(i+1, dmxValues[i]);
    //    }
    
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
    ofDrawBitmapString("my runtime: "+runtime_str, _x, _y);
    
}

void ofApp::saveGui(){
        gui_main.saveToFile("GUIs/gui_main.xml");
    hands_object.gui_sensor.saveToFile("GUIs/gui_sensor.xml");
    gui_osc.saveToFile("GUIs/gui_osc.xml");
}

void ofApp::checkGui(){
    hands_object.bDebug = bDebug;
    osc_object.bDebug = bDebug;
    
    if(old_firstVolume != firstVolume && old_secondVolume != secondVolume){
        old_firstVolume = firstVolume;
        old_secondVolume = secondVolume;
         for(auto & aHeart : allHearts){
             aHeart.setVolume(firstVolume, secondVolume);
         }
    }
    for(auto & aHeart : allHearts){
        aHeart.touchBrightness = touchBrightness;
        aHeart.firstMaxBrightness = firstMaxBrightness;
        aHeart.secondMaxBrightness = secondMaxBrightness;
    }
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
    
    
    int _w = 50; //11;
    int groupCnt = 0;
    int temp_group = 0;
    int temp_x = 0;
    int temp_y = 0;
    
    //        ofLog()<<"_groupSize*_devices "<<(_groupSize*_devices);
    for(int i=1; i<=(_groupSize*_devices); i++){
        
#ifdef USE_DMX
        int temp_level = dmx.getLevel(i);
        //        ofLog()<<i<<" temp_level "<<temp_level;
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

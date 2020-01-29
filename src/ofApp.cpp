#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(30);
    
    ofBackground(127);
    
//   ofLog()<<"my PID "<< getpid();
//    //I had to replace getpid() with GetCurrentProcessId() and include windows.h
//    ofSystem("'/bin/systemd-notify' --pid= "+ofToString(getpid()) +" WATCHDOG=1");
    
    //IP
    //    MX_IP = "188.188.188.154";
    //    US_IP = "188.188.188.153";
    
    broadCastIP = "188.188.188.255"; //not really broadcast. just an indicator if i should be listen to this message
    
    vector<string> ip_list = getLocalIPs();
    if(ip_list.empty()){
        myIP = "offline";
    }else{
        myIP = ip_list[0];
    }
    
    for(int i=0; i<ip_list.size(); i++){
        ofLog()<<ip_list[i];
    }
    
#ifdef TARGET_OSX
    ofLog()<<"set system volume";
    system("osascript -e \"set Volume 100\"");
#endif
    
    allHearts.resize(2);
    
    allHearts[0].setup("me",0,1,dmx,serialSendBuffer);
    allHearts[1].setup("other",1,3,dmx,serialSendBuffer);
    
    group_debug.setName("debug");
    
    group_debug.add(bFadeTest.set("fadeTest",false));
    group_debug.add(testDmxChannel.set("testDmxChan",1,1,4));
    group_debug.add(testDmxValues.set("testDmxValues",0,0,255));
    
    group_debug.add(triggerFakeMe.set("triggerMeFake",false));
    group_debug.add(meTestTouched.set("meTouched",false));
    group_debug.add(meTestBPM.set("meTestBPM",60,0,200));
    
    group_debug.add(triggerFakeOther.set("triggerOtherFake",false));
    group_debug.add(otherTestTouched.set("otherTouched",false));
    group_debug.add(otherTestBPM.set("otherTestBPM",60,0,200));
    
    //TODO: do not save debug states
    
    gui_main.setup();
    gui_main.setName("remotePulse");
    gui_main.setPosition(10,70);
    gui_main.setHeaderBackgroundColor(ofColor(255,0,0));
    gui_main.add(versionString.set("ver", ""));
    gui_main.add(lightViaDmx.set("lightViaDmx",false));
    gui_main.add(lightViaSerial.set("lightViaSerial",false));
    gui_main.add(bShowGui.set("showGui",true));
    gui_main.add(bDebug.set("debug",false));
    //    gui_main.add(systemVolume.set("systemVolume",100,0,100));
    gui_main.add(group_debug);
    
    gui_main.add(allHearts[0].group_heart);
    gui_main.add(allHearts[1].group_heart);
    
    
    gui_main.add(minBpmCounter.set("minBpmCount",2,0,4));
    gui_main.add(minBpm.set("minBpm",25,2,50)); //if below it will be ignored
    //    gui_main.add(beat2Offset.set("beat2Offset",0.2,0,1));
    
    gui_main.add(firstBeatOnDur.set("firstBeatOnDur",0.2,0,1));
    gui_main.add(firstPause.set("firstPause",0.2,0,1));
    gui_main.add(secondBeatOnDur.set("secondBeatOnDur",0.2,0,1));
    
    gui_main.add(touchBrightness.set("touchBright",100,0,255));
    gui_main.add(firstMinBrightness.set("firstMinBright",0,0,255));
    gui_main.add(firstMaxBrightness.set("firstMaxBright",127,0,255));
    gui_main.add(secondMinBrightness.set("secondMinBright",0,0,255));
    gui_main.add(secondMaxBrightness.set("secondMaxBright",127,0,255));
    gui_main.add(firstVolume.set("firstVol",0.5,0,1));
    gui_main.add(secondVolume.set("secondVol",0.5,0,1)); 
    gui_main.add(forceUnTouchDuration.set("forceUnTouch",7,0,20)); 
    
    
    gui_main.loadFromFile("GUIs/gui_main.xml");
    gui_main.minimizeAll();
    
    triggerFakeMe = false;
    meTestTouched = false;
    
    triggerFakeOther = false;
    otherTestTouched = false;
    
    allHearts[0].bUseSound = false;
    allHearts[1].bUseSound = true;
    
    bFadeTest = false;
    testDmxValues = 0;
    old_testDmxValues = 0;
    
    string temp_versionString = version_object.getVersionString("ofApp.h");
    if(temp_versionString != ""){
        versionString = temp_versionString;
        gui_main.saveToFile("GUIs/gui_main.xml");
    }else{
        versionString = APP_VERSION; 
    }
    ofLog()<<"used versionString "<<versionString;
    
    //    bEnableDMX = true;
    
    //---dmx
    dmxValues.resize(MAX_DMX_CHANNELS+1);
    fill(dmxValues.begin(), dmxValues.end(), 0);
    
    //MARK: -------DMX setup-----------
#ifdef USE_DMX
    
#if defined(TARGET_LINUX_ARM)
    // stuff for the raspi
    dmx.connect("ttyUSB0", MAX_DMX_CHANNELS ); // use the name
#elif defined(TARGET_OSX)
    // code for osx
    dmx.connect("serial-EN", MAX_DMX_CHANNELS ); // use the name
#else
    
#endif
    
    
#endif //dmx
    dmxDeviceString = dmx.getDeviceString();
    
    
    
#ifdef USE_OSC
    //--osc
    osc_object.setup(myIP, broadCastIP);
    osc_object.gui_osc.setPosition(430, 70);
    osc_object.init();
#endif
    
#ifdef USE_WEB
    web_object.setup(); //myIP, broadCastIP);
    web_object.gui_web.setPosition(430, 70);
    //    web_object.init();
#endif
    
    //---serial
    hands_object.setup(serialSendBuffer);
    
    bShowGui = true;
    test_delayTimer.setPeriod(10);
    
}

void ofApp::exit(){
    
    if(lightViaDmx){
        for(int i=1; i<=512; i++){
            dmx.setLevel(i,0);
        }
        
        dmx.update();
    }
    
#ifdef USE_OSC
    osc_object.exit();
#endif
#ifdef USE_WEB
    web_object.exit();
#endif
    
    hands_object.exit();
    //    hands_object.gui_sensor.saveToFile("GUIs/gui-sensor.xml");
}
//--------------------------------------------------------------
void ofApp::update(){
    ofSetWindowTitle(myIP+" | BPM-sender | " + web_object.getComputerID());
    
    checkGui();
    
    if(ofGetElapsedTimef() - logTimer > 2){
        logTimer = ofGetElapsedTimef() ;
        
        bool connectionOK = true; //false;
#ifdef USE_OSC
        osc_object.addAliveMessage(osc_object.sendToIP,runtime_str, hands_object.aliveCounter);
        
        connectionOK = ofIsStringInString(myIP, osc_object.sendToIP.get().substr (0,6));
#endif
        
#ifdef USE_WEB
        
        web_object.addAliveMessage(web_object.sendToID,runtime_str, hands_object.aliveCounter);
        //TODO: see if there is any reason for web interface needing a app restart
        connectionOK = true;
#endif
        
        if(connectionOK == true){
            
            ofBuffer buf;
            buf.set(ofGetTimestampString());
            ofBufferToFile("runLogs/heartbeat.txt", buf);
            
        }else{
            if(bDebug){
#ifdef USE_OSC
           ofLog()<<"wrong IP range. is"<<myIP<<" should be someting around "<<osc_object.sendToIP;
#endif
#ifdef USE_WEB
                    
#endif
            }
        }
        
    }
    
    if(initDone == false){
        if(initStage == 0){
            ofLog()<<"start init";
            initTimer = ofGetElapsedTimef();
            
            
            //            if(lightViaDmx){
            allHearts[0].setLevel(allHearts[0].beat1Channel,firstMaxBrightness);
            //            }
            allHearts[0].beatPlayer1.play();
            
            initDuration = 2;
            initStage++;
        } else if(initStage == 1 && ofGetElapsedTimef() - initTimer > initDuration){
            initTimer = ofGetElapsedTimef();
            
            //              if(lightViaDmx){
            allHearts[0].setLevel(allHearts[0].beat1Channel,0);
            allHearts[1].setLevel(allHearts[1].beat1Channel,firstMaxBrightness);
            //              }
            allHearts[1].beatPlayer1.play();
            
            initDuration = 2;
            initStage++;
        } else if(initStage == 2 && ofGetElapsedTimef() - initTimer > initDuration){
            initTimer = ofGetElapsedTimef();
            
            //   if(lightViaDmx){
            allHearts[1].setLevel(allHearts[1].beat1Channel,0);
            allHearts[0].setLevel(allHearts[0].beat2Channel,secondMaxBrightness);
            //  }
            allHearts[0].beatPlayer2.play();
            
            initDuration = 2;
            initStage++;
        } else if(initStage == 3 && ofGetElapsedTimef() - initTimer > initDuration){
            initTimer = ofGetElapsedTimef();
            
            //  if(lightViaDmx){
            allHearts[0].setLevel(allHearts[0].beat2Channel,0);
            allHearts[1].setLevel(allHearts[1].beat2Channel,secondMaxBrightness);
            //   }
            allHearts[1].beatPlayer2.play();
            
            initDuration = 2;
            initStage++;
        } else if(initStage == 4 && ofGetElapsedTimef() - initTimer > initDuration){
            initTimer = ofGetElapsedTimef();
            
            //  if(lightViaDmx){
            allHearts[0].setLevel(allHearts[0].beat1Channel,0);
            allHearts[0].setLevel(allHearts[0].beat2Channel,0);
            allHearts[1].setLevel(allHearts[1].beat1Channel,0);
            allHearts[1].setLevel(allHearts[1].beat2Channel,0);
            // }
            initDuration = 2;
            initDone = true;
            ofLog()<<"end init";
        }
    } else {
     
        //Mark:--pass on OSC received BPM and touch
#ifdef USE_OSC
        
        if(osc_object.gotBPM == true){
            allHearts[1].setBPM(osc_object.rxBPM);
            osc_object.gotBPM = false;
        }
        if(osc_object.gotTouch == true){
            allHearts[1].setTouch(osc_object.rxTouch);
            osc_object.gotTouch = false;
        }
#endif
        
#ifdef USE_WEB

        if(web_object.gotBPM == true){
            allHearts[1].setBPM(web_object.rxBPM);
            web_object.gotBPM = false;
        }
        if(web_object.gotTouch == true){
            allHearts[1].setTouch(web_object.rxTouch);
            web_object.gotTouch = false;
        }
#endif
        
        
        //-----
        if(bFadeTest == false){
            allHearts[0].update(); //, firstPause);
            allHearts[1].update(); //, firstPause);
        }
        
        //send out to OSC my BPM        
        //MARK:-----check serial and set local and remote
        //        hands_object.update();
        
        
        //TODO: if smaller than minBpm then ignor BPM but also unTouch
        if(hands_object.gotBPM == true && hands_object.bpm < minBpm){
            hands_object.gotBPM = false;
            hands_object.gotTouch = false;
            allHearts[0].setTouch(false);
            
#ifdef USE_OSC
            osc_object.addTouchMessage(osc_object.sendToIP, false);
#endif
#ifdef USE_WEB
            web_object.addTouchMessage(web_object.sendToID, false);
#endif
            
            ofLog()<<"bpm < minBPM so ignore it and unTouch";
        }
        
        
        if(hands_object.gotBPM == true){
            allHearts[0].setBPM(hands_object.bpm);
#ifdef USE_OSC
            osc_object.addBPMMessage(osc_object.sendToIP, hands_object.bpm);
#endif
#ifdef USE_WEB
             web_object.addBPMMessage(web_object.sendToID, hands_object.bpm);
#endif
            hands_object.gotBPM = false;
            if(hands_object.old_bpm != hands_object.bpm) bpmChangeTimer = ofGetElapsedTimef();
        }
        
        if(forceUnTouchDuration > 0 && (ofGetElapsedTimef() - bpmChangeTimer) >= forceUnTouchDuration){
            ofLog()<<" force unTouch after "<<forceUnTouchDuration<<" seconds";
            bpmChangeTimer = ofGetElapsedTimef();
            
            allHearts[0].setTouch(false);
#ifdef USE_OSC
            osc_object.addTouchMessage(osc_object.sendToIP, false);
#endif
#ifdef USE_WEB
            web_object.addTouchMessage(web_object.sendToID, false);
#endif
        }
        
        if(hands_object.gotTouch == true){
            ofLog()<<" hands_object.gotTouch == true ";
            allHearts[0].setTouch(hands_object.touch);
#ifdef USE_OSC
            osc_object.addTouchMessage(osc_object.sendToIP, hands_object.touch);
#endif
#ifdef USE_WEB
            web_object.addTouchMessage(web_object.sendToID, hands_object.touch);
#endif
            hands_object.gotTouch = false;
        }
        
#ifdef USE_OSC
        if(osc_object.bEnableOSC == true)  osc_object.update(myIP);
#endif
#ifdef USE_WEB
         if(web_object.bEnableWeb == true) web_object.update();
#endif
    }//end else if initDone
    
    //--------------
    
    
    
    //----debugging
    if(old_meTestTouched != meTestTouched){
        old_meTestTouched = meTestTouched;
        allHearts[0].setTouch(meTestTouched);
#ifdef USE_OSC
        osc_object.addTouchMessage(osc_object.sendToIP, meTestTouched);
#endif
#ifdef USE_WEB
         web_object.addTouchMessage(web_object.sendToID, meTestTouched);    
#endif
    }
    if(triggerFakeMe == true && meTestTouched == true){
        triggerFakeMe = false;
        allHearts[0].setBPM(meTestBPM); //allHearts[0].bpm = meTestBPM;
#ifdef USE_OSC
        osc_object.addBPMMessage(osc_object.sendToIP, meTestBPM);
#endif
#ifdef USE_WEB
        web_object.addBPMMessage(web_object.sendToID, meTestBPM);
#endif
    }
    
    if(old_otherTestTouched != otherTestTouched){
        old_otherTestTouched = otherTestTouched;
        allHearts[1].setTouch(otherTestTouched);
    }
    if(triggerFakeOther == true  && otherTestTouched == true ){
        triggerFakeOther = false;
        allHearts[1].setBPM(otherTestBPM); //allHearts[0].bpm = meTestBPM;
    }
    
    hands_object.updateSerial(bFadeTest);
    hands_object.update();
    if(lightViaDmx == true) dmx.update();
    
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    drawRuntime(ofGetWidth()/2, 10);
#ifdef USE_OSC
    osc_object.drawAliveMsg(ofGetWidth()/2, 10+30);
#endif
#ifdef USE_WEB
    web_object.draw(ofGetWidth()/3, ofGetHeight()/4);
    web_object.drawAliveMsg(ofGetWidth()/2, 10+30);    
#endif
    
    ofPushMatrix();
    ofTranslate(50, ofGetHeight() - 350);
    allHearts[0].draw(0, 0);
    allHearts[1].draw(300, 0); 
    ofPopMatrix();
    
    hands_object.draw(10,10);
    
#ifdef USE_DMX
    drawDmxBar(5,ofGetHeight() - 50, 2 ,24); 
#endif
    
    ofDrawBitmapStringHighlight("fps "+ofToString(ofGetFrameRate(),2), ofGetWidth()-100, ofGetHeight() - 30);
    
    if(bShowGui == true){
        gui_main.draw();
#ifdef USE_OSC
        osc_object.gui_osc.draw();
#endif
        
#ifdef USE_WEB
        web_object.gui_web.draw();
#endif
        hands_object.gui_sensor.draw();
    }
}

void ofApp::drawRuntime(int _x, int _y){
    
    float myTime = ofGetElapsedTimef();
    int myMin = myTime/ 60;
    int myHours = myTime / 3600;
    myMin = myMin % 60;
    int mySec = fmod(myTime,60);
    
    int temp_y = 10;
    runtime_str = ofToString(myHours)+":"+ofToString(myMin)+":"+ofToString(mySec);
    ofSetColor(0);
    ofDrawBitmapString("my runtime: "+runtime_str, _x, _y);
    
}

void ofApp::saveGui(){
    gui_main.saveToFile("GUIs/gui_main.xml");
    hands_object.gui_sensor.saveToFile("GUIs/gui_sensor.xml");
#ifdef USE_OSC
    osc_object.gui_osc.saveToFile("GUIs/gui_osc.xml");
#endif
#ifdef USE_WEB
    web_object.gui_web.saveToFile("GUIs/gui_web.xml");
#endif
}

void ofApp::checkGui(){
    
    hands_object.bDebug = bDebug;
#ifdef USE_OSC
    osc_object.bDebug = bDebug;
#endif
#ifdef USE_WEB
    web_object.bDebug = bDebug;
#endif
    /*
     if(old_systemVolume != systemVolume){
     old_systemVolume = systemVolume;
     ofLog()<<"osascript  55";
     
     //                 system("osascript -e \"set Volume 0.1\"");
     //        ofSystem("osascript -e \"set Volume "+ofToString(systemVolume)+"\"")
     
     string cmd = "osascript -e \"set Volume "+ofToString(systemVolume)+"\"";
     system(cmd.c_str());
     cout << cmd.c_str() << endl;
     
     }
     */
    
    if(old_firstVolume != firstVolume || old_secondVolume != secondVolume){
        old_firstVolume = firstVolume;
        old_secondVolume = secondVolume;
        ofLog()<<"aHeart.setVolume(firstVolume, secondVolume)";
        
        for(auto & aHeart : allHearts){
            aHeart.setVolume(firstVolume, secondVolume);
        }
    }
    
    for(auto & aHeart : allHearts){
        aHeart.touchBrightness = touchBrightness;
        aHeart.firstMinBrightness = firstMinBrightness;
        aHeart.firstMaxBrightness = firstMaxBrightness;
        aHeart.secondMinBrightness = secondMinBrightness;
        aHeart.secondMaxBrightness = secondMaxBrightness;
        aHeart.minBpmCounter = minBpmCounter;
        
        aHeart.firstBeatOnDur = firstBeatOnDur;
        aHeart.firstPause = firstPause;
        aHeart.secondBeatOnDur = secondBeatOnDur;
    }
    
    
    if(old_lightViaDmx != lightViaDmx){
        old_lightViaDmx = lightViaDmx;
        
        if(lightViaDmx){
            for(int i=1; i<=512; i++){
                dmx.setLevel(i,0);
            }
            
            dmx.update();
        }
    }
    
    if(old_testDmxValues != testDmxValues){
        old_testDmxValues = testDmxValues;
        ofLog()<<"testDmxValues "<<testDmxValues;
        allHearts[0].setLevel(testDmxChannel,testDmxValues);
    }
    
    allHearts[0].lightViaSerial = lightViaSerial;
    allHearts[0].lightViaDmx = lightViaDmx;
    allHearts[1].lightViaSerial = lightViaSerial;
    allHearts[1].lightViaDmx = lightViaDmx;
    
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
    if(lightViaDmx == false){
        ofDrawBitmapStringHighlight("!!DISABLED!!", 0, yy+=15);
        ofDrawBitmapStringHighlight("dmx device = "+dmxDeviceString,0,yy+=15);
    }else{
        ofDrawBitmapStringHighlight("uses DMX USB", 0, yy+=15);
        ofDrawBitmapStringHighlight("dmx device = "+dmxDeviceString,0,yy+=15);
    }
    
    int _w = 50; //11;
    int groupCnt = 0;
    int temp_group = 0;
    int temp_x = 0;
    int temp_y = 0;
    
    //        ofLog()<<"_groupSize*_devices "<<(_groupSize*_devices);
    for(int i=1; i<=(_groupSize*_devices); i++){
        
        int temp_level = 255;
        
        
        if(lightViaDmx)  temp_level = dmx.getLevel(i);
        //        ofLog()<<i<<" temp_level "<<temp_level;
        
        
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

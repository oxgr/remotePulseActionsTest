#include "ofApp.h"

//--------------------------------------------------------------as
void ofApp::setup(){
    
    ofSetFrameRate(30);
    
    ofBackground(127);
    
    //   ofLog()<<"my PID "<< getpid();
    //    //I had to replace getpid() with GetCurrentProcessId() and include windows.h
    //    ofSystem("'/bin/systemd-notify' --pid= "+ofToString(getpid()) +" WATCHDOG=1");
    
    
    
    
#ifdef TARGET_OSX
    ofLog()<<"set system volume";
    system("osascript -e \"set Volume 100\"");
#endif
    
    arial.load("Arial.ttf", 24, true, true);
    
    allHearts.resize(2);
    
    allHearts[0].setup("me",0,1,dmx,serialSendBuffer);
    allHearts[1].setup("other",1,3,dmx,serialSendBuffer);
    
    group_debug.setName("debug");
    
    group_debug.add(bFadeTest.set("fadeTest",false));
    group_debug.add(testDmxChannel.set("testDmxChan",1,1,512));
    group_debug.add(testDmxValues.set("testDmxValues",0,0,255));
    
    group_debug.add(triggerFakeMe.set("triggerMeFake",false));
    group_debug.add(meFakeTouched.set("meTouched",false));
    group_debug.add(meFakeBPM.set("meFakeBPM",60,0,200));
    
    group_debug.add(triggerFakeOther.set("triggerOtherFake",false));
    group_debug.add(otherFakeTouched.set("otherTouched",false));
    group_debug.add(otherFakeBPM.set("otherFakeBPM",60,0,200));
    
    group_autoFake.setName("autoFake");
    group_autoFake.add(minLocalActiveDuration.set("minLocalActiveDur",60,0,180));
    group_autoFake.add(maxLocalActiveDuration.set("maxLocalActiveDur",90,0,180));
    group_autoFake.add(minLocalInActiveDuration.set("minLocalInActiveDur",5,0,60));
    group_autoFake.add(maxLocalInActiveDuration.set("maxLocalInActiveDur",15,0,60));
    
    group_autoFake.add(remoteInActiveDuration.set("remoteInActiveDur",300,0,600));
    group_autoFake.add(fakeTouchDuration.set("fakeTouchDur",5,0,20));
    
    
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
    gui_main.add(bEnableAutoFake.set("enableAutoFake",false));  //if local is active for long enough but noone is on other end
    gui_main.add(bFakeMe.set("at Athens",false));  //if local sensor only report touch but no BPM then fake one
    gui_main.add(ip_index.set("IP index",0,0,4));
    //    gui_main.add(systemVolume.set("systemVolume",100,0,100));
    gui_main.add(group_debug);
    gui_main.add(group_autoFake);
    
    gui_main.add(allHearts[0].group_heart);
    gui_main.add(allHearts[1].group_heart);
    
    
    gui_main.add(minBpmCounter.set("minBpmCount",2,0,4));
    gui_main.add(minBpm.set("minBpm",25,2,50)); //if below it will be ignored
    //    gui_main.add(beat2Offset.set("beat2Offset",0.2,0,1));
    
    gui_main.add(firstBeatOnDur.set("firstBeatOnDur",0.2,0,1));
    gui_main.add(firstPause.set("firstPause",0.2,0,1));
    gui_main.add(secondBeatOnDur.set("secondBeatOnDur",0.2,0,1));
    
    gui_main.add(touchBrightness.set("touchBright",100,0,255));
    gui_main.add(unTouchBrightness.set("unTouchBright",100,0,255));
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
    meFakeTouched = false;
    
    triggerFakeOther = false;
    otherFakeTouched = false;
    
    allHearts[0].cur_tilt[0] = allHearts[0].unTouch_tilt_value1;
    allHearts[0].cur_tilt[1] = allHearts[0].unTouch_tilt_value2;
    allHearts[1].cur_tilt[0] = allHearts[1].unTouch_tilt_value1;
    allHearts[1].cur_tilt[1] = allHearts[1].unTouch_tilt_value2;    

    allHearts[0].cur_pan[0] = allHearts[0].unTouch_pan_value1;
    allHearts[0].cur_pan[1] = allHearts[0].unTouch_pan_value2;
    allHearts[1].cur_pan[0] = allHearts[1].unTouch_pan_value1;
    allHearts[1].cur_pan[1] = allHearts[1].unTouch_pan_value2;  
    
    
    gui_movingHead.setup();
    gui_movingHead.setName("moving head");
    gui_movingHead.setPosition(gui_main.getShape().getRight()+5,10);
    gui_movingHead.setHeaderBackgroundColor(ofColor(255,0,0));
    
    gui_movingHead.add(bUseMovingHead.set("use movingHead",true));
    gui_movingHead.add(bTakeFromHearts.set("use hearts",true));
    
    for(int i=0; i<2; i++){
        group_dmx_values.setName("dmx values");
        string letter = GetLetter(i);
        group_dmx_values.add(pan_angle_value[i].set("pan angle "+letter,0,-250,250));
        group_dmx_values.add(tilt_angle_value[i].set("tilt angle "+letter,0,-130,130));
        group_dmx_values.add(dimmer_value[i].set("dimmer "+letter,0,0,255));
        group_dmx_values.add(cyan_value[i].set("cyan "+letter,0,0,255));
        group_dmx_values.add(magenta_value[i].set("magenta "+letter,0,0,255));
        group_dmx_values.add(yellow_value[i].set("yellow "+letter,0,0,255));
        group_dmx_values.add(colorWheel_value[i].set("colorWheel "+letter,0,0,255));
        
        group_dmx_values.add(zoom_value[i].set("zoom "+letter,0,0,255));
        group_dmx_values.add(focus_value[i].set("focus "+letter,0,0,255));
        group_dmx_values.add(shutter_onOff[i].set("shutter onOff "+letter,false));
        group_dmx_values.add(lamp_onOff[i].set("lamp onOff "+letter,false));
        group_dmx_values.add(gobo_value[i].set("gobo "+letter,0,0,255));
    }
    
    group_dmx_channel.setName("dmx channels");
    group_dmx_channel.add(dmx_start_channels[0].set("A start addr",1,1,512));
    group_dmx_channel.add(dmx_start_channels[1].set("B start addr",41,1,512));
    
    group_dmx_channel.add(pan_channel.set("pan",1,1,512));
    group_dmx_channel.add(pan_fine_channel.set("pan fine",2,1,512));
    group_dmx_channel.add(tilt_channel.set("tilt",3,1,512));
    group_dmx_channel.add(tilt_fine_channel.set("tilt fine",4,1,512));
    group_dmx_channel.add(dimmer_channel.set("dimmer",21,1,512));
    group_dmx_channel.add(cyan_channel.set("cyan",5,1,512));
    group_dmx_channel.add(magenta_channel.set("magnets",6,1,512));
    group_dmx_channel.add(yellow_channel.set("yellow",7,1,512));
    group_dmx_channel.add(colorWheel_channel.set("cto",8,1,512));
    group_dmx_channel.add(zoom_channel.set("zoom",18,1,512));
    group_dmx_channel.add(focus_channel.set("focus",16,1,512));
    group_dmx_channel.add(gobo_channel.set("gobo",13,1,512));
    
    group_dmx_channel.add(shutter_channel.set("shutter",20,1,512));
    group_dmx_channel.add(shutter_closedValue.set("shutter closed",0,0,0));
    group_dmx_channel.add(shutter_openValue.set("shutter open",255,255,255));
    
    group_dmx_channel.add(lamp_channel.set("lamp",26,1,512));
    group_dmx_channel.add(lamp_onValue.set("lamp on",40,40,40));
    group_dmx_channel.add(lamp_offValue.set("lamp off",50,50,50));
    
    
    gui_movingHead.add(group_dmx_values);
    gui_movingHead.add(group_dmx_channel);
    gui_movingHead.loadFromFile("GUIs/gui_movingHead.xml");
    gui_movingHead.minimizeAll();
    
    //    allHearts[0].bUseSound = false;
    //    allHearts[1].bUseSound = true;
    
    bFadeTest = false;
    testDmxValues = 0;
    old_testDmxValues = 0;
    
    shutter_curValue[0] = shutter_closedValue;
    shutter_curValue[1] = shutter_closedValue;
    
    lamp_curValue[0] = lamp_offValue;
    lamp_curValue[1] = lamp_offValue;
    
    
    //IP
    //    MX_IP = "188.188.188.154";
    //    US_IP = "188.188.188.153";
    
    broadCastIP = "188.188.188.255"; //not really broadcast. just an indicator if i should be listen to this message
    
    vector<string> ip_list = getLocalIPs();
    for(int i=0; i<ip_list.size(); i++){
        ofLog()<<ip_list[i];
    }
    
    if(ip_list.empty()){
        myIP = "offline";
    }else{
        ofLog()<<"ip_index "<<ip_index<<" ip_list.size() "<<ip_list.size();
        ip_index = MIN(ip_index.get(),ip_list.size()-1);
        myIP = ip_list[ip_index];
        myIP = ofTrim(myIP);
    }
    
    ofLog()<<"myIP "<<myIP;
    
    
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
    
    localActiveDuration = 5; //int(ofRandom(minLocalActiveDuration,maxLocalActiveDuration));
    localInActiveDuration = int(ofRandom(minLocalInActiveDuration,maxLocalInActiveDuration));
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
    
    fakeMe_startTimer = 0;
    fakeMe_minDuration = 5;
    fakeMe_running = false;
    
    
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
    
#ifdef USE_WEB
    ofSetWindowTitle(myIP+" | BPM-sender | " + web_object.getComputerID());
#else
    ofSetWindowTitle(myIP+" | BPM-sender | ");
#endif
    
    checkGui();
    
    if(ofGetElapsedTimef() - logTimer > 5){
        logTimer = ofGetElapsedTimef() ;
        
        bool connectionOK = true; //false;
#ifdef USE_OSC
        osc_object.addAliveMessage(osc_object.sendToIP,runtime_str, hands_object.aliveCounter);
        
        connectionOK = ofIsStringInString(myIP, osc_object.sendToIP.get().substr (0,6));
#endif
        
#ifdef USE_WEB
        
        web_object.addAliveMessage(web_object.other_computerID,runtime_str, hands_object.aliveCounter);
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
                ofLog()<<"wrong IP range. is "<<myIP<<" should be someting around "<<osc_object.sendToIP;
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
            allHearts[0].setLevel(0,allHearts[0].beat1Channel,firstMaxBrightness);
            //            }
            allHearts[0].beatPlayer1.play();
            
            initDuration = 2;
            initStage++;
        } else if(initStage == 1 && ofGetElapsedTimef() - initTimer > initDuration){
            initTimer = ofGetElapsedTimef();
            
            //              if(lightViaDmx){
            allHearts[0].setLevel(0,allHearts[0].beat1Channel,0);
            allHearts[1].setLevel(0,allHearts[1].beat1Channel,firstMaxBrightness);
            //              }
            allHearts[1].beatPlayer1.play();
            
            initDuration = 2;
            initStage++;
        } else if(initStage == 2 && ofGetElapsedTimef() - initTimer > initDuration){
            initTimer = ofGetElapsedTimef();
            
            //   if(lightViaDmx){
            allHearts[1].setLevel(0,allHearts[1].beat1Channel,0);
            allHearts[0].setLevel(1,allHearts[0].beat2Channel,secondMaxBrightness);
            //  }
            allHearts[0].beatPlayer2.play();
            
            initDuration = 2;
            initStage++;
        } else if(initStage == 3 && ofGetElapsedTimef() - initTimer > initDuration){
            initTimer = ofGetElapsedTimef();
            
            //  if(lightViaDmx){
            allHearts[0].setLevel(1,allHearts[0].beat2Channel,0);
            allHearts[1].setLevel(1,allHearts[1].beat2Channel,secondMaxBrightness);
            //   }
            if(allHearts[0].bUseSound){
                allHearts[0].beatPlayer2.play();
            }else{
                allHearts[1].beatPlayer2.play();
            }
            
            
            initDuration = 2;
            initStage++;
        } else if(initStage == 4 && ofGetElapsedTimef() - initTimer > initDuration){
            initTimer = ofGetElapsedTimef();
            
            //  if(lightViaDmx){
            allHearts[0].setLevel(0,allHearts[0].beat1Channel,0);
            allHearts[0].setLevel(1,allHearts[0].beat2Channel,0);
            allHearts[1].setLevel(0,allHearts[1].beat1Channel,0);
            allHearts[1].setLevel(1,allHearts[1].beat2Channel,0);
            // }
            initDuration = 2;
            initDone = true;
            ofLog()<<"end init";
        }
    } else {
        
        //Mark:--pass on OSC received BPM and touch
        
        
#ifdef USE_OSC
        if(osc_object.gotBPM == true){
            remoteInActiveTimer = ofGetElapsedTimef();
            allHearts[1].setBPM(osc_object.rxBPM);
            osc_object.gotBPM = false;
        }
        if(osc_object.gotTouch == true){
            remoteInActiveTimer = ofGetElapsedTimef();
            allHearts[1].setTouch(osc_object.rxTouch);
            osc_object.gotTouch = false;
            
            otherFakeTouched = false;
            old_otherFakeTouched = false;
        }
        
#endif
        
#ifdef USE_WEB
        if(web_object.gotBPM == true){
            remoteInActiveTimer = ofGetElapsedTimef();
            allHearts[1].setBPM(web_object.rxBPM);
            web_object.gotBPM = false;
        }
        if(web_object.gotTouch == true){
            remoteInActiveTimer = ofGetElapsedTimef();
            allHearts[1].setTouch(web_object.rxTouch);
            web_object.gotTouch = false;
            
            otherFakeTouched = false;
            old_otherFakeTouched = false;
        }        
#endif
        
        if( allHearts[0].isTouched == false){
            localActiveTimer = ofGetElapsedTimef();
        } else{
            localInActiveTimer = ofGetElapsedTimef();
        }
        
        
        //MARK:fake other BPM
        //if local is active for long enough but no-one is on other end
        //then pick a fake other BPM
        if(bEnableAutoFake == true){
            if(ofGetElapsedTimef() - localActiveTimer > localActiveDuration && ofGetElapsedTimef() - remoteInActiveTimer > remoteInActiveDuration){
                
                localActiveTimer = ofGetElapsedTimef();
                ofLog()<<"set fake BPM for other, due to long non-interaction";
                otherFakeTouched = true;
                old_otherFakeTouched = true;
                allHearts[1].setTouch(otherFakeTouched);
                //                allHearts[1].update();
                otherFakeBPM = ofRandom(60,80);
                //                allHearts[1].bpmCounter = allHearts[1].minBpmCounter;
                //                allHearts[1].setBPM(otherFakeBPM);
                
                //                allHearts[1].setBPM(otherFakeBPM);
                
                if(allHearts[1].bpmCounter < allHearts[1].minBpmCounter){
                    localActiveDuration = fakeTouchDuration;
                }else{
                    localActiveDuration = int(ofRandom(minLocalActiveDuration,maxLocalActiveDuration));
                }
                localInActiveDuration = int(ofRandom(minLocalInActiveDuration,maxLocalInActiveDuration)); 
                
                allHearts[1].setBPM(otherFakeBPM);
            } else {
                if(ofGetElapsedTimef() - localInActiveTimer > localInActiveDuration){
                    otherFakeTouched = false;
                }
            }
        }
        //-----
        
        
        if(bFadeTest == false && osc_object.showState != SHOW_EMERGENCY && osc_object.showState != SHOW_STOP){
            allHearts[0].update(); //, firstPause);
            allHearts[1].update(); //, firstPause);
        }
        
        //send out to OSC my BPM        
        //MARK:-----check serial and set local and remote
        //        hands_object.update();
        
        
        if(hands_object.gotBPM == true){
            allHearts[0].setBPM(hands_object.bpm);
#ifdef USE_OSC
            osc_object.addBPMMessage(osc_object.sendToIP, hands_object.bpm);
#endif
#ifdef USE_WEB
            web_object.addBPMMessage(web_object.other_computerID, hands_object.bpm);
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
            web_object.addTouchMessage(web_object.other_computerID, false);
#endif
        }
        //MARK:fake my BPM
        //as soon as sensor detects touch fake a BPM, because in Athens Nikos did not produce a BPM but a touch 
        if(bFakeMe == true){
            if(hands_object.gotTouch == true){
                if(hands_object.touch == 1){
                    //detected touch
                    //                    hands_object.bpm = 71; //ofRandom(66,69);
                    //                    allHearts[0].setBPM(hands_object.bpm);
                    //                    allHearts[0].bpmCounter = minBpmCounter;
                    meFakeBPM = ofRandom(50,100);
                    triggerFakeMe = true;
                    meFakeTouched = true;
                    fakeMe_startTimer = ofGetElapsedTimef();
                    fakeMe_running = true;
                } else {
                    //detected un-touch
                    if(fakeMe_running == true && ofGetElapsedTimef() - fakeMe_startTimer > fakeMe_minDuration){
                        fakeMe_running = false;
                        triggerFakeMe = false;
                        meFakeTouched = false;
                    }
                }
            }
            
        }
        
        if(hands_object.gotTouch == true){
            ofLog()<<" hands_object.gotTouch == true ";
            allHearts[0].setTouch(hands_object.touch);
#ifdef USE_OSC
            osc_object.addTouchMessage(osc_object.sendToIP, hands_object.touch);
#endif
#ifdef USE_WEB
            web_object.addTouchMessage(web_object.other_computerID, hands_object.touch);
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
    if(old_meFakeTouched != meFakeTouched){
        old_meFakeTouched = meFakeTouched;
        allHearts[0].setTouch(meFakeTouched);
#ifdef USE_OSC
        osc_object.addTouchMessage(osc_object.sendToIP, meFakeTouched);
#endif
#ifdef USE_WEB
        web_object.addTouchMessage(web_object.other_computerID, meFakeTouched);    
#endif
    }
    if(triggerFakeMe == true && meFakeTouched == true){
        triggerFakeMe = false;
        allHearts[0].setBPM(meFakeBPM); //allHearts[0].bpm = meFakeBPM;
#ifdef USE_OSC
        osc_object.addBPMMessage(osc_object.sendToIP, meFakeBPM);
#endif
#ifdef USE_WEB
        web_object.addBPMMessage(web_object.other_computerID, meFakeBPM);
#endif
    }
    
    if(old_otherFakeTouched != otherFakeTouched){
        old_otherFakeTouched = otherFakeTouched;
        allHearts[1].setTouch(otherFakeTouched);
    }
    if(triggerFakeOther == true  && otherFakeTouched == true ){
        triggerFakeOther = false;
        allHearts[1].setBPM(otherFakeBPM); //allHearts[0].bpm = meFakeBPM;
    }
    
    hands_object.updateSerial(bFadeTest);
    hands_object.update();
    
    //dmx brightness get's updated in oneHeart objects
    //MARK: set DMX and STATES
    
    if(osc_object.showState == SHOW_START){
        
        for(int i =0; i<2; i++){
            int offset = dmx_start_channels[i]-1;
            dmx.setLevel(offset+dimmer_channel,127);
            lamp_onOff[i] = true;
            shutter_onOff[i] = true;
            allHearts[i].setVolume(firstVolume, secondVolume);
            
        }
        bTakeFromHearts = true;
        osc_object.showState = SHOW_NEUTRAL;
    }else if(osc_object.showState == SHOW_STOP){
        
        for(int i =0; i<2; i++){
            int offset = dmx_start_channels[i]-1;
            
            dmx.setLevel(offset+dimmer_channel,0);
                        lamp_onOff[i] = false;
            shutter_onOff[i] = false;
            allHearts[i].setVolume(0, 0);
            tilt_angle_value[0] = tilt_angle_value[0].getMin();
            tilt_angle_value[1] = tilt_angle_value[1].getMin();
            pan_angle_value[0] = pan_angle_value[0].getMin();
            pan_angle_value[1] = pan_angle_value[1].getMin();
        }
        bTakeFromHearts = false;
    } else if(osc_object.showState == SHOW_EMERGENCY){
        for(int i =0; i<2; i++){
            int offset = dmx_start_channels[i]-1;
            
            dmx.setLevel(offset+dimmer_channel,255);
                        lamp_onOff[i] = true;
            shutter_onOff[i] = true;
            allHearts[i].setVolume(0, 0);
            
            tilt_angle_value[0] = 0;
            tilt_angle_value[1] = 0;
            pan_angle_value[0] = 180;
            pan_angle_value[1] = 190;
        }
        bTakeFromHearts = false;
    } else if(osc_object.showState == SHOW_NEUTRAL){
        
    }
    
    if(lightViaDmx == true){
        if(bUseMovingHead == true){
            
            //            int start_1 = dmx_start_channle[0]-1; //upper mounted movinghead fixture
            //            int start_2 = dmx_start_channle[1]-1; //lower mounted
            
            for(int i =0; i<2; i++){
                int offset = dmx_start_channels[i]-1;
                
                if(bTakeFromHearts == false){
                    int pan_angleInt = ofMap(pan_angle_value[i],pan_angle_value[i].getMin(),pan_angle_value[i].getMax(),0,65536);
                    int lowerBit = pan_angleInt >> 8;
                    int higherBit = pan_angleInt % 256;
                    dmx.setLevel(offset+pan_channel,lowerBit);
                    dmx.setLevel(offset+pan_fine_channel,higherBit);
                    
                    int tilt_angleInt = ofMap(tilt_angle_value[i],tilt_angle_value[i].getMin(),tilt_angle_value[i].getMax(),0,65536);
                    lowerBit = tilt_angleInt >> 8;
                    higherBit = tilt_angleInt % 256;
                    dmx.setLevel(offset+tilt_channel,lowerBit);
                    dmx.setLevel(offset+tilt_fine_channel,higherBit);

                    dmx.setLevel(offset+dimmer_channel,ofMap(dimmer_value[i], dimmer_value[i].getMin(), dimmer_value[i].getMax(), 0, 255));
                }else{
                    if(allHearts[i].bUseLights){
                        //                        int pan_angleInt = ofMap(pan_angle_value[0],pan_angle_value[0].getMin(),pan_angle_value[0].getMax(),0,65536);
                        //                        int lowerBit = pan_angleInt >> 8;
                        //                        int higherBit = pan_angleInt % 256;
                        //                        dmx.setLevel(dmx_start_channels[0]-1+pan_channel,lowerBit);
                        //                        dmx.setLevel(dmx_start_channels[0]-1+pan_fine_channel,higherBit);
                        //                    
                        //                        pan_angleInt = ofMap(pan_angle_value[1],pan_angle_value[1].getMin(),pan_angle_value[1].getMax(),0,65536);
                        //                        lowerBit = pan_angleInt >> 8;
                        //                        higherBit = pan_angleInt % 256;
                        //                        dmx.setLevel(dmx_start_channels[1]-1+pan_channel,lowerBit);
                        //                        dmx.setLevel(dmx_start_channels[1]-1+pan_fine_channel,higherBit);
                        
                        tilt_angle_value[0] = allHearts[i].cur_tilt[0];
                        tilt_angle_value[1] = allHearts[i].cur_tilt[1];
                        
                        pan_angle_value[0] = allHearts[i].cur_pan[0];
                        pan_angle_value[1] = allHearts[i].cur_pan[1];
                        
                        int tilt_angleInt =  ofMap(tilt_angle_value[0],tilt_angle_value[i].getMin(),tilt_angle_value[i].getMax(),0,65536);
                        int lowerBit = tilt_angleInt >> 8;
                        int higherBit = tilt_angleInt % 256;
                        dmx.setLevel(dmx_start_channels[0]-1+tilt_channel,lowerBit);
                        dmx.setLevel(dmx_start_channels[0]-1+tilt_fine_channel,higherBit);
                        
                       
                        tilt_angleInt = ofMap(tilt_angle_value[1],tilt_angle_value[i].getMin(),tilt_angle_value[i].getMax(),0,65536);
                        lowerBit = tilt_angleInt >> 8;
                        higherBit = tilt_angleInt % 256;
                        dmx.setLevel(dmx_start_channels[1]-1+tilt_channel,lowerBit);
                        dmx.setLevel(dmx_start_channels[1]-1+tilt_fine_channel,higherBit);
                        
                        int pan_angleInt =  ofMap(pan_angle_value[0],pan_angle_value[i].getMin(),pan_angle_value[i].getMax(),0,65536);
                        lowerBit = pan_angleInt >> 8;
                        higherBit = pan_angleInt % 256;
                        dmx.setLevel(dmx_start_channels[0]-1+pan_channel,lowerBit);
                        dmx.setLevel(dmx_start_channels[0]-1+pan_fine_channel,higherBit);
                        
                        
                        pan_angleInt = ofMap(pan_angle_value[1],pan_angle_value[i].getMin(),pan_angle_value[i].getMax(),0,65536);
                        lowerBit = pan_angleInt >> 8;
                        higherBit = pan_angleInt % 256;
                        dmx.setLevel(dmx_start_channels[1]-1+pan_channel,lowerBit);
                        dmx.setLevel(dmx_start_channels[1]-1+pan_fine_channel,higherBit);
                        
                        dimmer_value[0] = allHearts[i].curDimmer[0];
                        dimmer_value[1] = allHearts[i].curDimmer[1];
                        dmx.setLevel(dmx_start_channels[0]-1+dimmer_channel,ofMap(dimmer_value[0], dimmer_value[0].getMin(), dimmer_value[0].getMax(), 0, 255));
                        dmx.setLevel(dmx_start_channels[1]-1+dimmer_channel,ofMap(dimmer_value[1], dimmer_value[1].getMin(), dimmer_value[1].getMax(), 0, 255));

                        
                    }
                    //                    ofLog()<<i<<" cur_tilt "<<allHearts[i].cur_tilt[i]<<" tilt_angleInt "<<tilt_angleInt<<" pan_angleInt "<<pan_angleInt;
                }
                
                
                
                dmx.setLevel(offset+focus_channel,ofMap(focus_value[i], focus_value[i].getMin(), focus_value[i].getMax(), 0, 255));
                dmx.setLevel(offset+zoom_channel,ofMap(zoom_value[i], zoom_value[i].getMin(), zoom_value[i].getMax(), 0, 255));
                
                dmx.setLevel(offset+cyan_channel,ofMap(cyan_value[i], cyan_value[i].getMin(), cyan_value[i].getMax(), 0, 255));
                dmx.setLevel(offset+magenta_channel,ofMap(magenta_value[i], magenta_value[i].getMin(), magenta_value[i].getMax(), 0, 255));
                dmx.setLevel(offset+yellow_channel,ofMap(yellow_value[i], yellow_value[i].getMin(), yellow_value[i].getMax(), 0, 255));
                dmx.setLevel(offset+colorWheel_channel,ofMap(colorWheel_value[i], colorWheel_value[i].getMin(), colorWheel_value[i].getMax(), 0, 255));
                
                dmx.setLevel(offset+gobo_channel,gobo_value[i]);
                
                dmx.setLevel(offset+shutter_channel,shutter_curValue[i]);
                //                ofLog()<<"focus_channel "<<(offset+focus_channel)<<" focus_value[i] "<<focus_value[i];
            }
        }//end if(bUseMovingHead == true)
        
        dmx.update();
    }//end if(lightViaDmx == true)
    
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
    if(otherFakeTouched == true){
        arial.drawString("F", 405, 50);
    }
    ofPopMatrix();
    
    hands_object.draw(10,10);
    
    float temp_dur0 = ofGetElapsedTimef() - localActiveTimer;
    float temp_dur1 = ofGetElapsedTimef() - localInActiveTimer;
    float temp_dur2 = ofGetElapsedTimef() - remoteInActiveTimer;
    
    ofSetColor(255);
    stringstream ss;
    ss<<"localActive "+ofToString(temp_dur0,1) + " / " +ofToString(localActiveDuration,1)<<endl;
    ss<<"localInActive "+ofToString(temp_dur1,1) + " / " +ofToString(localInActiveDuration,1)<<endl;
    ss<<"remoteInActive "+ofToString(temp_dur2,1) + " / " +ofToString(remoteInActiveDuration,1)<<endl;
    ofDrawBitmapString(ss.str(), ofGetWidth()-200, ofGetHeight() - 80);
    
#ifdef USE_DMX
    drawDmxBar(5,ofGetHeight() - 50, 40 ,2); 
#endif
    
    ofDrawBitmapStringHighlight("fps "+ofToString(ofGetFrameRate(),2), ofGetWidth()-100, ofGetHeight() - 30);
    
    
    
    
    
    if(bShowGui == true){
        gui_main.draw();
        gui_movingHead.draw();
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
    
    runtime_str = ofToString(myHours)+":"+ofToString(myMin)+":"+ofToString(mySec);
    ofSetColor(0);
    ofDrawBitmapString("my runtime: "+runtime_str, _x, _y);
    
}

void ofApp::saveGui(){
    gui_main.saveToFile("GUIs/gui_main.xml");
    gui_movingHead.saveToFile("GUIs/gui_movingHead.xml");
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
    
    for(int i=0; i<2; i++){
        if(shutter_onOff[i] != old_shutter_onOff[i]){
            old_shutter_onOff[i] = shutter_onOff[i];
            if(shutter_onOff[i] == false)  shutter_curValue[i] = shutter_closedValue;
            else shutter_curValue[i] = shutter_openValue;
        }
        if(lamp_onOff[i] != old_lamp_onOff[i]){
            old_lamp_onOff[i] = lamp_onOff[i];
            if(lamp_onOff[i] == false)  lamp_curValue[i] = lamp_offValue;
            else lamp_curValue[i] = lamp_onValue;
        }
    }
    
    
    
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
        aHeart.unTouchBrightness = unTouchBrightness;
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
        allHearts[0].setLevel(0,testDmxChannel,testDmxValues);
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
    
    if(key == 't'){
        if(hands_object.touch == 1){
            hands_object.touch = 0;
        }else{
            hands_object.touch = 1;
        }
        hands_object.gotTouch = true;
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
                ofLog() <<"add to ip list: "<< ip;
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
                ofLog() <<"add to ip list: "<< ip;
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
    
    float _w = (ofGetWidth()-_x) / (_groupSize*_devices); //11;
    int groupCnt = 0;
    int temp_group = 0;
    float temp_x = 0;
    float temp_y = 0;
    
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


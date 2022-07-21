#pragma once

#include "ofMain.h"

#include "ofxGui.h"

#include "ofxBiquadFilter.h"

#include "customDefines.h"

#ifdef USE_OSC

#include "oscNetwork.h"

#endif

#ifdef USE_WEB

#include "webNetwork.h"

#endif

#include "ofxTiming.h"

#include "sensorHandler.h"

//#ifdef USE_DMX

#include "ofxDmx.h"

//#endif

#include "oneHeart.h"

#include "versionNumbering.h"


#define APP_VERSION "42 20220721 b40ea3d"

class ofApp : public ofBaseApp{
public:
    
    ofParameter<string> versionString;
    versionNumbering version_object;
    
    void setup();
    void exit();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    ofParameterGroup group_debug;
    
    ofxPanel gui_main;
    ofParameter<bool> bShowGui;
    ofParameter<bool> bDebug;
    //    bool bShowGui;
    ofParameter<int> testDmxChannel;
    ofParameter<int> testDmxValues;
    int old_testDmxValues;
    ofParameter<bool> bFadeTest;
    
    void saveGui();
    void checkGui();
    float logTimer;
    
    bool initDone;
    int initStage;
    float initTimer;
    float initDuration;
    
    //---IP
    string myIP;
    
    string MX_IP, US_IP;
    string broadCastIP;
    
    int myID;
    vector<string> getLocalIPs();
    
    ofParameter<int> ip_index;
    //----serial button
    sensorHandler hands_object;
    vector<string> serialSendBuffer;
    
    
    //---OSC 
#ifdef USE_OSC
    oscNetwork osc_object;
#endif
#ifdef USE_WEB
    webNetwork web_object;
#endif
    
    //runtime
    void drawRuntime(int _x, int _y);
    string runtime_str;
    
    vector<oneHeart> allHearts;
    
    DelayTimer test_delayTimer;
    
    //--dmx
    ofxDmx dmx;
    ofParameter<bool> lightViaDmx;
    ofParameter<bool> lightViaSerial;
    bool old_lightViaDmx;
    bool old_lightViaSerial;
    
    string dmxDeviceString;
    vector<uint8_t> dmxValues;
//    ofParameter<bool> bEnableDMX;
//    bool old_bEnableDMX;
    void drawDmxBar(int _x, int _y, int _groupSize, int _devices);
  
    ofParameter<int> firstMinBrightness;
    ofParameter<int> firstMaxBrightness;
     ofParameter<int> secondMinBrightness;
    ofParameter<int> secondMaxBrightness;
    ofParameter<int> touchBrightness;
//    ofParameter<float> beat2Offset;
    
    ofParameter<float> firstBeatOnDur;
    ofParameter<float> firstPause;
    ofParameter<float> secondBeatOnDur;
    
    ofParameter<float> firstVolume;
    float old_firstVolume;
    ofParameter<float> secondVolume;
    float old_secondVolume;
    
    //---test
    ofParameter<bool> triggerFakeMe;
    ofParameter<bool> triggerFakeOther;
    
    ofParameter<int> meFakeBPM;
    ofParameter<int> otherFakeBPM;
    
    ofParameter<bool> meFakeTouched;
    bool old_meFakeTouched;
    ofParameter<bool> otherFakeTouched;
    bool old_otherFakeTouched;
    
    ofParameter<int> systemVolume;
    int old_systemVolume;
    
    ofParameter<int> minBpmCounter;
    ofParameter<int> minBpm;
    ofParameter<float> forceUnTouchDuration;
    float bpmChangeTimer;
    
    ofParameterGroup group_autoFake;
    ofParameter<bool> bEnableAutoFake;
    ofParameter<bool> bFakeMe;
    float fakeMe_startTimer;
    ofParameter<float> fakeMe_minDuration;
    bool fakeMe_running;
    
    ofParameter<int> minLocalActiveDuration;
    ofParameter<int> maxLocalActiveDuration;
    float localActiveTimer;
    float localActiveDuration;
    
    ofParameter<int> minLocalInActiveDuration;
    ofParameter<int> maxLocalInActiveDuration;
    float localInActiveTimer;
     float localInActiveDuration;
    
    float localFakeChangeTimer;
    
    ofParameter<int> remoteInActiveDuration;
    float remoteInActiveTimer;
    
    ofParameter<int> fakeTouchDuration;
    
        ofTrueTypeFont    arial;
    ofxPanel gui_movingHead;
    ofParameter<bool> bUseMovingHead;
    
    ofParameterGroup group_dmx_channel;
    ofParameterGroup group_dmx_values;
    
    
    ofParameter<int> pan_channel, pan_fine_channel;
    ofParameter<float> pan_angle_value;
    ofParameter<int> tilt_channel, tilt_fine_channel;
    ofParameter<float> tilt_angle_value; 
    
    ofParameter<int> zoom_channel, zoom_fine_channel;
    ofParameter<float> zoom_value;
    
    ofParameter<int> gobo_channel;
    ofParameter<int> gobo_value;
};

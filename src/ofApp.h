#pragma once

#include "ofMain.h"

enum SHOWSTATES{

    SHOW_NEUTRAL = 0,

    SHOW_START = 1,

    SHOW_STOP = 2,

    SHOW_RELAXED = 3,

    SHOW_EMERGENCY = 4

};

//

//enum SHOWSTATES_NAMES{

//    SHOW_NEURAL = "neutral",

//    SHOW_START = "start",

//    SHOW_STOP = "stop,

//    SHOW_RELAXED = "relaxed",

//    SHOW_EMERGENCY = "emergency"

//};

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


#define APP_VERSION "46 20220813 481b88d"

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
    ofParameter<int> unTouchBrightness;
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
    
    ofParameter<int> dmx_start_channels[2];
    
    ofParameter<int> pan_channel, pan_fine_channel;
    ofParameter<float> pan_angle_value[2];
    ofParameter<int> tilt_channel, tilt_fine_channel;
    ofParameter<float> tilt_angle_value[2]; 
    
    ofParameter<int> zoom_channel, zoom_fine_channel;
    ofParameter<float> zoom_value[2];
    
    ofParameter<int> gobo_channel;
    ofParameter<int> gobo_value[2];
    
    ofParameter<int> shutter_channel;
    ofParameter<bool> shutter_onOff[2];
    bool old_shutter_onOff[2];
    
    ofParameter<int> shutter_openValue;
    ofParameter<int> shutter_closedValue;
    int shutter_curValue[2];
    
    ofParameter<int> lamp_channel;
    ofParameter<bool> lamp_onOff[2];
    bool old_lamp_onOff[2];
    ofParameter<int> lamp_offValue;
    ofParameter<int> lamp_onValue;
    int lamp_curValue[2];
    
    ofParameter<int> dimmer_channel;
    ofParameter<int> dimmer_value[2];
    
    ofParameter<int> cyan_channel;
    ofParameter<int> cyan_value[2];
    ofParameter<int> magenta_channel;
    ofParameter<int> magenta_value[2];
    ofParameter<int> yellow_channel;
    ofParameter<int> yellow_value[2];
    ofParameter<int> colorWheel_channel;
    ofParameter<int> colorWheel_value[2];
    
    ofParameter<int> focus_channel;
    ofParameter<int> focus_value[2];
    
    ofParameter<bool> bTakeFromHearts;
    string GetLetter(int value)
    {
        value += 1; //so that int 0 gives us first letter i.e. A
        char letter = (char) ('A' - 1 + value);
        return ofToString(letter);
    }
};

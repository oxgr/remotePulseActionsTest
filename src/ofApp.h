#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxBiquadFilter.h"



#include "customDefines.h"
#include "sensorHandler"
#include "oscNetwork.h"
#include "ofxDmx.h"
#include "oneHeart.h"

#include "ofxTiming.h"

class ofApp : public ofBaseApp{
	public:
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
    
    ofxPanel gui_main;
    ofParameter<bool> bShowGui;
    ofParameter<bool> bDebug;
//    bool bShowGui;
    
    void saveGui();
    void checkGui();
    float logTimer;
    
    //---IP
    string myIP;

    string MX_IP, US_IP;
    string broadCastIP;
    
    int myID;
    vector<string> getLocalIPs();
    
    //----serial button
    sensorHandler hands_object;
    
    //---OSC 
    ofxPanel gui_osc;
    oscNetwork osc_object;
    
    //runtime
    void drawRuntime(int _x, int _y);
    string runtime_str;
    
    vector<oneHeart> allHearts;
    
    DelayTimer test_delayTimer;
    
    //--dmx
    ofxDmx dmx;
    string dmxDeviceString;
    vector<uint8_t> dmxValues;
    ofParameter<bool> bEnableDMX;
    void drawDmxBar(int _x, int _y, int _groupSize, int _devices);
    void renderDMX();
    
    ofParameter<int> meTestBPM;
     ofParameter<int> otherTestBPM;
    
    ofParameter<float> beat2Offset;
};

//
//
//
//  Created by Caroline Record on 4/10/19.
//
//
#include "ofMain.h"
#include "ofxJSON.h"
#include "ofxGui.h"

class socketHandler{

public:
    
    static ofEvent<ofxJSONElement> messageReceived;
    vector<string> tempToSend;
    
//    ofParameter<bool> showText;
//    ofParameter<int> textPosX;
//    ofParameter<int> textPosY;
    ofParameter<int> itvlSecCheckWeb;
    ofParameter<int> itvlSecCheckSoc;
    ofParameter<bool> hidePing;
    ofParameter<string> isConnected;
    ofParameter<string> isConnectedWeb;
    //ofParameter<bool> isAlive;
    ofParameter<string> textField;
//    ofxButton triggerSend;
    ofParameter<bool> triggerSend;
    //ofxButton clearDB;
//    ofxPanel gui_socket;
ofParameterGroup gui_socket;
    
    vector<string> recentMessages;
    //string to send
    // websocket methods
    //bool isAliveInit;
    //bool isAliveOld;
    bool stateChangedExternal;
    bool isConn;
    bool isConnWeb;
    
    int autoTimerWeb;
    int autoTimerSoc;
    
   // int intervalSec;
    
    deque<ofxJSONElement> messages;
    ofxJSONElement tempMessage;
    string computerID;
    //threadedPing pingTest;
    string roger = "roger";
    
    // auto refresh connection if lost the ability to receive messages
    bool bNewitemSent;
    int startItemWait;
    string lastItem;
    bool isLastCall;
    
    void setup();
    void update();
    void draw(int _x, int _y);
    string convertTime(string num);
    void triggerSendPressed(bool & triggerSend);
    void setupSockets();
    void checkInternet();
    
    
    void appExit();

    
    void checkDeque();
    
    bool bReconnectFinder;
    
    // http
    
    void sendToDBPost(string messages);
    void sendToDBPut(string messages);
    ofxJSONElement getDatabase();
    ofxJSONElement oldDb; 
    string theHttpAddress;
    int findID();
    void checkDatabase();
    
    string dbId;
    
    

private:

};


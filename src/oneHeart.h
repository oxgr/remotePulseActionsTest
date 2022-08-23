//
//  oneHeart.h
//  remotePulse
//
//  Created by Stephan Schulz on 2019-11-12.
//

#ifndef oneHeart_h
#define oneHeart_h

//#include "ofxTiming.h"

#include "ofxGui.h"
#include "ofxDropdown.h"

enum MIX_STATES{
    MIX_AB = 0,
    MIX_BA = 1,
    MIX_AA = 2,
    MIX_BB = 3
};

class fixture{
    
private:
    
public:
    int myID;
    string label;
    string myHeartLabel;
    
    ofParameterGroup group_fixture;
    ofParameter<int> fixtureIndex; // = -1;
    
    ofParameter<float> untouch_pan = 10;
    ofParameter<float> untouch_tilt = 45;
    ofParameter<float> untouch_dimmer = 10;
    
    ofParameter<float> touch_pan = 0;
    ofParameter<float> touch_tilt = 55;
    ofParameter<float> touch_dimmer;
    
    ofParameter<float> dimmerEasing;
    ofParameter<float> angleEasing;
    
    float dst_pan;
    float cur_pan;
    
    float dst_tilt;
    float cur_tilt;
    
    float cur_dimmer;
    float dst_dimmer;
    
    void setup(int _i, string _label, string _heartLabel){
        myID = _i;
        label = _label;
        myHeartLabel = _heartLabel;
        
        group_fixture.setName("fixture "+label);
        group_fixture.add(fixtureIndex.set("fixtureIndex",-1,0,1));
        
        group_fixture.add(untouch_pan.set("untouch pan",0,-270,270));
        group_fixture.add(untouch_tilt.set("untouch tilt",0,-135,135));
        group_fixture.add(touch_pan.set("touch pan",0,-270,270));
        group_fixture.add(touch_tilt.set("touch tilt",0,-135,135));
        
        group_fixture.add(untouch_dimmer.set("untouch dimmer",0,0,255));
        group_fixture.add(touch_dimmer.set("touch dimmer",127,0,255));
        
        group_fixture.add(dimmerEasing.set("dimmerEasing",0.2,0,1));
        group_fixture.add(angleEasing.set("angleEasing",0.1,0.001,1));
        cur_dimmer = 0;
    }
    
    void setTouch(bool _useLight, float _value){
        
        if(fixtureIndex == -1 || _useLight == false) return;
        ofLog()<<myHeartLabel<<" setTouch() fixture "<<label;
        
        dst_pan = touch_pan;
        dst_tilt = touch_tilt;
        dst_dimmer = touch_dimmer;
    }
//    void setTouch(bool _useLight, fixture & otherFixture){
//        
//        if(fixtureIndex == -1 || _useLight == false) return;
//        ofLog()<<myHeartLabel<<" setTouch() fixture "<<label;
//        
//        dst_pan = (touch_pan + otherFixture.touch_pan) / 2.0;
//        dst_tilt = (touch_tilt + otherFixture.touch_pan) / 2.0;
//        dst_dimmer = (touch_dimmer + otherFixture.touch_pan) / 2.0;
//    }
    
    void setUnTouch(bool _useLight, float _value){
       
        if(fixtureIndex == -1 || _useLight == false) return;
        ofLog()<<myHeartLabel<<" setUnTouch() fixture "<<label;
        
        dst_pan = _value; //untouch_pan;
        dst_tilt = _value; //untouch_tilt;
        dst_dimmer = _value; //untouch_dimmer;
    }
    
    void setDimmer(float _value){
        if(fixtureIndex == -1) return;
       
        if(cur_dimmer != _value && dst_dimmer != _value){
            ofLog()<<myHeartLabel<<" setDimmer() fixture "<<label<<" _value "<<_value;
            dst_dimmer = _value;
            cur_dimmer = _value;
        }
    }
    
    void update(){
        if(fixtureIndex == -1) return;
        
        cur_dimmer = dst_dimmer - ((dst_dimmer - cur_dimmer) * dimmerEasing);
        
        cur_pan = dst_pan - ((dst_pan - cur_pan) * angleEasing);
        cur_pan = int(cur_pan*100 + 0.5) / 100.0;
        cur_tilt = dst_tilt - ((dst_tilt - cur_tilt) * angleEasing);
        cur_tilt = int(cur_tilt*100 + 0.5) / 100.0; //reduce digita after comma to 2
    }
    
    string getInfo(){
        stringstream ss;
        
        ss<<"cur_pan "<<label<<": "<<cur_pan<<" -> "<<dst_pan<<endl;
        ss<<"cur_tilt "<<label<<": "<<cur_tilt<<" -> "<<dst_tilt<<endl;
        ss<<"cur_dimmer "<<label<<": "<<cur_dimmer<<" -> "<<dst_dimmer<<endl;
        
        return ss.str();
    }
};

class oneHeart {
private:

public:
    
    
    vector<fixture> myFixtures;
    string myLabel;
    int myIndex;
    
    ofxPanel gui_heart;
    
    unique_ptr<ofxIntDropdown> mixDropdown;
    ofParameter<int> pulseMix;
    
    ofParameter<bool> bUseSound;
    ofTrueTypeFont    arial;
    //BPM
    int bpm;
    int old_bpm;
    bool haveNewBPM;
    
    float bpmInSeconds;
    float old_bpmInSeconds;
    float bpmTimer;
    int bpmCounter;
    int minBpmCounter;
    
    int stage;
    
    int dmx_level_0, dmx_level_1;
    
    std::vector<string> * serialSendBuffer;
    string old_sendMessage;
    
    float lastTimer;
    float lastDuration;
    
    //__BPM
    //    DelayTimer bpm_delayTimer;
    LerpTimer bpm_lerpTimer;
    
    bool isTouched;
    bool old_isTouched;
    
    ofSoundPlayer  beatPlayer1;
    ofSoundPlayer  beatPlayer2;
    
    bool bStart2ndBeat;
    
    int firstMinBrightness;
    int firstMaxBrightness;
    int secondMinBrightness;
    int secondMaxBrightness;
    int touchBrightness;
    int unTouchBrightness;
    
    float firstBeatOnDur;
    float firstPause;
    float secondBeatOnDur;
    
    float firstVolume;
    float seconVolume;
    
    ofParameter<bool> bUseLights;
    
    bool lightViaDmx;
    bool lightViaSerial;
    
    int channelValues[513];
    
    
    void setup(string _label, int _index, std::vector<string> & _serialTxBuffer){
        
        myLabel = _label;
        myIndex = _index;
        
        serialSendBuffer = &_serialTxBuffer;
        
        lightViaDmx = false;
        lightViaSerial = false;
        arial.load("Arial.ttf", 24, true, true);
        
        bpm = 50;
        bpmInSeconds = 60.0 / bpm;
        
        
        
        beatPlayer1.load("sounds/S1.wav");
        beatPlayer2.load("sounds/S2.wav");
        beatPlayer1.setVolume(0.5f);
        beatPlayer2.setVolume(0.5f);
        beatPlayer1.setMultiPlay(false);
        beatPlayer2.setMultiPlay(false);
        
        
        myFixtures.resize(2);
        myFixtures[0].setup(0,"top",myLabel);
        myFixtures[1].setup(1,"bottom",myLabel);
        
        gui_heart.setup();
        gui_heart.setName("heart "+myLabel);
        gui_heart.setHeaderBackgroundColor(ofColor(255,0,0));
        gui_heart.add(bUseLights.set("useLights",false));
        gui_heart.add(bUseSound.set("useAudio",false));
        
        pulseMix.setName("pulse mix");
        mixDropdown = make_unique<ofxIntDropdown>(pulseMix);
        mixDropdown->enableCollapseOnSelection();
        mixDropdown->disableMultipleSelection();
        mixDropdown->add(0, "A B");
        mixDropdown->add(1, "B A");
        mixDropdown->add(2, "A A");
        mixDropdown->add(3, "B B");  
        gui_heart.add(mixDropdown.get());
        
        gui_heart.add(myFixtures[0].group_fixture);
        gui_heart.add(myFixtures[1].group_fixture);
        
        gui_heart.loadFromFile("GUIs/gui_heart_"+myLabel+".xml");
        
//        pulseMix = MIX_AB;
    }
    
    void setVolume(float _vol0, float _vol1){
        //        ofLog()<<"setVolume "<<_vol0<<" , "<<_vol1;
        firstVolume = _vol0;
        seconVolume = _vol1;
        beatPlayer1.setVolume(_vol0);
        beatPlayer2.setVolume(_vol1);
    }
    
    void setBPM(int _bpm){
        bpm = _bpm;
        ofLog()<<myLabel<<" setBPM() bpm "<<bpm<<" wait a few pulse before play "<<bpmCounter;
        bpmCounter++;
        
        lastTimer = ofGetElapsedTimef();
    }
    
    void setTouch(int _touch){
        isTouched = _touch;
        ofLog()<<myLabel<<" setTouch() _touch "<<isTouched;
    }
    
    void update(){ //bool _lightViaDmx, bool _lightViaSerial){ //}, float _beatPlayer2Offset){ //}, bool _touched){
        
        //        if(bUseLights == false) return;
        
        lastDuration = ofGetElapsedTimef() - lastTimer;
        
        if(isTouched == true){
            //touch
            
            if(old_isTouched != isTouched){
                old_isTouched = isTouched;
                stage = 0;
                bpmCounter = 0;
                
                if(pulseMix == MIX_AB){
                    myFixtures[0].setTouch(bUseLights);
                    myFixtures[1].setTouch(bUseLights);
                } else if(pulseMix == MIX_AA || pulseMix == MIX_BB){
                    myFixtures[0].setTouch(bUseLights);
                    myFixtures[1].setTouch(bUseLights);
                }
            } 
            
            if(stage == 0){
                
                if(bpmCounter >= minBpmCounter){
                    stage++;
                }
                
            }else if(stage == 1 && ofGetElapsedTimef() - bpmTimer >= bpmInSeconds){
                bpmTimer = ofGetElapsedTimef();
                
                if(bpm != -1 && old_bpm != bpm){
                    bpm = ofClamp(bpm,50,200);
                    old_bpm = bpm;
                    bpmCounter++;
                    bpmInSeconds = 60.0 / float(bpm);
                    
                    bpm_lerpTimer.setDuration(bpmInSeconds);
                    
                    ofLog()<<"bpmInSeconds "<<bpmInSeconds;
                    //            bpm_delayTimer.setPeriod(bpmInSeconds);
                    //            bpm_lerpTimer.setDuration(bpmInSeconds);
                    haveNewBPM = true;
                }
                
                //                ofLog()<<"tick() "<<bpm<<" , "<<bpmInSeconds;
                
                bStart2ndBeat = true;
                if(beatPlayer1.isPlaying() == false && bUseSound == true) beatPlayer1.play();
                //            ofLog()<<ofGetElapsedTimef()<<" beatPlayer1.play()";
                
                bpm_lerpTimer.setToValue(0);
                bpm_lerpTimer.lerpToValue(1);
            }
            
            //            if(beat1Channel != beat2Channel){
            //    ofLog()<<"bpm_lerpTimer.getProgress() "<<bpm_lerpTimer.getProgress()<<" bStart2ndBeat "<<bStart2ndBeat;
            if(bpm_lerpTimer.getProgress() >= firstPause && bStart2ndBeat == true){
                if(beatPlayer2.isPlaying() == false && bUseSound == true){
                    beatPlayer2.play();
                    //                ofLog()<<ofGetElapsedTimef()<<" beatPlayer2.play()";
                    bStart2ndBeat = false;
                }
            }
            //            }else{
            //                if(bpm_lerpTimer.getProgress() > (firstBeatOnDur+firstPause) && bStart2ndBeat == true){
            //                    if(beatPlayer2.isPlaying() == false && bUseSound == true){
            //                        beatPlayer2.play();
            //                        //                ofLog()<<ofGetElapsedTimef()<<" beatPlayer2.play()";
            //                        bStart2ndBeat = false;
            //                    }
            //                }
            //            }
            //    bpm_lerpTimer.lerpToValue(1);
            
            
            
            
            
            if(bpmCounter >= minBpmCounter){
                
                //                    setLevel(0, beat1Channel,touchBrightness);
                //                    setLevel(1, beat2Channel,touchBrightness);
                
                //                        if(beat1Channel != beat2Channel){
                //                            setLevel(beat1Channel,0);
                //                            setLevel(beat2Channel,touchBrightness);
                //                        }else{
                //                            setLevel(beat1Channel,touchBrightness);
                //                            setLevel(beat2Channel,touchBrightness);
                //                        }
                
                //                }else{
                
                
                if(pulseMix == MIX_AB){
                    //MARK: pulseMix == MIX_AB
                    //firstBeatOnDur = 0.6
                    
                    if(bpm_lerpTimer.getProgress() < firstBeatOnDur){
                        //jump to max brightness and then fade down to min for first beat
                        myFixtures[0].setDimmer(ofMap(bpm_lerpTimer.getProgress(), 0, firstBeatOnDur, firstMaxBrightness, firstMinBrightness,true));
                    }else{
                        //brightness at min for first beat
                        myFixtures[0].setDimmer(firstMinBrightness);
                    }
                    
                    //second beat
                    if(bpm_lerpTimer.getProgress() >= firstPause){ //} && bpm_lerpTimer.getProgress() < firstBeatOnDur){
                        myFixtures[1].setDimmer(ofMap(bpm_lerpTimer.getProgress(), firstPause, secondBeatOnDur, secondMaxBrightness, secondMinBrightness,true));
                    }else{
                        myFixtures[1].setDimmer(secondMinBrightness);
                    }
                    
                } else if(pulseMix == MIX_AA || pulseMix == MIX_BB){
                    //MARK: pulseMix == MIX_AA || pulseMix == MIX_BB
                    
                    if(bpm_lerpTimer.getProgress() < firstBeatOnDur){
                        //jump to max brightness and then fade down to min for first beat
                        myFixtures[0].setDimmer(ofMap(bpm_lerpTimer.getProgress(), 0, firstBeatOnDur, firstMaxBrightness, firstMinBrightness,true));
                        myFixtures[1].setDimmer(ofMap(bpm_lerpTimer.getProgress(), 0, firstBeatOnDur, firstMaxBrightness, firstMinBrightness,true));
                    }else if(bpm_lerpTimer.getProgress() >= firstBeatOnDur && bpm_lerpTimer.getProgress() < firstPause) {
                        myFixtures[0].setDimmer(firstMinBrightness);
                        myFixtures[1].setDimmer(firstMinBrightness);
                    }else if(bpm_lerpTimer.getProgress() >= firstPause && bpm_lerpTimer.getProgress() < secondBeatOnDur){
                        myFixtures[0].setDimmer(ofMap(bpm_lerpTimer.getProgress(), firstPause, secondBeatOnDur, secondMaxBrightness, secondMinBrightness,true));
                        myFixtures[1].setDimmer(ofMap(bpm_lerpTimer.getProgress(), firstPause, secondBeatOnDur, secondMaxBrightness, secondMinBrightness,true));
                    }else{
                        myFixtures[0].setDimmer(secondMinBrightness);
                        myFixtures[1].setDimmer(secondMinBrightness);
                    }
                }
                //                    //first beat
                //                    if(beat1Channel != beat2Channel){
                //                        //                        dmx_level_0 = 0;
                //                        //firstBeatOnDur = 0.6
                //                        if(bpm_lerpTimer.getProgress() < firstBeatOnDur){
                //                            dmx_level_0 = ofMap(bpm_lerpTimer.getProgress(), 0, firstBeatOnDur, firstMaxBrightness, firstMinBrightness,true);
                //                            setLevel(0,beat1Channel,dmx_level_0);
                //                        }else{
                //                            setLevel(0,beat1Channel,firstMinBrightness);
                //                        }
                //                        
                //                        //second beat
                //                        dmx_level_1 = 0;
                //                        if(bpm_lerpTimer.getProgress() >= firstPause){ //} && bpm_lerpTimer.getProgress() < firstBeatOnDur){
                //                            dmx_level_1 = ofMap(bpm_lerpTimer.getProgress(), firstPause, secondBeatOnDur, secondMaxBrightness, secondMinBrightness,true);
                //                            setLevel(1,beat2Channel,dmx_level_1);
                //                            
                //                            //                ofLog()<<myLabel<<" temp_level_0 "<<temp_level_0<<" temp_level_1 "<<temp_level_1<<" bpm_lerpTimer.getProgress() "<<bpm_lerpTimer.getProgress();
                //                        }else{
                //                            setLevel(1,beat2Channel,secondMinBrightness);
                //                        }
                //                        
                //                    }else{
                //                        //no dual channel bulb
                //                        //                        dmx_level_0 = 0;
                //                        //firstBeatOnDur = 0.6
                //                        //                        float stageOne = firstBeatOnDur;
                //                        //                        float stageTwo = stageOne+firstPause;
                //                        //                        float stageThree = stageTwo+secondBeatOnDur;
                //                        if(bpm_lerpTimer.getProgress() < firstBeatOnDur){
                //                            
                //                            dmx_level_0 = ofMap(bpm_lerpTimer.getProgress(), 0, firstBeatOnDur, firstMaxBrightness, firstMinBrightness,true);
                //                            setLevel(0,beat1Channel,dmx_level_0);
                //                            
                //                        }else if(bpm_lerpTimer.getProgress() >= firstBeatOnDur && bpm_lerpTimer.getProgress() < firstPause) {
                //                            
                //                            setLevel(0,beat1Channel,firstMinBrightness);
                //                            
                //                        }else if(bpm_lerpTimer.getProgress() >= firstPause && bpm_lerpTimer.getProgress() < secondBeatOnDur){
                //                            
                //                            dmx_level_1 = ofMap(bpm_lerpTimer.getProgress(), firstPause, secondBeatOnDur, secondMaxBrightness, secondMinBrightness,true);
                //                            setLevel(0,beat1Channel,dmx_level_1);
                //                            
                //                        }else{
                //                            setLevel(0,beat1Channel,firstMinBrightness);
                //                        }
                //                        
                //                    }//end else  if(beat1Channel != beat2Channel)
            }//end if(bpmCounter < minBpmCounter
            
        }else{
            //untouch
            if(old_isTouched != isTouched){
                old_isTouched = isTouched;
                beatPlayer1.stop();
                beatPlayer2.stop();
                
            }
            bpmCounter = 0;
            //
            //            setLevel(0,beat1Channel,unTouchBrightness);
            //            setLevel(1,beat2Channel,unTouchBrightness);
            //            dst_tilt_1 = unTouch_tilt_value1;
            //            dst_tilt_2 = unTouch_tilt_value2;
            myFixtures[0].setUnTouch(bUseLights);
            myFixtures[1].setUnTouch(bUseLights);
        }
        
        myFixtures[0].update();
        myFixtures[1].update();  
        
        
        //        ofLog()<<"dst_tilt "<<dst_tilt_1<<" , "<<dst_tilt_2;
        //        ofLog()<<"cur_tilt "<<cur_tilt[0]<<" , "<<cur_tilt[1];
    }
    
    
    void setLevel(int _index, int _channel, int _value){
        
        /*
         //        if(bUseLights == false) return;
         //        if(lightViaDmx == true){
         //            float curLevel = curDimmer[_index];
         //            float newLevel = curLevel + ((_value - curLevel) * dimmerEasing);
         ////            dmx->setLevel(_channel,int(newLevel));
         //            curDimmer[_index] = newLevel;
         ////            dimmersValue[_channel] = _value;
         //        }
         if(lightViaSerial){
         //             ofLog()<<"setLevel via serial chan "<<_channel<<" value "<<_value;
         //            string _label;
         string msg;
         msg = "["+ofToString(_value);
         if(_channel == 1) msg += "a]";
         else if(_channel == 2) msg += "b]";
         else if(_channel == 3) msg += "c]";
         else if(_channel == 4) msg += "d]";
         
         //            string msg = "["+ofToString(_value) +_label+"]"; //as per teensy handleSettingMsg() function in serial_com
         //            if(_channel >= 1 && _channel<= 4 && old_sendMessage != msg){
         if(_channel >= 1 && _channel<= 4 && channelValues[_channel] != _value){
         old_sendMessage = msg;
         serialSendBuffer->push_back(msg);
         }
         
         }
         channelValues[_channel] = _value;
         */
    }
    void draw(int _x, int _y){
        
        ofPushMatrix();
        ofTranslate(_x,_y);
        
        int tempY = 0;
        
        ofSetColor(0);
        ofFill();
        ofDrawRectangle(0,0,100,50);
        
        ofSetColor(255,255);
        ofNoFill();
        ofSetLineWidth(2);
        ofDrawRectangle(0,0,100,50);
        if(isTouched == false){
            ofDrawLine(0,0,100,50);
            ofDrawLine(100,0,0,50);
        }
        
        
        ofFill();
        ofSetColor(255, dmx_level_0);
        ofDrawCircle(25,25,20);
        ofNoFill();
        ofSetColor(255);
        ofDrawCircle(25,25,20);
        
        ofFill();
        ofSetColor(255, dmx_level_1);
        ofDrawCircle(75,25,20);
        ofNoFill();
        ofSetColor(255);
        ofDrawCircle(75,25,20);
        
        ofSetLineWidth(1);
        
        tempY+=60;
        
        ofSetColor(255);
        
        //        ofDrawBitmapString("meBPM "+ofToString(meBPM),0,tempY+=15);
        arial.drawString(myLabel+": "+ofToString(bpm), 0, tempY+=30);
        
        stringstream msg;
        
        msg << "touch "<<isTouched<<endl;
        msg << "bpm "<<bpm<<endl;
        msg << "bpmSec "<<bpmInSeconds<<endl;
        msg << "bpmCounter "<<bpmCounter<<endl;
        msg << "last time "<<ofToString(lastDuration,1)<<endl;
        msg << "beat1: " << beatPlayer1.isPlaying();
        msg << " beat2: " << beatPlayer2.isPlaying() << endl;
        
        msg << "bpm_lerpTimer:"
        << " from " << bpm_lerpTimer.getStartValue()
        << " to " << bpm_lerpTimer.getTargetValue()
        << " : " << int(bpm_lerpTimer.getValue())
        << " (" << int(100*bpm_lerpTimer.getProgress()) << "%)" << endl;
        
        msg << "pulseMix: " << pulseMix << "stage "<<stage<<endl;
        msg << myFixtures[0].getInfo();
        msg << myFixtures[1].getInfo();
        ofDrawBitmapString(msg.str(),0,tempY+=15);
        
        
        ofPopMatrix();
    }
    
    
};
#endif /* oneHeart_h */

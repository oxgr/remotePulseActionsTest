//
//  oneHeart.h
//  remotePulse
//
//  Created by Stephan Schulz on 2019-11-12.
//

#ifndef oneHeart_h
#define oneHeart_h

//#include "ofxTiming.h"

class oneHeart {
private:

    ofxDmx * dmx;

public:
    
    string myLabel;
    int myIndex;
    
    ofParameterGroup group_heart;
    
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
    
    int dmxStartChannel;
    
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
    ofParameter<int> beat1Channel;
    ofParameter<int> beat2Channel;
    ofParameter<float> dimmerEasing;
    int curDimmer[2];
    
    bool lightViaDmx;
    bool lightViaSerial;
    
    int channelValues[513];
//    int dimmersValue[2];
    
    float dst_tilt_1;
    float dst_tilt_2;
    
    float cur_tilt[2] = {45,45};
    
    ofParameter<float> unTouch_tilt_value1;
    ofParameter<float> unTouch_tilt_value2;
    ofParameter<float> touch_tilt_value1;
    ofParameter<float> touch_tilt_value2;
    ofParameter<float> tiltEasing;
    
    void setup(string _label, int _index, int _dmxStart, ofxDmx & dmx, std::vector<string> & _serialTxBuffer){
        
        this->dmx = &dmx;
        myLabel = _label;
        myIndex = _index;
        
        serialSendBuffer = &_serialTxBuffer;
            
        //        dmxStartChannel = _dmxStart;
        
        lightViaDmx = false;
        lightViaSerial = false;
        arial.load("Arial.ttf", 24, true, true);
        
        bpm = 50;
        bpmInSeconds = 60.0 / bpm;
        //        bpm_delayTimer.setPeriod(1); // to get it started
//        dimmersValue[0] = 0;
//        dimmersValue[1] = 0;  
        
        
        //        beatPlayer1.load("sounds/oneBeat.wav");
        //        beatPlayer2.load("sounds/onebeatPlayer2.wav");
        //        beatPlayer1.load("sounds/oneBeat3.wav");
        //        beatPlayer2.load("sounds/oneBeat4.wav");
        beatPlayer1.load("sounds/S1.wav");
        beatPlayer2.load("sounds/S2.wav");
        beatPlayer1.setVolume(0.5f);
        beatPlayer2.setVolume(0.5f);
        beatPlayer1.setMultiPlay(false);
        beatPlayer2.setMultiPlay(false);
        
        group_heart.setName("heart "+myLabel);
        group_heart.add(bUseLights.set("useLights",false));
        group_heart.add(beat1Channel.set("dmxChan beat1",1,1,512));
        group_heart.add(beat2Channel.set("dmxChan beat2",2,1,512));
        group_heart.add(bUseSound.set("useAudio",false));
        
        group_heart.add(unTouch_tilt_value1.set("untouch tilt 1",45,-135,135));
        group_heart.add(unTouch_tilt_value2.set("untouch tilt 2",45,-135,135));
        group_heart.add(touch_tilt_value1.set("touch tilt 1",55,-135,135));
        group_heart.add(touch_tilt_value2.set("touch tilt 2",55,-135,135));
        group_heart.add(tiltEasing.set("tiltEasing",0.1,0.001,1));
        group_heart.add(dimmerEasing.set("dimmerEasing",0.1,0.001,1));
        
        curDimmer[0] = 0;
        curDimmer[1] = 0;
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
        
//        lightViaDmx = _lightViaDmx;
//        lightViaSerial = _lightViaSerial;
        
        //        isTouched = _touched;
        
        lastDuration = ofGetElapsedTimef() - lastTimer;
        
        if(isTouched == true){
            
            if(old_isTouched != isTouched){
                old_isTouched = isTouched;
                stage = 0;
                bpmCounter = 0;
                
                dst_tilt_1 = touch_tilt_value1;
                dst_tilt_2 = touch_tilt_value2;
                //                beatPlayer1.stop();
                //                beatPlayer2.stop();
                
                //                bpm = 75;
                //                bpmInSeconds = 60.0 / float(bpm);
                //                bpm_delayTimer.setPeriod(bpmInSeconds);
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
            
            

            if(lightViaDmx || lightViaSerial){
                
                if(bpmCounter < minBpmCounter){
                    
                    setLevel(0, beat1Channel,touchBrightness);
                    setLevel(1, beat2Channel,touchBrightness);
                    
//                        if(beat1Channel != beat2Channel){
//                            setLevel(beat1Channel,0);
//                            setLevel(beat2Channel,touchBrightness);
//                        }else{
//                            setLevel(beat1Channel,touchBrightness);
//                            setLevel(beat2Channel,touchBrightness);
//                        }
                    
                }else{
                    
                    //first beat
                    if(beat1Channel != beat2Channel){
                        //                        dmx_level_0 = 0;
                        //firstBeatOnDur = 0.6
                        if(bpm_lerpTimer.getProgress() < firstBeatOnDur){
                            dmx_level_0 = ofMap(bpm_lerpTimer.getProgress(), 0, firstBeatOnDur, firstMaxBrightness, firstMinBrightness,true);
                            setLevel(0,beat1Channel,dmx_level_0);
                        }else{
                            setLevel(0,beat1Channel,firstMinBrightness);
                        }
                        
                        //second beat
                        dmx_level_1 = 0;
                        if(bpm_lerpTimer.getProgress() >= firstPause){ //} && bpm_lerpTimer.getProgress() < firstBeatOnDur){
                            dmx_level_1 = ofMap(bpm_lerpTimer.getProgress(), firstPause, secondBeatOnDur, secondMaxBrightness, secondMinBrightness,true);
                            setLevel(1,beat2Channel,dmx_level_1);
                            
                            //                ofLog()<<myLabel<<" temp_level_0 "<<temp_level_0<<" temp_level_1 "<<temp_level_1<<" bpm_lerpTimer.getProgress() "<<bpm_lerpTimer.getProgress();
                        }else{
                            setLevel(1,beat2Channel,secondMinBrightness);
                        }
                        
                    }else{
                        //no dual channel bulb
                        //                        dmx_level_0 = 0;
                        //firstBeatOnDur = 0.6
                        //                        float stageOne = firstBeatOnDur;
                        //                        float stageTwo = stageOne+firstPause;
                        //                        float stageThree = stageTwo+secondBeatOnDur;
                        if(bpm_lerpTimer.getProgress() < firstBeatOnDur){
                            
                            dmx_level_0 = ofMap(bpm_lerpTimer.getProgress(), 0, firstBeatOnDur, firstMaxBrightness, firstMinBrightness,true);
                            setLevel(0,beat1Channel,dmx_level_0);
                            
                        }else if(bpm_lerpTimer.getProgress() >= firstBeatOnDur && bpm_lerpTimer.getProgress() < firstPause) {
                            
                            setLevel(0,beat1Channel,firstMinBrightness);
                            
                        }else if(bpm_lerpTimer.getProgress() >= firstPause && bpm_lerpTimer.getProgress() < secondBeatOnDur){
                            
                            dmx_level_1 = ofMap(bpm_lerpTimer.getProgress(), firstPause, secondBeatOnDur, secondMaxBrightness, secondMinBrightness,true);
                            setLevel(0,beat1Channel,dmx_level_1);
                            
                        }else{
                            setLevel(0,beat1Channel,firstMinBrightness);
                        }
                        
                    }//end else  if(beat1Channel != beat2Channel)
                }//end if(bpmCounter < minBpmCounter
            }
        }else{
            
            if(old_isTouched != isTouched){
                old_isTouched = isTouched;
                beatPlayer1.stop();
                beatPlayer2.stop();
                
            }
            bpmCounter = 0;

            setLevel(0,beat1Channel,unTouchBrightness);
            setLevel(1,beat2Channel,unTouchBrightness);
            dst_tilt_1 = unTouch_tilt_value1;
            dst_tilt_2 = unTouch_tilt_value2;

        }
        
        cur_tilt[0] = cur_tilt[0] + ((dst_tilt_1 - cur_tilt[0]) * tiltEasing);
        cur_tilt[1] = cur_tilt[1] + ((dst_tilt_2 - cur_tilt[1]) * tiltEasing);
        cur_tilt[0] = int(cur_tilt[0]*100 + 0.5) / 100.0;
        cur_tilt[1] = int(cur_tilt[1]*100 + 0.5) / 100.0;
        
//        ofLog()<<"dst_tilt "<<dst_tilt_1<<" , "<<dst_tilt_2;
//        ofLog()<<"cur_tilt "<<cur_tilt[0]<<" , "<<cur_tilt[1];
    }
    
    
    void setLevel(int _index, int _channel, int _value){
        
        if(bUseLights == false) return;
        if(lightViaDmx == true){
            float curLevel = curDimmer[_index];
            float newLevel = curLevel + ((_value - curLevel) * dimmerEasing);
//            dmx->setLevel(_channel,int(newLevel));
            curDimmer[_index] = newLevel;
//            dimmersValue[_channel] = _value;
        }
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
        
        msg << "lerpTimer:"
        << " from " << bpm_lerpTimer.getStartValue()
        << " to " << bpm_lerpTimer.getTargetValue()
        << " : " << int(bpm_lerpTimer.getValue())
        << " (" << int(100*bpm_lerpTimer.getProgress()) << "%)" << endl;
        msg << "tilt 1 " << cur_tilt[0] << "->" << dst_tilt_1<<endl;
        msg << "tilt 2 " << cur_tilt[1] << "->" << dst_tilt_2<<endl;
        ofDrawBitmapString(msg.str(),0,tempY+=15);
        
        
        ofPopMatrix();
    }
    
    
};
#endif /* oneHeart_h */

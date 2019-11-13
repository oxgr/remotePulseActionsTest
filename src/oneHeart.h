//
//  oneHeart.h
//  remotePulse
//
//  Created by Stephan Schulz on 2019-11-12.
//

#ifndef oneHeart_h
#define oneHeart_h

#include "ofxTiming.h"

class oneHeart {
private:
    ofxDmx * dmx;
public:
    
    string myLabel;
    int myIndex;
    
    ofParameter<bool> bUseSound;
    
    //BPM
    int bpm;
    int old_bpm;
    bool haveNewBPM;
    
    float bpmInSeconds;
    float old_bpmInSeconds;
    float bpmTimer;
    int bpmCounter;
    
    int stage;
    
    //__BPM
    //    DelayTimer bpm_delayTimer;
    LerpTimer bpm_lerpTimer;
    
    bool isTouched;
    bool old_isTouched;
    
    ofSoundPlayer  beatPlayer1;
    ofSoundPlayer  beatPlayer2;
    
    bool bStart2ndBeat;
    
    int dmxStartChannel;
    
    int firstMaxBrightness;
    int secondMaxBrightness;
    int touchBrightness;
    
    float firstVolume;
    float seconVolume;
    
    void setup(string _label, int _index, int _dmxStart, ofxDmx & dmx){
        
        this->dmx = &dmx;
        myLabel = _label;
        myIndex = _index;
        dmxStartChannel = _dmxStart;
        
        bpm = 50;
        bpmInSeconds = 60.0 / bpm;
        //        bpm_delayTimer.setPeriod(1); // to get it started
        
        
        
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
        
    }
    
    void setVolume(float _vol0, float _vol1){
        firstVolume = _vol0;
        seconVolume = _vol1;
        beatPlayer1.setVolume(_vol0);
        beatPlayer2.setVolume(_vol1);
    }
    
    void setBPM(int _bpm){
        bpm = _bpm;
        ofLog()<<myLabel<<"setBPM() bpm "<<bpm<<" wait a few pulse before play "<<bpmCounter;
        bpmCounter++;
    }
    
    void setTouch(int _touch){
        isTouched = _touch;
        ofLog()<<myLabel<<"setTouch() _touch "<<isTouched;
    }
    
    void update(bool _useDmx, float _beatPlayer2Offset, bool _touched){
        
        //        isTouched = _touched;
        
        if(isTouched == true){
            
            if(old_isTouched != isTouched){
                old_isTouched = isTouched;
                stage = 0;
                bpmCounter = 0;
                //                beatPlayer1.stop();
                //                beatPlayer2.stop();
                
                //                bpm = 75;
                //                bpmInSeconds = 60.0 / float(bpm);
                //                bpm_delayTimer.setPeriod(bpmInSeconds);
            }
            
            if(stage == 0){
                
                if(bpmCounter >= 2){
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
                
                //                if(old_bpmInSeconds != bpmInSeconds){
                //                    old_bpmInSeconds = bpmInSeconds;
                //                    ofLog()<<"BPM "<<bpm<<" in bpmInSeconds "<<bpmInSeconds;
                ////                    bpm_delayTimer.setPeriod(bpmInSeconds);
                //                   
                
                
                bpm_lerpTimer.setToValue(0);
                bpm_lerpTimer.lerpToValue(1);
            }
            
            
            //    ofLog()<<"bpm_lerpTimer.getProgress() "<<bpm_lerpTimer.getProgress()<<" bStart2ndBeat "<<bStart2ndBeat;
            if(bpm_lerpTimer.getProgress() > _beatPlayer2Offset && bStart2ndBeat == true){
                if(beatPlayer2.isPlaying() == false && bUseSound == true){
                    beatPlayer2.play();
                    //                ofLog()<<ofGetElapsedTimef()<<" beatPlayer2.play()";
                    bStart2ndBeat = false;
                }
            }
            
            //    bpm_lerpTimer.lerpToValue(1);
            
            
            
            if(_useDmx){
                
                if(bpmCounter < 3){
                      dmx->setLevel(0 + dmxStartChannel,0);
                      dmx->setLevel(1 + dmxStartChannel,touchBrightness);
                    
                }else{
                    
                    //first beat
                    int temp_level_0 = 0;
                    
                    if(bpm_lerpTimer.getProgress() < 0.6){
                        temp_level_0 = ofMap(bpm_lerpTimer.getProgress(), 0, 0.6, firstMaxBrightness, 0,true);
                        dmx->setLevel(0 + dmxStartChannel,temp_level_0);
                    }else{
                        dmx->setLevel(0 + dmxStartChannel,0);
                    }
                    
                    //second beat
                    int temp_level_1 = 0;
                    
                    
                    if(bpm_lerpTimer.getProgress() > _beatPlayer2Offset && bpm_lerpTimer.getProgress() < 0.6){
                        temp_level_1 = ofMap(bpm_lerpTimer.getProgress(), _beatPlayer2Offset, 0.6, secondMaxBrightness, 0,true);
                        dmx->setLevel(1 + dmxStartChannel,temp_level_1);
                        
                        //                ofLog()<<myLabel<<" temp_level_0 "<<temp_level_0<<" temp_level_1 "<<temp_level_1<<" bpm_lerpTimer.getProgress() "<<bpm_lerpTimer.getProgress();
                    }else{
                        dmx->setLevel(1 + dmxStartChannel,0);
                    }
                }//end if(bpmCounter < 3
            }
        }else{
            
            if(old_isTouched != isTouched){
                old_isTouched = isTouched;
                beatPlayer1.stop();
                beatPlayer2.stop();
                
            }
            bpmCounter = 0;
            
            dmx->setLevel(0 + dmxStartChannel,0);
            dmx->setLevel(1 + dmxStartChannel,0);
        }
        
    }
    void draw(int _x, int _y){
        
        ofPushMatrix();
        ofTranslate(_x,_y);
        
        int tempY = 0;
        ofSetColor(255);
        //        ofDrawBitmapString("meBPM "+ofToString(meBPM),0,tempY+=15);
        ofDrawBitmapString(myLabel,0,tempY+=15);
        ofDrawBitmapString("touch "+ofToString(isTouched),0,tempY+=15);
        ofDrawBitmapString("bpm "+ofToString(bpm),0,tempY+=15);
        ofDrawBitmapString("bpmSec "+ofToString(bpmInSeconds),0,tempY+=15);
        ofDrawBitmapString("bpmCounter "+ofToString(bpmCounter),0,tempY+=15);
        
        
        ofSetColor(255);
        stringstream msg;
        msg << "beatPlayer1.isPlaying: " << beatPlayer1.isPlaying() << endl;
        msg << "beatPlayer2.isPlaying: " << beatPlayer2.isPlaying() << endl;
        
        msg << "lerpTimer:"
        << " from " << bpm_lerpTimer.getStartValue()
        << " to " << bpm_lerpTimer.getTargetValue()
        << " : " << int(bpm_lerpTimer.getValue())
        << " (" << int(100*bpm_lerpTimer.getProgress()) << "%)" << endl;
        ofDrawBitmapString(msg.str(),0,tempY+=15);
        
        ofPopMatrix();
    }
    
    
};
#endif /* oneHeart_h */

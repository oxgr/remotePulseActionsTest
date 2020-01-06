//
//  versionNumbering.h
//  NDI-mic-sender
//
//  Created by Stephan Schulz on 2019-10-21.
//

#ifndef versionNumbering_h
#define versionNumbering_h
class versionNumbering{
private:
    
public:
    string commitVersion;
    
    
#ifdef TARGET_OSX
    bool generateVersionNumber(){
        
        string xcodeMsg = ofSystem("xcode-select -p");
        ofLog()<<"xcodeMsg "<<xcodeMsg<<"@";
        string commit;
        string versionDate;
        string versionCount;
        
        if( xcodeMsg != "" ){
            
            //check development computer for current git version
            commit = ofSystem("git rev-parse --short HEAD");
            ofStringReplace(commit,"\n","");
            cout<<"git rev-parse HEAD --commit "<<commit<<endl;
            
            versionCount = ofSystem("git rev-list HEAD --count");
            ofStringReplace(versionCount,"\n","");
            cout<<"git rev-list HEAD --count "<<versionCount<<endl;
            
            versionDate = ofSystem("git log -1 --date=short --pretty=format:%cd");
            //    string versionDate = ofSystem("git log -1 --date=short --pretty=format:%Y%m%d");
            //    string versionDate = ofSystem("git rev-list --format=format:'%ai' --max-count=1 `git rev-parse HEAD`");
            ofStringReplace(versionDate,"\n","");
            ofStringReplace(versionDate,"-","");
            ofLog()<<"versionDate "<<versionDate; //.substr(2);
            
            commitVersion = versionCount+" "+versionDate+" "+commit;
            
            ofLog()<<"commitVersion;["<<commitVersion<<"]";
            if(versionCount == "" || versionDate == "" ||  commit == ""){
                return false;
            }else{
                return true;
            }
        } else{
            //if computer is not the dev computer, and can not generate git version number
            //we just take the one present in the ofApp.h file
            //if none is present in ofApp.h and not git versioning present on PC, it will stay empty
            return false;
        }
    }
#else
    bool generateVersionNumber(){
        return false;   
    }
#endif
    
    
    string getVersionString(string _fileName){
        
        bool foundNumber = generateVersionNumber();
        ofLog()<<"foundNumber?:"<<foundNumber;
        
        if(foundNumber){
            
            //write the git version in to file
            string filePath = ofToDataPath("../../src/"+_fileName);
            
            ofBuffer inBuffer = ofBufferFromFile(filePath);
            string outputText = "";
            
            bool bSearching = true;
            
            if(inBuffer.size()) {
                for (ofBuffer::Line it = inBuffer.getLines().begin(), end = inBuffer.getLines().end(); it != end; ++it) {
                    
                    string line = *it;
                    
                    if(line.empty() == false) {
                        
                        if(bSearching == true){
                            
                            if(ofIsStringInString(line,"APP_VERSION")){
                                
                                bSearching = false;
                                outputText +="\n"; 
                                outputText += "#define APP_VERSION \""+commitVersion+"\"";
                                outputText +="\n"; 
                                
                            }else if(ofIsStringInString(line,"class ofApp")){
                                
                                bSearching = false;
                                outputText +="\n"; 
                                outputText += "#define APP_VERSION \""+commitVersion+"\"";
                                outputText +="\n"; 
                                outputText += line;
                                outputText +="\n"; 
                            } else{
                                outputText += line;
                                outputText +="\n"; 
                            }
                            
                        } else{
                            outputText += line;   
                        }
                        outputText +="\n"; 
                        
                        
                    }//end if(line.empty() == false)
                    
                }//end for
            }//end if(inBuffer.size())
            
            ofBuffer outBuffer;
            outBuffer.set(outputText);
            ofBufferToFile(filePath, outBuffer);
            
        }//end if( xcodeMsg != "" )
        else{
            commitVersion = "";
        }
        return commitVersion;
    }
};

#endif /* versionNumbering_h */

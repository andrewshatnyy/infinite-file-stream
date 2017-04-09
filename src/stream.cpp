#include <nan.h>
#include <string>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include "streaming-worker.h"

using namespace std;
class Streamer : public StreamingWorker {
  public:
    Streamer(Callback *data, Callback *complete, 
        Callback *error_callback, 
        v8::Local<v8::Object> & options) 
        : StreamingWorker(data, complete, error_callback){

        size = 512;

        if (options->IsObject() ) {
            v8::Local<v8::Value> size_ = options->Get(New<v8::String>("size").ToLocalChecked());
            if ( size_->IsNumber() ) {
                size = size_->NumberValue();
            }
        }

    }

    ~Streamer(){}
    //open file and create infinite loop            
    void openFile(const AsyncProgressWorker::ExecutionProgress& progress, string fileName){
        ifstream file(fileName); 
        if(!file){
            HandleErrorCallback();
        }
        looping(progress, file);
        
    }
    //infinite loop
    void looping(const AsyncProgressWorker::ExecutionProgress& progress, std::ifstream &file){
        //initial set up
        int actualSize = 0;
        int len = fileSize(file);
        char buff[size];
        
        while(len > 0){
            // setting up actual size for buffer
            actualSize = bufferSize(len, size);
            
            // reading from file 
            file.read(buff, actualSize);
            
            //check if all characters can be read
            if(!file){
                HandleErrorCallback();
                file.close();
            }

            buff[actualSize] = '\0';

            //send buffer to node
            Message tosend("value", buff);
            writeToNode(progress, tosend);
            
            //reset conditions
            if(len == 0){
                reset(len, file);
            }
        }
    }

    //return buffer size
    int bufferSize(int &len, int size){
        int actualSize = 0;
        if(len >= size){
            actualSize = size;
            len -= size;
        } else { 
            actualSize = len;
            len -= actualSize;
        };
        return actualSize;
    }
    
    //return size of file
    int fileSize(std::ifstream &file){
        file.seekg(0, file.end);
        int len = file.tellg();
        file.seekg(0, file.beg);
        return len;
    }

    void reset(int &len, std::ifstream &file){
        file.seekg(0, file.beg);
        len = fileSize(file);
    }

    void Execute (const AsyncProgressWorker::ExecutionProgress& progress) {
        string fileName;
        Message m = fromNode.read();
        fileName = m.data;
        openFile(progress, fileName);
    }
  private:
    int size;
};

StreamingWorker * create_worker(Callback *data
    , Callback *complete
    , Callback *error_callback, v8::Local<v8::Object> & options) {
 return new Streamer(data, complete, error_callback, options);
}

NODE_MODULE(accumulate, StreamWorkerWrapper::Init)
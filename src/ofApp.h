#pragma once

#include "ofMain.h"
#include "ofxHistogram.h"
#include "ofxEMD.h"
#include "ofxAssignment.h"

class ofApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    
    void scan_dir_imgs(ofDirectory dir);
    vector<double> getWeightsVector2(vector<vector<vector<float> > > hist, int numBins_);
    void saveImage();
    
    ofxHistogram histogram;
    ofxEMD emd;
    ofxAssignment solver;
    
    int nx, ny;
    int w, h;
    int displayH;
    int numBins;
    bool view;
    float aspect;
    
    ofImage target;
    vector<ofFile> imageFiles;
    vector<ofImage> candidates;
    vector<ofImage> targetCrops;
    vector<vector<vector<vector<float> > > > targetH;
    vector<vector<vector<vector<float> > > > candidateH;
    vector<vector<double> > costs;
    vector<int> assignment;
};

#include "ofApp.h"

const string allowed_ext[] = {"jpg", "png", "gif", "jpeg"};


//--------------------------------------------------------------
void ofApp::scan_dir_imgs(ofDirectory dir){
    ofDirectory new_dir;
    int size = dir.listDir();
    for (int i = 0; i < size; i++){
        if (dir.getFile(i).isDirectory()){
            new_dir = ofDirectory(dir.getFile(i).getAbsolutePath());
            new_dir.listDir();
            new_dir.sort();
            scan_dir_imgs(new_dir);
        } else if (std::find(std::begin(allowed_ext),
                             std::end(allowed_ext),
                             dir.getFile(i).getExtension()) != std::end(allowed_ext)) {
            imageFiles.push_back(dir.getFile(i));
        }
    }
}

//--------------------------------------------------------------
vector<double> ofApp::getWeightsVector2(vector<vector<vector<float> > > hist, int numBins_) {
    vector<double> weights;
    for (int ir=0; ir<numBins_; ir++) {
        for (int ig=0; ig<numBins_; ig++) {
            for (int ib=0; ib<numBins_; ib++) {
                int idx = ib + numBins_*ig + numBins_*numBins_*ir;
                weights.push_back((double)hist[ir][ig][ib]);
            }
        }
    }
    return weights;
}

//--------------------------------------------------------------
void ofApp::setup(){
    
    // change these according to whatever images you want to use
    // if you want to use the animals dataset (imageDir), instructions for downloading it can be found: https://github.com/genekogan/ofxTSNE
    string targetPath = "/Users/gene/bin/tsne/mosaics/mary_blair.jpg";
    string imageDir = "/Users/gene/Code/of_v0.9.0_osx_release/addons/ofxTSNE/example-images/bin/data/animals";
    numBins = 2;
    nx = 94;
    ny = 68;
    displayH = 64;
    
    // get images recursively from directory
    ofLog() << "Gathering images...";
    ofDirectory dir = ofDirectory(imageDir);
    scan_dir_imgs(dir);
    if (imageFiles.size() < nx * ny) {
        ofLog(OF_LOG_ERROR, "There are less images in the directory than the grid size requested (nx*ny="+ofToString((nx*ny))+"). Exiting to save you trouble...");
        ofExit(); // not enough images to fill the grid, so quitting
    }
    
    // get histograms from tiles of main image
    target.load(targetPath);
    float tw = target.getWidth() / nx;
    float th = target.getHeight() / ny;
    aspect = tw / th;
    for (int r=0; r<ny; r++) {
        for (int c=0; c<nx; c++) {
            ofImage roi;
            roi.setFromPixels(target.getPixels());
            float tx = c * tw;
            float ty = r * th;
            roi.crop(tx, ty, tw, th);
            targetH.push_back(histogram.getHistogram3d(roi, numBins));
            targetCrops.push_back(roi);
        }
    }
    
    // get histograms of all images
    ofLog() << "Getting histograms...";
    for(int i=0; i<nx*ny; i++) {
        if (i % 20 == 0) {
            ofLog() << " - get histogram for image "<<i<<" / "<<nx*ny<<" ("<<imageFiles.size()<<" in dir)";
        }
        
        // load image
        ofImage img;
        img.load(imageFiles[i]);
        
        // crop to aspect ratio
        if (img.getWidth() > aspect * img.getHeight()) {
            img.crop((img.getWidth() - aspect * img.getHeight()) * 0.5, 0, aspect * img.getHeight(), img.getHeight());
        }
        else if (aspect * img.getHeight() > img.getWidth()) {
            img.crop(0, (aspect * img.getHeight() - img.getWidth()) * 0.5, img.getWidth(), img.getWidth() / aspect);
        }
        img.resize(w, h);
        
        // get histogram
        candidates.push_back(img);
        candidateH.push_back(histogram.getHistogram3d(img, numBins));
    }
    
    // for getting EMD, feature vector
    int numTotal = numBins * numBins * numBins;
    vector<ofVec3f> colorBins;
    for (int ir=0; ir<numBins; ir++) {
        for (int ig=0; ig<numBins; ig++) {
            for (int ib=0; ib<numBins; ib++) {
                ofVec3f cb = ofVec3f(ir * 256.0f / numBins, ig * 256.0f / numBins, ib * 256.0f / numBins);
                colorBins.push_back(cb);
            }
        }
    }
    
    // compute EMD for each pair of tile and target images, dump into cost matrix
    ofLog(OF_LOG_NOTICE, "computing EMD between every tile and candidate image");
    vector<vector<double> > costs;
    for (int t=0; t<targetH.size(); t++) {
        if (t%20 == 0) {
            cout << "EMD ("<<t<<"/"<<targetH.size()<<")"<<endl;
        }
        vector<double> weights1 = getWeightsVector2(targetH[t], numBins);
        vector<double> costTile;
        for (int c=0; c<candidateH.size(); c++) {
            vector<double> weights2 = getWeightsVector2(candidateH[c], numBins);
            double e = emd.getEmd(colorBins, weights1, colorBins, weights2);
            costTile.push_back(e);
        }
        costs.push_back(costTile);
    }
    
    ofLog(OF_LOG_NOTICE, "solving grid");
    assignment = solver.solve(costs);
    
    ofLog(OF_LOG_NOTICE, "finished... save image");
    saveImage();

    ofExit();
}

//--------------------------------------------------------------
void ofApp::update(){
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    float w = ofGetWidth() / nx;
    float h = ofGetHeight() / ny;
    if (view) {
        for (int r=0; r<ny*nx; r++) {
            targetCrops[r].draw((r%nx)*w, floor(r/nx)*h, w, h);
        }
    }
    else {
        for (int r=0; r<ny*nx; r++) {
            int row = floor(r / nx);
            int col = r % nx;
            candidates[assignment[r]].draw(col * w, row * h, w*0.97, h*0.97);
        }
    }
}

//--------------------------------------------------------------
void ofApp::saveImage(){
    float w = displayH * aspect;
    float h = displayH;
    ofFbo fbo;
    fbo.allocate(nx * w, ny * h);
    fbo.begin();
    ofClear(0, 255);
    for (int r=0; r<ny*nx; r++) {
        int row = floor(r / nx);
        int col = r % nx;
        candidates[assignment[r]].draw(col * w, row * h, w, h);
    }
    fbo.end();
    ofPixels pixels;
    fbo.readToPixels(pixels);
    ofImage final;
    final.setFromPixels(pixels);
    final.save("myMosaic.png");
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key==' '){
        view = !view;
    }
}

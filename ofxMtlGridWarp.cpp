//
//  ofxMtlGridWarp.cpp
//  ofxMtlGridWarp
//
//  Created by Elie Zananiri on 11-07-15.
//  Copyright 2011 Departement. All rights reserved.
//

#include "ofxMtlGridWarp.h"

#include "ofxXmlSettings.h"

#define kPointNone      -1
#define kPointThreshold 30
#define kPointSize       4

//--------------------------------------------------------------
ofxMtlGridWarp::ofxMtlGridWarp() {
    onPoint = kPointNone;
    scale = 1;
    bTracking = false;
}

//--------------------------------------------------------------
ofxMtlGridWarp::~ofxMtlGridWarp() {
    delete [] pts;
}

//--------------------------------------------------------------
void ofxMtlGridWarp::init(int _numRows, int _numCols, float _x, float _y, float _width, float _height) {
    rows = _numRows + 1;
    cols = _numCols + 1;
    pts = new ofPoint[rows * cols];
    
    // set the starting coordinates
    for (int c=0; c < cols; c++) {
        for (int r=0; r < rows; r++) {
            int i = r * cols + c;
            pts[i].set(_x + c * (_width / (float)_numCols), _y + r * (_height / (float)_numRows));
        }
    }
    
    onPoint = kPointNone;
    bTracking = false;
}

//--------------------------------------------------------------
void ofxMtlGridWarp::setScale(float _scale) {
    scale = _scale;
}

//--------------------------------------------------------------
void ofxMtlGridWarp::draw() {
    ofSetColor(255, 255, 0);

    // draw the vertical lines
    for (int r=0; r < rows - 1; r++) {
        for (int c=0; c < cols; c++) {
            int i1 = r * cols + c;
            int i2 = (r+1) * cols + c;
            
            ofLine(pts[i1], pts[i2]);  
        }
    }
    
    // draw the horizontal lines
    for (int c=0; c < cols - 1; c++) {
        for (int r=0; r < rows; r++) {
            int i1 = c + r * cols;
            int i2 = (c+1) + r * cols;
            
            ofLine(pts[i1], pts[i2]);  
        }
    }
    
    // draw the points
    for (int i=0; i < rows * cols; i++) {
        if (i == onPoint)
            ofSetColor(255, 0, 0);
        else              
            ofSetColor(255, 255, 0);
        ofRect(pts[i].x, pts[i].y, kPointSize, kPointSize);
    }
}

//--------------------------------------------------------------
void ofxMtlGridWarp::startTracking() {
    ofAddListener(ofEvents.mousePressed, this, &ofxMtlGridWarp::mousePressed);
    ofAddListener(ofEvents.mouseReleased, this, &ofxMtlGridWarp::mouseReleased);
    ofAddListener(ofEvents.mouseDragged, this, &ofxMtlGridWarp::mouseDragged);
    
    bTracking = true;
}

//--------------------------------------------------------------
void ofxMtlGridWarp::stopTracking() {
    ofRemoveListener(ofEvents.mousePressed, this, &ofxMtlGridWarp::mousePressed);
    ofRemoveListener(ofEvents.mouseReleased, this, &ofxMtlGridWarp::mouseReleased);
    ofRemoveListener(ofEvents.mouseDragged, this, &ofxMtlGridWarp::mouseDragged);
    
    bTracking = false;
}

//--------------------------------------------------------------
void ofxMtlGridWarp::toggleTracking() {
    if (bTracking) stopTracking();
    else startTracking();
}

//--------------------------------------------------------------
void ofxMtlGridWarp::mousePressed(ofMouseEventArgs& args) {
    
}

//--------------------------------------------------------------
void ofxMtlGridWarp::mouseDragged(ofMouseEventArgs& args) {
    ofPoint mouse(args.x / scale, args.y / scale);
    for (int i=0; i < rows * cols; i++) {	
        if (pts[i].distance(mouse) < kPointThreshold) {
            onPoint = i;
            pts[i].set(mouse);

            break;
        }
    }	
}

//--------------------------------------------------------------
void ofxMtlGridWarp::mouseReleased(ofMouseEventArgs& args) {
    onPoint = kPointNone;
}

//--------------------------------------------------------------
bool ofxMtlGridWarp::load(const string& _filename) {
    ofxXmlSettings doc;
    
    if (!doc.loadFile(_filename)) {
        ofLog(OF_LOG_ERROR, "Unable to load grid warp points XML!");
        return false;
    }
    
    // load the points
    doc.pushTag("points");
    {
        int n = MIN(doc.getNumTags("pt"), rows*cols);
        for (int i=0; i < n; i++) {
            doc.pushTag("pt", i);
            {
                pts[i].set(doc.getValue("x", pts[i].x), doc.getValue("y", pts[i].y));
            }
            doc.popTag();
        }
    }
    doc.popTag();
    
    return true;
}

//--------------------------------------------------------------
bool ofxMtlGridWarp::save(const string& _filename) {
    ofxXmlSettings doc;
    
    // save the points
    doc.addTag("points");
    doc.pushTag("points");
    for (int i=0; i < rows*cols; i++) {
        doc.addTag("pt");
        doc.pushTag("pt", i);
        {
            doc.setValue("x", pts[i].x);
            doc.setValue("y", pts[i].y);
        }
        doc.popTag();
    }
    
    doc.saveFile(_filename);
    
    return true;
}

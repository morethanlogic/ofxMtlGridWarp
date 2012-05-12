//
//  ofxMtlGridWarp.h
//  ofxMtlGridWarp
//
//  Created by Elie Zananiri on 11-07-15.
//  Copyright 2011 Departement. All rights reserved.
//

#pragma once

#include "ofMain.h"

//========================================================================
class ofxMtlGridWarp {

public:
                ofxMtlGridWarp();
                ~ofxMtlGridWarp();
    
    void        init(int _numRows, int _numCols, float _x, float _y, float _width, float _height);
    void        setOffset(float _xOffset, float _yOffset);
    void        setScale(float _scale);
    
    void        draw();
    
    void        startTracking();
    void        stopTracking();
    void        toggleTracking();
    
    bool        load(const string& _filename);
    bool        save(const string& _filename);
    
    int         rows, cols;
    ofPoint*    pts;
	int			onPoint;
    float       xOffset, yOffset;
    float       scale;
    bool        bTracking;
    
private:
    void        mousePressed(ofMouseEventArgs& args);
    void        mouseDragged(ofMouseEventArgs& args);
    void        mouseReleased(ofMouseEventArgs& args);
    
};
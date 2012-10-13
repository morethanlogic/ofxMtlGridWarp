//
//  ofxMtlGridWarp.cpp
//  ofxMtlGridWarp
//
//  Created by Elie Zananiri on 11-07-15.
//  Copyright 2011 Departement. All rights reserved.
//

#include "ofxMtlGridWarp.h"

#include "ofxXmlSettings.h"

//--------------------------------------------------------------
ofxMtlGridWarp::ofxMtlGridWarp() {
    xOffset = 0;
    yOffset = 0;
    scale = 1;
    bTracking = false;
    
    selectedVertex = 1;
}

//--------------------------------------------------------------
ofxMtlGridWarp::~ofxMtlGridWarp() {
    delete [] pts;
}

//--------------------------------------------------------------
void ofxMtlGridWarp::init(int _numRows, int _numCols, float _x, float _y, float _width, float _height) {
    rows = _numRows + 1;
    cols = _numCols + 1;
    
    sliceWidth = _width / (float)_numCols;
    sliceHeight = _height / (float)_numRows;
    
    // set the starting coordinates
    //
    pts = new ofPoint[rows * cols];
    for (int c=0; c < cols; c++) {
        for (int r=0; r < rows; r++) {
            int i = r * cols + c;
            pts[i].set(_x + c * sliceWidth,
                       _y + r * sliceHeight);
        }
    }
    
    selectedVertex = -1;
    bTracking = false;
}

//--------------------------------------------------------------
void ofxMtlGridWarp::setOffset(float _xOffset, float _yOffset) {
    xOffset = _xOffset;
    yOffset = _yOffset;
}

//--------------------------------------------------------------
void ofxMtlGridWarp::setScale(float _scale) {
    scale = _scale;
}

//--------------------------------------------------------------
void ofxMtlGridWarp::draw( ofTexture &_text ) {
    
    ofPushStyle();
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_TEXTURE_2D);
    
	ofSetColor(255);
    ofEnableAlphaBlending();
    
	glEnable(GL_SMOOTH);
	glShadeModel(GL_SMOOTH);
    _text.bind();
    mesh.draw();
    _text.unbind();
    
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_NORMALIZE);
	glDisable(GL_DEPTH_TEST);
    ofPopStyle();
    
    
    if (bTracking){
        
        //  Make a mesh from the grid
        //
        mesh.clear();
        
        //  Create the variables
        //
        int nVertexCount = (int) ( cols * rows * 6 );
        
        int width = _text.getWidth();
        int height = _text.getHeight();
        
        ofVec3f *pVertices	= new ofVec3f[nVertexCount];		// Allocate Vertex Data
        ofVec2f *pTexCoords	= new ofVec2f[nVertexCount];		// Allocate Tex Coord Data
        
        int     nX, nY, nTri, indexX, indexY, nIndex=0; // Create Variables
        
        for(int r = 0; r < rows -1; r++){
            for(int c = 0; c < cols -1; c++){
                for( nTri = 0; nTri < 6; nTri++ ){
                
                    indexX = c + ( ( nTri == 1 || nTri == 2 || nTri == 5 ) ? 1 : 0 );
                    indexY = r + ( ( nTri == 2 || nTri == 4 || nTri == 5 ) ? 1 : 0 );
                
                    int pIndex = indexX + indexY * cols;
                    pVertices[nIndex] = pts[pIndex];
                
                    // 3	0 --- 1		nTri reference
                    // | \	  \	  |
                    // |   \	\ |
                    // 4 --- 5	  2
                
                    // Stretch The Texture Across The Entire Mesh
                    pTexCoords[nIndex].x = ofMap(indexX,0,cols,0,width);
                    pTexCoords[nIndex].y = ofMap(indexY,0,rows,0,height);
                
                    // Increment Our Index
                    nIndex++;
                }
            }
        }
        
        mesh.addVertices(pVertices, nVertexCount);
        mesh.addTexCoords(pTexCoords, nVertexCount);
    
        delete [] pVertices;
        delete [] pTexCoords;
        
        //  DRAW the GRID
        //
        ofPushStyle();
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
            if (i == selectedVertex)
                ofSetColor(255, 0, 0);
            else
                ofSetColor(255, 255, 0);
            ofRect(pts[i].x-5, pts[i].y-5, 10, 10);
        }
        ofPopStyle();
    }
}

//--------------------------------------------------------------
void ofxMtlGridWarp::startTracking() {
    ofAddListener(ofEvents().mousePressed, this, &ofxMtlGridWarp::mousePressed);
    ofAddListener(ofEvents().mouseReleased, this, &ofxMtlGridWarp::mouseReleased);
    ofAddListener(ofEvents().mouseDragged, this, &ofxMtlGridWarp::mouseDragged);
    
    bTracking = true;
    selectedVertex = -1;
}

//--------------------------------------------------------------
void ofxMtlGridWarp::stopTracking() {
    ofRemoveListener(ofEvents().mousePressed, this, &ofxMtlGridWarp::mousePressed);
    ofRemoveListener(ofEvents().mouseReleased, this, &ofxMtlGridWarp::mouseReleased);
    ofRemoveListener(ofEvents().mouseDragged, this, &ofxMtlGridWarp::mouseDragged);
    
    bTracking = false;
    selectedVertex = -1;
}

//--------------------------------------------------------------
void ofxMtlGridWarp::toggleTracking() {
    if (bTracking) stopTracking();
    else startTracking();
}

//--------------------------------------------------------------
void ofxMtlGridWarp::mousePressed(ofMouseEventArgs& args) {
    ofPoint mouse((args.x - xOffset) / scale, (args.y - yOffset) / scale);
    for (int i=0; i < rows * cols; i++) {
        if (pts[i].distance(mouse) < 30) {
            
            selectedVertex = i;
            pts[i].set(mouse);
            
            break;
        }
    }
}

//--------------------------------------------------------------
void ofxMtlGridWarp::mouseDragged(ofMouseEventArgs& args) {
    ofPoint mouse((args.x - xOffset) / scale, (args.y - yOffset) / scale);
    pts[selectedVertex].set(mouse);
}

//--------------------------------------------------------------
void ofxMtlGridWarp::mouseReleased(ofMouseEventArgs& args) {
    selectedVertex = -1;
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

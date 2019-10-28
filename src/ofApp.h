#pragma once

#include "ofMain.h"
#include "ofxFFTFile.h"
#include "ofxSpout2Sender.h"
#include "ofxXmlSettings.h"
#include "ofxOsc.h"

// listen on port 8000
#define PORT 8000
#define NUM_MSG_STRINGS 20

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	void exit();
	
private:
	void loadImage();

	ofImage colormap, bumpmap;
	int imgIndex;
	GLUquadricObj *quadric;
	// spout
	ofxSpout2::Sender spout;
	ofFbo 						fbo;
	ofShader 					shader;
	int 						targetWidth, targetHeight;
	ofxXmlSettings 				settings;

	ofSoundPlayer soundPlayer;
	float audioThreshold;
	float audioPeakDecay;
	float audioMaxDecay;
	bool audioMirror;
	ofxFFTFile fftFile;
	float audioValue;
	float factor;
	float maxHeight;
	ofVec3f  twod;
	float angleX;
	float angleY;
	float angleZ;
	float startTime;
	float currentTime;
	bool isPlaying;
	// osc
	ofTrueTypeFont font;
	ofxOscReceiver receiver;

	int current_msg_string;
	string msg_strings[NUM_MSG_STRINGS];
	float timers[NUM_MSG_STRINGS];

	int mouseX, mouseY;
	string mouseButtonState;

	ofImage receivedImage;
};

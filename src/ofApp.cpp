#include "ofApp.h"
// idees passer du logo 2d à 3D
//--------------------------------------------------------------
void ofApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	// load settings.xml
	if (settings.loadFile("humanet.xml") == false) {
		ofLog() << "humanet.xml not found";
	}
	settings.pushTag("settings");
	targetWidth = settings.getValue("targetWidth", 1280);
	targetHeight = settings.getValue("targetHeight", 720);
	ofLogNotice("targetWidth: " + ofToString(targetWidth) + " targetHeight: " + ofToString(targetHeight));
	fbo.allocate(targetWidth, targetHeight);
	fbo.begin();
	ofClear(0, 0, 0, 0);
	fbo.end();

	/*for (int i = 1; i <= 8; i++) {
		std::ostringstream sout;
		sout << "p00" << std::setfill('0') << std::setw(2) << i << ".jpg";
		ofImage image;
		image.load(sout.str());
		images.push_back(image);
	} */
	imgIndex = 1;
	ofBackground(0, 0, 0);
	ofDisableArbTex();
	ofEnableAlphaBlending();
	ofSetFrameRate(30);
	glEnable(GL_DEPTH_TEST);
	audioThreshold = 0.9;
	audioPeakDecay = 0.915;
	audioMaxDecay = 0.995;
	audioMirror = false;
	titleImage.loadImage("p-(0).jpg");
	colormap.loadImage("p-(1).jpg");
	bumpmap.loadImage("p-(1).jpg");

	quadric = gluNewQuadric();
	gluQuadricTexture(quadric, GL_TRUE);
	gluQuadricNormals(quadric, GLU_SMOOTH);

	shader.load("shaders/displace.vert", "shaders/displace.frag");

	fftFile.setMirrorData(false);
	fftFile.setup();

	soundPlayer.loadSound("humanet.wav");
	factor = 1.0f;
	angleX = 180.0f;
	angleY = 0.0f;
	angleZ = 180.0f;
	startTime = 1200.0f;
	isPlaying = false;
	//osc
	// listen on the given port
	cout << "listening for osc messages on port " << PORT << "\n";
	receiver.setup(PORT);

	current_msg_string = 0;
	mouseX = 0;
	mouseY = 0;
	mouseButtonState = "";
	current = 4;
	oscInt0 = 0;
	ibar = 1;
	beat = 1;

}

//--------------------------------------------------------------
void ofApp::update() {
	// hide old messages
	for (int i = 0; i < NUM_MSG_STRINGS; i++) {
		if (timers[i] < ofGetElapsedTimef()) {
			msg_strings[i] = "";
		}
	}

	// check for waiting messages
	while (receiver.hasWaitingMessages()) {
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(m);

		// check for mouse moved message
		if (m.getAddress() == "/mouse/position") {
			// both the arguments are int32's
			mouseX = m.getArgAsInt32(0);
			mouseY = m.getArgAsInt32(1);
		}
		// check for mouse button message
		else if (m.getAddress() == "/mouse/button") {
			// the single argument is a string
			mouseButtonState = m.getArgAsString(0);
		}
		// check for an image being sent (note: the size of the image depends greatly on your network buffer sizes - if an image is too big the message won't come through ) 
		else if (m.getAddress() == "/image") {
			ofBuffer buffer = m.getArgAsBlob(0);
			titleImage.load(buffer);
		}
		else {
			// unrecognized message: display on the bottom of the screen
			string msg_string;
			oscAddr = m.getAddress();
			msg_string = m.getAddress();
			msg_string += ": ";
			for (int i = 0; i < m.getNumArgs(); i++) {
				// get the argument type
				msg_string += m.getArgTypeName(i);
				msg_string += ":";
				// display the argument - make sure we get the right type
				if (m.getArgType(i) == OFXOSC_TYPE_INT32) {
					msg_string += ofToString(m.getArgAsInt32(i));
					oscInt0 = m.getArgAsInt32(i);
				}
				else if (m.getArgType(i) == OFXOSC_TYPE_FLOAT) {
					msg_string += ofToString(m.getArgAsFloat(i));
				}
				else if (m.getArgType(i) == OFXOSC_TYPE_STRING) {
					msg_string += m.getArgAsString(i);
				}
				else {
					msg_string += "unknown";
				}
			}
			// add to the list of strings to display
			msg_strings[current_msg_string] = msg_string;
			if (oscAddr == "/beat") {
				// int32 1 to 4 beat from Transthor
				beat = oscInt0 - 1;
				current = ibar * 4 + beat;

			}
			else if (oscAddr == "/bar") {
				if (imgIndex != oscInt0 - 400) {
					imgIndex = oscInt0 - 400;
					loadImage();
				}
				ibar = oscInt0;
				current = ibar * 4 + beat;
			}
			else if (oscAddr == "/play") {
				isPlaying = (oscInt0 == 1);
			}
			timers[current_msg_string] = ofGetElapsedTimef() + 5.0f;
			current_msg_string = (current_msg_string + 1) % NUM_MSG_STRINGS;
			// clear the next line
			msg_strings[current_msg_string] = "";
		}

	}
	if (soundPlayer.getIsPlaying() == true) {
		fftFile.setThreshold(audioThreshold);
		fftFile.setPeakDecay(audioPeakDecay);
		fftFile.setMaxDecay(audioMaxDecay);
		fftFile.setMirrorData(audioMirror);
		fftFile.update();
		int numOfVerts = 512;

		float * audioData = new float[numOfVerts];
		fftFile.getFftPeakData(audioData, numOfVerts);
		audioValue = audioData[8];  // yg1: 220
	}
	//if (audioValue < 0.01) {
	//	//if (factor == 0.0f) factor = 0.0f;
	//	factor -= 0.1;
	//}
	//else {
	//	factor = 0.0f;
	//}
	//factor = ofMap(mouseY, 0.0f, ofGetHeight(), -1.0f, 1.0f);
	ofSetWindowTitle("angle: " + ofToString(angleX, 2) + "img: " + ofToString(imgIndex, 2) + " mult: " + ofToString(factor, 2) + " - " + ofToString(audioValue, 2) + " - " + ofToString(ofGetFrameNum(), 2) + " img " + ofToString(imgIndex) + " current " + ofToString(current) + " bar " + ofToString(ibar) + " beat " + ofToString(beat));

}

//--------------------------------------------------------------
void ofApp::draw() {

	ofSetColor(255);
	if (isPlaying) {
		maxHeight = audioValue * 50 * factor + ofGetFrameNum() / 200;
		fbo.begin();
		ofClear(0, 0, 0, 0);
		ofPushMatrix();
		twod.set(0.30, 0.59, 0.11);
		//twod.set(0.0, 0.0, 0.0);
		shader.begin();
		shader.setUniformTexture("colormap", colormap, 1);
		shader.setUniformTexture("bumpmap", bumpmap, 2);
		//shader.setUniformTexture("colormap", image, 1);
		//shader.setUniformTexture("bumpmap", image, 2);
		shader.setUniform1f("maxHeight", maxHeight);
		shader.setUniform3f("twod", twod);
		if (isPlaying) {
			currentTime = ofGetElapsedTimef() - startTime;
		}
		else {
			currentTime = 0.0f;
		}
		if (currentTime < 25.7f) {
			shader.setUniform1f("time", currentTime);
			angleX = 0.0f;
			angleY = 90.0f;
			angleZ = 180.0f;
			//ofTranslate(targetWidth / 2, targetHeight / 2);
			ofTranslate(ofLerp(targetWidth / 4, targetWidth / 2, currentTime / 50), ofLerp(targetHeight, targetHeight / 2, currentTime / 50));
		}
		else {
			shader.setUniform1f("time", 40.0f);
			angleX = 360.0f + (mouseX / 1.0 + 0.01) * sinf(float(ofGetFrameNum()) / 500.0f);
			angleY = 360.0f + (mouseY / 1.0 + 0.01) * sinf(float(ofGetFrameNum()) / 500.0f);
			angleZ = 0.0f;
			ofTranslate(targetWidth / 2, targetHeight / 2);
		}
		//shader.setUniform1f("factor", factor);
		//ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);

		ofRotateY(angleX);
		ofRotateX(angleY);
		ofRotate(-90, 1, 0, 0);
		//gluSphere(quadric, 150, 400, 400);
		gluSphere(quadric, 160 + ofGetFrameNum() / 200, 400, 400); //300 taille sphere
		shader.end();
		fbo.end();

		ofPopMatrix();

		spout.sendTexture(fbo.getTexture(), "humanet");
		fbo.draw(0, 0, targetWidth, targetHeight);
	}
	else {
		// not playing
		if (titleImage.getWidth() > 0) {
			titleImage.draw(0, 0);
			spout.sendTexture(titleImage.getTexture(), "humanet");
			ofDrawBitmapString("Image:", 10, 160);
		}
	}
	string buf;
	buf = "listening for osc messages on port" + ofToString(PORT);
	ofDrawBitmapString(buf, 10, 20);

	// draw mouse state
	buf = "mouse: " + ofToString(mouseX, 4) + " " + ofToString(mouseY, 4);
	ofDrawBitmapString(buf, 430, 20);
	ofDrawBitmapString(mouseButtonState, 580, 20);

	for (int i = 0; i < NUM_MSG_STRINGS; i++) {
		ofDrawBitmapString(msg_strings[i], 10, 60 + 15 * i);
	}

	ofDrawBitmapString("current: " + ofToString(current), 20, 40);
}
void ofApp::loadImage() {
	bool bFileThere = false;
	string fileName = "p-(" + ofToString(imgIndex) + ").jpg";
	/*if (imgIndex < 10) {
		fileName = "p000" + ofToString(imgIndex) + ".jpg";
	}
	else {
		fileName = "p00" + ofToString(imgIndex) + ".jpg";
	} */

	fstream fin;
	string fileNameInOF = ofToDataPath(fileName); // since OF files are in the data directory, we need to do this  
	fin.open(fileNameInOF.c_str(), ios::in);
	if (fin.is_open()) {
		cout << " file exists" << fileName << endl;
		bFileThere = true;
	}
	fin.close();
	if (bFileThere) {
		colormap.loadImage(fileName);
		bumpmap.loadImage(fileName);
	}

}
//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == OF_KEY_RIGHT) {
		//link.setQuantum(link.quantum() + 1);
		imgIndex++;
		loadImage();
	}
	else if (key == OF_KEY_LEFT) {
		//link.setQuantum(link.quantum() - 1);
		imgIndex--;
		if (imgIndex < 1) imgIndex = 1;
		loadImage();
	}
	else if (key == OF_KEY_UP) {
		//link.setTempo(link.tempo() + 1);
		factor++;
	}
	else if (key == OF_KEY_DOWN) {
		//link.setTempo(link.tempo() - 1);
		factor--;
		if (factor < 1) factor = 1;
	}
	if (key == ' ') {
		isPlaying = !isPlaying;
		startTime = ofGetElapsedTimef();
		if (isPlaying) {
			soundPlayer.stop();
		}
		else {
			soundPlayer.play();
		}
	}
	//if (key == 'f')	 ofSetFullscreen(true);


}
void ofApp::exit() {
	spout.exit();
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}


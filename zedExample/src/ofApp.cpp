#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	zed.setResolution(ZED_RESOLUTION_HD720); //ZED_RESOLUTION_VGA, ZED_RESOLUTION_HD720, ZED_RESOLUTION_HD1080, ZED_RESOLUTION_HD2K
	zed.setFps(30);							//See the top of ofxKuZed.h for list of available FPS
	zed.setDepthModeQuality(ZED_DEPTH_MODE_QUALITY);	//ZED_DEPTH_MODE_PERFORMANCE, ZED_DEPTH_MODE_MEDIUM, ZED_DEPTH_MODE_QUALITY
	zed.setDepthPostprocess(ZED_DEPTH_POSTPROCESS_STANDARD); //ZED_DEPTH_POSTPROCESS_FILL, ZED_DEPTH_POSTPROCESS_STANDARD
	zed.setUseImages(true);
	zed.setUseDepth(true);

	bool flipY = true;
	bool flipZ = true;
	zed.setUsePointCloud(true, true, flipY, flipZ);	//points, colors, flipY, flipZ
	zed.init();
}

//--------------------------------------------------------------
void ofApp::update()
{
	zed.update();

	//Compute masked image by combining left color image and thresholded depth values
	ofPixels &left = zed.getLeftPixels();
	ofFloatPixels &depth_mm = zed.getDepthPixels_mm();
	
	int w = zed.getWidth();
	int h = zed.getHeight();
	masked.allocate(w, h, 3);
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			float depth_value = depth_mm.getData()[x + w*y];
			ofColor color = (depth_value <= threshold_mm && depth_value > 0) ? left.getColor(x, y) : ofColor(0);
			masked.setColor(x, y, color);
		}
	}
	maskedTexture.loadData(masked);
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofBackground(0);
	float W = ofGetWidth();
	float H = ofGetHeight();
	if (drawing_page == 1) {	//draw images, depth, depth mask
		ofSetColor(255);
		zed.drawLeft(0, 0, W / 2, H / 2);
		zed.drawRight(W / 2, 0, W / 2, H / 2);
		zed.drawDepth(0, H / 2, W / 2, H / 2, 0, 5000);

		maskedTexture.draw(W / 2, H / 2, W / 2, H / 2);
	}
	if (drawing_page == 2) {	//draw point cloud
		ofEnableDepthTest();
		easyCam.begin();
		ofSetColor(255);
		zed.drawPointCloud();
		easyCam.end();
		ofDisableDepthTest();
	}
	string info;
	if (zed.started()) info += "ZED started"; 
	else info += "ZED not started";
	info += ", " + ofToString(zed.getWidth()) + " x " + ofToString(zed.getHeight());
	info += ", camera fps " + ofToString(zed.getFps()) + ", keys: 1,2-page, -,= - threshold";
	info += "\nthreshold_mm: " + ofToString(threshold_mm) + "    FPS: " + ofToString(ofGetFrameRate());
	ofDrawBitmapStringHighlight(info, 20, 20);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == '1') drawing_page = 1;
	if (key == '2') drawing_page = 2;
	if (key == '-') threshold_mm -= 100;
	if (key == '=') threshold_mm += 100;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

/*
#zedExample
This is an example of using ofxKuZed addon for ZED camera.

After starting, it draws left and right RGB images,
depth image and also masked image, which is left RGB image, 
truncated by depth values using depth threshold.

Press '9' and '0' to change depth view range (affects depth image).
Press '-' and '=' to change depth threshold (affects masked image).

Press '2' to switch to point cloud view. 

Here use mouse for changing view position, angle and scale.
Double click to reset view.

Press '1' to switch back to images view.

Keys:
* '1','2' - select page (images and depth / point cloud)
* '9','0' - adjust depth view range.
* '-','=' - adjust depth threshold

Requirements and installation details see in addon's file ofxKuZed.h
*/

#pragma once

#include "ofMain.h"
#include "ofxKuZed.h"
class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	ofxKuZed zed;

	float view_range_mm = 5000;		//depth view range

	float threshold_mm = 2000;	//depth threshold
	ofPixels masked;			//masked image
	ofTexture maskedTexture;	//masked texture

	//drawing_page: 1 - images, depth, masked, 2 - point cloud
	int drawing_page = 1;

	ofEasyCam easyCam;

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

};

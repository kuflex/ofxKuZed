/*
#zedExample
This is an example of using ofxKuZed addon for ZED camera.

After starting, it draws left and right RGB images,
depth image and masked image, obtained as left image, 
truncated by depth values using threshold.

Press '-' and '=' to change threshold.

Press '2' to switch to point cloud view. 

Here use mouse for changine view position, angle and scale.

Press '1' to switch back to images view.

Keys:
* '1','2' - select page (images and depth / point cloud)
* '-','=' - adjust threshold

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

	//computing depth mask using threshold_mm
	float threshold_mm = 2000;
	ofPixels masked;
	ofTexture maskedTexture;

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

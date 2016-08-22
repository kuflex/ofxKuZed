#ofxKuZed
Addon for working with StereoLab ZED camera in openFrameworks.

ZED camera is a passive stereo camera, allowing to obtain depth data on sunlight at distance 20 meters.
* https://www.stereolabs.com/zed/specs/
* https://kuflex.wordpress.com/2016/08/18/overview-of-stereolabs-zed-camera-lightweight-outdoors-depth-camera/

##Addon's features
* It wraps all basic camera settings such as resolution, fps and depth computing quality
* It provides CPU-access to: 
  - left and right rectified RGB images (as ofPixels, ofTexture)
  - depth data in millimeters (as ofFloatPixels, ofPixels, ofTexture)
  - point cloud with colors (as vectors of ofPoint and ofColor)
* It uses "lazy" updating of all pixel arrays and textures: they are updated only by request to save CPU resources.
* It includes an example '''zedExample''', demonstrating work with RGB and depth images, and point cloud from camera.

![zedCamera](https://github.com/kuflex/ofxKuZed/raw/master/docs/ofxKuZed-1.jpg "zedCamera example")

##Simple usage:

In ofApp.h:
```
	...
	#include "ofxKuZed.h"
	...
	class ofApp {
		...
		ofxKuZed zed;
		...
	};
```

In ofApp.cpp:
```
ofApp::setup() {
		zed.setResolution(ZED_RESOLUTION_HD720);
		zed.setFps(30);			
		//see zed.set... functions for more settings
		zed.init();
	}

	void ofApp::update() {
		zed.update();
		//ofPixels &left = zed.getLeftPixels();	//access to left image colors
		//ofFloatPixels &depth_mm = zed.getDepthPixels_mm(); //access to depth
	}

	void ofApp::draw()	{
		float W = ofGetWidth();
		float H = ofGetHeight();
		ofSetColor(255);
		//zed.drawLeft(0, 0);	//draw left color image, it's synced with depth image
		zed.drawDepth(0, H / 2, W / 2, H / 2, 0, 5000);	//0 and 5000 - depth range to draw as white and black

		//To draw point cloud: 
		//Define "ofEasyCam easyCam" in ofApp class
		//ofEnableDepthTest();
		//easyCam.begin();
		//ofSetColor(255);
		//zed.drawPointCloud();
		//easyCam.end();
		//ofDisableDepthTest();
	}
```

##Advanced usage

See an example '''zedExample''' distributed with addon, and '''ofxKuZed.h''' file.

##Requirements
Currently addon is tested on Windows 10.

To compile the addon's example you need:
* Windows
* Installed Microsoft Visual Studio 2015 Community
* openFrameworks 0.9.3

* Installed Cuda 7.5 (and NVidia videocard)

   If you are using another CUDA:
   
       Write proper path in parameter <CUDA_DIR> in file '''zedExample.vcxproj'''

* Installed ZED SDK (example project is configured for '''ZED_SDK_WinSetup_v1.0.0c.exe''')

   If you are using older ZED SDK, you need:
   
       Write proper path in <ZED_SDK_DIR> in file''' zedExample.vcxproj''',
   
       Adjust project's properties to valid paths and libs names in the following settings:
       - C/C++ - General - Additional include directories
	   - Linker - General - Additional library directories
	   - Linker - Input - Additional dependencies

To run the example you need:
* ZED camera
* USB 3.0 is preferrable (camera works with USB 2.0 too, but slower)


##Credits
The addon is based on 
* ofxZED addon by Design & Systems and Stefan Wagner https://github.com/andsynchrony/ofxZED
* ZED SDK API.

Addon is written by Kuflex lab, https://github.com/kuflex/ofxKuZed

###TODOs: 
* Implement settings for RGB images (brightness, contrast)
* Port to Linux
* Make masking using GPU

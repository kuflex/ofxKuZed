/*==============================================================================
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
* Port to Linux
* Implement settings for RGB images (brightness, contrast)
* Implement masking using GPU
* Implement recording/playing SVO
==============================================================================*/

#pragma once

#include "ofMain.h"
#include <zed/Camera.hpp>

//Available ZED resolutions
const int ZED_RESOLUTION_HD2K = sl::zed::HD2K;	//2208*1242, supported framerate : 15 fps 
const int ZED_RESOLUTION_HD1080 = sl::zed::HD1080;	//1920*1080, supported framerates : 15, 30 fps
const int ZED_RESOLUTION_HD720 = sl::zed::HD720; //1280*720, supported framerates : 15, 30, 60 fps
const int ZED_RESOLUTION_VGA = sl::zed::VGA;	//672*376, supported framerates : 15, 30, 60, 100 fps

//ZED depth computing quality
const int ZED_DEPTH_MODE_PERFORMANCE = sl::zed::PERFORMANCE; //Fastest mode, also requires less GPU memory, the disparity map is less robust
const int ZED_DEPTH_MODE_MEDIUM = sl::zed::MEDIUM; //Balanced quality mode, requires less GPU memory but the disparity map is a little less detailed
const int ZED_DEPTH_MODE_QUALITY = sl::zed::QUALITY; //Better quality mode, the disparity map is more precise

//ZED depth postprocessing mode
const int ZED_DEPTH_POSTPROCESS_FILL = sl::zed::FILL;			//Occlusion filling, edge sharpening, advanced post-filtering.
const int ZED_DEPTH_POSTPROCESS_STANDARD = sl::zed::STANDARD;	//No occlusion filling


class ofxKuZed
{
public:
	ofxKuZed();
	~ofxKuZed();

	//==== Usage ====
	void init();
	void update();
	void close();

	//Flags for grabing color images, depth, point cloud
	void setUseImages(bool useImages);				//default: true
	void setUseDepth(bool useDepth);				//default: true
	void setUsePointCloud(bool usePointCloud, bool usePointCloudColors, bool flipY = true, bool flipZ = true);	//default: true,true

	bool started();		//is ZED working now

	bool isFrameNew();		//TODO will be useful for threaded implementation
	int getWidth();
	int getHeight();

	//All textures and pixels arrays are "lazy" updated,
	//that is thay are updated only by request
	ofFloatPixels &getDepthPixels_mm();
	ofPixels &getDepthPixels_grayscale(float min_depth_mm = 0.0, float max_depth_mm = 5000.0);
	ofTexture &getDepthTexture(float min_depth_mm = 0.0, float max_depth_mm = 5000.0);	

	ofTexture &getLeftTexture();
	ofPixels &getLeftPixels();
	ofTexture &getRightTexture();
	ofPixels &getRightPixels();

	vector<ofPoint> &getPointCloud();
	vector<ofColor> &getPointCloudColors();
	vector<ofFloatColor> &getPointCloudFloatColors();	//required for ofMesh

	void drawLeft( float x, float y, float w=0, float h=0 );
	void drawRight( float x, float y, float w=0, float h=0 );
	void drawDepth(float x, float y, float w=0, float h=0, float min_mm = 0, float max_mm = 5000);
	void drawPointCloud();	//draws a mesh of points


	//==== Basic settings ====
	//Note: you should call set... functions before init()

	//Camera resolution
	void setResolution(int zed_resolution_mode );	//default: ZED_RESOLUTION_HD720
			//ZED_RESOLUTION_HD2K		2208*1242, supported framerate : 15 fps
			//ZED_RESOLUTION_HD1080		1920*1080, supported framerates : 15, 30 fps
			//ZED_RESOLUTION_HD720		1280*720, supported framerates : 15, 30, 60 fps 
			//ZED_RESOLUTION_VGA		672*376, supported framerates : 15, 30, 60, 100 fps 

	//Camera FPS
	void setFps(float fps);						//default: 0
	float getFps();

	//==== Advanced settings ====
	//Note: you should call set... functions before init()

	//Graphics card on which the computation will be done. The default value -1 search the more powerful usable GPU.
	void setGpuDevice(int gpu_id);		//default: -1


	//Quality of the disparity map, affects the level of details and also the computation time.
	void setDepthModeQuality(int depth_mode);	//default: ZED_DEPTH_MODE_QUALITY
			//ZED_DEPTH_MODE_PERFORMANCE	Fastest mode, also requires less GPU memory, the disparity map is less robust
			//ZED_DEPTH_MODE_MEDIUM			Balanced quality mode, requires less GPU memory but the disparity map is a little less detailed 
			//ZED_DEPTH_MODE_QUALITY		Better quality mode, the disparity map is more precise 


	//Depth postprocessing mode
	void setDepthPostprocess(int postprocess_mode);  //default: ZED_POSTPROCESS_STANDARD
			//ZED_DEPTH_POSTPROCESS_FILL			Occlusion filling, edge sharpening, advanced post-filtering.
			//ZED_DEPTH_POSTPROCESS_STANDARD		No occlusion filling


	//The minimum depth value that will be computed, mm
	void setDepthMinimumDistance(int min_dist);	
			//default: -1
			//for HD720 it is 525mm
			//To speedup, increase the value.

	//Vertical flip
	void setVFlip(bool vflip);			//default: false

	//Output some information about the current status of initialization
	void setVerboseOutput(bool verbose);	//default: false

private:
	//Settings
	sl::zed::InitParams params_;
	int resolution_ = ZED_RESOLUTION_HD720;
	float fps_ = 0.0;
	int postprocessMode_ = ZED_DEPTH_POSTPROCESS_STANDARD;

	bool useImages_ = true;
	bool useDepth_ = true;
	bool usePointCloud_ = true;
	bool usePointCloudColors_ = true;

	bool pointCloudFlipY_ = true;
	bool pointCloudFlipZ_ = true;

	//Camera
	sl::zed::Camera* zed_ = 0;
	int w_;
	int h_;

	//Buffers
	ofPixels leftPixels_, rightPixels_, depthPixels_grayscale_;
	ofTexture leftTexture_, rightTexture_, depthTexture_;
	ofFloatPixels depthPixels_mm_;
	vector<ofPoint> pointCloud_;
	vector<ofColor> pointCloudColors_;
	vector<ofFloatColor> pointCloudFloatColors_;

	//Flags for lazy updating
	bool leftPixelsDirty_, rightPixelsDirty_, leftTextureDirty_, rightTextureDirty_;
	bool depthPixels_mm_Dirty_, depthPixels_grayscale_Dirty_, depthTextureDirty_;
	bool pointCloudDirty_, pointCloudFloatColorsDirty_;
		
	
	void markBuffersDirty(bool dirty);	//Mark all buffers dirty (need to update by request)
	void fillPointCloud();

};


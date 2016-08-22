#include "ofxKuZed.h"

//------------------------------------------------------------------------------------------------------
ofxKuZed::ofxKuZed()
{
	setDepthModeQuality(ZED_DEPTH_MODE_QUALITY);
	markBuffersDirty(false);
}


//------------------------------------------------------------------------------------------------------
ofxKuZed::~ofxKuZed()
{
	close();
}

//------------------------------------------------------------------------------------------------------
void ofxKuZed::init()
{
	close();

//	bUseColorImage = useColorImage;
//	bUseDepthImage = useDepthImage;
	ofLog() << "Starting ZED camera..." << endl;
	zed_ = new sl::zed::Camera(sl::zed::ZEDResolution_mode(resolution_), fps_);
	sl::zed::ERRCODE zederr = zed_->init(params_);

	if (zederr != sl::zed::SUCCESS)
	{
		ofLog() << "ERROR starting ZED: " << sl::zed::errcode2str(zederr) << endl;
		close();
	}
	else
	{
		ofLog() << "ZED started." << endl;
	}

	//We will allocate buffers anyway, even if no camera
	w_ = (started()) ? zed_->getImageSize().width : 1280;	
	h_ = (started()) ? zed_->getImageSize().height : 720;

	depthPixels_grayscale_.allocate(w_, h_, 1);
	depthPixels_mm_.allocate(w_, h_, 1);

	leftPixels_.allocate(w_, h_, 3);
	rightPixels_.allocate(w_, h_, 3);

	leftTexture_.allocate(w_, h_, GL_RGB, false);
	rightTexture_.allocate(w_, h_, GL_RGB, false);
	depthTexture_.allocate(w_, h_, GL_LUMINANCE, false);

}

//------------------------------------------------------------------------------------------------------
void ofxKuZed::close() {
	if (zed_) {
		ofLog() << "Closing ZED..." << endl;
		delete zed_;
		zed_ = 0;
		markBuffersDirty(false);
	}
}

//------------------------------------------------------------------------------------------------------
void ofxKuZed::setUseImages(bool useImages)
{
	useImages_ = useImages;
}

//------------------------------------------------------------------------------------------------------
void ofxKuZed::setUseDepth(bool useDepth)
{
	useDepth_ = useDepth;
}

//------------------------------------------------------------------------------------------------------
void ofxKuZed::setUsePointCloud(bool usePointCloud, bool usePointCloudColors, bool flipY, bool flipZ)
{
	usePointCloud_ = usePointCloud;
	usePointCloudColors_ = usePointCloudColors;
	pointCloudFlipY_ = flipY;
	pointCloudFlipZ_ = flipZ;
}

//------------------------------------------------------------------------------------------------------
//Mark all buffers dirty (need to update by request)
void ofxKuZed::markBuffersDirty(bool dirty) {
	//Mark all buffers dirty (need to update by request)
	leftPixelsDirty_ = dirty;
	rightPixelsDirty_ = dirty;
	leftTextureDirty_ = dirty;
	rightTextureDirty_ = dirty;
	depthPixels_mm_Dirty_ = dirty;
	depthPixels_grayscale_Dirty_ = dirty;
	depthTextureDirty_ = dirty;
	pointCloudDirty_ = dirty;
	pointCloudFloatColorsDirty_ = dirty;
}
//------------------------------------------------------------------------------------------------------
void ofxKuZed::update()
{
	if (started()) {
		if (useImages_ || useDepth_ || usePointCloud_) {
			//Grab data
			bool computeDepth = (useDepth_ || usePointCloud_);
			bool computeXYZ = usePointCloud_;
			zed_->grab(sl::zed::SENSING_MODE(postprocessMode_), computeDepth, computeDepth, computeXYZ);
			markBuffersDirty(true);
		}
	}

}

//------------------------------------------------------------------------------------------------------
bool ofxKuZed::started()
{
	return (zed_ != 0);
}

//------------------------------------------------------------------------------------------------------
bool ofxKuZed::isFrameNew()
{
	return true;
}

//------------------------------------------------------------------------------------------------------
int ofxKuZed::getWidth()
{
	return w_;
}

//------------------------------------------------------------------------------------------------------
int ofxKuZed::getHeight()
{
	return h_;
}

//------------------------------------------------------------------------------------------------------
ofFloatPixels & ofxKuZed::getDepthPixels_mm()
{
	if (started()) {
		if (depthPixels_mm_Dirty_) {
			depthPixels_mm_Dirty_ = false;
			sl::zed::Mat zedView = zed_->retrieveMeasure(sl::zed::MEASURE::DEPTH);

			float *pix = depthPixels_mm_.getData();
			int step = zedView.step / 4;
			for (int y = 0; y < h_; y++) {
				for (int x = 0; x < w_; x++) {
					float pixel = ((float*)(zedView.data))[x + step*y];
					pix[x + y * w_] = pixel;
				}
			}
		}
	}

	return depthPixels_mm_;
}

//------------------------------------------------------------------------------------------------------
ofPixels & ofxKuZed::getDepthPixels_grayscale(float min_depth_mm, float max_depth_mm)
{
	if (started()) {
		if (depthPixels_grayscale_Dirty_) {
			depthPixels_grayscale_Dirty_ = false;

			sl::zed::Mat zedView = zed_->normalizeMeasure(sl::zed::MEASURE::DEPTH, min_depth_mm, max_depth_mm);

			uchar *pix = depthPixels_grayscale_.getData();

			//ofLog() << zedView.width << " // " << zedView.height << endl;
			for (int y = 0; y < h_; y++) {
				for (int x = 0; x < w_; x++) {
					sl::uchar3 pixel = zedView.getValue(x, y);
					int index = (x + y * w_);
					pix[index] = pixel.c1;
				}
			}
		}
	}

	return depthPixels_grayscale_;
}

//------------------------------------------------------------------------------------------------------
ofTexture &ofxKuZed::getDepthTexture(float min_depth_mm, float max_depth_mm)
{
	if (started()) {
		if (!useDepth_) {
			ofLogWarning() << "ZED: trying to access depth buffer. You need to call setUseDepth(true) before it!" << endl;
		}
		else {
			if (depthTextureDirty_) {
				depthTextureDirty_ = false;
				depthTexture_.loadData(getDepthPixels_grayscale(min_depth_mm, max_depth_mm));
			}
		}
	}
	return depthTexture_;
}

//------------------------------------------------------------------------------------------------------
ofPixels & ofxKuZed::getLeftPixels()
{
	if (started()) {
		if (!useImages_) {
			ofLogWarning() << "ZED: trying to access left image pixels. You need to call setUseImages(true) before it!" << endl;
		}
		else {
			if (leftPixelsDirty_) {
				leftPixelsDirty_ = false;
				sl::zed::Mat zedView = zed_->retrieveImage(sl::zed::SIDE::LEFT);
				uchar *pix = leftPixels_.getData();

				for (int y = 0; y < h_; y++) {
					for (int x = 0; x < w_; x++) {
						sl::uchar3 pixel = zedView.getValue(x, y);
						int index = 3 * (x + y * w_);
						pix[index + 0] = pixel.c3;
						pix[index + 1] = pixel.c2;
						pix[index + 2] = pixel.c1;
					}
				}
			}
		}
	}
	return leftPixels_;
}


//------------------------------------------------------------------------------------------------------
ofTexture & ofxKuZed::getLeftTexture()
{
	if (started()) {
		if (!useImages_) {
			ofLogWarning() << "ZED: trying to access left image. You need to call setUseImages(true) before it!" << endl;
		}
		else {
			if (leftTextureDirty_) {
				leftTextureDirty_ = false;
				leftTexture_.loadData(getLeftPixels());
			}
		}
	}
	return leftTexture_;
}

//------------------------------------------------------------------------------------------------------
ofPixels & ofxKuZed::getRightPixels()
{
	if (started()) {
		if (!useImages_) {
			ofLogWarning() << "ZED: trying to access right image pixels. You need to call setUseImages(true) before it!" << endl;
		}
		else {
			if (rightPixelsDirty_) {
				rightPixelsDirty_ = false;
				sl::zed::Mat zedView = zed_->retrieveImage(sl::zed::SIDE::RIGHT);
				uchar *pix = rightPixels_.getData();
				for (int y = 0; y < h_; y++) {
					for (int x = 0; x < w_; x++) {
						sl::uchar3 pixel = zedView.getValue(x, y);
						int index = 3 * (x + y * w_);
						pix[index + 0] = pixel.c3;
						pix[index + 1] = pixel.c2;
						pix[index + 2] = pixel.c1;
					}
				}
			}
		}
	}
	return rightPixels_;
}

//------------------------------------------------------------------------------------------------------
ofTexture & ofxKuZed::getRightTexture()
{
	if (started()) {
		if (!useImages_) {
			ofLogWarning() << "ZED: trying to access right image. You need to call setUseImages(true) before it!" << endl;
		}
		else {
			if (rightTextureDirty_) {
				rightTextureDirty_ = false;
				rightTexture_.loadData(getRightPixels());
			}
		}
	}
	return rightTexture_;
}

//------------------------------------------------------------------------------------------------------
void ofxKuZed::fillPointCloud() {
	if (started()) {
		if (!usePointCloud_) {
			ofLogWarning() << "ZED: trying to access point cloud. You need to call setUsePointCloud(true,true|false) before it!" << endl;
		}
		else {
			if (pointCloudDirty_) {
				pointCloudDirty_ = false;

				if (!usePointCloudColors_) {
					sl::zed::Mat zedView = (zed_->retrieveMeasure(sl::zed::MEASURE::XYZ));
					//XYZ, 3D coordinates of the image points, 4 channels, FLOAT  (the 4th channel may contains the colors)

					int w = zedView.width;
					int h = zedView.height;
					int step = zedView.step / 4;
					pointCloud_.resize(w*h);
					pointCloudColors_.clear();

					float *data = (float*)(zedView.data);
					for (int y = 0; y < h; y++) {
						for (int x = 0; x < w; x++) {
							int index = x * 4 + step*y;
							pointCloud_[x+w*y] = ofPoint(data[index], data[index + 1], data[index + 2]);
						}
					}
				}
				else {
					sl::zed::Mat zedView = (zed_->retrieveMeasure(sl::zed::MEASURE::XYZRGBA));
					//XYZRGBA, 3D coordinates and Color of the image , 4 channels, FLOAT (the 4th channel encode 4 UCHAR for color) \ingroup Enumerations*/
					int w = zedView.width;
					int h = zedView.height;
					int step = zedView.step / 4;
					int step_char = zedView.step;
					pointCloud_.resize(w*h);
					pointCloudColors_.resize(w*h);

					float *data = (float*)(zedView.data);
					uchar *data_char = zedView.data;
					for (int y = 0; y < h; y++) {
						for (int x = 0; x < w; x++) {
							int index = x * 4 + step*y;
							int index_color = (index+3)*4;
							pointCloud_[x + w*y] = ofPoint(data[index], data[index + 1], data[index + 2]);
							pointCloudColors_[x + w*y] = ofColor(data_char[index_color], data_char[index_color + 1], data_char[index_color + 2], data_char[index_color + 3]);
						}
					}
				}
				//flip points if required
				if (pointCloudFlipY_) {
					for (size_t i = 0; i < pointCloud_.size(); i++) {
						pointCloud_[i].y = -pointCloud_[i].y;
					}
				}
				if (pointCloudFlipZ_) {
					for (size_t i = 0; i < pointCloud_.size(); i++) {
						pointCloud_[i].z = -pointCloud_[i].z;
					}
				}
			}
		}
	}

}

//------------------------------------------------------------------------------------------------------
vector<ofPoint>& ofxKuZed::getPointCloud()
{
	fillPointCloud();
	return pointCloud_;
}

//------------------------------------------------------------------------------------------------------
vector<ofColor>& ofxKuZed::getPointCloudColors()
{
	fillPointCloud();
	return pointCloudColors_;
}

//------------------------------------------------------------------------------------------------------
vector<ofFloatColor>& ofxKuZed::getPointCloudFloatColors()
{
	if (pointCloudFloatColorsDirty_) {
		pointCloudFloatColorsDirty_ = false;
		fillPointCloud();
		//convert pointCloudColors_ to pointCloudFloatColors_
		size_t n = pointCloudColors_.size();
		pointCloudFloatColors_.resize(n);
		for (size_t i = 0; i < n; i++) {
			pointCloudFloatColors_[i] = pointCloudColors_[i];
		}

	}
	return pointCloudFloatColors_;
}

//------------------------------------------------------------------------------------------------------
void ofxKuZed::drawLeft(float x, float y, float w, float h)
{
	if (w == 0) w = w_;
	if (h == 0) h = h_;
	getLeftTexture().draw(x, y, w, h);
}

//------------------------------------------------------------------------------------------------------
void ofxKuZed::drawRight(float x, float y, float w, float h)
{
	if (w == 0) w = w_;
	if (h == 0) h = h_;
	getRightTexture().draw(x, y, w, h);
}

//------------------------------------------------------------------------------------------------------
void ofxKuZed::drawDepth(float x, float y, float w, float h, float min_mm, float max_mm)
{
	if (w == 0) w = w_;
	if (h == 0) h = h_;
	getDepthTexture(min_mm, max_mm).draw(x, y, w, h);

}

//------------------------------------------------------------------------------------------------------
void ofxKuZed::drawPointCloud()
{
	vector<ofPoint> &points = getPointCloud();
	vector<ofFloatColor> &colors = getPointCloudFloatColors();
	ofMesh mesh;
	mesh.addVertices(points);
	if (colors.size() == points.size()) mesh.addColors(colors);
	mesh.drawVertices();
}

//------------------------------------------------------------------------------------------------------
void ofxKuZed::setResolution(int zed_resolution)
{
	resolution_ = zed_resolution;
}

//------------------------------------------------------------------------------------------------------
void ofxKuZed::setFps(float fps)
{
	fps_ = fps;
}

float ofxKuZed::getFps()
{
	return fps_;
}

//------------------------------------------------------------------------------------------------------
void ofxKuZed::setGpuDevice(int gpu_id)
{
	params_.device = gpu_id;
}

//------------------------------------------------------------------------------------------------------
void ofxKuZed::setDepthModeQuality(int depth_mode)
{
	params_.mode = sl::zed::MODE(depth_mode);
}

//------------------------------------------------------------------------------------------------------
void ofxKuZed::setDepthPostprocess(int postprocess_mode)
{
	postprocessMode_ = postprocess_mode;
}

//------------------------------------------------------------------------------------------------------
void ofxKuZed::setDepthMinimumDistance(int min_dist)
{
	params_.minimumDistance = min_dist;
}

//------------------------------------------------------------------------------------------------------
void ofxKuZed::setVFlip(bool vflip)
{
	params_.vflip = vflip;
}

//------------------------------------------------------------------------------------------------------
void ofxKuZed::setVerboseOutput(bool verbose)
{
	params_.verbose = verbose;
}

//------------------------------------------------------------------------------------------------------

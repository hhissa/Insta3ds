#pragma once

#include <3ds.h>
#include <jpeglib.h>

class ImageBuffer 
{
    private:

    public:
	void initBuffers();
	void createJPEGS();
	void WritePicturetoFrameBufferRGB565();
	void WritePictureToJPEGRGB565();

}

//represents the camera data
class CameraModel
{
    private:
	std::stack<std::pair<int, int>> CameraContextPairs;

	void SetCamera(int _Camera);
	int GetCurrentCamera();

	void UpdateContext();
	int GetCurrentContext();


    public:
	Camera
	void initCamera(int _Camera);
}

// controls display of camera data
class CameraView
{

}

// interface for the camera data
class CameraController
{
    private:
        CameraModel* model;
	CameraView* view;
    public:
	CameraController(CameraModel* _m, CameraView* _v)
	{
	    model = _m;
	    view = _v;

	    initCamera();
	}
	
	// ask the user which cameras they want want (view)
	//
	// store that selection in the model (model)
	//
	// display that to the user (view)
	void initCamera();

}

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <deque>

#include <3ds.h>
#include <citro2d.h>
#include <jpeglib.h>


#define WIDTH 400
#define HEIGHT 240
#define SCREEN_SIZE WIDTH *HEIGHT * 2
#define BUF_SIZE SCREEN_SIZE * 2

class ImageBuffer 
{

    public:
	void initBuffers();
	void createJPEGS();
	void writePicturetoFrameBufferRGB565(void *fb, void* img, u16 x, u16 y, u16 width, u16 height);
	void WritePictureToJPEGRGB565();

};

//represents the camera data
class CameraModel
{
    private:
	std::deque<std::pair<u32, CAMU_Context>> CameraContextPairs;

	void setCameraContextPair(u32 _Camera);

	void updateContext();

    public:
	CameraModel()
	{
	    CameraContextPairs.emplace_back(SELECT_NONE, CONTEXT_NONE);
	}

	std::pair<u32, CAMU_Context> getCurrentCameraContextPair();

	void initCamera(u32 _Camera, CAMU_Size _Display);

	void takePicture(u8* buf);

	void cleanup();
};

// controls display of camera data
class CameraView
{
    private:
        touchPosition touch;
	C3D_RenderTarget* top;
	C3D_RenderTarget* bottom;

	u32 curCamera;
        
	void newFrame(C3D_RenderTarget *target);
        void endFrame();
    public:
	CameraView()
	{
	    gfxInitDefault();
            C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	    C2D_Prepare();

	    top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	    bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT); 
	}
	void initView();

        int CameraSelectionButton(int x, int y, u32 clr);

	void cleanup();

};

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

	};
	
	void initController();
	// ask the user which cameras they want want (view)
	//
	// store that selection in the model (model)
	//
	// display that to the user (view)
	void selectCamera();

	//capture img to framebuffer
	//
	//display the img to fb
	//
	

	//close all services
	void cleanup();
	

};

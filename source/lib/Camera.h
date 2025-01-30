#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <deque>
#include <vector>

#include <3ds.h>
#include <citro2d.h>
#include <jpeglib.h>


#define WIDTH 400
#define HEIGHT 240
#define SCREEN_SIZE WIDTH *HEIGHT * 2
#define BUF_SIZE SCREEN_SIZE * 2

#define WAIT_TIMEOUT 300000000ULL

class ImageBuffer 
{
    private:
	u8* buffer;
    public:
	u8* getBuffer();
	void initBuffers();
	void freeBuffers();
	void createJPEGS();
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
	    CameraContextPairs.emplace_back(SELECT_OUT1, CONTEXT_A);
	}

	std::pair<u32, CAMU_Context> getCurrentCameraContextPair();

	void initCamera(u32 _Camera, CAMU_Size _Display);

	//return number of activated cameras
	int getNumCams(u32 x);

	void takePicture(u8* buf);

	void cleanup();
};

// controls display of camera data
class CameraView
{
    private:
        touchPosition touch;
	u32 curCamera;
        
	void newFrame(C3D_RenderTarget *target);
        void endFrame();
    public:

        C3D_RenderTarget* top;
	C3D_RenderTarget* bottom;

	CameraView()
	{
	    gfxInitDefault();

            gfxSetDoubleBuffering(GFX_TOP, true);
            gfxSetDoubleBuffering(GFX_BOTTOM, false);

            C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	    C2D_Prepare();

	    top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	    bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT); 
	}

	void initView();

        void writePicturetoFramebufferRGB565(void *fb, void* img, u16 x, u16 y, u16 width, u16 height);

        int CameraSelectionButton(int x, int y, u32 clr);

	void presentImage(u8* img);

	void cleanup();

};

// interface for the camera data
class CameraController
{
    private:
        CameraModel* model;
	CameraView* view;
	ImageBuffer* buf;

    public:
	CameraController(CameraModel* _m, CameraView* _v, ImageBuffer* _b)
	{
	    model = _m;
	    view = _v;
            buf = _b;
	    buf->initBuffers();
	};
	
	void initController();
	// ask the user which cameras they want want (view)
	//
	// store that selection in the model (model)
	//
	// display that to the user (view)
	void selectCamera();

	//capture img to buffer
	void takePhoto();

	//display image
	void presentPhoto();

	//close all services
	void cleanup();
	

};

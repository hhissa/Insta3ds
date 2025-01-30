#include "Camera.h"

//1: Image Buffer

void ImageBuffer::initBuffers()
{
    buffer = new u8[BUF_SIZE];
}

void ImageBuffer::freeBuffers()
{
    delete buffer;
}

u8* ImageBuffer::getBuffer()
{
    return buffer;
}

//2: CameraModel

std::pair<u32, CAMU_Context> CameraModel::getCurrentCameraContextPair()
{
    return CameraContextPairs.back();
}

void CameraModel::setCameraContextPair(u32 _Camera)
{
    if(CameraContextPairs.size() >= 2)
    {
	CameraContextPairs.pop_front();
    }
    //figure out how to switch context like this !CAMU_context
    CameraContextPairs.emplace_back(_Camera, CameraContextPairs.back().second);

}

int CameraModel::getNumCams(u32 x)
{    
    return __builtin_popcount(x);
}

void CameraModel::initCamera(u32 _Camera, CAMU_Size _Display)
{
    std::pair<u32, CAMU_Context> _curCameraPair = getCurrentCameraContextPair();
    u32 _curCamera = _curCameraPair.first;
    CAMU_Context _curContext = _curCameraPair.second;

    if(_curCamera != _Camera)
    {
	setCameraContextPair(_Camera);
    }

    camInit();
    CAMU_SetSize(_curCamera, _Display, _curContext);
    CAMU_SetOutputFormat(_curCamera, OUTPUT_RGB_565, _curContext);
    CAMU_SetNoiseFilter(_curCamera, true);
    CAMU_SetAutoExposure(_curCamera, true);
    CAMU_SetAutoWhiteBalance(_curCamera, true);
}

void CameraModel::takePicture(u8 *buf)
{
    std::pair<u32, CAMU_Context> _curCameraPair = getCurrentCameraContextPair();
    u32 _curCamera = _curCameraPair.first;
    CAMU_Context _curContext = _curCameraPair.second;

    u32 bufSize;
    CAMU_GetMaxBytes(&bufSize, WIDTH, HEIGHT);
    CAMU_SetTransferBytes(PORT_BOTH, bufSize, WIDTH, HEIGHT);

    int numCams = getNumCams(_curCamera);
    CAMU_Activate(_curCamera);
    
    /*inizialize capture handles and clear buffers*/
    std::vector<Handle> camReceiveEvents = {};
    
    for (int i = 0; i < numCams; i++)
    {
	camReceiveEvents[i] = 0;
    }
    
    CAMU_ClearBuffer(PORT_BOTH);
    CAMU_SynchronizeVsyncTiming(SELECT_OUT1, SELECT_OUT2);

    CAMU_StartCapture(PORT_BOTH);

    for (int i = 0; i < numCams; i++)
    {
	CAMU_SetReceiving(&camReceiveEvents[i], buf + (i * SCREEN_SIZE), i%2==0?PORT_CAM1:PORT_CAM2, SCREEN_SIZE, (s16)bufSize);
        svcWaitSynchronization(camReceiveEvents[i], WAIT_TIMEOUT);
    }
    
    CAMU_PlayShutterSound(SHUTTER_SOUND_TYPE_NORMAL);

    /*stop capture, close handles and clear active cameras*/
    CAMU_StopCapture(PORT_BOTH);

    for (int i = 0; i < numCams; i++)
    {
	    svcCloseHandle(camReceiveEvents[i]);
    }

    CAMU_Activate(SELECT_NONE);
}

void CameraModel::cleanup()
{
    camExit();
}

//3: CameraView

void CameraView::newFrame(C3D_RenderTarget *target)
{
    u32 clrClear = C2D_Color32(0xFF, 0xD8, 0xB0, 0x68);
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(target, clrClear);
    C2D_SceneBegin(target);
    
}

void CameraView::endFrame()
{
    C3D_FrameEnd(0);
}

int CameraView::CameraSelectionButton(int x, int y, u32 clr)
{
    int hit = 0;
    newFrame(bottom);
    hidTouchRead(&touch);
    if(sqrt(pow(abs(x-touch.px), 2) + pow(abs(y - touch.py), 2)) < 25)
    {
	hit = 1;
	clr = C2D_Color32(0x00, 0xFF, 0x00, 0xFF);
    } 
    C2D_DrawCircleSolid(x, y, 0, 25, clr);
    endFrame();
    return hit;
}

void CameraView::writePicturetoFramebufferRGB565(void *fb, void* img, u16 x, u16 y, u16 width, u16 height) 
{
    u8 *fb_8 = (u8*) fb;
    u16 *img_16 = (u16*) img;
    int i, j, draw_x, draw_y;
    for(j = 0; j < height; j++) {
	for(i = 0; i < width; i++) {
	    draw_y = y + height - j;
	    draw_x = x + i;
	    u32 v = (draw_y + draw_x * height) * 3;
            u16 data = img_16[j * width + i];
	    uint8_t b = ((data >> 11) & 0x1F) << 3;
	    uint8_t g = ((data >> 5) & 0x3F) << 2;
	    uint8_t r = (data & 0x1F) << 3;
	    fb_8[v] = r;
	    fb_8[v+1] = g;
	    fb_8[v+2] = b;
        }
    }
}

void CameraView::cleanup()
{
    gfxExit();
}


//4: CameraController

void CameraController::selectCamera()
{
    //ask the user for the desired camera
    u32 clrWhite = C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF);
    int hit = view->CameraSelectionButton(200, 120, clrWhite);
    std::pair<u32, CAMU_Context> _p = model->getCurrentCameraContextPair();

    //set desired cam

    model->initCamera((_p.first + hit)%3, SIZE_CTR_TOP_LCD);

}

void CameraController::takePhoto()
{
    u8* buffer = buf->getBuffer();
    model->takePicture(buffer);
}

void CameraController::presentPhoto()
{
    u8* buffer = buf->getBuffer();
    view->writePicturetoFramebufferRGB565(view->top, buffer, 0, 0, WIDTH, HEIGHT);
}

void CameraController::cleanup()
{
    model->cleanup();
    view->cleanup();
    buf->freeBuffers();
    acExit();
}

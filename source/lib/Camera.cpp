#include "Camera.h"

//1: Image Buffer
//
void ImageBuffer::writePicturetoFrameBufferRGB565(void *fb, void* img, u16 x, u16 y, u16 width, u16 height) {
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

CameraModel::cleanup()
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
    C2D_DrawCircleSolid(x, y, 0, 25, clr);
    hidTouchRead(&touch);
    if(sqrt(pow(abs(touch.px - x), 2) + pow(abs(touch.py - y), 2)) < 25)
    {
	hit = 1;
    }
    endFrame();
    return hit;
}

void cleanup()
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

void cleanup()
{
    model->cleanup();
    view->cleanup();
    acExit();
}

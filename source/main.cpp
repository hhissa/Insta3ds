
#include <3ds.h>
#include <stdio.h>
#include <jpeglib.h>
#include "lib/Camera.h"
int main()
{
    acInit();
    gfxInitDefault();

    gfxSetDoubleBuffering(GFX_TOP, true);
    gfxSetDoubleBuffering(GFX_BOTTOM, false);

    u32 kDown;
    u32 kHeld;

    CameraModel _m;
    CameraView _v;
    CameraController controller(&_m, &_v);

    while (aptMainLoop())
    {
	hidScanInput();
        kDown = hidKeysDown();
	kHeld = hidKeysHeld();

	if (kDown & KEY_START)
	{
	    break;
	}

	controller.selectCamera();
    }
}


#include <3ds.h>
#include <stdio.h>
#include <jpeglib.h>
#include "lib/Camera.h"
int main()
{
    acInit();    

    u32 kDown;
    u32 kHeld;

    CameraModel _m;
    CameraView _v;
    ImageBuffer _b;
    CameraController controller(&_m, &_v, &_b);

    gfxFlushBuffers();
    gspWaitForVBlank();
    gfxSwapBuffers();

    bool held_R = false;

    while (aptMainLoop())
    {
	hidScanInput();
        kDown = hidKeysDown();
	kHeld = hidKeysHeld();

	if (kDown & KEY_START)
	{
	    break;
	}
	
	if ((kHeld & KEY_R) && !held_R)
	{
	    
	    gfxFlushBuffers();
            gspWaitForVBlank();
	    gfxSwapBuffers();
	    held_R = true;
	    controller.takePhoto();
	}

        else if (!(kHeld & KEY_R))
	{
	    held_R = false;
	}

	controller.presentPhoto();
        gfxFlushBuffers();
	gspWaitForVBlank();
	gfxSwapBuffers();
    }

    controller.cleanup();

    return 0;
}

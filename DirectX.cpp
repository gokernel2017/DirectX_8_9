//-------------------------------------------------------------------
//
// Template DirectX 8 & 9:
//
// FILE:
//   DirectX.cpp
//
// COMPILE:
//
//   g++ DirectX.cpp -o DirectX -ld3dx8d -ld3d8 -lwinmm -DDX8
//   or
//   g++ DirectX.cpp -o DirectX -ld3dx9d -ld3d9 -lwinmm -DDX9
//
//   COMPILE DIRECTX 8:
//     compile.bat
//
// PROJECT:
//   https://github.com/gokernel2017/directx_8_9
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#include <windows.h>
#include <stdio.h>
#include <time.h>

#ifdef DX8
    #include "DX8/d3dx8.h"
    typedef LPDIRECT3D8         LPDIRECT3D;
    typedef LPDIRECT3DDEVICE8   LPDIRECT3DDEVICE;
    #define Direct3DCreate      Direct3DCreate8
#endif
#ifdef DX9
    #include "DX9/d3dx9.h"
    typedef LPDIRECT3D9         LPDIRECT3D;
    typedef LPDIRECT3DDEVICE9   LPDIRECT3DDEVICE;
    #define Direct3DCreate      Direct3DCreate9
#endif

struct Vertex {
    float x, y, z, rhw;
    DWORD color;
};

static char       ClassName[] = "Application_Class_Name";
LPDIRECT3D D3D = NULL;
LPDIRECT3DDEVICE  device = NULL;
int color = D3DCOLOR_XRGB(255,130,30);

HFONT hFont;// =  (HFONT)GetStockObject (DEFAULT_GUI_FONT);//SYSTEM_FONT);
LPD3DXFONT pFont = NULL;
HRESULT r = 0;

//-------------------------------------------------------------------
//
// Frame Rate API:
//
// Copyright (C) 2001-2012  Andreas Schiffler
//
//-------------------------------------------------------------------
#define FPS_MIN		30
#define FPS_MAX   200
#define UINT32    unsigned int

struct FRAME_RATE {
    UINT32  framecount;
		float   rateticks;
		UINT32  baseticks;
		UINT32  lastticks;
		UINT32  rate;
}FPS;

UINT32 FPS_GetTicks (void) {
    UINT32 ticks = timeGetTime();

    /*
    * Since baseticks!=0 is used to track initialization
    * we need to ensure that the tick count is always >0
    * since SDL_GetTicks may not have incremented yet and
    * return 0 depending on the timing of the calls.
    */
    if (ticks == 0)
        return 1;
    else
        return ticks;
}
void FPS_Init (void) {
    FPS.framecount = 0;
    FPS.rate = FPS_MIN;
    FPS.rateticks = (1000.0f / (float) FPS_MIN);
    FPS.baseticks = FPS_GetTicks();
    FPS.lastticks = FPS.baseticks;
    printf ("\nUsing Frame Rate API:\n");
    printf ("Copyright (C) 2001-2012  Andreas Schiffler\n\n");

}
void FPS_SetFps (UINT32 rate) {
    if ((rate >= FPS_MIN) && (rate <= FPS_MAX)) {
		    FPS.framecount = 0;
		    FPS.rate = rate;
		    FPS.rateticks = (1000.0f / (float) rate);
    }
}
void FPS_Delay (void) {
    UINT32 current_ticks;
    UINT32 target_ticks;

    FPS.framecount++;

    // Get/calc ticks
    //
    current_ticks = FPS_GetTicks();
    FPS.lastticks = current_ticks;
    target_ticks = FPS.baseticks + (UINT32) ((float) FPS.framecount * FPS.rateticks);

    if (current_ticks <= target_ticks) {
        Sleep (target_ticks - current_ticks);
    } else {
        FPS.framecount = 0;
		    FPS.baseticks = FPS_GetTicks();
    }
}

static LRESULT WINAPI WindowProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

    case WM_DESTROY:
        PostQuitMessage (0); 
        break;

    default:
        return DefWindowProc (hwnd, msg, wParam, lParam);
    }
    return 0;
}

LPDIRECT3DDEVICE gxCreateDevice (HWND hwnd, int FullScreen) {
    LPDIRECT3DDEVICE dev = NULL;
    D3DPRESENT_PARAMETERS d3dpp;
    D3DDISPLAYMODE d3ddm;

    if ((D3D = Direct3DCreate (D3D_SDK_VERSION)) == NULL)
  return NULL;

    if (FAILED(D3D->GetAdapterDisplayMode (D3DADAPTER_DEFAULT, &d3ddm)))
  return NULL;

    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = d3ddm.Format;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.Windowed   = TRUE;

    // Create the D3DDevice
    if (FAILED(D3D->CreateDevice (
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hwnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING, //D3DCREATE_HARDWARE_VERTEXPROCESSING
        &d3dpp,
        &dev
        )))
        return NULL;

    return dev;
}

HWND gxInit (int x, int y, int w, int h) {
    WNDCLASSEX wc;
    wc.cbSize			   = sizeof(WNDCLASSEX);
    wc.style			   = CS_CLASSDC;
    wc.lpfnWndProc   = WindowProc;
    wc.cbClsExtra		 = 0;
    wc.cbWndExtra		 = 0;
    wc.hInstance		 = GetModuleHandle (NULL);
    wc.hIcon			   = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor		   = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_BACKGROUND;//COLOR_WINDOW+1;
    wc.lpszMenuName	 = NULL;
    wc.lpszClassName = ClassName;
    wc.hIconSm		   = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))
        return NULL;

    x = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
    y = ((GetSystemMetrics(SM_CYSCREEN) - h) / 2) - (GetSystemMetrics(SM_CYCAPTION)/2);

    HWND win = CreateWindowEx (
        0,                        // Extended possibilites for variation
        ClassName,                // Classname
        #ifdef DX8
        "GX API ( OpenGL/DirectX ) : DirectX 8",
        #endif
        #ifdef DX9
        "GX API ( OpenGL/DirectX ) : DirectX 9",
        #endif
        #ifdef GL
        "GX API ( OpenGL/DirectX ) : OpenGL",
        #endif
        // WS_OVERLAPPEDWINDOW,   // default window
        WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
        x, y, w, h,
        HWND_DESKTOP,             // The window is a child-window to desktop
        NULL,                     // No menu
        GetModuleHandle (NULL),   // Program Instance handler
        NULL                      // No Window Creation data
        );

    return win;
}

void gxRun (void(*idle)(void)) {
    MSG msg;
    if (idle) {
        for (;;) {
            if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE)) {
                if(msg.message == WM_QUIT)
                    break;
                TranslateMessage (&msg);
                DispatchMessage (&msg);
            }
            idle ();
        }
    } else {
        while (GetMessage (&msg, NULL, 0, 0)) {
            TranslateMessage (&msg);
            DispatchMessage  (&msg);
        }
    }
}// gxRun ()

void gxBeginScene (void) {
//    device->Clear (0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
    device->Clear (0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
    device->BeginScene ();
}
void gxEndScene (void) {
    device->EndScene ();
    device->Present (NULL, NULL, NULL, NULL);
}

void gxDrawRect (int x, int y, int w, int h) {

    Vertex p[] = {
        { x, y,   0, 1, color },    { x+w, y,    0, 1, color },  // --
        { x+w, y, 0, 1, color },    { x+w, y+h,  0, 1, color },  // |
        { x+w, y+h, 0, 1, color },  { x, y+h,  0, 1, color },  // --
        { x, y+h, 0, 1, color },    { x, y,  0, 1, color }  // |
    };

    #define SVertexType D3DFVF_XYZRHW | D3DFVF_DIFFUSE

    #ifdef DX8
    device->SetVertexShader (SVertexType);
    #endif
    #ifdef DX9
    device->SetFVF (SVertexType);
    #endif
    device->DrawPrimitiveUP (D3DPT_LINELIST, 4, p, sizeof(Vertex));
}

int x = 100, count;
char buf[100] = { 'F', 'P', 'S', ':', ' ', '6', '0', 0, 0 };
void idle (void) {
    static int fps=0, t1=0, t2=0;

    gxBeginScene();

    gxDrawRect (x, 150, 320, 240);
    x += 2;
    if (x > 750) x = 10;

    fps++;
    t1 = time(NULL);
    if (t1 != t2) {
        t2 = t1;
        sprintf (buf, "FPS(Rate 60): %d | %d", fps, count);
        count++;
        fps=0;
    }


    if (r==S_OK) {
        // Rectangle where the text will be located
        RECT TextRect = {10,10,0,0};

        // Inform font it is about to be used
        pFont->Begin();

        // Calculate the rectangle the text will occupy
//        pFont->DrawText(buf, -1, &TextRect, DT_CALCRECT, 0 );

        // Output the text, left aligned
        //pFont->DrawText("Hello World 2", -1, &TextRect, DT_LEFT, color );
        pFont->DrawText (buf, -1, &TextRect, DT_LEFT, color );

        // Finish up drawing
        pFont->End();
    }


    gxEndScene();
    FPS_Delay ();
}


void CreateText (void) {

    hFont =  (HFONT)GetStockObject (SYSTEM_FIXED_FONT);

    // Create the D3DX Font
    r = D3DXCreateFont(device, hFont, &pFont);

}

int main (int argc, char **argv) {
    HWND win = NULL;

    if ((win = gxInit (100,100,800,600))) {

        if ((device = gxCreateDevice(win,1))) {

            #ifdef DX8
            printf ("DirextX 8 D3D_SDK_VERSION: %d\n", D3D_SDK_VERSION);
            #endif
            #ifdef DX9
            printf ("DirextX 9 D3D_SDK_VERSION: %d\n", D3D_SDK_VERSION);
            #endif

            ShowWindow (win,1);

            timeBeginPeriod (1);

            FPS_Init ();

            FPS_SetFps (60);

            CreateText ();

            gxRun (idle);

            // Free objects:
            if (pFont)
                pFont->Release();
		        device->Release();
		        D3D->Release();
        }
        else {
            printf ("DirectX Failed\n");
        }
    }

    printf ("Exiting With Sucess:\n");
}

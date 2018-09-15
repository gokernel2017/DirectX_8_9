//-------------------------------------------------------------------
//
// Template DirectX 8 & 9:
//
// FILE:
//   basic.cpp
//
// COMPILE:
//
//   g++ basic.cpp -o basic -ld3d8 -lwinmm -DDX8
//   or
//   g++ basic.cpp -o basic -ld3d9 -lwinmm -DDX9
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
//#include <windows.h>
#include <stdio.h>

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

static char       ClassName[] = "Application_Class_Name";
LPDIRECT3DDEVICE  device = NULL;


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
    LPDIRECT3D D3D = NULL;
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
        WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
        x, y, w, h,
        HWND_DESKTOP,             // The window is a child-window to desktop
        NULL,                     // No menu
        GetModuleHandle (NULL),   // Program Instance handler
        NULL                      // No Window Creation data
        );

    return win;
}

void gxRun (void) {
    MSG msg;

    while (GetMessage (&msg, NULL, 0, 0)) {
        TranslateMessage (&msg);
        DispatchMessage  (&msg);
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
    int color = D3DCOLOR_XRGB(255,130,30);
    struct Vertex {
        float x, y, z, rhw;
        DWORD color;
    } p[] = {
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

            gxBeginScene();
            gxDrawRect (100, 150, 320, 240);
            gxEndScene();

            gxRun ();
        }
        else {
            printf ("DirectX Failed\n");
        }
    }

    printf ("Exiting With Sucess:\n");
}

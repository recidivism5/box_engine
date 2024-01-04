//standard library includes:
#include <stdint.h>

//windows includes:
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define UNICODE
#define COBJMACROS
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>

//local includes:
#include <error.h>

IDXGISwapChain *swapChain;
ID3D11Device *device;
ID3D11DeviceContext *deviceContext;
ID3D11RenderTargetView *renderTargetView;
ID3D11DepthStencilView *depthStencilView;

LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LRESULT result = 0;
    switch(msg)
    {
        case WM_CREATE:{
            UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(DEBUG_BUILD)
            creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
            D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_11_0};
            DXGI_SWAP_CHAIN_DESC swapChainDesc = {
                .BufferDesc.Width = 0,
                .BufferDesc.Height = 0,
                .BufferDesc.RefreshRate.Numerator = 0,
                .BufferDesc.RefreshRate.Denominator = 0,
                .BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
                .BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
                .BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED,

                .SampleDesc.Count = 1,
                .SampleDesc.Quality = 0,

                .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
                .BufferCount = 1,

                .OutputWindow = hwnd,
                .Windowed = TRUE,
                .SwapEffect = DXGI_SWAP_EFFECT_DISCARD,
                .Flags = 0
            };
            ASSERT(SUCCEEDED(D3D11CreateDeviceAndSwapChain(
                0,
                D3D_DRIVER_TYPE_HARDWARE,
                0,
                creationFlags,
                featureLevels,
                ARRAYSIZE(featureLevels),
                D3D11_SDK_VERSION,
                &swapChainDesc,
                &swapChain,
                &device,
                0,
                &deviceContext
            )));

            ID3D11Texture2D *backBuffer;
            ASSERT(SUCCEEDED(swapChain->lpVtbl->GetBuffer(swapChain, 0, &IID_ID3D11Texture2D, (void**)&backBuffer)));
            ASSERT(SUCCEEDED(device->lpVtbl->CreateRenderTargetView(device, (ID3D11Resource *)backBuffer, 0, &renderTargetView)));

            D3D11_TEXTURE2D_DESC depthStencilDesc;
            backBuffer->lpVtbl->GetDesc(backBuffer, &depthStencilDesc);

            backBuffer->lpVtbl->Release(backBuffer);

            depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

            ID3D11Texture2D* depthStencil;

            device->lpVtbl->CreateTexture2D(device, &depthStencilDesc, 0, &depthStencil);
            device->lpVtbl->CreateDepthStencilView(device, (ID3D11Resource *)depthStencil, 0, &depthStencilView);

            depthStencil->lpVtbl->Release(depthStencil);
            break;
        }
        case WM_PAINT:{
            deviceContext->lpVtbl->OMSetRenderTargets(deviceContext, 1, &renderTargetView, depthStencilView);
            FLOAT clearColor[4] = {0.1f, 0.2f, 0.6f, 1.0f};
            deviceContext->lpVtbl->ClearRenderTargetView(deviceContext, renderTargetView, clearColor);
            deviceContext->lpVtbl->ClearDepthStencilView(deviceContext, depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
            swapChain->lpVtbl->Present(swapChain, 1, 0);
            return 0;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }
        case WM_SIZE:
        {
            //global_windowDidResize = true;
            break;
        }
        default:
            result = DefWindowProcW(hwnd, msg, wparam, lparam);
    }
    return result;
}

int main(int argc, char **argv){
    WNDCLASSEXW wcex = {
        .cbSize = sizeof(wcex),
        .style = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = wndProc,
        .hInstance = GetModuleHandleW(0),
        .hIcon = LoadIconW(0,IDI_APPLICATION),
        .hCursor = LoadCursorW(0,IDC_ARROW),
        .lpszClassName = L"box_engine",
        .hIconSm = LoadIconW(0,IDI_APPLICATION)
    };
    ASSERT(RegisterClassExW(&wcex));

    int width = 640;
    int height = 480;

    RECT initialRect = {0, 0, width, height};
    AdjustWindowRectEx(&initialRect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_OVERLAPPEDWINDOW);
    LONG initialWidth = initialRect.right - initialRect.left;
    LONG initialHeight = initialRect.bottom - initialRect.top;

    HWND hwnd = CreateWindowExW(
        WS_EX_OVERLAPPEDWINDOW,
        wcex.lpszClassName,
        L"Game",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        GetSystemMetrics(SM_CXSCREEN)/2-initialWidth/2,
        GetSystemMetrics(SM_CYSCREEN)/2-initialHeight/2,
        initialWidth, 
        initialHeight,
        0, 0, wcex.hInstance, 0);
    ASSERT(hwnd);

    MSG msg;
    while (GetMessageA(&msg,0,0,0)){
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return 0;
}

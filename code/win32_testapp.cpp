#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <xinput.h>
#include <dsound.h>

#include <math.h>

#define internal static

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef int32 bool32;

struct win32_offscreen_buffer
{
    // NOTE: Pixels are alwasy 32-bits wide, Memory Order BB GG RR XX
    BITMAPINFO Info;
    void *Memory;
    int Width;
    int Height;
    int Pitch;
};

struct win32_window_dimension
{
    int Width;
    int Height;
};

static bool GlobalRunning;
static win32_offscreen_buffer GlobalBackbuffer;

internal void Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int width, int height)
{
    if(Buffer->Memory)
    {
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
    }

    Buffer->Width = width;
    Buffer->Height = height;

    int BytesPerPixel = 4;

    // NOTE: When the biHeight field is negative, this is the clue to
    // Windows to treat this bitmap as top-down, not bottom-up, meaning that
    // the first three bytes of the image are the color for the top left pixel
    // in the bitmap, not the bottom left!
    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
    Buffer->Info.bmiHeader.biPlanes = 1;
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;

    int BitmapMemorySize = (Buffer->Width * Buffer->Height) * BytesPerPixel;
    Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
    Buffer->Pitch = width * BytesPerPixel;
}

internal void Win32DisplayBufferInWindow(
    HDC deviceContext,
    int WindowWidth,
    int WindowHeight,
    win32_offscreen_buffer Buffer)
{
    StretchDIBits(
        deviceContext,
        0, 0, Buffer.Width, Buffer.Height,
        0, 0, WindowWidth, WindowHeight,
        Buffer.Memory,
        &Buffer.Info,
        DIB_RGB_COLORS, SRCCOPY);
}

win32_window_dimension Win32GetWindowDimension(HWND hwnd)
{
    win32_window_dimension result;

    RECT ClientRect;
    GetClientRect(hwnd, &ClientRect);
    result.Width = ClientRect.right - ClientRect.left;
    result.Height = ClientRect.bottom - ClientRect.top;

    return result;
}

void PaintIt(HWND hwnd)
{
    PAINTSTRUCT Paint;
    HDC DeviceContext = BeginPaint(hwnd, &Paint);

    win32_window_dimension Dimension = Win32GetWindowDimension(hwnd);

    Win32DisplayBufferInWindow(
        DeviceContext,
        Dimension.Width,
        Dimension.Height,
        GlobalBackbuffer);

    EndPaint(hwnd, &Paint);
}

LRESULT CALLBACK MainProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    LRESULT result = 0;

    switch(msg)
    {
        case WM_DESTROY:
            OutputDebugString(L"WM_DESTROY\n");
            GlobalRunning = false;
        case WM_CLOSE:
            OutputDebugString(L"WM_CLOSE\n");
            GlobalRunning = false;
        case WM_ACTIVATEAPP:
            OutputDebugString(L"WM_ACTIVATEAPP\n");
            break;
        case WM_PAINT:
        {
            OutputDebugString(L"WM_PAINT\n");
            PaintIt(hwnd);
            break;
        }
        case WM_SIZE:
        {
            OutputDebugString(L"WM_SIZE\n");
        }
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            uint32 VKCode = wParam;
            bool WasDown = ((lParam & (1 << 30)) != 0);
            bool IsDown = ((lParam & (1 << 31)) == 0);

            // The check here ignores key-repeats.
            if (WasDown != IsDown)
            {
                if(VKCode == 'W')
                {
                }
                else if(VKCode == 'A')
                {
                }
                else if(VKCode == 'S')
                {
                }
                else if(VKCode == 'D')
                {
                }
                else if(VKCode == 'Q')
                {
                }
                else if(VKCode == 'E')
                {
                }
                else if(VKCode == VK_UP)
                {
                }
                else if(VKCode == VK_LEFT)
                {
                }
                else if(VKCode == VK_DOWN)
                {
                }
                else if(VKCode == VK_RIGHT)
                {
                }
                else if(VKCode == VK_ESCAPE)
                {
                    OutputDebugStringA("ESCAPE: ");
                    if(IsDown)
                    {
                        OutputDebugStringA("IsDown ");
                    }
                    if(WasDown)
                    {
                        OutputDebugStringA("WasDown");
                    }
                    OutputDebugStringA("\n");
                }
                else if(VKCode == VK_SPACE)
                {
                }
            }

            bool32 AltKeyWasDown = (lParam & (1 << 29));
            if (VKCode == VK_F4 && AltKeyWasDown)
            {
                GlobalRunning = false;
            }

             break;
        }
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return result;
}

int CALLBACK WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nShowCmd)
{

    WNDCLASS wc = {};
    Win32ResizeDIBSection(&GlobalBackbuffer, 1280, 720);

    wc.lpfnWndProc = MainProc;
    wc.lpszClassName = L"TestAppClass";
    wc.hInstance = hInstance;
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        L"Test App",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        0,
        0,
        hInstance,
        0);

    if (hwnd)
    {
        GlobalRunning = true;
        ShowWindow(hwnd, nShowCmd);
        HDC DeviceContext = GetDC(hwnd);

        while (GlobalRunning)
        {
            MSG msg = {};
            while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_QUIT)
                {
                    GlobalRunning = false;
                }

                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            win32_window_dimension Dimension = Win32GetWindowDimension(hwnd);
            Win32DisplayBufferInWindow(DeviceContext, Dimension.Width, Dimension.Height, GlobalBackbuffer);
        }
    }

    return 0;
}

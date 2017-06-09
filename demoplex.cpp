/*
    Demoplex, C version
    By leat & joric
    #gamedev_flame, 16 oct 2006
*/

#include <windows.h>
#include <math.h>

#pragma comment(lib,"gdi32")
#pragma comment(lib,"user32")

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define BMP_WIDTH 320
#define BMP_HEIGHT 200

#define CLASSNAME "Demoplex"

HINSTANCE g_hInstance;
HBITMAP g_hBitmap;
HDC g_hDC;
unsigned long g_dwPalette[256];
void *g_pBits = NULL;

unsigned short float2ushort(float num)
{
    return (unsigned short)(num + 0.5);
}

void InitPalette(void)
{
    unsigned char a;
    unsigned char b = 0xff;
    unsigned char c = 0;
    unsigned long offset = 0;
    unsigned char red, green, blue;

    do {
        a = c & 0xc0;
        if(a == 0) a = 0x20;
        b &= 0x3f; // 63, 0, 1, 2, ..., 63, 0, 1, 2, ...
        // blue
        if(a & 0x20) red = b << 2;
         else red = (((b >> 1)+b) >> 1) << 2;
        // green
        if(a & 0x40) green = b << 2;
         else green = (((b >> 1)+b) >> 1) << 2;
        // red
        if(a & 0x80) blue = b << 2;
         else blue = (((b >> 1)+b) >> 1) << 2;
        offset &= 0xff;
        g_dwPalette[offset] = (blue << 16) | (green << 8) | (red);
        b++;
        offset--;

    } while(--c);
}

void Function(void)
{
    static unsigned short iA = 1952;
    static unsigned short iB = 106;
    static unsigned short iC = 104;
    static unsigned short iD = 33788;
    static unsigned short iE = 61;
    static unsigned short iH = 0;
    static unsigned short sBX = 0x03C9;

    float lg2 = (float) log10(2.0);
    float lg2t = 1 / lg2;

    unsigned short ccc = 0;
    unsigned short iOffset = 0;
    unsigned char k = 0;

    while (1)
    {

        unsigned short d = sBX;
        float x = ((float) ((short) iC)) / ((float) ((short) iB));
        float y = ((float) ((short) iA)) / ((float) ((short) iB));
        unsigned char h = 192;

        while (2)
        {

            iD = d;

            float z =
                ((float) ((short) iD)) / ((float) ((short) iE));

            float sinz = sin(z);
            float cosz = cos(z);

            float a = y * cosz - sinz - x;
            float b = y * cosz - sinz;
            float c = y * sinz + cosz;

            short i = 3;
            float num = lg2;
            float tmp;

            do
            {
                if (a < 0.0f)
                    a = -a;
                if (a >= num)
                {
                    num = a;
                    ccc = i - 3;
                }
                tmp = a;
                a = b;
                b = c;
                c = tmp;
            }
            while (--i);

            while (i > ((short) ccc))
            {
                tmp = a;
                a = b;
                b = c;
                c = tmp;
                i--;
            }

            x = b / (a / 10.0f);
            y = c / (a / 10.0f);
            iD = float2ushort(y);
            iH = float2ushort(x * x + y * y);
            k = (((unsigned char) iH) >> 2) + h;
            iH = float2ushort(x);
            if (h != 192)
                break;
            if (iC < 2334)
                break;
            ccc++;

            if (ccc == 0)
            {
                d = d << 2;
                h = (0x40 & ((unsigned char) d)) - 0xb7;
                y -= lg2t;
                x = (x - lg2t) * y;
                y += x;
                k = 0xb7 | ((unsigned char) iD) |
                    ((unsigned char) iH);
                if (k)
                    continue;
            }
            ccc--;

            if (ccc == 0)
            {
                x -= (float) ((short) iH);
                y -= (float) ((short) iD);
                d = 0x3f;
                h = 0;
                iD--;
                if (((short) iD) > 0)
                    continue;
            }

            break;

        }

        if (iOffset < (BMP_WIDTH * BMP_HEIGHT))
        {
            (unsigned long) *((unsigned long *) g_pBits + iOffset) =
                g_dwPalette[k];
        }
        iOffset++;
        iA++;

        if (iA == 160)
        {
            iA = -iA;
            iC -= 4;
            if (iC == 65136)
            {
                iC = -iC;
                sBX += 4;
                break;
            }
        }
    }
}

void CreateBmp(HWND hWnd)
{
    BITMAPINFO binfo;
    HDC hdc = GetDC(hWnd);
    binfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    binfo.bmiHeader.biWidth = BMP_WIDTH;
    binfo.bmiHeader.biHeight = BMP_HEIGHT;
    binfo.bmiHeader.biPlanes = 1;
    binfo.bmiHeader.biBitCount = 32;
    binfo.bmiHeader.biCompression = 0;
    binfo.bmiHeader.biSizeImage = BMP_WIDTH * BMP_HEIGHT * 4;
    g_hBitmap =
        CreateDIBSection(hdc, &binfo, DIB_RGB_COLORS, &g_pBits, NULL,
                 NULL);
    g_hDC = CreateCompatibleDC(hdc);
    SelectObject(g_hDC, g_hBitmap);
    ReleaseDC(hWnd, hdc);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{


    PAINTSTRUCT ps;
    HDC hdc;
    RECT rect;
    switch (message)
    {

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        Function();
        GetClientRect(hWnd, &rect);
        SetStretchBltMode(hdc, HALFTONE);
        StretchBlt(hdc, 0, 0, rect.right, rect.bottom, g_hDC, 0,
               BMP_HEIGHT - 1, BMP_WIDTH, -BMP_HEIGHT, SRCCOPY);
        EndPaint(hWnd, &ps);
        break;
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
            SendMessage(hWnd, WM_SYSCOMMAND, SC_CLOSE, NULL);
        break;
    case WM_CREATE:
        CreateBmp(hWnd);
        InitPalette();
        break;
    case WM_TIMER:
        InvalidateRect(hWnd, NULL, FALSE);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

int main()
{

    MSG msg;
    HWND hWnd;
    WNDCLASS wc;

    g_hInstance = GetModuleHandle(NULL);

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC) WndProc;
    wc.hInstance = g_hInstance;
    wc.hbrBackground = NULL;
    wc.lpszClassName = CLASSNAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.lpszMenuName = NULL;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;

    RegisterClass(&wc);

    int w = WINDOW_WIDTH;
    int h = WINDOW_HEIGHT;

    if (!(hWnd = CreateWindow(wc.lpszClassName, wc.lpszClassName,
        WS_OVERLAPPEDWINDOW, 0, 0, w, h, NULL, NULL, 
        wc.hInstance, NULL))) return FALSE;

    RECT rc;

    SetRect(&rc, 0, 0, w, h);

    AdjustWindowRectEx(&rc, GetWindowLong(hWnd, GWL_STYLE),
        GetMenu(hWnd) != NULL, GetWindowLong(hWnd, GWL_EXSTYLE));

    w = rc.right - rc.left;
    h = rc.bottom - rc.top;

    SetWindowPos(hWnd, NULL, (GetSystemMetrics(SM_CXSCREEN) - w) / 2,
        (GetSystemMetrics(SM_CYSCREEN) - h) / 2, w, h,
        SWP_NOZORDER | SWP_NOACTIVATE);

    ShowWindow(hWnd, TRUE);
    UpdateWindow(hWnd);

    SetTimer(hWnd, 0, 1, NULL);
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int) msg.wParam;
}

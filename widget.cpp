#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <gdiplus.h>
#include <ctime>
#include <string>
#include <stdio.h>
#include <shlobj.h>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "advapi32.lib")

using namespace Gdiplus;

// ── Global State ─────────────────────────────────────────────────────────────
ULONG_PTR g_gdiplusToken;
HWND g_hWnd = NULL;
bool g_setupMode = true;
int g_xPos = 500, g_yPos = 1000;
POINT g_dragStart = { 0, 0 };
bool g_isDragging = false;

std::wstring g_cachedDateStr = L"";
DWORD g_lastDateFetch = 0;

// DPI scale factor (1.0 = 96 DPI, 1.25 = 120 DPI, 1.5 = 144 DPI, etc.)
float g_dpiScale = 1.0f;

bool g_hiddenForFullscreen = false;

// ── Fullscreen App Detection ─────────────────────────────────────────────────
// Returns true if the foreground window completely covers its monitor
// (e.g. a movie player, game, or browser in fullscreen mode).
bool IsFullscreenAppRunning() {
    HWND hForeground = GetForegroundWindow();
    if (!hForeground) return false;

    // Desktop and shell windows aren't "fullscreen apps"
    if (hForeground == GetDesktopWindow() || hForeground == GetShellWindow())
        return false;

    // Ignore our own widget
    if (hForeground == g_hWnd) return false;

    RECT rcWnd;
    GetWindowRect(hForeground, &rcWnd);

    // Get the monitor the foreground window is on
    HMONITOR hMon = MonitorFromWindow(hForeground, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfo(hMon, &mi);

    // If the window covers the entire monitor, it's fullscreen
    return (rcWnd.left   <= mi.rcMonitor.left  &&
            rcWnd.top    <= mi.rcMonitor.top   &&
            rcWnd.right  >= mi.rcMonitor.right &&
            rcWnd.bottom >= mi.rcMonitor.bottom);
}

std::wstring GetConfigPath() {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    std::wstring ws(path);
    size_t pos = ws.find_last_of(L"\\/");
    if (pos != std::wstring::npos) {
        ws = ws.substr(0, pos);
    }
    return ws + L"\\widget.cfg";
}

void LoadConfig() {
    std::wstring path = GetConfigPath();
    std::string npath(path.begin(), path.end());
    FILE* file = fopen(npath.c_str(), "r");
    if (file) {
        int x, y, setup;
        if (fscanf(file, "%d,%d,%d", &x, &y, &setup) == 3) {
            g_xPos = x;
            g_yPos = y;
            g_setupMode = (setup != 0);
        }
        fclose(file);
    }
}

void SaveConfig() {
    std::wstring path = GetConfigPath();
    std::string npath(path.begin(), path.end());
    FILE* file = fopen(npath.c_str(), "w");
    if (file) {
        fprintf(file, "%d,%d,%d", g_xPos, g_yPos, g_setupMode ? 1 : 0);
        fclose(file);
    }
}

// ── Startup Registry Helpers ────────────────────────────────────────────────
static const wchar_t* STARTUP_REG_KEY = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
static const wchar_t* STARTUP_REG_VALUE = L"NepaliDateWidget";

bool IsStartupEnabled() {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, STARTUP_REG_KEY, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
        return false;
    DWORD type = 0, size = 0;
    bool exists = (RegQueryValueExW(hKey, STARTUP_REG_VALUE, NULL, &type, NULL, &size) == ERROR_SUCCESS);
    RegCloseKey(hKey);
    return exists;
}

void SetStartupEnabled(bool enable) {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, STARTUP_REG_KEY, 0, KEY_SET_VALUE, &hKey) != ERROR_SUCCESS)
        return;
    if (enable) {
        wchar_t exePath[MAX_PATH];
        GetModuleFileNameW(NULL, exePath, MAX_PATH);
        std::wstring val = std::wstring(L"\"") + exePath + L"\"";
        RegSetValueExW(hKey, STARTUP_REG_VALUE, 0, REG_SZ,
                       (const BYTE*)val.c_str(), (DWORD)((val.size() + 1) * sizeof(wchar_t)));
    } else {
        RegDeleteValueW(hKey, STARTUP_REG_VALUE);
    }
    RegCloseKey(hKey);
}

// ── Silent Subprocess Execution ─────────────────────────────────────────────
std::string ExecCmdSilent(const char* cmd) {
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    HANDLE hRead, hWrite;
    if (!CreatePipe(&hRead, &hWrite, &sa, 0)) return "";
    SetHandleInformation(hRead, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.hStdOutput = hWrite;
    si.hStdError = hWrite;
    si.wShowWindow = SW_HIDE;

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    std::string result = "";
    char cmdCopy[512];
    snprintf(cmdCopy, sizeof(cmdCopy), "%s", cmd);
    
    if (CreateProcessA(NULL, cmdCopy, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        CloseHandle(hWrite);
        
        char buffer[128];
        DWORD bytesRead;
        while (ReadFile(hRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            result += buffer;
        }
        
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        CloseHandle(hWrite);
    }
    CloseHandle(hRead);
    return result;
}

// ── Nepali Date Calculator (Calls Python for Accuracy) ───────────────────────
std::wstring GetNepaliDateString() {
    DWORD now = GetTickCount();
    // Refresh every 1 hour (3600000 ms) or if empty/error and 10 seconds have passed to prevent fast looping
    if (g_cachedDateStr.empty() || g_cachedDateStr == L"Date Error" || (now - g_lastDateFetch > 3600000)) {
        // Prevent aggressive looping if Python fails
        if (g_cachedDateStr == L"Date Error" && (now - g_lastDateFetch < 10000)) {
            return g_cachedDateStr;
        }
        
        g_lastDateFetch = now;
        
        std::string result = ExecCmdSilent("py -c \"import nepali_datetime; nd=nepali_datetime.date.today(); print(f'{nd.year} / {nd.month:02d} / {nd.day:02d}')\"");
        
        while (!result.empty() && (result.back() == '\n' || result.back() == '\r')) {
            result.pop_back();
        }
        
        if (!result.empty()) {
            int len = MultiByteToWideChar(CP_UTF8, 0, result.c_str(), -1, NULL, 0);
            std::wstring wstr(len, 0);
            MultiByteToWideChar(CP_UTF8, 0, result.c_str(), -1, &wstr[0], len);
            if (!wstr.empty() && wstr.back() == L'\0') wstr.pop_back();
            g_cachedDateStr = wstr;
        } else {
            g_cachedDateStr = L"Date Error";
        }
    }
    return g_cachedDateStr;
}

// ── Custom Drawing Engine (Per-Pixel Alpha) ──────────────────────────────────
void RenderWidget(HWND hWnd) {
    // Use DPI-scaled dimensions for the render surface to get crisp pixels
    int rawWidth = (int)(175 * g_dpiScale);
    int rawHeight = (int)(48 * g_dpiScale);

    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    // Create a 32-bit ARGB DIB section for proper per-pixel alpha
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = rawWidth;
    bmi.bmiHeader.biHeight = -rawHeight;  // Top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* pBits = NULL;
    HBITMAP hBitmap = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);

    Graphics graphics(hdcMem);
    graphics.SetSmoothingMode(SmoothingModeHighQuality);
    graphics.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
    graphics.SetPixelOffsetMode(PixelOffsetModeHighQuality);
    graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

    // Clear background with per-pixel transparency
    graphics.Clear(Color(0, 0, 0, 0));

    float s = g_dpiScale;  // Shorthand for scale factor

    if (g_setupMode) {
        SolidBrush bgBrush(Color(230, 31, 31, 31));
        graphics.FillRectangle(&bgBrush, 0, 0, rawWidth, rawHeight);
    }

    // 1. Render Flag (all coordinates scaled by DPI)
    REAL cx = 10.0f * s, cy = rawHeight / 2.0f, h = 24.0f * s;
    REAL bd = 2.0f * s;
    REAL h1 = h * 0.52f, h2 = h - h1;
    REAL w1 = h1 * 0.85f, w2 = h2 * 1.12f;
    REAL top = cy - h / 2.0f, mid = top + h1;

    SolidBrush blueBrush(Color(255, 0, 56, 147));
    SolidBrush crimsonBrush(Color(255, 220, 20, 60));
    SolidBrush whiteBrush(Color(255, 255, 255, 255));

    // Blue Border Triangles
    PointF t1_out[] = { PointF(cx, top), PointF(cx, mid + bd), PointF(cx + w1 + bd, mid) };
    PointF t2_out[] = { PointF(cx, mid - bd), PointF(cx, top + h), PointF(cx + w2 + bd, top + h) };
    graphics.FillPolygon(&blueBrush, t1_out, 3);
    graphics.FillPolygon(&blueBrush, t2_out, 3);

    // Crimson Inner Triangles
    PointF t1_in[] = { PointF(cx + bd, top + bd), PointF(cx + bd, mid), PointF(cx + w1, mid) };
    PointF t2_in[] = { PointF(cx + bd, mid), PointF(cx + bd, top + h - bd), PointF(cx + w2, top + h - bd) };
    graphics.FillPolygon(&crimsonBrush, t1_in, 3);
    graphics.FillPolygon(&crimsonBrush, t2_in, 3);

    // Sun & Moon Symbol
    REAL symbolSize = 4.0f * s;
    graphics.FillEllipse(&whiteBrush, cx + bd + w1 * 0.38f, top + bd + h1 * 0.3f, symbolSize, symbolSize);
    graphics.FillEllipse(&whiteBrush, cx + bd + w2 * 0.36f, mid + h2 * 0.4f, symbolSize, symbolSize);

    // 2. Render Text (DPI-scaled font)
    FontFamily fontFamily(L"Segoe UI");
    Font font(&fontFamily, 12.0f * s, FontStyleBold, UnitPixel);
    std::wstring dateStr = GetNepaliDateString();

    RectF textRect(cx + (w1 > w2 ? w1 : w2) + 12.0f * s, 0.0f, (REAL)rawWidth - 50.0f * s, (REAL)rawHeight);
    StringFormat format;
    format.SetAlignment(StringAlignmentNear);
    format.SetLineAlignment(StringAlignmentCenter);

    graphics.DrawString(dateStr.c_str(), -1, &font, textRect, &format, &whiteBrush);

    // Apply per-pixel alpha channel to OS Window
    POINT ptDst = { g_xPos, g_yPos };
    SIZE sizeDst = { rawWidth, rawHeight };
    POINT ptSrc = { 0, 0 };
    BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

    UpdateLayeredWindow(hWnd, hdcScreen, &ptDst, &sizeDst, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

    SelectObject(hdcMem, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
}

// ── Win32 Message Loop Engine ────────────────────────────────────────────────
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        SetTimer(hWnd, 1, 1000, NULL);   // Render timer
        SetTimer(hWnd, 2, 250, NULL);    // Z-order enforcement timer
        break;

    case WM_TIMER:
        if (wParam == 1) {
            RenderWidget(hWnd);
        } else if (wParam == 2) {
            // ── Fullscreen detection: hide during movies/games ────────
            bool fullscreen = IsFullscreenAppRunning();
            if (fullscreen && !g_hiddenForFullscreen) {
                g_hiddenForFullscreen = true;
                ShowWindow(hWnd, SW_HIDE);
            } else if (!fullscreen && g_hiddenForFullscreen) {
                g_hiddenForFullscreen = false;
                ShowWindow(hWnd, SW_SHOWNOACTIVATE);
                RenderWidget(hWnd);
            }
            // Re-assert TOPMOST (only when visible)
            if (!g_hiddenForFullscreen) {
                SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOSENDCHANGING);
            }
        }
        break;

    // ── THE KEY FIX: Prevent taskbar thumbnail previews from hiding us ────
    // When you hover over a taskbar app icon, Windows internally calls
    // ShowOwnedPopups(hTaskbar, FALSE), which sends WM_SHOWWINDOW with
    // wParam=FALSE and lParam=SW_PARENTCLOSING to all windows owned by
    // the taskbar. By intercepting this and returning 0 (not calling
    // DefWindowProc), we refuse to be hidden.
    case WM_SHOWWINDOW:
        if (wParam == FALSE && lParam == SW_PARENTCLOSING) {
            return 0;  // Block the hide — stay visible
        }
        return DefWindowProc(hWnd, msg, wParam, lParam);

    case WM_LBUTTONDOWN:
        if (!g_setupMode) break;  // Locked — don't allow dragging
        g_isDragging = true;
        SetCapture(hWnd);
        GetCursorPos(&g_dragStart);
        g_dragStart.x -= g_xPos;
        g_dragStart.y -= g_yPos;
        break;

    case WM_MOUSEMOVE:
        if (g_isDragging) {
            POINT pt;
            GetCursorPos(&pt);
            g_xPos = pt.x - g_dragStart.x;
            g_yPos = pt.y - g_dragStart.y;
            RenderWidget(hWnd);
        }
        break;

    case WM_LBUTTONUP:
        if (g_isDragging) {
            g_isDragging = false;
            ReleaseCapture();
            SaveConfig();
        }
        break;

    case WM_RBUTTONUP: {
        HMENU hMenu = CreatePopupMenu();
        AppendMenu(hMenu, MF_STRING, 1, g_setupMode ? L"Lock Position & Make Transparent" : L"Adjust Position");

        bool startupOn = IsStartupEnabled();
        AppendMenu(hMenu, MF_STRING | (startupOn ? MF_CHECKED : 0), 3, L"Run at Startup");

        AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
        AppendMenu(hMenu, MF_STRING, 2, L"Exit");

        POINT pt;
        GetCursorPos(&pt);
        SetForegroundWindow(hWnd);
        int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, NULL);
        DestroyMenu(hMenu);

        if (cmd == 1) {
            g_setupMode = !g_setupMode;
            SaveConfig();
            RenderWidget(hWnd);
        } else if (cmd == 2) {
            PostQuitMessage(0);
        } else if (cmd == 3) {
            SetStartupEnabled(!startupOn);
        }
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

// ── Application Entrypoint ───────────────────────────────────────────────────
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    // ── DPI Awareness: Render at native resolution, no bitmap scaling ────
    // This is the #1 fix for blurry text and flag. Without this, Windows
    // renders the widget at 96 DPI then stretches the bitmap up, blurring it.
    typedef BOOL (WINAPI *SetProcessDpiAwarenessContextFunc)(HANDLE);
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hUser32) {
        auto fn = (SetProcessDpiAwarenessContextFunc)GetProcAddress(hUser32, "SetProcessDpiAwarenessContext");
        if (fn) {
            fn(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        } else {
            SetProcessDPIAware();  // Fallback for older Windows 10
        }
    }

    // Compute DPI scale
    HDC hdc = GetDC(NULL);
    g_dpiScale = GetDeviceCaps(hdc, LOGPIXELSX) / 96.0f;
    ReleaseDC(NULL, hdc);

    LoadConfig();

    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);

    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"NepaliTaskbarWidgetClass";
    ATOM atom = RegisterClassEx(&wc);

    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
    
    // Bounds check to ensure the widget isn't lost off-screen due to old configs
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    if (g_yPos < 0 || g_yPos >= screenHeight) {
        if (hTaskbar) {
            RECT taskbarRect;
            GetWindowRect(hTaskbar, &taskbarRect);
            g_yPos = taskbarRect.top + (taskbarRect.bottom - taskbarRect.top) / 2 - 24;
            if (g_yPos < 0 || g_yPos > screenHeight) g_yPos = screenHeight - 60;
        } else {
            g_yPos = screenHeight - 60;
        }
    }

    // DPI-scaled window size
    int winW = (int)(175 * g_dpiScale);
    int winH = (int)(48 * g_dpiScale);

    // Create a frameless, tool-window with layered (translucent) attributes as a POPUP
    g_hWnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
        L"NepaliTaskbarWidgetClass",
        L"Nepali Date Taskbar Widget",
        WS_POPUP,
        g_xPos, g_yPos, winW, winH,
        NULL, NULL, hInstance, NULL
    );

    // ── Set the Taskbar as the OWNER of this window ─────────────────────
    // This keeps the widget mathematically ABOVE the taskbar in Z-order.
    // The WM_SHOWWINDOW handler above prevents the taskbar's thumbnail
    // preview system from hiding us via ShowOwnedPopups().
    if (hTaskbar) {
        SetWindowLongPtr(g_hWnd, GWLP_HWNDPARENT, (LONG_PTR)hTaskbar);
    }

    ShowWindow(g_hWnd, SW_SHOWNOACTIVATE);
    RenderWidget(g_hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    GdiplusShutdown(g_gdiplusToken);
    return 0;
}

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <winhttp.h>
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

// ── App Version ──────────────────────────────────────────────────────────────
#define APP_VERSION L"3.4.1"
#define GITHUB_REPO_API L"/repos/aayushlbef/Nepali-Date-Widget/releases/latest"
#define GITHUB_RELEASE_URL L"https://github.com/aayushlbef/Nepali-Date-Widget/releases/tag/"
#define WM_UPDATE_AVAILABLE (WM_USER + 2)
#define WM_UPDATE_NOT_FOUND (WM_USER + 3)
#define WM_UPDATE_ERROR     (WM_USER + 4)

// ── Global State ─────────────────────────────────────────────────────────────
ULONG_PTR g_gdiplusToken;
HWND g_hWnd = NULL;
bool g_setupMode = true;
int g_xPos = 500, g_yPos = 1000;
POINT g_dragStart = { 0, 0 };
bool g_isDragging = false;
bool g_isMenuOpen = false;
bool g_showDay = true;

// DPI scale factor (1.0 = 96 DPI, 1.25 = 120 DPI, 1.5 = 144 DPI, etc.)

// DPI scale factor (1.0 = 96 DPI, 1.25 = 120 DPI, 1.5 = 144 DPI, etc.)
float g_dpiScale = 1.0f;

bool g_hiddenForFullscreen = false;

// ── Theme Detection ──────────────────────────────────────────────────────────
bool g_isLightTheme = false;   // true = Windows light theme, false = dark
wchar_t g_latestVersion[64] = {0};  // Filled by update-check thread

bool DetectWindowsTheme() {
    // Returns true if Windows is using a light taskbar/system theme
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD value = 0, size = sizeof(DWORD), type = REG_DWORD;
        // SystemUsesLightTheme controls the taskbar/system chrome color
        if (RegQueryValueExW(hKey, L"SystemUsesLightTheme", NULL, &type,
                             (LPBYTE)&value, &size) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return (value != 0);
        }
        RegCloseKey(hKey);
    }
    return false;  // Default to dark theme if registry read fails
}

// ── Update Checker ───────────────────────────────────────────────────────────
// Compare "3.3.0" vs "3.4.0" style version strings
bool IsVersionNewer(const wchar_t* current, const wchar_t* latest) {
    int cMaj = 0, cMin = 0, cPat = 0;
    int lMaj = 0, lMin = 0, lPat = 0;
    const wchar_t* c = current;
    const wchar_t* l = latest;
    if (*c == L'v' || *c == L'V') c++;
    if (*l == L'v' || *l == L'V') l++;
    swscanf(c, L"%d.%d.%d", &cMaj, &cMin, &cPat);
    swscanf(l, L"%d.%d.%d", &lMaj, &lMin, &lPat);
    if (lMaj != cMaj) return lMaj > cMaj;
    if (lMin != cMin) return lMin > cMin;
    return lPat > cPat;
}

// Minimal JSON extractor: finds "key":"value" and writes value into out[]
bool ExtractJsonString(const char* json, const char* key, wchar_t* out, int outLen) {
    char needle[128];
    snprintf(needle, sizeof(needle), "\"%s\"", key);
    const char* p = strstr(json, needle);
    if (!p) return false;
    p += strlen(needle);
    while (*p == ' ' || *p == ':' || *p == '\t' || *p == '\n' || *p == '\r') p++;
    if (*p != '"') return false;
    p++;  // skip opening quote
    int i = 0;
    while (*p && *p != '"' && i < outLen - 1) {
        out[i++] = (wchar_t)(*p++);
    }
    out[i] = 0;
    return i > 0;
}

DWORD WINAPI CheckForUpdateThread(LPVOID lpParam) {
    bool isManual = (bool)(INT_PTR)lpParam;

    HINTERNET hSession = WinHttpOpen(L"NepaliDateWidget/" APP_VERSION,
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) {
        if (isManual) PostMessage(g_hWnd, WM_UPDATE_ERROR, 0, 0);
        return 0;
    }

    HINTERNET hConnect = WinHttpConnect(hSession, L"api.github.com",
        INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) {
        WinHttpCloseHandle(hSession);
        if (isManual) PostMessage(g_hWnd, WM_UPDATE_ERROR, 0, 0);
        return 0;
    }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", GITHUB_REPO_API,
        NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        if (isManual) PostMessage(g_hWnd, WM_UPDATE_ERROR, 0, 0);
        return 0;
    }

    if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
            WINHTTP_NO_REQUEST_DATA, 0, 0, 0) ||
        !WinHttpReceiveResponse(hRequest, NULL)) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        if (isManual) PostMessage(g_hWnd, WM_UPDATE_ERROR, 0, 0);
        return 0;
    }

    // Read the full response body
    std::string response;
    char buf[4096];
    DWORD bytesRead = 0;
    while (WinHttpReadData(hRequest, buf, sizeof(buf) - 1, &bytesRead) && bytesRead > 0) {
        buf[bytesRead] = 0;
        response += buf;
        bytesRead = 0;
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    // Extract tag_name from the JSON response
    wchar_t tagName[64] = {0};
    if (ExtractJsonString(response.c_str(), "tag_name", tagName, 64)) {
        if (IsVersionNewer(APP_VERSION, tagName)) {
            wcscpy_s(g_latestVersion, tagName);
            PostMessage(g_hWnd, WM_UPDATE_AVAILABLE, 0, 0);
        } else {
            if (isManual) PostMessage(g_hWnd, WM_UPDATE_NOT_FOUND, 0, 0);
        }
    } else {
        if (isManual) PostMessage(g_hWnd, WM_UPDATE_ERROR, 0, 0);
    }
    return 0;
}

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
        int x, y, setup, showDay = 1;
        int n = fscanf(file, "%d,%d,%d,%d", &x, &y, &setup, &showDay);
        if (n >= 3) {
            g_xPos = x;
            g_yPos = y;
            g_setupMode = (setup != 0);
            if (n >= 4) g_showDay = (showDay != 0);
        }
        fclose(file);
    }
}

void SaveConfig() {
    std::wstring path = GetConfigPath();
    std::string npath(path.begin(), path.end());
    FILE* file = fopen(npath.c_str(), "w");
    if (file) {
        fprintf(file, "%d,%d,%d,%d", g_xPos, g_yPos, g_setupMode ? 1 : 0, g_showDay ? 1 : 0);
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

// ── Silent Subprocess Execution (Removed) ───────────────────────────────────

// ── Nepali Date Calculator (Pure C++) ────────────────────────────────────────
#include "bs_data.h"

std::wstring GetNepaliDateString() {
    static WORD s_lastDay = 0, s_lastMonth = 0, s_lastYear = 0;
    static std::wstring s_cachedDateStr = L"";

    SYSTEMTIME st;
    GetLocalTime(&st);
    
    // Ultimate minimal resource check: only recalculate if the day has changed!
    if (st.wDay == s_lastDay && st.wMonth == s_lastMonth && st.wYear == s_lastYear && !s_cachedDateStr.empty()) {
        return s_cachedDateStr;
    }
    
    s_lastDay = st.wDay;
    s_lastMonth = st.wMonth;
    s_lastYear = st.wYear;
    
    // Calculate days since 1918-04-13 (BS 1975-01-01)
    FILETIME ftNow, ftRef;
    SystemTimeToFileTime(&st, &ftNow);
    
    SYSTEMTIME stRef = {0};
    stRef.wYear = 1918;
    stRef.wMonth = 4;
    stRef.wDay = 13;
    SystemTimeToFileTime(&stRef, &ftRef);
    
    ULARGE_INTEGER uNow, uRef;
    uNow.LowPart = ftNow.dwLowDateTime;
    uNow.HighPart = ftNow.dwHighDateTime;
    uRef.LowPart = ftRef.dwLowDateTime;
    uRef.HighPart = ftRef.dwHighDateTime;
    
    // Number of 100-nanosecond intervals per day = 10,000,000 * 60 * 60 * 24 = 864,000,000,000
    long long diff = uNow.QuadPart - uRef.QuadPart;
    int days = (int)(diff / 864000000000LL);
    
    if (days < 0) {
        return L"Date Error";
    }

    int bs_year = 1975;
    int bs_month = 1;
    int bs_day = 1;
    
    while (bs_year <= 2100) {
        int days_in_month = bs_month_days[bs_year - 1975][bs_month - 1];
        if (days >= days_in_month) {
            days -= days_in_month;
            bs_month++;
            if (bs_month > 12) {
                bs_month = 1;
                bs_year++;
            }
        } else {
            break;
        }
    }
    bs_day += days;
    
    if (bs_year > 2100) {
        s_cachedDateStr = L"Date Error";
        return s_cachedDateStr;
    }
    
    wchar_t buffer[64];
    swprintf(buffer, 64, L"%d / %02d / %02d", bs_year, bs_month, bs_day);
    s_cachedDateStr = std::wstring(buffer);
    
    return s_cachedDateStr;
}

// ── Custom Drawing Engine (Per-Pixel Alpha) ──────────────────────────────────
void RenderWidget(HWND hWnd) {
    // Use DPI-scaled dimensions for the render surface to get crisp pixels
    int baseWidth = g_showDay ? 190 : 155;
    int rawWidth = (int)(baseWidth * g_dpiScale);
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
    graphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
    graphics.SetPixelOffsetMode(PixelOffsetModeHighQuality);
    graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

    // Clear background with per-pixel transparency
    graphics.Clear(Color(0, 0, 0, 0));

    float s = g_dpiScale;  // Shorthand for scale factor

    if (g_setupMode) {
        // Adapt background to theme so it's visible on both light and dark taskbars
        SolidBrush bgBrush(g_isLightTheme ? Color(210, 230, 230, 230) : Color(230, 31, 31, 31));
        graphics.FillRectangle(&bgBrush, 0, 0, rawWidth, rawHeight);
    }

    // Pre-calculate positions to create a tight, sequential layout: [Day] [Gap] [Flag] [Gap] [Date]
    REAL h = 24.0f * s;
    REAL flagWidth = 0.822f * h;
    
    // We will compute cx dynamically later after measuring the day string width, 
    // but for now we set up the Y coordinates.
    REAL cy = rawHeight / 2.0f;
    REAL top = cy - h / 2.0f;

    // 1. Measure Text and Calculate Sequential Coordinates
    FontFamily fontFamily(L"Segoe UI");
    Font font(&fontFamily, 12.0f * s, FontStyleBold, UnitPixel);
    std::wstring dateStr = GetNepaliDateString();

    StringFormat formatNear;
    formatNear.SetAlignment(StringAlignmentNear);
    formatNear.SetLineAlignment(StringAlignmentCenter);

    REAL gap = 12.0f * s;
    REAL cx, dateX;

    if (g_showDay) {
        SYSTEMTIME st;
        GetLocalTime(&st);
        const wchar_t* daysOfWeek[] = { L"Sun", L"Mon", L"Tue", L"Wed", L"Thu", L"Fri", L"Sat" };
        const wchar_t* dayStr = daysOfWeek[st.wDayOfWeek];

        // Measure day string
        RectF dayMeasure;
        graphics.MeasureString(dayStr, -1, &font, RectF(0,0,1000,100), &formatNear, &dayMeasure);

        // Day box on the left
        REAL boxPaddingX = 5.0f * s;
        REAL boxWidth = dayMeasure.Width + boxPaddingX * 2.0f;
        REAL boxHeight = h - 2.0f * s;
        REAL boxX = 10.0f * s;
        REAL boxY = (rawHeight - boxHeight) / 2.0f;

        // Flag in the middle
        cx = boxX + boxWidth + gap;
        dateX = cx + flagWidth + gap;

        // 2. Render Day Box
        SolidBrush boxBrush(g_isLightTheme ? Color(25, 0, 0, 0) : Color(35, 255, 255, 255));
        GraphicsPath path;
        REAL r = 4.0f * s;
        REAL d = r * 2.0f;
        path.AddArc(boxX, boxY, d, d, 180, 90);
        path.AddArc(boxX + boxWidth - d, boxY, d, d, 270, 90);
        path.AddArc(boxX + boxWidth - d, boxY + boxHeight - d, d, d, 0, 90);
        path.AddArc(boxX, boxY + boxHeight - d, d, d, 90, 90);
        path.CloseFigure();
        graphics.FillPath(&boxBrush, &path);

        // Draw day text centered in the box
        SolidBrush dayTextBrush(g_isLightTheme ? Color(255, 20, 20, 20) : Color(255, 255, 255, 255));
        StringFormat formatCenter;
        formatCenter.SetAlignment(StringAlignmentCenter);
        formatCenter.SetLineAlignment(StringAlignmentCenter);
        RectF dayTextRect(boxX, boxY, boxWidth, boxHeight);
        graphics.DrawString(dayStr, -1, &font, dayTextRect, &formatCenter, &dayTextBrush);
    } else {
        // No day box: flag on the left, date after it
        cx = 10.0f * s;
        dateX = cx + flagWidth + gap;
    }

    // Use dark text on light theme, white text on dark theme
    SolidBrush textBrush(g_isLightTheme ? Color(255, 20, 20, 20) : Color(255, 255, 255, 255));


    // 3. Render Flag (now that cx is computed)
    SolidBrush blueBrush(Color(255, 0, 56, 147));
    SolidBrush crimsonBrush(Color(255, 220, 20, 60));
    SolidBrush whiteBrush(Color(255, 255, 255, 255));

    PointF polyOuter[5] = {
        PointF(cx, top),
        PointF(cx + 0.765f * h, top + 0.543f * h),
        PointF(cx + 0.266f * h, top + 0.543f * h),
        PointF(cx + 0.822f * h, top + h),
        PointF(cx, top + h)
    };
    graphics.FillPolygon(&blueBrush, polyOuter, 5);

    PointF polyInner[5] = {
        PointF(cx + 0.042f * h, top + 0.073f * h),
        PointF(cx + 0.674f * h, top + 0.501f * h),
        PointF(cx + 0.232f * h, top + 0.501f * h),
        PointF(cx + 0.715f * h, top + 0.958f * h),
        PointF(cx + 0.042f * h, top + 0.958f * h)
    };
    graphics.FillPolygon(&crimsonBrush, polyInner, 5);
    
    // Moon
    REAL moonOuterDiam = 0.20f * h;
    REAL moonOuterX = cx + 0.22f * h - (moonOuterDiam / 2.0f);
    REAL moonOuterY = top + 0.32f * h - (moonOuterDiam / 2.0f);
    graphics.FillEllipse(&whiteBrush, moonOuterX, moonOuterY, moonOuterDiam, moonOuterDiam);
    
    REAL moonInnerDiam = 0.18f * h;
    REAL moonInnerX = moonOuterX + 0.01f * h;
    REAL moonInnerY = moonOuterY - 0.04f * h;
    graphics.FillEllipse(&crimsonBrush, moonInnerX, moonInnerY, moonInnerDiam, moonInnerDiam);
    
    REAL moonStarDiam = 0.09f * h;
    REAL moonStarX = cx + 0.22f * h - (moonStarDiam / 2.0f);
    REAL moonStarY = top + 0.33f * h - (moonStarDiam / 2.0f);
    graphics.FillEllipse(&whiteBrush, moonStarX, moonStarY, moonStarDiam, moonStarDiam);

    // Sun
    REAL sunDiam = 0.18f * h;
    REAL sunX = cx + 0.26f * h - (sunDiam / 2.0f);
    REAL sunY = top + 0.73f * h - (sunDiam / 2.0f);
    graphics.FillEllipse(&whiteBrush, sunX, sunY, sunDiam, sunDiam);


    // 4. Render Date Text
    RectF dateRect(dateX, 0.0f, (REAL)rawWidth - dateX, (REAL)rawHeight);
    graphics.DrawString(dateStr.c_str(), -1, &font, dateRect, &formatNear, &textBrush);

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

// ── System Tray Icon Setup ───────────────────────────────────────────────────
#define WM_TRAYICON (WM_USER + 1)
NOTIFYICONDATAW g_nid = {};

void AddTrayIcon(HWND hWnd) {
    g_nid.cbSize = sizeof(NOTIFYICONDATAW);
    g_nid.hWnd = hWnd;
    g_nid.uID = 1;
    g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    g_nid.uCallbackMessage = WM_TRAYICON;
    g_nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(1)); // Custom embedded logo
    wcscpy_s(g_nid.szTip, L"Nepali Date Widget");
    Shell_NotifyIconW(NIM_ADD, &g_nid);
}

void RemoveTrayIcon() {
    Shell_NotifyIconW(NIM_DELETE, &g_nid);
}

void ShowContextMenu(HWND hWnd, POINT pt) {
    HMENU hMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_STRING, 1, g_setupMode ? L"Lock Position & Make Transparent" : L"Adjust Position");

    bool startupOn = IsStartupEnabled();
    AppendMenu(hMenu, MF_STRING | (startupOn ? MF_CHECKED : 0), 3, L"Run at Startup");
    AppendMenu(hMenu, MF_STRING | (g_showDay ? MF_CHECKED : 0), 5, L"Show Day");
    AppendMenu(hMenu, MF_STRING, 4, L"Check for Updates");

    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hMenu, MF_STRING, 2, L"Exit");

    SetForegroundWindow(hWnd);
    g_isMenuOpen = true;
    int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, NULL);
    g_isMenuOpen = false;
    // Post a dummy message to ensure the menu dismisses properly when clicking outside
    PostMessage(hWnd, WM_NULL, 0, 0);
    DestroyMenu(hMenu);

    if (cmd == 1) {
        g_setupMode = !g_setupMode;
        SaveConfig();
        RenderWidget(hWnd);
    } else if (cmd == 2) {
        PostQuitMessage(0);
    } else if (cmd == 3) {
        SetStartupEnabled(!startupOn);
    } else if (cmd == 4) {
        HANDLE hThread = CreateThread(NULL, 0, CheckForUpdateThread, (LPVOID)TRUE, 0, NULL);
        if (hThread) CloseHandle(hThread);
    } else if (cmd == 5) {
        g_showDay = !g_showDay;
        SaveConfig();
        RenderWidget(hWnd);
    }
}

// ── Win32 Message Loop Engine ────────────────────────────────────────────────
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        AddTrayIcon(hWnd);
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
            // Re-assert TOPMOST (only when visible and menu is closed)
            if (!g_hiddenForFullscreen && !g_isMenuOpen) {
                SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOSENDCHANGING);
                
                // Fast Startup fix: dynamically repair taskbar parent relationship
                HWND hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
                if (hTaskbar && hTaskbar != (HWND)GetWindowLongPtr(hWnd, GWLP_HWNDPARENT)) {
                    SetWindowLongPtr(hWnd, GWLP_HWNDPARENT, (LONG_PTR)hTaskbar);
                }
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
        POINT pt;
        GetCursorPos(&pt);
        ShowContextMenu(hWnd, pt);
        break;
    }

    case WM_TRAYICON:
        if (lParam == WM_RBUTTONUP || lParam == WM_LBUTTONUP) {
            POINT pt;
            GetCursorPos(&pt);
            ShowContextMenu(hWnd, pt);
        }
        break;

    case WM_DESTROY:
        RemoveTrayIcon();
        PostQuitMessage(0);
        break;

    // ── Update notification ──────────────────────────────────────────────────
    case WM_UPDATE_AVAILABLE: {
        wchar_t msg[256];
        swprintf(msg, 256, L"A new version (%s) is available!\n\nWould you like to download it?", g_latestVersion);
        if (MessageBoxW(hWnd, msg, L"Nepali Date Widget \u2014 Update Available",
                MB_YESNO | MB_ICONINFORMATION) == IDYES) {
            std::wstring url = GITHUB_RELEASE_URL + std::wstring(g_latestVersion);
            ShellExecuteW(NULL, L"open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
        }
        break;
    }
    
    case WM_UPDATE_NOT_FOUND:
        MessageBoxW(hWnd, L"You are already running the latest version.", 
                    L"Nepali Date Widget \u2014 Up to Date", MB_OK | MB_ICONINFORMATION);
        break;

    case WM_UPDATE_ERROR:
        MessageBoxW(hWnd, L"Failed to check for updates. Please check your internet connection.", 
                    L"Nepali Date Widget \u2014 Error", MB_OK | MB_ICONERROR);
        break;

    // ── Theme change detection ─────────────────────────────────────────────
    case WM_SETTINGCHANGE:
        if (lParam && wcscmp((LPCWSTR)lParam, L"ImmersiveColorSet") == 0) {
            g_isLightTheme = DetectWindowsTheme();
            RenderWidget(hWnd);
        }
        break;

    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

// ── Application Entrypoint ───────────────────────────────────────────────────
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    HANDLE hMutex = CreateMutex(NULL, TRUE, L"NepaliDateWidget_Mutex_Unique_App_ID");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        MessageBox(NULL, L"The application is already running.", L"Nepali Date Widget", MB_OK | MB_ICONINFORMATION);
        CloseHandle(hMutex);
        return 0;
    }

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

    // Detect initial Windows theme
    g_isLightTheme = DetectWindowsTheme();

    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);

    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"NepaliTaskbarWidgetClass";
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(1));
    wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(1));
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
    int winW = (int)((g_showDay ? 190 : 155) * g_dpiScale);
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

    // ── Launch background update check (non-blocking) ────────────────────
    HANDLE hThread = CreateThread(NULL, 0, CheckForUpdateThread, (LPVOID)FALSE, 0, NULL);
    if (hThread) CloseHandle(hThread);  // Fire-and-forget

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    GdiplusShutdown(g_gdiplusToken);

    if (hMutex) {
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
    }
    
    return 0;
}
#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <winhttp.h>
#include <gdiplus.h>
#include <ctime>
#include <string>
#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <shlobj.h>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "shell32.lib")

using namespace Gdiplus;

// ── App Version ──────────────────────────────────────────────────────────────
#define APP_VERSION L"3.4.2"
#define GITHUB_REPO_API L"/repos/aayushlbef/Nepali-Date-Widget/releases/latest"
#define GITHUB_RELEASE_URL L"https://github.com/aayushlbef/Nepali-Date-Widget/releases/tag/"
#define WM_UPDATE_AVAILABLE (WM_USER + 2)
#define WM_UPDATE_NOT_FOUND (WM_USER + 3)
#define WM_UPDATE_ERROR     (WM_USER + 4)
#define WM_HOLIDAYS_LOADED  (WM_USER + 5)
#define WM_HOLIDAYS_FAILED  (WM_USER + 6)

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

// ── Nepali Date Calculator (Pure C++) ────────────────────────────────────────
#include "bs_data.h"

const wchar_t* kNepaliMonthNamesEN[] = {
    L"Baisakh", L"Jestha", L"Ashadh", L"Shrawan", L"Bhadra", L"Ashwin",
    L"Kartik", L"Mangsir", L"Poush", L"Magh", L"Falgun", L"Chaitra"
};

const wchar_t* kNepaliMonthNamesNP[] = {
    L"\u092C\u0948\u0936\u093E\u0916",             // Baisakh (बैशाख)
    L"\u091C\u0947\u0920",                         // Jestha (जेठ)
    L"\u0905\u0938\u093E\u0930",                   // Ashadh (असार)
    L"\u0936\u094D\u0930\u093E\u0935\u0923",       // Shrawan (श्रावण)
    L"\u092D\u0926\u094C",                         // Bhadra (भदौ)
    L"\u0905\u0938\u094B\u091C",                   // Ashwin (असोज)
    L"\u0915\u093E\u0930\u094D\u0924\u093F\u0915", // Kartik (कार्तिक)
    L"\u092E\u0902\u0938\u093F\u0930",             // Mangsir (मंसिर)
    L"\u092A\u0941\u0937",                         // Poush (पुष)
    L"\u092E\u093E\u0918",                         // Magh (माघ)
    L"\u092B\u093E\u0932\u094D\u0917\u0941\u0928", // Falgun (फाल्गुन)
    L"\u091A\u0948\u0924"                          // Chaitra (चैत)
};

const wchar_t* kEnglishMonthNames[] = {
    L"January", L"February", L"March", L"April", L"May", L"June",
    L"July", L"August", L"September", L"October", L"November", L"December"
};

const wchar_t* kNepaliDayNamesEN[] = {
    L"Sunday", L"Monday", L"Tuesday", L"Wednesday", L"Thursday", L"Friday", L"Saturday"
};

const wchar_t* kNepaliDayNamesNP[] = {
    L"\u0906\u0907\u0924\u092C\u093E\u0930",  // Sunday (आइतबार)
    L"\u0938\u094B\u092E\u092C\u093E\u0930",  // Monday (सोमबार)
    L"\u092E\u0902\u0917\u0932\u092C\u093E\u0930", // Tuesday (मंगलबार)
    L"\u092C\u0941\u0927\u092C\u093E\u0930",  // Wednesday (बुधबार)
    L"\u092C\u093F\u0939\u0940\u092C\u093E\u0930", // Thursday (बिहीबार)
    L"\u0936\u0941\u0915\u094D\u0930\u092C\u093E\u0930", // Friday (शुक्रबार)
    L"\u0936\u0928\u093F\u092C\u093E\u0930"   // Saturday (शनिबार)
};

// ── Forward declaration for Calendar window handle ───────────────────────────
extern HWND g_hCalWnd;

// ── Nepali Public Holidays & Notable Festivals Engine ────────────────────────
struct NepaliHoliday {
    int month;          // 1 = Baisakh, ..., 12 = Chaitra
    int day;            // Day of BS month (1..32)
    bool isPublicHoliday;
    std::wstring titleNP;
    std::wstring titleEN;
    std::wstring category;
    std::wstring description;
};

enum HolidayFetchState {
    FETCH_IDLE,
    FETCH_SUCCESS,
    FETCH_DOWNLOADING,
    FETCH_ERROR_OFFLINE
};

#define HOLIDAY_FILE_MAGIC 0x4857444E
#define HOLIDAY_FILE_VERSION 1
#define ENCODE_XOR_KEY 0x5A

int g_loadedHolidayYear = 0;
std::vector<NepaliHoliday> g_currentYearHolidays;
HolidayFetchState g_holidayFetchState = FETCH_IDLE;
bool g_isHolidayFetchInProgress = false;
DWORD g_lastHolidayFetchAttemptTime = 0;

std::string WideToUtf8(const std::wstring& wstr) {
    if (wstr.empty()) return "";
    int len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string str(len, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &str[0], len, NULL, NULL);
    return str;
}

std::wstring Utf8ToWide(const std::string& str) {
    if (str.empty()) return L"";
    int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
    std::wstring wstr(len, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstr[0], len);
    return wstr;
}

std::wstring GetHolidayCachePath(int bsYear) {
    wchar_t appDataPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, appDataPath))) {
        std::wstring dir = std::wstring(appDataPath) + L"\\NepaliDateWidget";
        CreateDirectoryW(dir.c_str(), NULL);
        return dir + L"\\holidays_" + std::to_wstring(bsYear) + L".dat";
    }
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    std::wstring ws(path);
    size_t pos = ws.find_last_of(L"\\/");
    if (pos != std::wstring::npos) {
        ws = ws.substr(0, pos);
    }
    return ws + L"\\holidays_" + std::to_wstring(bsYear) + L".dat";
}

bool SaveEncodedHolidayFile(int bsYear, const std::vector<NepaliHoliday>& holidays) {
    std::wstring filePath = GetHolidayCachePath(bsYear);
    FILE* f = _wfopen(filePath.c_str(), L"wb");
    if (!f) return false;

    uint32_t magic = HOLIDAY_FILE_MAGIC;
    uint16_t version = HOLIDAY_FILE_VERSION;
    uint16_t year = (uint16_t)bsYear;
    uint16_t count = (uint16_t)holidays.size();

    fwrite(&magic, sizeof(magic), 1, f);
    fwrite(&version, sizeof(version), 1, f);
    fwrite(&year, sizeof(year), 1, f);
    fwrite(&count, sizeof(count), 1, f);

    for (const auto& h : holidays) {
        uint8_t m = (uint8_t)h.month;
        uint8_t d = (uint8_t)h.day;
        uint8_t pub = h.isPublicHoliday ? 1 : 0;
        fwrite(&m, sizeof(m), 1, f);
        fwrite(&d, sizeof(d), 1, f);
        fwrite(&pub, sizeof(pub), 1, f);

        auto writeEncStr = [&](const std::wstring& ws) {
            std::string utf8 = WideToUtf8(ws);
            uint16_t len = (uint16_t)utf8.size();
            fwrite(&len, sizeof(len), 1, f);
            for (size_t i = 0; i < utf8.size(); ++i) {
                uint8_t b = (uint8_t)utf8[i] ^ ENCODE_XOR_KEY;
                fwrite(&b, 1, 1, f);
            }
        };

        writeEncStr(h.titleNP);
        writeEncStr(h.titleEN);
        writeEncStr(h.category);
        writeEncStr(h.description);
    }
    fclose(f);
    return true;
}

bool LoadEncodedHolidayFile(int bsYear, std::vector<NepaliHoliday>& outHolidays) {
    std::wstring filePath = GetHolidayCachePath(bsYear);
    FILE* f = _wfopen(filePath.c_str(), L"rb");
    if (!f) return false;

    uint32_t magic = 0;
    uint16_t version = 0;
    uint16_t year = 0;
    uint16_t count = 0;

    if (fread(&magic, sizeof(magic), 1, f) != 1 || magic != HOLIDAY_FILE_MAGIC) { fclose(f); return false; }
    if (fread(&version, sizeof(version), 1, f) != 1 || version != HOLIDAY_FILE_VERSION) { fclose(f); return false; }
    if (fread(&year, sizeof(year), 1, f) != 1 || year != bsYear) { fclose(f); return false; }
    if (fread(&count, sizeof(count), 1, f) != 1) { fclose(f); return false; }

    outHolidays.clear();
    for (uint16_t i = 0; i < count; ++i) {
        NepaliHoliday h;
        uint8_t m = 0, d = 0, pub = 0;
        if (fread(&m, sizeof(m), 1, f) != 1) break;
        if (fread(&d, sizeof(d), 1, f) != 1) break;
        if (fread(&pub, sizeof(pub), 1, f) != 1) break;
        h.month = m;
        h.day = d;
        h.isPublicHoliday = (pub != 0);

        auto readEncStr = [&]() -> std::wstring {
            uint16_t len = 0;
            if (fread(&len, sizeof(len), 1, f) != 1) return L"";
            std::string utf8(len, '\0');
            for (uint16_t j = 0; j < len; ++j) {
                uint8_t b = 0;
                if (fread(&b, 1, 1, f) != 1) break;
                utf8[j] = (char)(b ^ ENCODE_XOR_KEY);
            }
            return Utf8ToWide(utf8);
        };

        h.titleNP = readEncStr();
        h.titleEN = readEncStr();
        h.category = readEncStr();
        h.description = readEncStr();
        outHolidays.push_back(h);
    }
    fclose(f);
    return !outHolidays.empty();
}

bool ParseHolidaysJson(const std::string& json, int expectedYear, std::vector<NepaliHoliday>& outHolidays) {
    outHolidays.clear();
    const char* p = json.c_str();
    const char* holArray = strstr(p, "\"holidays\"");
    if (!holArray) return false;

    const char* openBracket = strchr(holArray, '[');
    if (!openBracket) return false;

    p = openBracket + 1;
    while (*p && *p != ']') {
        const char* objStart = strchr(p, '{');
        if (!objStart) break;
        const char* objEnd = strchr(objStart, '}');
        if (!objEnd) break;

        std::string obj(objStart, objEnd - objStart + 1);
        NepaliHoliday h;
        h.month = 0;
        h.day = 0;
        h.isPublicHoliday = false;

        const char* mStr = strstr(obj.c_str(), "\"month\"");
        if (mStr) {
            mStr += 7;
            while (*mStr == ' ' || *mStr == ':') mStr++;
            h.month = atoi(mStr);
        }

        const char* dStr = strstr(obj.c_str(), "\"day\"");
        if (dStr) {
            dStr += 5;
            while (*dStr == ' ' || *dStr == ':') dStr++;
            h.day = atoi(dStr);
        }

        const char* pubStr = strstr(obj.c_str(), "\"isPublicHoliday\"");
        if (pubStr) {
            pubStr += 17;
            while (*pubStr == ' ' || *pubStr == ':') pubStr++;
            if (strncmp(pubStr, "true", 4) == 0) h.isPublicHoliday = true;
        }

        auto extractStr = [](const std::string& src, const char* key) -> std::wstring {
            std::string searchKey = std::string("\"") + key + "\"";
            const char* kPos = strstr(src.c_str(), searchKey.c_str());
            if (!kPos) return L"";
            kPos += searchKey.size();
            while (*kPos == ' ' || *kPos == ':') kPos++;
            if (*kPos != '"') return L"";
            kPos++;

            std::string val = "";
            while (*kPos && *kPos != '"') {
                if (*kPos == '\\' && *(kPos + 1)) {
                    kPos++;
                    if (*kPos == 'u' && isxdigit(*(kPos+1)) && isxdigit(*(kPos+2)) && isxdigit(*(kPos+3)) && isxdigit(*(kPos+4))) {
                        char hex[5] = { *(kPos+1), *(kPos+2), *(kPos+3), *(kPos+4), 0 };
                        wchar_t wc = (wchar_t)strtol(hex, NULL, 16);
                        wchar_t tempW[2] = { wc, 0 };
                        char tempUtf8[8] = { 0 };
                        WideCharToMultiByte(CP_UTF8, 0, tempW, 1, tempUtf8, sizeof(tempUtf8), NULL, NULL);
                        val += tempUtf8;
                        kPos += 4;
                    } else if (*kPos == 'n') {
                        val += '\n';
                    } else if (*kPos == 't') {
                        val += '\t';
                    } else if (*kPos == '"') {
                        val += '"';
                    } else if (*kPos == '\\') {
                        val += '\\';
                    } else {
                        val += *kPos;
                    }
                } else {
                    val += *kPos;
                }
                kPos++;
            }
            return Utf8ToWide(val);
        };

        h.titleNP = extractStr(obj, "titleNP");
        h.titleEN = extractStr(obj, "titleEN");
        h.category = extractStr(obj, "category");
        h.description = extractStr(obj, "description");

        if (h.month >= 1 && h.month <= 12 && h.day >= 1 && h.day <= 32) {
            outHolidays.push_back(h);
        }

        p = objEnd + 1;
    }

    return !outHolidays.empty();
}

bool HttpDownloadString(const wchar_t* host, const wchar_t* path, std::string& outBody) {
    HINTERNET hSession = WinHttpOpen(L"NepaliDateWidget/" APP_VERSION,
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) return false;

    // Fast timeouts so offline / DNS failures return quickly and never freeze threads
    WinHttpSetTimeouts(hSession, 2000, 2500, 3000, 3000);

    HINTERNET hConnect = WinHttpConnect(hSession, host, INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) {
        WinHttpCloseHandle(hSession);
        return false;
    }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", path,
        NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }

    bool success = false;
    if (WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
        WinHttpReceiveResponse(hRequest, NULL)) {
        
        DWORD statusCode = 0;
        DWORD statusSize = sizeof(statusCode);
        WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                            WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &statusSize, WINHTTP_NO_HEADER_INDEX);

        if (statusCode == 200) {
            std::string response;
            char buf[4096];
            DWORD bytesRead = 0;
            while (WinHttpReadData(hRequest, buf, sizeof(buf) - 1, &bytesRead) && bytesRead > 0) {
                buf[bytesRead] = 0;
                response.append(buf, bytesRead);
                bytesRead = 0;
            }
            if (!response.empty()) {
                outBody = response;
                success = true;
            }
        }
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return success;
}

DWORD WINAPI FetchHolidaysThread(LPVOID lpParam) {
    int bsYear = (int)(INT_PTR)lpParam;
    g_isHolidayFetchInProgress = true;
    g_holidayFetchState = FETCH_DOWNLOADING;
    g_lastHolidayFetchAttemptTime = GetTickCount();

    std::string jsonBody;
    bool downloaded = false;

    // 1. Try primary GitHub raw URL
    std::wstring path1 = L"/aayushlbef/Nepali-Date-Widget/main/data/holidays_" + std::to_wstring(bsYear) + L".json";
    if (HttpDownloadString(L"raw.githubusercontent.com", path1.c_str(), jsonBody)) {
        downloaded = true;
    }

    // 2. Try secondary jsDelivr CDN
    if (!downloaded) {
        std::wstring path2 = L"/gh/aayushlbef/Nepali-Date-Widget@main/data/holidays_" + std::to_wstring(bsYear) + L".json";
        if (HttpDownloadString(L"cdn.jsdelivr.net", path2.c_str(), jsonBody)) {
            downloaded = true;
        }
    }

    if (downloaded) {
        std::vector<NepaliHoliday> parsed;
        if (ParseHolidaysJson(jsonBody, bsYear, parsed)) {
            SaveEncodedHolidayFile(bsYear, parsed);
            g_currentYearHolidays = parsed;
            g_loadedHolidayYear = bsYear;
            g_holidayFetchState = FETCH_SUCCESS;
            g_isHolidayFetchInProgress = false;
            if (g_hWnd) PostMessage(g_hWnd, WM_HOLIDAYS_LOADED, (WPARAM)bsYear, 0);
            return 0;
        }
    }

    // Failed to download and not available in cache
    g_holidayFetchState = FETCH_ERROR_OFFLINE;
    g_isHolidayFetchInProgress = false;
    if (g_hWnd) PostMessage(g_hWnd, WM_HOLIDAYS_FAILED, (WPARAM)bsYear, 0);
    return 0;
}

void EnsureHolidaysLoadedForCurrentYear(int bsYear, bool forceRefresh = false) {
    if (!forceRefresh && g_loadedHolidayYear == bsYear && g_holidayFetchState == FETCH_SUCCESS) {
        return;
    }

    // 1. Try encoded cache file first if not force refresh
    if (!forceRefresh) {
        std::vector<NepaliHoliday> cached;
        if (LoadEncodedHolidayFile(bsYear, cached)) {
            g_currentYearHolidays = cached;
            g_loadedHolidayYear = bsYear;
            g_holidayFetchState = FETCH_SUCCESS;
            return;
        }
    }

    // 2. Prevent multiple concurrent fetch threads
    if (g_isHolidayFetchInProgress) {
        return;
    }

    // 3. If offline failure already occurred and not force refresh, rate-limit retries (30-second cooldown)
    if (!forceRefresh && g_holidayFetchState == FETCH_ERROR_OFFLINE) {
        DWORD now = GetTickCount();
        if (now - g_lastHolidayFetchAttemptTime < 30000) {
            return;
        }
    }

    g_lastHolidayFetchAttemptTime = GetTickCount();
    HANDLE hThread = CreateThread(NULL, 0, FetchHolidaysThread, (LPVOID)(INT_PTR)bsYear, 0, NULL);
    if (hThread) CloseHandle(hThread);
}

const NepaliHoliday* GetHolidayForBS(int bsYear, int bsMonth, int bsDay) {
    if (g_holidayFetchState != FETCH_SUCCESS || g_loadedHolidayYear != bsYear) {
        return NULL;
    }
    for (size_t i = 0; i < g_currentYearHolidays.size(); ++i) {
        if (g_currentYearHolidays[i].month == bsMonth && g_currentYearHolidays[i].day == bsDay) {
            return &g_currentYearHolidays[i];
        }
    }
    return NULL;
}

inline int GetBSDaysInMonth(int bsYear, int bsMonth) {
    if (bsYear < 1975 || bsYear > 2100 || bsMonth < 1 || bsMonth > 12) return 30;
    return bs_month_days[bsYear - 1975][bsMonth - 1];
}

inline long long BSToDaysSinceEpoch(int bsYear, int bsMonth, int bsDay) {
    if (bsYear < 1975 || bsYear > 2100 || bsMonth < 1 || bsMonth > 12) return -1;
    long long days = 0;
    for (int y = 1975; y < bsYear; ++y) {
        for (int m = 0; m < 12; ++m) {
            days += bs_month_days[y - 1975][m];
        }
    }
    for (int m = 1; m < bsMonth; ++m) {
        days += bs_month_days[bsYear - 1975][m - 1];
    }
    days += (bsDay - 1);
    return days;
}

inline int GetBSMonthStartDayOfWeek(int bsYear, int bsMonth) {
    long long days = BSToDaysSinceEpoch(bsYear, bsMonth, 1);
    if (days < 0) return 0;
    return (int)((6 + days) % 7); // 0 = Sun, 1 = Mon, ..., 6 = Sat
}

inline bool BSToAD(int bsYear, int bsMonth, int bsDay, int& adYear, int& adMonth, int& adDay, int& outDayOfWeek) {
    long long days = BSToDaysSinceEpoch(bsYear, bsMonth, bsDay);
    if (days < 0) return false;
    outDayOfWeek = (int)((6 + days) % 7);

    SYSTEMTIME stRef = { 0 };
    stRef.wYear = 1918;
    stRef.wMonth = 4;
    stRef.wDay = 13;
    FILETIME ftRef;
    SystemTimeToFileTime(&stRef, &ftRef);

    ULARGE_INTEGER uRef;
    uRef.LowPart = ftRef.dwLowDateTime;
    uRef.HighPart = ftRef.dwHighDateTime;
    uRef.QuadPart += (ULONGLONG)days * 864000000000ULL;

    FILETIME ftTarget;
    ftTarget.dwLowDateTime = uRef.LowPart;
    ftTarget.dwHighDateTime = uRef.HighPart;
    SYSTEMTIME stTarget;
    FileTimeToSystemTime(&ftTarget, &stTarget);

    adYear = stTarget.wYear;
    adMonth = stTarget.wMonth;
    adDay = stTarget.wDay;
    return true;
}

inline bool ADToBS(int adYear, int adMonth, int adDay, int& bsYear, int& bsMonth, int& bsDay, int& outDayOfWeek) {
    SYSTEMTIME stNow = { 0 };
    stNow.wYear = (WORD)adYear;
    stNow.wMonth = (WORD)adMonth;
    stNow.wDay = (WORD)adDay;
    FILETIME ftNow;
    SystemTimeToFileTime(&stNow, &ftNow);

    SYSTEMTIME stRef = { 0 };
    stRef.wYear = 1918;
    stRef.wMonth = 4;
    stRef.wDay = 13;
    FILETIME ftRef;
    SystemTimeToFileTime(&stRef, &ftRef);

    ULARGE_INTEGER uNow, uRef;
    uNow.LowPart = ftNow.dwLowDateTime;
    uNow.HighPart = ftNow.dwHighDateTime;
    uRef.LowPart = ftRef.dwLowDateTime;
    uRef.HighPart = ftRef.dwHighDateTime;

    if (uNow.QuadPart < uRef.QuadPart) return false;
    long long diff = uNow.QuadPart - uRef.QuadPart;
    long long days = diff / 864000000000LL;
    outDayOfWeek = (int)((6 + days) % 7);

    int y = 1975;
    int m = 1;
    int d = 1;

    while (y <= 2100) {
        int dim = bs_month_days[y - 1975][m - 1];
        if (days >= dim) {
            days -= dim;
            m++;
            if (m > 12) {
                m = 1;
                y++;
            }
        } else {
            break;
        }
    }
    d += (int)days;
    if (y > 2100) return false;

    bsYear = y;
    bsMonth = m;
    bsDay = d;
    return true;
}

inline void GetCurrentBSDate(int& outYear, int& outMonth, int& outDay, int& outDayOfWeek) {
    SYSTEMTIME st;
    GetLocalTime(&st);
    ADToBS(st.wYear, st.wMonth, st.wDay, outYear, outMonth, outDay, outDayOfWeek);
}

inline std::wstring ToDevanagariNum(int num) {
    static const wchar_t devanagariDigits[] = { L'०', L'१', L'२', L'३', L'४', L'५', L'६', L'७', L'८', L'९' };
    wchar_t buf[32];
    swprintf(buf, 32, L"%d", num);
    std::wstring result = L"";
    for (int i = 0; buf[i] != 0; ++i) {
        if (buf[i] >= L'0' && buf[i] <= L'9') {
            result += devanagariDigits[buf[i] - L'0'];
        } else {
            result += buf[i];
        }
    }
    return result;
}

std::wstring GetNepaliDateString() {
    static WORD s_lastDay = 0, s_lastMonth = 0, s_lastYear = 0;
    static std::wstring s_cachedDateStr = L"";

    SYSTEMTIME st;
    GetLocalTime(&st);
    
    // Minimal resource check: only recalculate if the day has changed!
    if (st.wDay == s_lastDay && st.wMonth == s_lastMonth && st.wYear == s_lastYear && !s_cachedDateStr.empty()) {
        return s_cachedDateStr;
    }
    
    s_lastDay = st.wDay;
    s_lastMonth = st.wMonth;
    s_lastYear = st.wYear;
    
    int bs_year, bs_month, bs_day, bs_dow;
    if (!ADToBS(st.wYear, st.wMonth, st.wDay, bs_year, bs_month, bs_day, bs_dow)) {
        return L"Date Error";
    }
    
    wchar_t buffer[64];
    swprintf(buffer, 64, L"%d / %02d / %02d", bs_year, bs_month, bs_day);
    s_cachedDateStr = std::wstring(buffer);
    
    return s_cachedDateStr;
}

// ── Shared Drawing Helpers ───────────────────────────────────────────────────
void AddRoundedRectangle(GraphicsPath& path, REAL x, REAL y, REAL w, REAL h, REAL r) {
    REAL d = r * 2.0f;
    if (d > w) d = w;
    if (d > h) d = h;
    path.AddArc(x, y, d, d, 180, 90);
    path.AddArc(x + w - d, y, d, d, 270, 90);
    path.AddArc(x + w - d, y + h - d, d, d, 0, 90);
    path.AddArc(x, y + h - d, d, d, 90, 90);
    path.CloseFigure();
}

void DrawNepaliFlag(Graphics& graphics, REAL cx, REAL top, REAL h) {
    REAL flagWidth = 0.822f * h;
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
}

void DrawIconButton(Graphics& graphics, const RectF& rect, const wchar_t* text, bool isHovered, bool isLight, Font& font) {
    GraphicsPath path;
    AddRoundedRectangle(path, rect.X, rect.Y, rect.Width, rect.Height, 6.0f * g_dpiScale);
    
    if (isHovered) {
        SolidBrush hoverBg(isLight ? Color(35, 0, 0, 0) : Color(45, 255, 255, 255));
        graphics.FillPath(&hoverBg, &path);
        Pen hoverBorder(isLight ? Color(70, 0, 0, 0) : Color(70, 255, 255, 255), 1.0f);
        graphics.DrawPath(&hoverBorder, &path);
    } else {
        SolidBrush normalBg(isLight ? Color(15, 0, 0, 0) : Color(20, 255, 255, 255));
        graphics.FillPath(&normalBg, &path);
    }

    SolidBrush textBrush(isLight ? (isHovered ? Color(255, 0, 0, 0) : Color(220, 40, 40, 40))
                                 : (isHovered ? Color(255, 255, 255, 255) : Color(220, 220, 220, 220)));
    StringFormat format;
    format.SetAlignment(StringAlignmentCenter);
    format.SetLineAlignment(StringAlignmentCenter);
    graphics.DrawString(text, -1, &font, rect, &format, &textBrush);
}

// ── Calendar Popup State & Engine ────────────────────────────────────────────
HWND g_hCalWnd = NULL;
bool g_isCalendarOpen = false;
DWORD g_lastCalCloseTime = 0;
int g_calBaseX = 0, g_calBaseY = 0;
bool g_flyoutOnLeft = false;
int g_calYear = 2082;
int g_calMonth = 11;
int g_calSelectedDay = 9;
int g_hoveredBtn = -1;
int g_hoveredCell = -1;
bool g_hasDragged = false;

enum CalendarButtonId {
    BTN_NONE = -1,
    BTN_PREV_MONTH = 102,
    BTN_NEXT_MONTH = 103,
    BTN_TODAY = 105,
    BTN_CLOSE = 106,
    BTN_RETRY_FETCH = 107
};

void HideCalendar();
void ShowCalendar(HWND hWidgetWnd);
void ToggleCalendar(HWND hWidgetWnd);

void RenderCalendar(HWND hWnd) {
    if (!hWnd) return;

    // Enforce calendar strictly to current BS year
    int curBSY = 0, curBSM = 0, curBSD = 0, curBSDOW = 0;
    GetCurrentBSDate(curBSY, curBSM, curBSD, curBSDOW);
    g_calYear = curBSY;

    if (g_calMonth < 1) g_calMonth = 1;
    if (g_calMonth > 12) g_calMonth = 12;

    int totalD = GetBSDaysInMonth(g_calYear, g_calMonth);
    if (g_calSelectedDay < 1) g_calSelectedDay = 1;
    if (g_calSelectedDay > totalD) g_calSelectedDay = totalD;

    EnsureHolidaysLoadedForCurrentYear(g_calYear);

    const NepaliHoliday* selHoliday = GetHolidayForBS(g_calYear, g_calMonth, g_calSelectedDay);
    bool isOfflineMode = (g_holidayFetchState == FETCH_ERROR_OFFLINE && g_currentYearHolidays.empty());
    bool isDownloading = (g_holidayFetchState == FETCH_DOWNLOADING && g_currentYearHolidays.empty());
    bool hasEvent = (selHoliday != NULL || isOfflineMode || isDownloading);

    HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfo(hMon, &mi);

    int baseW = 420;
    int baseH = 440;
    float s = g_dpiScale;

    float flyoutW = 380.0f;
    float flyoutH = 290.0f;
    float arrowW = 12.0f;
    float flyoutTotalW = (flyoutW + (isOfflineMode || isDownloading ? 12.0f : arrowW)) * s;

    // Determine if flyout fits on right or should flip to left side
    bool isFlyoutOnLeft = false;
    if (hasEvent) {
        if (g_calBaseX + (int)(baseW * s) + (int)flyoutTotalW > mi.rcWork.right) {
            isFlyoutOnLeft = true;
        }
    }
    g_flyoutOnLeft = isFlyoutOnLeft;

    int rawW = (int)((hasEvent ? (baseW + arrowW + flyoutW + 12.0f) : baseW) * s);
    int rawH = (int)(baseH * s);

    int screenX = g_calBaseX;
    float calCardOffsetX = 0.0f;
    if (hasEvent && isFlyoutOnLeft) {
        screenX = g_calBaseX - (int)flyoutTotalW;
        if (screenX < mi.rcWork.left) screenX = mi.rcWork.left;
        calCardOffsetX = (float)(g_calBaseX - screenX);
    }
    int screenY = g_calBaseY;

    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = rawW;
    bmi.bmiHeader.biHeight = -rawH;
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
    graphics.Clear(Color(0, 0, 0, 0));

    FontFamily fontFamily(L"Segoe UI");
    Font fontTitle(&fontFamily, 14.0f * s, FontStyleBold, UnitPixel);
    Font fontSubTitle(&fontFamily, 11.0f * s, FontStyleRegular, UnitPixel);
    Font fontButton(&fontFamily, 12.0f * s, FontStyleBold, UnitPixel);
    Font fontNavTitle(&fontFamily, 15.5f * s, FontStyleBold, UnitPixel);
    Font fontNavSub(&fontFamily, 11.5f * s, FontStyleRegular, UnitPixel);
    Font fontDayHeader(&fontFamily, 12.5f * s, FontStyleBold, UnitPixel);
    Font fontBSDay(&fontFamily, 16.5f * s, FontStyleBold, UnitPixel);
    Font fontADDay(&fontFamily, 11.0f * s, FontStyleRegular, UnitPixel);
    Font fontDetailL1(&fontFamily, 16.5f * s, FontStyleBold, UnitPixel);
    Font fontDetailL2(&fontFamily, 13.0f * s, FontStyleBold, UnitPixel);
    Font fontDetailL3(&fontFamily, 12.5f * s, FontStyleRegular, UnitPixel);
    Font fontBadge(&fontFamily, 10.0f * s, FontStyleBold, UnitPixel);

    StringFormat formatNear, formatCenter, formatFar;
    formatNear.SetAlignment(StringAlignmentNear);
    formatNear.SetLineAlignment(StringAlignmentCenter);
    formatCenter.SetAlignment(StringAlignmentCenter);
    formatCenter.SetLineAlignment(StringAlignmentCenter);
    formatFar.SetAlignment(StringAlignmentFar);
    formatFar.SetLineAlignment(StringAlignmentCenter);

    // ── 1. Render Calendar Base Card ─────────────────────────────────────────
    graphics.TranslateTransform(calCardOffsetX, 0.0f);

    GraphicsPath cardPath;
    AddRoundedRectangle(cardPath, 1.0f * s, 1.0f * s, (baseW - 2.0f) * s, (baseH - 2.0f) * s, 14.0f * s);
    
    if (g_isLightTheme) {
        SolidBrush cardBrush(Color(255, 255, 255, 255));
        graphics.FillPath(&cardBrush, &cardPath);
        Pen cardPen(Color(50, 0, 0, 0), 1.0f * s);
        graphics.DrawPath(&cardPen, &cardPath);
    } else {
        SolidBrush cardBrush(Color(255, 30, 30, 36));
        graphics.FillPath(&cardBrush, &cardPath);
        Pen cardPen(Color(65, 255, 255, 255), 1.0f * s);
        graphics.DrawPath(&cardPen, &cardPath);
    }

    // Top Header (Flag + Title + Today Button + Close Button)
    DrawNepaliFlag(graphics, 18.0f * s, 16.0f * s, 26.0f * s);

    SolidBrush textPrimary(g_isLightTheme ? Color(255, 20, 20, 20) : Color(255, 255, 255, 255));
    SolidBrush textSubtle(g_isLightTheme ? Color(180, 100, 100, 100) : Color(180, 170, 170, 175));
    SolidBrush crimsonText(Color(255, 235, 35, 65));

    RectF titleRect(48.0f * s, 14.0f * s, 240.0f * s, 18.0f * s);
    graphics.DrawString(L"Nepali Calendar", -1, &fontTitle, titleRect, &formatNear, &textPrimary);

    RectF subTitleRect(48.0f * s, 33.0f * s, 240.0f * s, 16.0f * s);
    graphics.DrawString(L"\u0928\u0947\u092A\u093E\u0932\u0940 \u092A\u093E\u0924\u094D\u0930\u094B \u2022 Bikram Sambat", -1, &fontSubTitle, subTitleRect, &formatNear, &textSubtle);

    RectF todayBtnRect(300.0f * s, 14.0f * s, 68.0f * s, 30.0f * s);
    DrawIconButton(graphics, todayBtnRect, L"Today", g_hoveredBtn == BTN_TODAY, g_isLightTheme, fontButton);

    RectF closeBtnRect(374.0f * s, 14.0f * s, 30.0f * s, 30.0f * s);
    DrawIconButton(graphics, closeBtnRect, L"\u2715", g_hoveredBtn == BTN_CLOSE, g_isLightTheme, fontButton);

    // Navigation Bar (< Prev Month, Month Year, Next Month >)
    RectF prevMonthRect(18.0f * s, 60.0f * s, 36.0f * s, 34.0f * s);
    DrawIconButton(graphics, prevMonthRect, L"\u2039", g_hoveredBtn == BTN_PREV_MONTH, g_isLightTheme, fontButton);

    RectF nextMonthRect(366.0f * s, 60.0f * s, 36.0f * s, 34.0f * s);
    DrawIconButton(graphics, nextMonthRect, L"\u203A", g_hoveredBtn == BTN_NEXT_MONTH, g_isLightTheme, fontButton);

    int mIdx = g_calMonth - 1;
    std::wstring navTitle = std::wstring(kNepaliMonthNamesEN[mIdx]) + L" " + std::to_wstring(g_calYear) +
                           L"  \u2022  " + kNepaliMonthNamesNP[mIdx] + L" " + ToDevanagariNum(g_calYear);
    RectF navTitleRect(58.0f * s, 58.0f * s, 304.0f * s, 22.0f * s);
    graphics.DrawString(navTitle.c_str(), -1, &fontNavTitle, navTitleRect, &formatCenter, &textPrimary);

    int adY1 = 0, adM1 = 0, adD1 = 0, dow1 = 0;
    int adY2 = 0, adM2 = 0, adD2 = 0, dow2 = 0;
    BSToAD(g_calYear, g_calMonth, 1, adY1, adM1, adD1, dow1);
    BSToAD(g_calYear, g_calMonth, totalD, adY2, adM2, adD2, dow2);

    std::wstring adRangeStr;
    if (adM1 == adM2) {
        adRangeStr = std::wstring(kEnglishMonthNames[adM1 - 1]) + L" " + std::to_wstring(adD1) +
                     L" - " + std::to_wstring(adD2) + L", " + std::to_wstring(adY1);
    } else {
        adRangeStr = std::wstring(kEnglishMonthNames[adM1 - 1]) + L" " + std::to_wstring(adD1) +
                     L" - " + kEnglishMonthNames[adM2 - 1] + L" " + std::to_wstring(adD2) +
                     L", " + std::to_wstring(adY2);
    }
    RectF navSubRect(58.0f * s, 80.0f * s, 304.0f * s, 16.0f * s);
    graphics.DrawString(adRangeStr.c_str(), -1, &fontNavSub, navSubRect, &formatCenter, &textSubtle);

    // Days of Week Header Row
    const wchar_t* dowNames[] = { L"Sun", L"Mon", L"Tue", L"Wed", L"Thu", L"Fri", L"Sat" };
    float colW = 55.0f * s;
    float rowH = 48.0f * s;
    float gridLeft = 18.0f * s;
    float gridTop = 104.0f * s;

    for (int i = 0; i < 7; i++) {
        RectF headerRect(gridLeft + i * colW, gridTop, colW, 26.0f * s);
        SolidBrush dowBrush(i == 6 ? Color(255, 235, 35, 65) : (i == 0 ? (g_isLightTheme ? Color(255, 200, 30, 30) : Color(255, 255, 100, 100)) : (g_isLightTheme ? Color(255, 100, 100, 100) : Color(255, 170, 170, 175))));
        graphics.DrawString(dowNames[i], -1, &fontDayHeader, headerRect, &formatCenter, &dowBrush);
    }

    // Month Days Grid (6 Rows x 7 Columns)
    int startDow = GetBSMonthStartDayOfWeek(g_calYear, g_calMonth);
    float cellsTop = 138.0f * s;

    float selectedCellCenterY = cellsTop + 24.0f * s;

    for (int r = 0; r < 6; r++) {
        for (int c = 0; c < 7; c++) {
            int cellIdx = r * 7 + c;
            int dayNum = cellIdx - startDow + 1;

            if (dayNum >= 1 && dayNum <= totalD) {
                float cellX = gridLeft + c * colW;
                float cellY = cellsTop + r * rowH;
                RectF cellRect(cellX + 2.0f * s, cellY + 2.0f * s, colW - 4.0f * s, rowH - 4.0f * s);

                bool isToday = (g_calYear == curBSY && g_calMonth == curBSM && dayNum == curBSD);
                bool isSelected = (dayNum == g_calSelectedDay);
                bool isHovered = (g_hoveredCell == cellIdx);
                bool isSaturday = (c == 6);

                const NepaliHoliday* hInfo = GetHolidayForBS(g_calYear, g_calMonth, dayNum);
                bool isHoliday = (hInfo != NULL && hInfo->isPublicHoliday);
                bool isFestival = (hInfo != NULL && !hInfo->isPublicHoliday);

                if (isSelected) {
                    selectedCellCenterY = cellY + (rowH / 2.0f);
                }

                // Cell Background Styling
                GraphicsPath cellPath;
                AddRoundedRectangle(cellPath, cellRect.X, cellRect.Y, cellRect.Width, cellRect.Height, 8.0f * s);

                if (isToday) {
                    SolidBrush todayBg(Color(255, 220, 20, 60));
                    graphics.FillPath(&todayBg, &cellPath);
                } else if (isSelected) {
                    SolidBrush selBg(g_isLightTheme ? Color(40, 0, 56, 147) : Color(50, 255, 255, 255));
                    graphics.FillPath(&selBg, &cellPath);
                    Pen selPen(g_isLightTheme ? Color(180, 0, 56, 147) : Color(180, 255, 255, 255), 1.5f * s);
                    graphics.DrawPath(&selPen, &cellPath);
                } else if (isHovered) {
                    SolidBrush hovBg(g_isLightTheme ? Color(25, 0, 0, 0) : Color(30, 255, 255, 255));
                    graphics.FillPath(&hovBg, &cellPath);
                }

                // BS Day Number
                std::wstring bsDayStr = std::to_wstring(dayNum);
                RectF bsDayRect(cellRect.X, cellRect.Y + 4.0f * s, cellRect.Width, 22.0f * s);

                SolidBrush bsDayBrush(isToday ? Color(255, 255, 255, 255) :
                                      (isHoliday || isSaturday ? Color(255, 235, 35, 65) :
                                      (g_isLightTheme ? Color(255, 20, 20, 20) : Color(255, 255, 255, 255))));

                graphics.DrawString(bsDayStr.c_str(), -1, &fontBSDay, bsDayRect, &formatCenter, &bsDayBrush);

                // English AD Day Sub-Number
                int cAdY = 0, cAdM = 0, cAdD = 0, cDow = 0;
                BSToAD(g_calYear, g_calMonth, dayNum, cAdY, cAdM, cAdD, cDow);
                std::wstring adDayStr = std::to_wstring(cAdD);
                RectF adDayRect(cellRect.X, cellRect.Y + 26.0f * s, cellRect.Width, 14.0f * s);

                SolidBrush adDayBrush(isToday ? Color(220, 255, 255, 255) :
                                      (g_isLightTheme ? Color(160, 100, 100, 100) : Color(160, 170, 170, 175)));

                graphics.DrawString(adDayStr.c_str(), -1, &fontADDay, adDayRect, &formatCenter, &adDayBrush);

                // Indicator dot for Holidays/Events
                if (isHoliday || isFestival) {
                    float dotDiam = 4.5f * s;
                    float dotX = cellRect.X + (cellRect.Width - dotDiam) / 2.0f;
                    float dotY = cellRect.Y + cellRect.Height - dotDiam - 2.0f * s;

                    SolidBrush dotBrush(isToday ? Color(255, 255, 255, 255) :
                                        (isHoliday ? Color(255, 235, 35, 65) : Color(255, 0, 120, 215)));
                    graphics.FillEllipse(&dotBrush, dotX, dotY, dotDiam, dotDiam);
                }
            }
        }
    }

    graphics.ResetTransform();

    // ── 2. Render Event Card / Offline Notice Flyout ─────────────────────────
    if (hasEvent) {
        float fx, fy;
        if (isFlyoutOnLeft) {
            fx = 6.0f * s;
            fy = (isOfflineMode || isDownloading) ? (14.0f * s) : std::max(14.0f * s, std::min((baseH - flyoutH - 14.0f) * s, selectedCellCenterY - (flyoutH / 2.0f) * s));
        } else {
            fx = calCardOffsetX + (baseW + (isOfflineMode || isDownloading ? 12.0f : arrowW)) * s;
            fy = (isOfflineMode || isDownloading) ? (14.0f * s) : std::max(14.0f * s, std::min((baseH - flyoutH - 14.0f) * s, selectedCellCenterY - (flyoutH / 2.0f) * s));
        }
        float fw = flyoutW * s;
        float fh = flyoutH * s;
        float r = 12.0f * s;

        std::wstring cardTitleNP;
        std::wstring cardTitleEN;
        std::wstring cardDescription;
        bool isPubHol = false;
        bool isOffline = false;

        if (isOfflineMode) {
            isOffline = true;
            cardTitleNP = L"\u0907\u0928\u094D\u091F\u0930\u0928\u0947\u091F \u091C\u0921\u093E\u0928 \u0906\u0935\u0936\u094D\u092F\u0915";
            cardTitleEN = L"Internet Connection Required";
            cardDescription = L"Please connect to the internet to load all events and public holidays for Bikram Sambat " +
                              std::to_wstring(g_calYear) + L". The calendar will automatically sync and cache once connected.";
        } else if (isDownloading) {
            cardTitleNP = L"\u0924\u093E\u0932\u093F\u0915\u093E \u0921\u093E\u0909\u0928\u0932\u094B\u0921 \u0939\u0941\u0901\u0926\u0948\u091B...";
            cardTitleEN = L"Downloading Holidays Schedule...";
            cardDescription = L"Fetching official Bikram Sambat " + std::to_wstring(g_calYear) +
                              L" public holidays and events in background. Please wait a moment...";
        } else if (selHoliday) {
            cardTitleNP = selHoliday->titleNP;
            cardTitleEN = selHoliday->titleEN;
            cardDescription = selHoliday->description;
            isPubHol = selHoliday->isPublicHoliday;
        }

        // Measure needed height for description
        StringFormat formatDesc;
        formatDesc.SetAlignment(StringAlignmentNear);
        formatDesc.SetLineAlignment(StringAlignmentNear);
        RectF layoutDesc(0, 0, (flyoutW - 48.0f) * s, 1000.0f);
        RectF measuredDesc;
        graphics.MeasureString(cardDescription.c_str(), -1, &fontDetailL3, layoutDesc, &formatDesc, &measuredDesc);

        float neededDescH = measuredDesc.Height;
        float dynamicH = 145.0f + (neededDescH / s) + (isOffline ? 45.0f : 20.0f);
        if (dynamicH < 220.0f) dynamicH = 220.0f;
        if (dynamicH > 380.0f) dynamicH = 380.0f;
        fh = dynamicH * s;

        GraphicsPath bubblePath;
        if (isOfflineMode || isDownloading) {
            AddRoundedRectangle(bubblePath, fx, fy, fw, fh, r);
        } else if (isFlyoutOnLeft) {
            // Speech bubble with triangle arrow on the RIGHT pointing to the calendar cell
            float tipX = calCardOffsetX - 2.0f * s;
            float tipY = selectedCellCenterY;
            float topY = tipY - 10.0f * s;
            float botY = tipY + 10.0f * s;

            bubblePath.AddArc(fx, fy, r * 2.0f, r * 2.0f, 180, 90);
            bubblePath.AddArc(fx + fw - r * 2.0f, fy, r * 2.0f, r * 2.0f, 270, 90);
            bubblePath.AddLine(fx + fw, fy + r, fx + fw, topY);
            bubblePath.AddLine(fx + fw, topY, tipX, tipY);
            bubblePath.AddLine(tipX, tipY, fx + fw, botY);
            bubblePath.AddLine(fx + fw, botY, fx + fw, fy + fh - r);
            bubblePath.AddArc(fx + fw - r * 2.0f, fy + fh - r * 2.0f, r * 2.0f, r * 2.0f, 0, 90);
            bubblePath.AddArc(fx, fy + fh - r * 2.0f, r * 2.0f, r * 2.0f, 90, 90);
            bubblePath.CloseFigure();
        } else {
            // Speech bubble with triangle arrow on the LEFT pointing to the calendar cell
            float tipX = calCardOffsetX + baseW * s + 2.0f * s;
            float tipY = selectedCellCenterY;
            float topY = tipY - 10.0f * s;
            float botY = tipY + 10.0f * s;

            bubblePath.AddArc(fx, fy, r * 2.0f, r * 2.0f, 180, 90);
            bubblePath.AddArc(fx + fw - r * 2.0f, fy, r * 2.0f, r * 2.0f, 270, 90);
            bubblePath.AddArc(fx + fw - r * 2.0f, fy + fh - r * 2.0f, r * 2.0f, r * 2.0f, 0, 90);
            bubblePath.AddArc(fx, fy + fh - r * 2.0f, r * 2.0f, r * 2.0f, 90, 90);
            bubblePath.AddLine(fx, fy + fh - r, fx, botY);
            bubblePath.AddLine(fx, botY, tipX, tipY);
            bubblePath.AddLine(tipX, tipY, fx, topY);
            bubblePath.AddLine(fx, topY, fx, fy + r);
            bubblePath.CloseFigure();
        }

        SolidBrush flyoutBg(g_isLightTheme ? Color(255, 255, 255, 255) : Color(255, 30, 30, 36));
        graphics.FillPath(&flyoutBg, &bubblePath);
        Pen flyoutPen(g_isLightTheme ? Color(60, 0, 0, 0) : Color(70, 255, 255, 255), 1.0f * s);
        graphics.DrawPath(&flyoutPen, &bubblePath);

        // Header, Badges, Divider, Title, Description, Retry Button... (Standard rendering omitted for brevity)

        // Retry button if offline
        if (isOffline) {
            RectF retryBtnRect(fx + 26.0f * s, fy + fh - 50.0f * s, 140.0f * s, 30.0f * s);
            DrawIconButton(graphics, retryBtnRect, L"\u21BB Retry / \u092A\u0941\u0928\u0903 \u092A\u094D\u0930\u092F\u093E\u0938", g_hoveredBtn == BTN_RETRY_FETCH, g_isLightTheme, fontButton);
        }
    }

    // Apply per-pixel alpha channel
    POINT ptDst = { screenX, screenY };
    SIZE sizeDst = { rawW, rawH };
    POINT ptSrc = { 0, 0 };
    BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

    UpdateLayeredWindow(hWnd, hdcScreen, &ptDst, &sizeDst, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

    SelectObject(hdcMem, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
}

LRESULT CALLBACK CalendarWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_ACTIVATE:
        if (LOWORD(wParam) == WA_INACTIVE) {
            HideCalendar();
        }
        break;

    case WM_KILLFOCUS:
        HideCalendar();
        break;

    case WM_MOUSEMOVE: {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        float s = g_dpiScale;
        float calOffset = (g_flyoutOnLeft ? (380.0f + 12.0f) : 0.0f);
        float bx = x / s - calOffset;
        float by = y / s;

        int newBtn = BTN_NONE;
        int newCell = -1;

        float retryX = (g_flyoutOnLeft ? (6.0f + 26.0f) : (420.0f + 12.0f + 26.0f));

        if (bx >= 18.0f && bx <= 54.0f && by >= 60.0f && by <= 94.0f) {
            newBtn = BTN_PREV_MONTH;
        } else if (bx >= 366.0f && bx <= 402.0f && by >= 60.0f && by <= 94.0f) {
            newBtn = BTN_NEXT_MONTH;
        } else if (bx >= 300.0f && bx <= 368.0f && by >= 14.0f && by <= 44.0f) {
            newBtn = BTN_TODAY;
        } else if (bx >= 374.0f && bx <= 404.0f && by >= 14.0f && by <= 44.0f) {
            newBtn = BTN_CLOSE;
        } else if (g_holidayFetchState == FETCH_ERROR_OFFLINE && g_currentYearHolidays.empty() &&
                   (x / s) >= retryX && (x / s) <= (retryX + 140.0f) &&
                   by >= 190.0f && by <= 270.0f) {
            newBtn = BTN_RETRY_FETCH;
        } else if (bx >= 18.0f && bx < 403.0f && by >= 138.0f && by < 426.0f) {
            int c = (int)((bx - 18.0f) / 55.0f);
            int r = (int)((by - 138.0f) / 48.0f);
            if (c >= 0 && c < 7 && r >= 0 && r < 6) {
                int cellIdx = r * 7 + c;
                int startDow = GetBSMonthStartDayOfWeek(g_calYear, g_calMonth);
                int totalD = GetBSDaysInMonth(g_calYear, g_calMonth);
                int dayNum = cellIdx - startDow + 1;
                if (dayNum >= 1 && dayNum <= totalD) {
                    newCell = cellIdx;
                }
            }
        }

        if (newBtn != g_hoveredBtn || newCell != g_hoveredCell) {
            g_hoveredBtn = newBtn;
            g_hoveredCell = newCell;
            RenderCalendar(hWnd);
        }

        TRACKMOUSEEVENT tme = { sizeof(tme) };
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = hWnd;
        TrackMouseEvent(&tme);
        break;
    }

    case WM_MOUSELEAVE:
        if (g_hoveredBtn != BTN_NONE || g_hoveredCell != -1) {
            g_hoveredBtn = BTN_NONE;
            g_hoveredCell = -1;
            RenderCalendar(hWnd);
        }
        break;

    case WM_LBUTTONDOWN: {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        float s = g_dpiScale;
        float calOffset = (g_flyoutOnLeft ? (380.0f + 12.0f) : 0.0f);
        float bx = x / s - calOffset;
        float by = y / s;
        float retryX = (g_flyoutOnLeft ? (6.0f + 26.0f) : (420.0f + 12.0f + 26.0f));

        if (bx >= 18.0f && bx <= 54.0f && by >= 60.0f && by <= 94.0f) {
            if (g_calMonth > 1) {
                g_calMonth--;
                RenderCalendar(hWnd);
            }
        } else if (bx >= 366.0f && bx <= 402.0f && by >= 60.0f && by <= 94.0f) {
            if (g_calMonth < 12) {
                g_calMonth++;
                RenderCalendar(hWnd);
            }
        } else if (bx >= 300.0f && bx <= 368.0f && by >= 14.0f && by <= 44.0f) {
            int cy, cm, cd, cdow;
            GetCurrentBSDate(cy, cm, cd, cdow);
            g_calYear = cy;
            g_calMonth = cm;
            g_calSelectedDay = cd;
            RenderCalendar(hWnd);
        } else if (bx >= 374.0f && bx <= 404.0f && by >= 14.0f && by <= 44.0f) {
            HideCalendar();
        } else if (g_hoveredBtn == BTN_RETRY_FETCH ||
                  (g_holidayFetchState == FETCH_ERROR_OFFLINE && g_currentYearHolidays.empty() &&
                   (x / s) >= retryX && (x / s) <= (retryX + 140.0f) &&
                   by >= 190.0f && by <= 270.0f)) {
            EnsureHolidaysLoadedForCurrentYear(g_calYear, true);
            RenderCalendar(hWnd);
        } else if (bx >= 18.0f && bx < 403.0f && by >= 138.0f && by < 426.0f) {
            int c = (int)((bx - 18.0f) / 55.0f);
            int r = (int)((by - 138.0f) / 48.0f);
            if (c >= 0 && c < 7 && r >= 0 && r < 6) {
                int cellIdx = r * 7 + c;
                int startDow = GetBSMonthStartDayOfWeek(g_calYear, g_calMonth);
                int totalD = GetBSDaysInMonth(g_calYear, g_calMonth);
                int dayNum = cellIdx - startDow + 1;
                if (dayNum >= 1 && dayNum <= totalD) {
                    g_calSelectedDay = dayNum;
                    RenderCalendar(hWnd);
                }
            }
        }
        break;
    }

    case WM_MOUSEWHEEL: {
        short delta = GET_WHEEL_DELTA_WPARAM(wParam);
        if (delta > 0 && g_calMonth > 1) g_calMonth--;
        else if (delta < 0 && g_calMonth < 12) g_calMonth++;
        RenderCalendar(hWnd);
        break;
    }

    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) HideCalendar();
        else if (wParam == VK_LEFT) { if (g_calSelectedDay > 1) g_calSelectedDay--; RenderCalendar(hWnd); }
        else if (wParam == VK_RIGHT) { int maxD = GetBSDaysInMonth(g_calYear, g_calMonth); if (g_calSelectedDay < maxD) g_calSelectedDay++; RenderCalendar(hWnd); }
        else if (wParam == VK_UP) { if (g_calSelectedDay > 7) g_calSelectedDay -= 7; RenderCalendar(hWnd); }
        else if (wParam == VK_DOWN) { int maxD = GetBSDaysInMonth(g_calYear, g_calMonth); if (g_calSelectedDay + 7 <= maxD) g_calSelectedDay += 7; RenderCalendar(hWnd); }
        break;

    case WM_DESTROY:
        g_hCalWnd = NULL;
        break;

    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

void HideCalendar() {
    if (!g_isCalendarOpen) return;
    g_isCalendarOpen = false;
    if (g_hCalWnd) ShowWindow(g_hCalWnd, SW_HIDE);
}

void ShowCalendar(HWND hWidgetWnd) {
    if (!g_hCalWnd || g_setupMode) return;
    
    int cy, cm, cd, cdow;
    GetCurrentBSDate(cy, cm, cd, cdow);
    g_calYear = cy;
    g_calMonth = cm;
    g_calSelectedDay = cd;
    g_hoveredBtn = BTN_NONE;
    g_hoveredCell = -1;

    EnsureHolidaysLoadedForCurrentYear(cy);

    int calW = (int)(420 * g_dpiScale);
    int calH = (int)(440 * g_dpiScale);

    RECT rcWidget;
    GetWindowRect(hWidgetWnd, &rcWidget);

    HMONITOR hMon = MonitorFromWindow(hWidgetWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfo(hMon, &mi);

    int widgetCenterX = rcWidget.left + (rcWidget.right - rcWidget.left) / 2;
    int x = widgetCenterX - calW / 2;
    int y = rcWidget.top - calH - (int)(10 * g_dpiScale);

    // Position below widget if above screen top or work area
    if (y < mi.rcWork.top) {
        y = rcWidget.bottom + (int)(10 * g_dpiScale);
    }
    // Clamp inside vertical screen bounds
    if (y + calH > mi.rcWork.bottom) {
        y = mi.rcWork.bottom - calH - (int)(8 * g_dpiScale);
    }
    
    // Clamp inside horizontal screen bounds for the 420px calendar card
    if (x + calW > mi.rcWork.right - (int)(10 * g_dpiScale)) {
        x = mi.rcWork.right - calW - (int)(10 * g_dpiScale);
    }
    if (x < mi.rcWork.left + (int)(10 * g_dpiScale)) {
        x = mi.rcWork.left + (int)(10 * g_dpiScale);
    }

    g_calBaseX = x;
    g_calBaseY = y;

    RenderCalendar(g_hCalWnd);
    ShowWindow(g_hCalWnd, SW_SHOW);
    SetForegroundWindow(g_hCalWnd);
    g_isCalendarOpen = true;
}

void ToggleCalendar(HWND hWidgetWnd) {
    if (g_isCalendarOpen) {
        HideCalendar();
    } else {
        ShowCalendar(hWidgetWnd);
    }
}

// ── Custom Drawing Engine (Per-Pixel Alpha) ──────────────────────────────────
void RenderWidget(HWND hWnd) {
    // Automatically ensure holiday data is loaded for the current BS year
    static int s_lastCheckedBSYear = 0;
    int curBSY = 0, curBSM = 0, curBSD = 0, curBSDOW = 0;
    GetCurrentBSDate(curBSY, curBSM, curBSD, curBSDOW);
    if (curBSY != s_lastCheckedBSYear) {
        s_lastCheckedBSYear = curBSY;
        EnsureHolidaysLoadedForCurrentYear(curBSY);
    }

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
    
    // Y coordinates
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

    // 3. Render Flag
    DrawNepaliFlag(graphics, cx, top, h);

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
    AppendMenu(hMenu, MF_STRING, 7, L"Open Full Calendar");
    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hMenu, MF_STRING, 1, g_setupMode ? L"Lock Position & Make Transparent" : L"Adjust Position");

    bool startupOn = IsStartupEnabled();
    AppendMenu(hMenu, MF_STRING | (startupOn ? MF_CHECKED : 0), 3, L"Run at Startup");
    AppendMenu(hMenu, MF_STRING | (g_showDay ? MF_CHECKED : 0), 5, L"Show Day");
    AppendMenu(hMenu, MF_STRING, 4, L"Check for Updates");
    AppendMenu(hMenu, MF_STRING, 6, L"Support / Donate \u2764");

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
        if (g_setupMode && g_isCalendarOpen) {
            HideCalendar();
        }
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
    } else if (cmd == 6) {
        ShellExecuteW(NULL, L"open", L"https://aayushlbef.github.io/Nepali-Date-Widget/#sponsor", NULL, NULL, SW_SHOWNORMAL);
    } else if (cmd == 7) {
        ToggleCalendar(hWnd);
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

            // Auto-recovery: periodically retry fetching holidays in background when offline
            if (g_holidayFetchState == FETCH_ERROR_OFFLINE && !g_isHolidayFetchInProgress) {
                DWORD now = GetTickCount();
                if (now - g_lastHolidayFetchAttemptTime >= 30000) {
                    int cy, cm, cd, cdow;
                    GetCurrentBSDate(cy, cm, cd, cdow);
                    EnsureHolidaysLoadedForCurrentYear(cy, true);
                }
            }
        } else if (wParam == 2) {
            // ── Fullscreen detection: hide during movies/games ────────
            bool fullscreen = IsFullscreenAppRunning();
            if (fullscreen && !g_hiddenForFullscreen) {
                g_hiddenForFullscreen = true;
                if (g_isCalendarOpen) HideCalendar();
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
            }
        }
        break;

    // ── THE KEY FIX: Prevent taskbar thumbnail previews from hiding us ────
    case WM_SHOWWINDOW:
        if (wParam == FALSE && lParam == SW_PARENTCLOSING) {
            return 0;  // Block the hide — stay visible
        }
        return DefWindowProc(hWnd, msg, wParam, lParam);

    case WM_LBUTTONDOWN:
        if (g_setupMode) {
            g_isDragging = true;
            g_hasDragged = false;
            SetCapture(hWnd);
            GetCursorPos(&g_dragStart);
            g_dragStart.x -= g_xPos;
            g_dragStart.y -= g_yPos;
        }
        break;

    case WM_MOUSEMOVE:
        if (g_isDragging) {
            POINT pt;
            GetCursorPos(&pt);
            int newX = pt.x - g_dragStart.x;
            int newY = pt.y - g_dragStart.y;
            if (abs(newX - g_xPos) > 2 || abs(newY - g_yPos) > 2) {
                g_hasDragged = true;
            }
            g_xPos = newX;
            g_yPos = newY;
            RenderWidget(hWnd);
        }
        break;

    case WM_LBUTTONUP:
        if (g_isDragging) {
            g_isDragging = false;
            ReleaseCapture();
            SaveConfig();
        } else {
            // Only toggle calendar when locked in position (!g_setupMode)
            if (!g_setupMode) {
                if (GetTickCount() - g_lastCalCloseTime > 250) {
                    ToggleCalendar(hWnd);
                }
            }
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

    case WM_HOLIDAYS_LOADED:
        RenderWidget(hWnd);
        if (g_isCalendarOpen && g_hCalWnd) {
            RenderCalendar(g_hCalWnd);
        }
        break;

    case WM_HOLIDAYS_FAILED:
        if (g_isCalendarOpen && g_hCalWnd) {
            RenderCalendar(g_hCalWnd);
        }
        break;

    // ── Theme change detection ─────────────────────────────────────────────
    case WM_SETTINGCHANGE:
        if (lParam && wcscmp((LPCWSTR)lParam, L"ImmersiveColorSet") == 0) {
            g_isLightTheme = DetectWindowsTheme();
            RenderWidget(hWnd);
            if (g_isCalendarOpen && g_hCalWnd) {
                RenderCalendar(g_hCalWnd);
            }
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

    // Register Widget Class
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"NepaliTaskbarWidgetClass";
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(1));
    wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(1));
    RegisterClassEx(&wc);

    // Register Calendar Popup Class
    WNDCLASSEX wcCal = { 0 };
    wcCal.cbSize = sizeof(WNDCLASSEX);
    wcCal.lpfnWndProc = CalendarWndProc;
    wcCal.hInstance = hInstance;
    wcCal.lpszClassName = L"NepaliCalendarPopupClass";
    wcCal.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassEx(&wcCal);

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

    // Create main widget window
    g_hWnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
        L"NepaliTaskbarWidgetClass",
        L"Nepali Date Taskbar Widget",
        WS_POPUP,
        g_xPos, g_yPos, winW, winH,
        NULL, NULL, hInstance, NULL
    );

    // Create calendar popup window
    g_hCalWnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
        L"NepaliCalendarPopupClass",
        L"Nepali Calendar",
        WS_POPUP,
        0, 0, (int)(824 * g_dpiScale), (int)(440 * g_dpiScale),
        NULL, NULL, hInstance, NULL
    );

    // ── Set the Taskbar as the OWNER of this window ─────────────────────
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

    if (g_hCalWnd) {
        DestroyWindow(g_hCalWnd);
        g_hCalWnd = NULL;
    }

    GdiplusShutdown(g_gdiplusToken);

    if (hMutex) {
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
    }
    
    return 0;
}
# Nepali Date Taskbar Widget

A lightweight, standalone C++ desktop widget for Windows that beautifully displays the current Nepali (Bikram Sambat) date, specially design to view it directly above your taskbar.

## 🚀 Features
- **100% Standalone:** No Python, no pip packages, and no dependencies required. Just run the `.exe`!
- **Zero-Distraction Design:** Frameless, borderless, and perfectly transparent. It integrates seamlessly into your desktop environment.
- **Smart Auto-Hide:** Automatically detects when you're playing games or watching movies in fullscreen and hides itself so it doesn't get in your way.
- **DPI Aware:** Renders crystal-clear text and graphics on high-resolution (4K) monitors without blurriness.
- **Interactive System Tray:** Easily lock/unlock its position, set it to run at startup, or exit the widget via the right-click context menu or the system tray icon.

## ⚡ Ultra-Efficient Resource Management
This widget is engineered from the ground up to consume absolutely minimal system resources (essentially 0.00% CPU usage and tiny RAM footprint). 

**How it achieves this:**
Instead of constantly running complex date conversion algorithms, the widget uses an ultra-efficient caching technique:
1. Every second, it asks the Windows OS for the current Gregorian (AD) Date—a nearly zero-cost operation.
2. It checks if the Day, Month, or Year has changed since the last frame.
3. Only if the day has physically changed (which happens exactly once a day at 12:00 AM) will the widget perform the Bikram Sambat conversion logic and re-render the text.
4. For the other 86,399 seconds of the day, it skips all math entirely and just paints the cached text to the screen!

This ensures you have a perfectly accurate, real-time widget that doesn't drain your laptop's battery or steal CPU cycles from your games.

## 🛠️ How It Works
The widget uses the native Windows Win32 API and GDI+ to render directly to your screen:
- **Z-Order Enforcement:** It sets itself as a child of the `Shell_TrayWnd` (the Windows Taskbar) and uses a 250ms background timer to assert its `HWND_TOPMOST` status. This prevents Windows from hiding it behind other windows.
- **Taskbar Previews:** It aggressively intercepts the `WM_SHOWWINDOW` API call to veto `SW_PARENTCLOSING` requests, stopping the Windows Taskbar thumbnail system from accidentally hiding it when you hover over open apps.
- **Native BS Conversion:** The entire Bikram Sambat (1975-2100) calendar mapping is embedded directly into the C++ binary. This eliminates the need for external Python scripts and guarantees offline functionality out-of-the-box.

## 🏃 Getting Started
1. Download `NepaliDateWidget.exe`.
2. Double click to run it.
3. Right-click the widget to unlock its position and drag it to your preferred spot.
4. Right-click again and select "Lock Position" to make it click-through and transparent! You can also check "Run at Startup" so you never have to launch it manually again.

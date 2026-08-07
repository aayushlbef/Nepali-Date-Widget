# Nepali Date Taskbar Widget

A lightweight, native C++ desktop widget for Windows that beautifully displays the current Nepali (Bikram Sambat) date, specially designed to be viewed directly above your taskbar.

## 🚀 Features
- **Theme Auto-Adapt:** Automatically detects your Windows Light/Dark mode and adjusts its colors and text rendering seamlessly.
- **Auto-Updates:** Built-in update checker uses the GitHub Releases API on a background thread to notify you when a new version is available.
- **Customizable Layout:** Toggle the Day of the Week (Sun, Mon, Tue) next to the Nepali flag directly from the context menu.
- **Zero-Distraction Design:** Frameless, borderless, and perfectly transparent with high-quality GDI+ per-pixel alpha blending.
- **Smart Auto-Hide:** Automatically detects when you're playing games or watching movies in fullscreen and hides itself so it doesn't get in your way.
- **DPI Aware:** Renders crystal-clear text and graphics on high-resolution (4K) monitors without blurriness.
- **Professional Installer:** Comes with a standard Windows Installer (`Setup.exe`) for easy installation, Start Menu shortcuts, and clean uninstalls (no Admin privileges required).

## ⚡ Ultra-Efficient Resource Management
This widget is engineered from the ground up to consume absolutely minimal system resources (essentially 0.00% CPU usage and a tiny RAM footprint). 

**How it achieves this:**
Instead of constantly running complex date conversion algorithms, the widget uses an ultra-efficient caching technique:
1. Every second, it asks the Windows OS for the current Gregorian (AD) Date—a nearly zero-cost operation.
2. It checks if the Day, Month, or Year has changed since the last frame.
3. Only if the day has physically changed (which happens exactly once a day at 12:00 AM) will the widget perform the Bikram Sambat conversion logic and re-render the text.
4. For the other 86,399 seconds of the day, it skips all math entirely and just paints the cached text to the screen!

This ensures you have a perfectly accurate, real-time widget that doesn't drain your laptop's battery or steal CPU cycles from your games.

## 🛠️ How It Works
The widget uses the native Windows Win32 API and GDI+ to render directly to your screen:
- **Z-Order Enforcement:** It uses a 250ms background timer to assert its `HWND_TOPMOST` status, preventing Windows from hiding it behind other windows.
- **Taskbar Previews:** It aggressively intercepts the `WM_SHOWWINDOW` API call to veto `SW_PARENTCLOSING` requests, stopping the Windows Taskbar thumbnail system from accidentally hiding it when you hover over open apps.
- **Native BS Conversion:** The entire Bikram Sambat (1975-2100) calendar mapping is embedded directly into the C++ binary. This guarantees offline functionality out-of-the-box.
- **WinHTTP API:** Update checks are performed safely on a detached background thread using native Windows networking to ensure the UI never stutters.

## 🏃 Getting Started
1. Download `NepaliDateWidget_Setup.exe` from the latest Release.
2. Run the installer to add it to your system.
3. Right-click the widget to unlock its position and drag it to your preferred spot on your screen.
4. Right-click again and select **"Lock Position"** to make it click-through and transparent! 
5. Use the right-click menu to configure it to **Run at Startup** or toggle the **Show Day** feature.

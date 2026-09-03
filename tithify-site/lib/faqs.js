export const FAQS = [
  {
    q: "What is Tithify?",
    a: "Tithify is a lightweight, native C++ desktop widget for Windows that displays the current Nepali (Bikram Sambat — वि.सं.) date directly above your taskbar. It includes an interactive Bikram Sambat calendar (1975–2100 BS), Nepali government public holidays, auto light/dark theme adaptation, and uses virtually 0% CPU at idle."
  },
  {
    q: "How do I show the Nepali date on my Windows taskbar?",
    a: "Simply install Tithify using the one-line PowerShell command (`irm https://raw.githubusercontent.com/aayushlbef/Tithify/main/install.ps1 | iex`) or download the standard Windows installer. Once launched, Tithify floats unobtrusively above your taskbar, showing the live Bikram Sambat (वि.सं.) date."
  },
  {
    q: "Is Tithify compatible with Windows 11 and Windows 10?",
    a: "Yes! Tithify is fully tested and optimized for both Windows 11 and Windows 10 (64-bit). It includes native DPI awareness for crystal-clear text rendering across 1080p, 1440p, and 4K high-resolution monitors."
  },
  {
    q: "Does Tithify require an active internet connection?",
    a: "No. The entire Bikram Sambat date conversion engine (covering years 1975 to 2100 BS) is embedded natively in the C++ binary. It works 100% offline with zero network requests or background tracking."
  },
  {
    q: "Does Tithify show Nepali public holidays and festivals?",
    a: "Yes. Clicking the widget opens an interactive Bikram Sambat calendar that highlights Saturdays, government public holidays, and cultural festivals like Dashain, Tihar, Chhath, and Lhosar with dual English and Devanagari details."
  },
  {
    q: "How much CPU and RAM does Tithify consume?",
    a: "Virtually zero. Unlike heavy web-based apps or Electron widgets, Tithify is written in pure native C++ with the Win32 API and GDI+. It consumes 0.00% CPU at idle and takes less than 8MB of RAM."
  },
  {
    q: "Can I customize the widget position and lock it?",
    a: "Yes. Right-click the widget, select 'Unlock Position', and drag it anywhere above your taskbar or screen edge. Right-click again and choose 'Lock Position' to make it transparent, click-through, and locked in place."
  },
  {
    q: "Is Tithify free and open-source?",
    a: "Yes. Tithify is 100% free and open-source under the permissive MIT License. You can review the code, suggest features, or contribute on GitHub."
  }
];

# Nepali Date Widget — Marketing Site

A Next.js (App Router) marketing site for the Nepali Date Taskbar Widget, built with Tailwind CSS and Framer Motion.

## Signature idea

The hero is scroll-scrubbed: as you scroll through the first viewport, the whole scene — sky, mountains, moon → sun, and the floating widget itself — animates from night to day, live-demonstrating the product's "Theme Auto-Adapt" feature instead of just describing it in a bullet point.

## Run locally

```bash
npm install
npm run dev
```

Open http://localhost:3000

## Build

```bash
npm run build
npm start
```

## Structure

- `app/page.js` — assembles all sections
- `components/Hero.js` — scroll-driven day/night hero scene (the signature moment)
- `components/WidgetPill.js` — reusable animated replica of the real widget
- `components/CalendarShowcase.js` — interactive BS calendar mockup (try the arrows)
- `components/Efficiency.js` — animated CPU / efficiency stats
- `components/HowItWorks.js` — Win32 technical highlights
- `components/Install.js` — copyable install commands (PowerShell / Winget / Scoop)
- `components/GettingStarted.js` — post-install four-step guide
- `components/Nav.js`, `components/Footer.js`

## Notes

- Fonts (Sora, Inter, Noto Sans Devanagari) load from Google Fonts in `app/layout.js`.
- The taskbar clock in `Hero.js` is a static placeholder — wire it to `Date.now()` if you want it live.
- All motion respects `prefers-reduced-motion` (see `app/globals.css`).
- Tested with `next build` — production build is clean.

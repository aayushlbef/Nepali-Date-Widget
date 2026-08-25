/** @type {import('tailwindcss').Config} */
module.exports = {
  content: [
    "./app/**/*.{js,jsx}",
    "./components/**/*.{js,jsx}",
  ],
  theme: {
    extend: {
      colors: {
        night: {
          950: "#050811",
          900: "#0b1120",
          800: "#111a30",
          700: "#182644",
        },
        crimson: {
          50: "#fff1f2",
          100: "#ffe4e6",
          200: "#fecdd3",
          500: "#f43f5e",
          600: "#dc2626",
          700: "#be123c",
          DEFAULT: "#dc2626",
          dark: "#991b1b",
        },
        sapphire: {
          50: "#eff6ff",
          100: "#dbeafe",
          200: "#bfdbfe",
          500: "#3b82f6",
          600: "#2563eb",
          700: "#1d4ed8",
          800: "#1e40af",
          DEFAULT: "#2563eb",
        },
        himalaya: {
          blue: "#2563eb",
          indigo: "#4f46e5",
          crimson: "#dc2626",
        },
      },
      fontFamily: {
        display: ["var(--font-display)"],
        body: ["var(--font-body)"],
        deva: ["var(--font-deva)"],
      },
      boxShadow: {
        glow: "0 0 50px -10px rgba(220, 38, 38, 0.45)",
        "glow-blue": "0 0 50px -10px rgba(37, 99, 235, 0.45)",
        "glow-crimson": "0 10px 30px -5px rgba(220, 38, 38, 0.35)",
        pill: "0 8px 30px -6px rgba(0,0,0,0.45), 0 0 0 1px rgba(255,255,255,0.06) inset",
      },
      backgroundImage: {
        "grain": "url(\"data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' width='120' height='120'%3E%3Cfilter id='n'%3E%3CfeTurbulence type='fractalNoise' baseFrequency='0.9' numOctaves='2' stitchTiles='stitch'/%3E%3C/filter%3E%3Crect width='100%25' height='100%25' filter='url(%23n)' opacity='0.035'/%3E%3C/svg%3E\")",
      },
      keyframes: {
        drift: {
          "0%,100%": { transform: "translateY(0px) translateX(0px)" },
          "50%": { transform: "translateY(-14px) translateX(6px)" },
        },
        twinkle: {
          "0%,100%": { opacity: 0.2 },
          "50%": { opacity: 1 },
        },
        rise: {
          "0%": { transform: "translateY(24px)", opacity: 0 },
          "100%": { transform: "translateY(0)", opacity: 1 },
        },
      },
      animation: {
        drift: "drift 6s ease-in-out infinite",
        twinkle: "twinkle 3s ease-in-out infinite",
        rise: "rise 0.8s cubic-bezier(0.16,1,0.3,1) forwards",
      },
    },
  },
  plugins: [],
};

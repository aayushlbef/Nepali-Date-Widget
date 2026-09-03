"use client";

import { motion } from "framer-motion";
import { Layers, ShieldCheck, WifiOff, Palette } from "lucide-react";

const ITEMS = [
  {
    icon: Layers,
    title: "Stays on top",
    body: "A quiet 250ms timer re-asserts HWND_TOPMOST, so Windows never buries it behind other windows.",
  },
  {
    icon: ShieldCheck,
    title: "Survives taskbar previews",
    body: "Intercepts WM_SHOWWINDOW to veto SW_PARENTCLOSING, so hovering an open app can't hide it by accident.",
  },
  {
    icon: WifiOff,
    title: "Works 100% offline",
    body: "Converts and renders the Bikram Sambat date locally on your machine — zero network requests, tracking, or internet connection needed.",
  },
  {
    icon: Palette,
    title: "Syncs with Windows live",
    body: "Listens for WM_SETTINGCHANGE and repaints the instant you flip Light or Dark mode.",
  },
];

export default function HowItWorks() {
  return (
    <section className="relative bg-gradient-to-b from-white via-[#f4f8ff] to-[#eaf4ff] py-28 md:py-36">
      <div className="max-w-7xl mx-auto px-6 md:px-10 grid lg:grid-cols-2 gap-16 items-start">
        <motion.div
          initial={{ opacity: 0, y: 20 }}
          whileInView={{ opacity: 1, y: 0 }}
          viewport={{ once: true, margin: "-80px" }}
          transition={{ duration: 0.6 }}
          className="lg:sticky lg:top-32"
        >
          <span className="text-sm font-semibold tracking-[0.18em] uppercase text-sapphire-600">
            Under the hood
          </span>
          <h2 className="mt-4 font-display text-4xl md:text-5xl font-extrabold text-slate-900 tracking-tight">
            Native Win32.
            <br />
            Nothing borrowed.
          </h2>
          <p className="mt-5 text-lg text-slate-600 leading-relaxed max-w-md">
            Zero Electron bloat, no Chromium or Node.js runtime. Built in pure native C++ with the Win32 API and GDI+, communicating directly with Windows.
          </p>
        </motion.div>

        <div className="space-y-5">
          {ITEMS.map((it, i) => (
            <motion.div
              key={it.title}
              initial={{ opacity: 0, x: 24 }}
              whileInView={{ opacity: 1, x: 0 }}
              viewport={{ once: true, margin: "-60px" }}
              transition={{ duration: 0.5, delay: i * 0.08 }}
              className="flex gap-5 rounded-2xl bg-white border border-blue-100/80 p-6 shadow-[0_4px_20px_-4px_rgba(37,99,235,0.05)] hover:shadow-lg hover:border-sapphire-200 transition-all"
            >
              <div className="w-11 h-11 shrink-0 rounded-xl bg-slate-900 flex items-center justify-center text-white">
                <it.icon className="w-5 h-5" />
              </div>
              <div>
                <h3 className="font-display font-bold text-slate-900">{it.title}</h3>
                <p className="mt-1.5 text-sm text-slate-600 leading-relaxed">{it.body}</p>
              </div>
            </motion.div>
          ))}
        </div>
      </div>
    </section>
  );
}

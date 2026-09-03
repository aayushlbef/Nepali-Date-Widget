"use client";

import { useState } from "react";
import { motion, AnimatePresence } from "framer-motion";
import { ChevronDown, HelpCircle } from "lucide-react";

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

export default function FAQ() {
  const [openIndex, setOpenIndex] = useState(0);

  return (
    <section id="faq" className="relative bg-white py-28 md:py-36 overflow-hidden">
      <div className="max-w-4xl mx-auto px-6 md:px-10">
        <motion.div
          initial={{ opacity: 0, y: 20 }}
          whileInView={{ opacity: 1, y: 0 }}
          viewport={{ once: true, margin: "-80px" }}
          transition={{ duration: 0.6 }}
          className="text-center max-w-xl mx-auto mb-16"
        >
          <span className="inline-flex items-center gap-1.5 text-sm font-semibold tracking-[0.18em] uppercase text-sapphire-600">
            <HelpCircle className="w-4 h-4 text-crimson" />
            Frequently Asked Questions
          </span>
          <h2 className="mt-4 font-display text-4xl md:text-5xl font-extrabold text-slate-900 tracking-tight">
            Everything you need to know.
          </h2>
          <p className="mt-4 text-lg text-slate-600">
            Common questions about Tithify, Bikram Sambat calendar features, Windows taskbar integration, and performance.
          </p>
        </motion.div>

        <div className="space-y-4">
          {FAQS.map((faq, i) => {
            const isOpen = openIndex === i;
            return (
              <div
                key={i}
                className="rounded-2xl border border-slate-200/80 bg-slate-50/50 hover:bg-slate-50 transition-colors overflow-hidden"
              >
                <button
                  type="button"
                  onClick={() => setOpenIndex(isOpen ? -1 : i)}
                  className="flex w-full items-center justify-between p-6 text-left font-display font-semibold text-slate-900 text-lg md:text-xl gap-4"
                  aria-expanded={isOpen}
                >
                  <span>{faq.q}</span>
                  <ChevronDown
                    className={`w-5 h-5 text-slate-500 transition-transform duration-200 shrink-0 ${
                      isOpen ? "rotate-180 text-crimson" : ""
                    }`}
                  />
                </button>
                <AnimatePresence initial={false}>
                  {isOpen && (
                    <motion.div
                      initial={{ height: 0, opacity: 0 }}
                      animate={{ height: "auto", opacity: 1 }}
                      exit={{ height: 0, opacity: 0 }}
                      transition={{ duration: 0.25, ease: "easeInOut" }}
                    >
                      <div className="px-6 pb-6 text-slate-600 leading-relaxed text-base border-t border-slate-200/40 pt-4">
                        {faq.a}
                      </div>
                    </motion.div>
                  )}
                </AnimatePresence>
              </div>
            );
          })}
        </div>
      </div>
    </section>
  );
}

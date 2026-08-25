"use client";

import { useState } from "react";
import { motion } from "framer-motion";
import { Check, Copy, Terminal } from "lucide-react";

const OPTIONS = [
  {
    label: "PowerShell",
    cmd: "irm -useb https://raw.githubusercontent.com/aayushlbef/Tithify/main/install.ps1 | iex",
  },
  {
    label: "Winget",
    cmd: "winget install Aayush.Tithify",
    soon: true,
  },
  {
    label: "Scoop",
    cmd: "scoop install https://raw.githubusercontent.com/aayushlbef/Tithify/main/tithify.json",
  },
];

export default function Install() {
  const [tab, setTab] = useState(0);
  const [copied, setCopied] = useState(false);

  const copy = async () => {
    try {
      await navigator.clipboard.writeText(OPTIONS[tab].cmd);
      setCopied(true);
      setTimeout(() => setCopied(false), 1600);
    } catch {}
  };

  return (
    <section id="install" className="relative bg-gradient-to-b from-[#f4f8ff] to-white py-28 md:py-36 overflow-hidden">
      <div className="max-w-5xl mx-auto px-6 md:px-10 text-center">
        <motion.div
          initial={{ opacity: 0, y: 20 }}
          whileInView={{ opacity: 1, y: 0 }}
          viewport={{ once: true, margin: "-80px" }}
          transition={{ duration: 0.6 }}
        >
          <span className="text-sm font-semibold tracking-[0.18em] uppercase text-sapphire-600">
            Two minutes to install
          </span>
          <h2 className="mt-4 font-display text-4xl md:text-5xl font-extrabold text-slate-900 tracking-tight">
            Pick your way in.
          </h2>
          <p className="mt-4 text-lg text-slate-600">
            Command line, package manager, or a plain installer — whichever fits your setup.
          </p>
        </motion.div>

        <motion.div
          layout
          initial={{ opacity: 0, y: 24 }}
          whileInView={{ opacity: 1, y: 0 }}
          viewport={{ once: true, margin: "-80px" }}
          transition={{ duration: 0.4, ease: [0.16, 1, 0.3, 1] }}
          className={`mt-12 mx-auto rounded-2xl bg-slate-950 border border-slate-800 text-left shadow-2xl shadow-slate-300/40 overflow-hidden transition-[max-width] duration-300 ease-out ${
            tab === 2 ? "max-w-5xl" : "max-w-3xl"
          }`}
        >
          <div className="flex items-center gap-1 px-3 pt-3">
            {OPTIONS.map((o, i) => (
              <button
                key={o.label}
                onClick={() => setTab(i)}
                className={`focus-ring px-4 py-2 text-sm font-medium rounded-t-lg transition-colors ${
                  tab === i ? "bg-slate-800 text-white" : "text-slate-400 hover:text-slate-200"
                }`}
              >
                {o.label}
                {o.soon && (
                  <span className="ml-2 px-1.5 py-0.5 rounded-full text-[10px] font-semibold bg-amber-400/20 text-amber-300">
                    Coming soon
                  </span>
                )}
              </button>
            ))}
          </div>
          <div className="bg-slate-900 px-5 py-5 flex items-center gap-3 border-t border-slate-800">
            <Terminal className="w-4 h-4 text-emerald-400 shrink-0" />
            <code className="flex-1 text-xs sm:text-sm text-emerald-300 font-mono whitespace-nowrap overflow-x-auto [scrollbar-width:none] [-ms-overflow-style:none] [&::-webkit-scrollbar]:hidden">
              {OPTIONS[tab].cmd}
            </code>
            <button
              onClick={copy}
              aria-label="Copy command"
              className="focus-ring shrink-0 w-9 h-9 rounded-lg bg-white/5 hover:bg-white/10 flex items-center justify-center text-slate-300 transition-colors"
            >
              {copied ? <Check className="w-4 h-4 text-emerald-400" /> : <Copy className="w-4 h-4" />}
            </button>
          </div>
          {OPTIONS[tab].soon && (
            <p className="px-5 py-3 text-sm text-amber-500 font-medium border-t border-slate-800/80 bg-amber-500/5">
              Winget support is coming soon — this command will work once the package is published.
            </p>
          )}
        </motion.div>

        <motion.p
          initial={{ opacity: 0 }}
          whileInView={{ opacity: 1 }}
          viewport={{ once: true }}
          transition={{ duration: 0.6, delay: 0.2 }}
          className="mt-6 text-sm text-slate-500"
        >
          Prefer a manual download? Grab{" "}
          <a
            href="https://github.com/aayushlbef/Tithify/releases/latest"
            className="text-crimson font-semibold hover:underline focus-ring"
          >
            Tithify_Setup.exe
          </a>{" "}
          from the latest release. Unsigned builds may trigger a SmartScreen notice —
          <span className="font-medium text-slate-700"> More Info → Run Anyway</span> gets you through.
        </motion.p>
      </div>
    </section>
  );
}

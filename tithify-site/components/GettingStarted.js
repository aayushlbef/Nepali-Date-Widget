"use client";

import { motion } from "framer-motion";

const STEPS = [
  { title: "Unlock & position", body: "Right-click the widget to unlock it, then drag it anywhere above your taskbar." },
  { title: "Lock it in", body: "Right-click again and choose Lock Position — it becomes click-through and seamless." },
  { title: "Open the calendar", body: "Left-click any time to bring up the full interactive Bikram Sambat calendar." },
  { title: "Make it yours", body: "Right-click for startup, day-of-week, theme, and update settings." },
];

export default function GettingStarted() {
  return (
    <section className="relative bg-white pt-16 pb-20 md:pt-20 md:pb-28">
      <div className="max-w-5xl mx-auto px-6 md:px-10">
        <motion.div
          initial={{ opacity: 0, y: 16 }}
          whileInView={{ opacity: 1, y: 0 }}
          viewport={{ once: true }}
          transition={{ duration: 0.5 }}
          className="text-center max-w-lg mx-auto"
        >
          <span className="text-sm font-semibold tracking-[0.18em] uppercase text-sapphire-600">
            After install
          </span>
          <h2 className="mt-4 font-display text-4xl md:text-5xl font-extrabold text-slate-900 tracking-tight">
            Four clicks to settle in.
          </h2>
        </motion.div>

        <div className="mt-16 relative">
          <div className="hidden md:block absolute top-6 left-0 right-0 h-px bg-blue-100/80" />
          <div className="grid md:grid-cols-4 gap-10 md:gap-6">
            {STEPS.map((s, i) => (
              <motion.div
                key={s.title}
                initial={{ opacity: 0, y: 24 }}
                whileInView={{ opacity: 1, y: 0 }}
                viewport={{ once: true, margin: "-60px" }}
                transition={{ duration: 0.5, delay: i * 0.1 }}
                className="group relative"
              >
                <div className="relative z-10 w-12 h-12 rounded-full bg-slate-900 group-hover:bg-crimson group-hover:shadow-glow-crimson text-white flex items-center justify-center font-display font-bold transition-all">
                  {i + 1}
                </div>
                <h3 className="mt-5 font-display font-bold text-slate-900">{s.title}</h3>
                <p className="mt-2 text-sm text-slate-600 leading-relaxed">{s.body}</p>
              </motion.div>
            ))}
          </div>
        </div>
      </div>
    </section>
  );
}

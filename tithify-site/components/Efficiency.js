"use client";

import { useEffect, useRef, useState } from "react";
import { motion, useInView, animate } from "framer-motion";

function Counter({ from = 0, to, decimals = 0, suffix = "", prefix = "" }) {
  const ref = useRef(null);
  const inView = useInView(ref, { once: true, margin: "-60px" });
  const [val, setVal] = useState(from);

  useEffect(() => {
    if (!inView) return;
    const controls = animate(from, to, {
      duration: 1.6,
      ease: [0.16, 1, 0.3, 1],
      onUpdate: (v) => setVal(v),
    });
    return () => controls.stop();
  }, [inView, from, to]);

  return (
    <span ref={ref} className="tabular-nums">
      {prefix}
      {val.toFixed(decimals)}
      {suffix}
    </span>
  );
}

export default function Efficiency() {
  return (
    <section id="efficiency" className="relative bg-gradient-to-b from-[#eaf4ff] via-[#f4f8ff] to-white pt-16 pb-20 md:pt-20 md:pb-28 overflow-hidden">
      <div className="max-w-7xl mx-auto px-6 md:px-10">
        <motion.div
          initial={{ opacity: 0, y: 16 }}
          whileInView={{ opacity: 1, y: 0 }}
          viewport={{ once: true }}
          transition={{ duration: 0.5 }}
          className="max-w-xl"
        >
          <span className="text-sm font-semibold tracking-[0.18em] uppercase text-sapphire-600">
            Ultra-efficient by design
          </span>
          <h2 className="mt-3 md:mt-4 font-display text-4xl md:text-5xl font-extrabold text-slate-900 tracking-tight">
            It checks the clock.
            <br />
            It doesn't watch it.
          </h2>
          <p className="mt-5 text-lg text-slate-600 leading-relaxed">
            Once a second, the widget asks Windows for today's date — a near-zero-cost call. Only
            when the day actually changes does it run the Bikram Sambat conversion and repaint.
          </p>
        </motion.div>

        <div className="mt-16 grid sm:grid-cols-3 gap-5">
          <Stat label="Idle CPU usage">
            <Counter prefix="< " to={0.1} decimals={1} suffix="%" />
          </Stat>
          <Stat label="Conversions run, per day">
            <Counter to={1} />
          </Stat>
          <Stat label="Seconds spent just painting cache">
            <Counter to={86399} />
          </Stat>
        </div>

        <motion.div
          initial={{ opacity: 0 }}
          whileInView={{ opacity: 1 }}
          viewport={{ once: true, margin: "-80px" }}
          transition={{ duration: 0.8, delay: 0.2 }}
          className="mt-14 rounded-2xl border border-blue-100/80 bg-white p-6 md:p-8 shadow-[0_4px_24px_-4px_rgba(37,99,235,0.06)]"
        >
          <div className="flex items-center justify-between text-xs font-medium text-slate-500 mb-3">
            <span>00:00:00</span>
            <span className="font-semibold text-slate-700">a single day, visualized</span>
            <span>23:59:59</span>
          </div>
          <div className="relative h-3.5 rounded-full bg-slate-100 overflow-hidden">
            <motion.div
              initial={{ width: "0%" }}
              whileInView={{ width: "100%" }}
              viewport={{ once: true, margin: "-80px" }}
              transition={{ duration: 2.2, ease: "easeInOut" }}
              className="absolute inset-y-0 left-0 bg-gradient-to-r from-sapphire-500 via-blue-500 to-crimson rounded-full"
            />
            <motion.div
              initial={{ left: "0%" }}
              whileInView={{ left: "99.6%" }}
              viewport={{ once: true, margin: "-80px" }}
              transition={{ duration: 2.2, ease: "easeInOut" }}
              className="absolute -top-1.5 w-6 h-6 rounded-full bg-crimson border-2 border-white shadow-md shadow-crimson/30"
            />
          </div>
          <p className="mt-4 text-sm text-slate-600">
            The single red tick is the one moment of real work — midnight, when the date rolls over.
          </p>
        </motion.div>
      </div>
    </section>
  );
}

function Stat({ label, children }) {
  return (
    <motion.div
      initial={{ opacity: 0, y: 20 }}
      whileInView={{ opacity: 1, y: 0 }}
      viewport={{ once: true, margin: "-60px" }}
      transition={{ duration: 0.5 }}
      className="rounded-2xl border border-blue-100/80 bg-white p-7 shadow-[0_4px_24px_-4px_rgba(37,99,235,0.06)] hover:shadow-lg hover:border-sapphire-200 transition-all"
    >
      <div className="font-display text-4xl md:text-5xl font-extrabold text-slate-900">{children}</div>
      <div className="mt-2 text-sm font-medium text-slate-500">{label}</div>
    </motion.div>
  );
}

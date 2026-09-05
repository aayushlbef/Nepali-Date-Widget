"use client";

import { useState } from "react";
import { motion, AnimatePresence } from "framer-motion";
import { ChevronDown, HelpCircle } from "lucide-react";

import { FAQS } from "@/lib/faqs";

export default function FAQ() {
  const [openIndex, setOpenIndex] = useState(0);

  return (
    <section id="faq" className="relative bg-white pt-16 pb-20 md:pt-20 md:pb-28 overflow-hidden">
      <div className="max-w-4xl mx-auto px-6 md:px-10">
        <motion.div
          initial={{ opacity: 0, y: 16 }}
          whileInView={{ opacity: 1, y: 0 }}
          viewport={{ once: true }}
          transition={{ duration: 0.5 }}
          className="text-center max-w-xl mx-auto mb-10 md:mb-12"
        >
          <span className="inline-flex items-center gap-1.5 text-sm font-semibold tracking-[0.18em] uppercase text-sapphire-600">
            <HelpCircle className="w-4 h-4 text-crimson" />
            Frequently Asked Questions
          </span>
          <h2 className="mt-3 md:mt-4 font-display text-4xl md:text-5xl font-extrabold text-slate-900 tracking-tight">
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

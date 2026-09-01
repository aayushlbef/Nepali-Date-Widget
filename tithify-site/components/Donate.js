"use client";

import { motion } from "framer-motion";
import Image from "next/image";
import { basePath } from "@/lib/basePath";

const DONATE_METHODS = [
  {
    name: "eSewa",
    logo: `${basePath}/donate/esewa-logo.png`,
    qr: `${basePath}/donate/esewa-qr.jpg`,
    accent: "border-emerald-500/20 hover:border-emerald-500/40",
    badgeBg: "bg-emerald-50 text-emerald-700 border border-emerald-200/60",
    description: "Scan using eSewa Mobile App to support Tithify development",
  },
  {
    name: "Khalti",
    logo: `${basePath}/donate/khalti-logo.png`,
    qr: `${basePath}/donate/khalti-qr.png`,
    accent: "border-purple-500/20 hover:border-purple-500/40",
    badgeBg: "bg-purple-50 text-purple-700 border border-purple-200/60",
    description: "Scan using Khalti Digital Wallet app to tip the creator",
  },
];

export default function Donate() {
  return (
    <section id="donate" className="relative bg-gradient-to-b from-white via-[#f4f8ff] to-[#eaf4ff] py-28 md:py-36 overflow-hidden">
      {/* Anchor for legacy or context menu links pointing to #sponsor */}
      <div id="sponsor" className="absolute top-0 left-0" />
      <div className="max-w-5xl mx-auto px-6 md:px-10">
        <motion.div
          initial={{ opacity: 0, y: 20 }}
          whileInView={{ opacity: 1, y: 0 }}
          viewport={{ once: true, margin: "-80px" }}
          transition={{ duration: 0.6 }}
          className="text-center max-w-xl mx-auto"
        >
          <span className="text-sm font-semibold tracking-[0.18em] uppercase text-sapphire-600">
            Support the developer
          </span>
          <h2 className="mt-4 font-display text-4xl md:text-5xl font-extrabold text-slate-900 tracking-tight">
            Keep Tithify growing.
          </h2>
          <p className="mt-4 text-lg text-slate-600 leading-relaxed">
            Tithify is completely free and open-source. If it brings convenience to your everyday workflow, consider scanning a QR code below to support ongoing development.
          </p>
        </motion.div>

        <div className="mt-16 grid grid-cols-1 md:grid-cols-2 gap-8 max-w-3xl mx-auto">
          {DONATE_METHODS.map((method, idx) => (
            <motion.div
              key={method.name}
              initial={{ opacity: 0, y: 26 }}
              whileInView={{ opacity: 1, y: 0 }}
              viewport={{ once: true, margin: "-60px" }}
              transition={{ duration: 0.5, delay: idx * 0.15 }}
              className={`relative rounded-3xl bg-white border ${method.accent} p-8 shadow-xl shadow-slate-200/50 flex flex-col items-center text-center group transition-all duration-300 hover:-translate-y-1`}
            >
              {/* Header with Logo */}
              <div className="h-12 flex items-center justify-center mb-6">
                <Image
                  src={method.logo}
                  alt={`${method.name} logo`}
                  width={140}
                  height={48}
                  className="h-10 w-auto object-contain transition-transform duration-300 group-hover:scale-105"
                />
              </div>

              {/* QR Code Frame */}
              <div className="relative p-3 bg-white rounded-2xl border border-slate-100 shadow-inner group-hover:shadow-md transition-shadow">
                <div className="relative w-56 h-56 rounded-xl overflow-hidden bg-slate-50 flex items-center justify-center">
                  <Image
                    src={method.qr}
                    alt={`${method.name} QR Code`}
                    width={224}
                    height={224}
                    className="w-full h-full object-cover transition-transform duration-300 group-hover:scale-102"
                  />
                </div>
              </div>

              {/* Footer info */}
              <div className="mt-6 flex flex-col items-center gap-2">
                <span className={`px-3 py-1 rounded-full text-xs font-semibold uppercase tracking-wider ${method.badgeBg}`}>
                  {method.name} QR
                </span>
                <p className="text-sm text-slate-600 max-w-xs mt-1">
                  {method.description}
                </p>
              </div>
            </motion.div>
          ))}
        </div>
      </div>
    </section>
  );
}

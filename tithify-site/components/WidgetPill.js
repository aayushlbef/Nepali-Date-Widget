"use client";

import { motion, useTransform } from "framer-motion";
import NepalFlag from "./NepalFlag";

/**
 * The signature element: a live replica of the taskbar widget itself.
 * `progress` is a framer MotionValue 0→1 driving a night→day theme swap,
 * mirroring the product's real "Theme Auto-Adapt" behaviour.
 */
export default function WidgetPill({ progress, size = "lg", className = "" }) {
  const bg = useTransform(progress, [0, 1], ["rgba(6,9,18,0.92)", "rgba(255,255,255,0.96)"]);
  const border = useTransform(progress, [0, 1], ["rgba(255,255,255,0.08)", "rgba(15,23,42,0.08)"]);
  const text = useTransform(progress, [0, 1], ["#f8fafc", "#0f172a"]);
  const dayPillBg = useTransform(progress, [0, 1], ["#1c2540", "#0f172a"]);
  const dayPillText = useTransform(progress, [0, 1], ["#f8fafc", "#ffffff"]);
  const glow = useTransform(
    progress,
    [0, 1],
    ["0 8px 40px -8px rgba(37,99,235,0.45)", "0 8px 30px -10px rgba(15,23,42,0.18)"]
  );

  const dims =
    size === "lg"
      ? "px-5 py-4 gap-4 text-2xl md:text-3xl rounded-2xl"
      : "px-4 py-2.5 gap-3 text-base rounded-xl";

  return (
    <motion.div
      style={{ backgroundColor: bg, borderColor: border, boxShadow: glow }}
      className={`inline-flex items-center border backdrop-blur-md ${dims} ${className}`}
    >
      <motion.span
        style={{ backgroundColor: dayPillBg, color: dayPillText }}
        className={`font-display font-bold rounded-lg ${
          size === "lg" ? "px-4 py-2 text-lg md:text-xl" : "px-2.5 py-1 text-xs"
        }`}
      >
        Sun
      </motion.span>
      <NepalFlag className={size === "lg" ? "w-8 h-9 md:w-9 md:h-10" : "w-5 h-6"} />
      <motion.span style={{ color: text }} className="font-display font-bold tracking-tight tabular-nums">
        2083 / 04 / 24
      </motion.span>
    </motion.div>
  );
}

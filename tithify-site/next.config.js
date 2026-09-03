/** @type {import('next').NextConfig} */
const { PHASE_PRODUCTION_BUILD } = require("next/constants");

module.exports = (phase) => {
  // Hosted on custom domain:
  // https://tithify.guptaaayush.com.np
  // Served directly from root domain.
  const basePath = "";

  /** @type {import('next').NextConfig} */
  const nextConfig = {
    reactStrictMode: true,
    poweredByHeader: false,

    // Static export for GitHub Pages — produces a plain out/ folder instead
    // of a server bundle, since Pages has no Node.js runtime.
    output: "export",

    // Repo Pages is served from a subpath, so every internal link/asset
    // needs this prefix baked in. Next.js automatically prepends basePath
    // to <Image src="/..."> and static asset URLs — no manual path edits needed.
    basePath,

    images: {
      // No server on GitHub Pages to run the on-demand AVIF/WebP optimizer,
      // so images are served as-is instead.
      unoptimized: true,
    },
  };

  return nextConfig;
};

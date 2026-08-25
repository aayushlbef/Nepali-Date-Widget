// Must match `basePath` in next.config.js exactly.
//
// Next's <Image> component is supposed to auto-prefix local image URLs with
// basePath, but with `output: "export"` + `images.unoptimized: true` (both
// required for GitHub Pages) it doesn't reliably do that — the browser ends
// up requesting the image at the domain root instead of under the repo
// subpath, which 404s. Prefixing manually here works around it.
export const basePath =
  process.env.NODE_ENV === "production" ? "/Tithify" : "";

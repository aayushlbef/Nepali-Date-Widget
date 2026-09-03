import "./globals.css";
import { Sora, Inter, Noto_Sans_Devanagari } from "next/font/google";

const sora = Sora({
  subsets: ["latin"],
  weight: ["400", "500", "600", "700", "800"],
  variable: "--font-display",
  display: "swap",
});

const inter = Inter({
  subsets: ["latin"],
  weight: ["400", "500", "600"],
  variable: "--font-body",
  display: "swap",
});

const notoDevanagari = Noto_Sans_Devanagari({
  subsets: ["devanagari"],
  weight: ["500", "600", "700"],
  variable: "--font-deva",
  display: "swap",
});

const siteUrl = "https://tithify.guptaaayush.com.np";

export const metadata = {
  metadataBase: new URL(siteUrl),
  title: {
    default: "Tithify — Nepali Date Windows Taskbar Widget",
    template: "%s — Tithify",
  },
  description:
    "Tithify is a lightweight, native Windows widget that shows the Bikram Sambat date above your taskbar. Theme auto-adapt, offline calendar, zero CPU idle cost.",
  keywords: [
    "Tithify",
    "Nepali date widget",
    "Bikram Sambat",
    "BS calendar Windows",
    "Nepali calendar taskbar",
    "Nepali date converter",
    "Windows widget",
  ],
  authors: [{ name: "Aayush" }],
  creator: "Aayush",
  applicationName: "Tithify",
  alternates: {
    canonical: siteUrl,
  },
  robots: {
    index: true,
    follow: true,
    googleBot: {
      index: true,
      follow: true,
      "max-image-preview": "large",
    },
  },
  openGraph: {
    type: "website",
    url: siteUrl,
    siteName: "Tithify",
    title: "Tithify — Nepali Date Windows Taskbar Widget",
    description:
      "A lightweight, native Windows widget that shows the Bikram Sambat date above your taskbar. Theme auto-adapt, offline calendar, zero CPU idle cost.",
    images: [
      {
        // Absolute URL — metadataBase is set to https://tithify.guptaaayush.com.np.
        url: `${siteUrl}/widiget_transparent_shot.png`,
        width: 700,
        height: 467,
        alt: "Tithify widget preview above the Windows taskbar",
      },
    ],
    locale: "en_US",
  },
  twitter: {
    card: "summary_large_image",
    title: "Tithify — Nepali Date Windows Taskbar Widget",
    description:
      "A lightweight, native Windows widget that shows the Bikram Sambat date above your taskbar.",
    images: [`${siteUrl}/widiget_transparent_shot.png`],
  },
};

export const viewport = {
  themeColor: "#0b1120",
  width: "device-width",
  initialScale: 1,
};

const jsonLd = {
  "@context": "https://schema.org",
  "@type": "SoftwareApplication",
  name: "Tithify",
  applicationCategory: "UtilitiesApplication",
  operatingSystem: "Windows",
  description:
    "A lightweight, native Windows widget that shows the Bikram Sambat date above your taskbar. Theme auto-adapt, offline calendar, zero CPU idle cost.",
  offers: {
    "@type": "Offer",
    price: "0",
    priceCurrency: "USD",
  },
  url: siteUrl,
};

export default function RootLayout({ children }) {
  return (
    <html
      lang="en"
      className={`${sora.variable} ${inter.variable} ${notoDevanagari.variable}`}
    >
      <head>
        <script
          type="application/ld+json"
          // eslint-disable-next-line react/no-danger
          dangerouslySetInnerHTML={{ __html: JSON.stringify(jsonLd) }}
        />
      </head>
      <body>{children}</body>
    </html>
  );
}

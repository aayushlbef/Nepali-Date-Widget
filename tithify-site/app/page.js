import dynamic from "next/dynamic";
import Nav from "@/components/Nav";
import Hero from "@/components/Hero";
import Footer from "@/components/Footer";

// Below-the-fold sections are code-split into their own chunks so the
// initial bundle only has to include what's needed for the first paint
// (Nav + Hero). Each still renders on the server for SEO/content-on-load;
// this only changes how the JS is bundled and fetched.
const Features = dynamic(() => import("@/components/Features"));
const CalendarShowcase = dynamic(() => import("@/components/CalendarShowcase"));
const HowItWorks = dynamic(() => import("@/components/HowItWorks"));
const Efficiency = dynamic(() => import("@/components/Efficiency"));
const Install = dynamic(() => import("@/components/Install"));
const GettingStarted = dynamic(() => import("@/components/GettingStarted"));
const FAQ = dynamic(() => import("@/components/FAQ"));
const Donate = dynamic(() => import("@/components/Donate"));

export default function Home() {
  return (
    <main id="top">
      <Nav />
      <Hero />
      <Features />
      <CalendarShowcase />
      <HowItWorks />
      <Efficiency />
      <Install />
      <GettingStarted />
      <FAQ />
      <Donate />
      <Footer />
    </main>
  );
}

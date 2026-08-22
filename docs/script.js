/* ============================================
   Nepali Date Widget — Interactive Scripts
   ============================================ */

// === Copy Address to Clipboard ===
function copyAddress(btn) {
    const address = btn.parentElement.querySelector('.wallet-address').textContent;
    navigator.clipboard.writeText(address).then(function() {
        btn.classList.add('copied');
        btn.querySelector('span').textContent = 'Copied!';
        setTimeout(function() {
            btn.classList.remove('copied');
            btn.querySelector('span').textContent = 'Copy';
        }, 2000);
    });
}

(function () {
    'use strict';

    // === DOM Elements ===
    const navbar = document.getElementById('navbar');
    const mobileToggle = document.getElementById('mobileToggle');
    const navLinks = document.getElementById('navLinks');

    // === Navbar Scroll Effect ===
    function handleNavbarScroll() {
        if (window.scrollY > 50) {
            navbar.classList.add('scrolled');
        } else {
            navbar.classList.remove('scrolled');
        }
    }
    
    // === Mobile Nav Toggle ===
    function handleMobileToggle() {
        navLinks.classList.toggle('active');
        mobileToggle.classList.toggle('active');
    }

    // === Scroll Reveal Animation ===
    function initScrollReveal() {
        const revealElements = document.querySelectorAll(
            '.feature-card, .arch-step, .goal-card, .why-card, .payment-card, .section-header'
        );

        revealElements.forEach(el => el.classList.add('reveal'));

        const observer = new IntersectionObserver((entries) => {
            entries.forEach(entry => {
                if (entry.isIntersecting) {
                    entry.target.classList.add('visible');
                }
            });
        }, {
            threshold: 0.1,
            rootMargin: '0px 0px -50px 0px'
        });

        revealElements.forEach(el => observer.observe(el));
    }

    // === Performance Meter Animation ===
    function initPerfMeters() {
        const meters = document.querySelectorAll('.meter-fill');
        
        const observer = new IntersectionObserver((entries) => {
            entries.forEach(entry => {
                if (entry.isIntersecting) {
                    const targetWidth = entry.target.style.width;
                    entry.target.style.width = '0%';
                    requestAnimationFrame(() => {
                        requestAnimationFrame(() => {
                            entry.target.style.width = targetWidth;
                        });
                    });
                }
            });
        }, { threshold: 0.5 });

        meters.forEach(m => observer.observe(m));
    }

    // === Smooth Anchor Scrolling ===
    function initSmoothScroll() {
        document.querySelectorAll('a[href^="#"]').forEach(anchor => {
            anchor.addEventListener('click', (e) => {
                const target = document.querySelector(anchor.getAttribute('href'));
                if (target) {
                    e.preventDefault();
                    target.scrollIntoView({ behavior: 'smooth', block: 'start' });
                    navLinks.classList.remove('active');
                }
            });
        });
    }

    // === Parallax for Glow Orbs ===
    function handleParallax() {
        const scrollY = window.scrollY;
        const glows = document.querySelectorAll('.hero-glow');
        glows.forEach((glow, i) => {
            const speed = 0.1 + i * 0.05;
            glow.style.transform = `translateY(${scrollY * speed}px)`;
        });
    }

    // === Theme Toggle ===
    function initThemeToggle() {
        const themeToggleBtn = document.getElementById('themeToggle');
        
        function setTheme(theme) {
            document.documentElement.setAttribute('data-theme', theme);
            localStorage.setItem('theme', theme);
        }

        function toggleTheme() {
            const currentTheme = document.documentElement.getAttribute('data-theme') || 'dark';
            const newTheme = currentTheme === 'light' ? 'dark' : 'light';
            setTheme(newTheme);
        }

        const systemThemeMedia = window.matchMedia('(prefers-color-scheme: light)');
        
        systemThemeMedia.addEventListener('change', (e) => {
            if (!localStorage.getItem('theme')) {
                setTheme(e.matches ? 'light' : 'dark');
            }
        });

        if (themeToggleBtn) {
            themeToggleBtn.addEventListener('click', (e) => {
                e.preventDefault();
                e.stopPropagation();
                toggleTheme();
            });
        }
    }

    // === CLI Quick Install Tabs & Copy ===
    function initCliTabs() {
        const tabs = document.querySelectorAll('.cli-tab');
        const copyBtn = document.getElementById('cliCopyBtn');

        tabs.forEach(tab => {
            tab.addEventListener('click', () => {
                const target = tab.getAttribute('data-cli');
                
                tabs.forEach(t => t.classList.remove('active'));
                tab.classList.add('active');

                document.querySelectorAll('.cli-code').forEach(code => {
                    code.classList.remove('active');
                });

                const activeCode = document.getElementById(`code-${target}`);
                if (activeCode) {
                    activeCode.classList.add('active');
                }
            });
        });

        if (copyBtn) {
            copyBtn.addEventListener('click', () => {
                const activeCode = document.querySelector('.cli-code.active');
                if (!activeCode) return;

                const textToCopy = activeCode.textContent.trim();
                navigator.clipboard.writeText(textToCopy).then(() => {
                    copyBtn.classList.add('copied');
                    const span = copyBtn.querySelector('span');
                    if (span) span.textContent = 'Copied!';

                    setTimeout(() => {
                        copyBtn.classList.remove('copied');
                        if (span) span.textContent = 'Copy';
                    }, 2000);
                });
            });
        }
    }

    // === Initialize ===
    function init() {
        // Force initial scroll state
        window.scrollTo(0, 0);
        
        // Event listeners
        window.addEventListener('scroll', () => {
            requestAnimationFrame(() => {
                handleNavbarScroll();
                handleParallax();
            });
        }, { passive: true });

        if (mobileToggle) {
            mobileToggle.addEventListener('click', handleMobileToggle);
        }

        // Init modules
        initScrollReveal();
        initPerfMeters();
        initSmoothScroll();
        initThemeToggle();
        initCliTabs();

        // Initial calls
        handleNavbarScroll();
    }

    // Wait for DOM
    if (document.readyState === 'loading') {
        document.addEventListener('DOMContentLoaded', init);
    } else {
        init();
    }
})();

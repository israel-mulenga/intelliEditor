// Smooth scroll and active nav link highlighting
document.addEventListener('DOMContentLoaded', function () {
    const links = Array.from(document.querySelectorAll('.nav-link'));
    if (!links.length) return;

    links.forEach(link => {
        link.addEventListener('click', function (e) {
            e.preventDefault();
            const target = document.querySelector(this.getAttribute('href'));
            if (target) target.scrollIntoView({ behavior: 'smooth', block: 'start' });
        });
    });

    const setActive = () => {
        const fromTop = window.scrollY + 140;
        links.forEach(link => {
            const section = document.querySelector(link.getAttribute('href'));
            if (!section) return;
            if (section.offsetTop <= fromTop && section.offsetTop + section.offsetHeight > fromTop) {
                link.classList.add('text-white');
                link.classList.remove('text-slate-300');
            } else {
                link.classList.remove('text-white');
                link.classList.add('text-slate-300');
            }
        });
    };

    setActive();
    window.addEventListener('scroll', setActive, { passive: true });
    window.addEventListener('resize', setActive);
});

# Welcome

<p align="center">
  <img id="epoch-logo" src="default-image.png" width="20%" alt="Epoch logo">
</p>

<script>
  function updateLogo() {
    const scheme = document.body.getAttribute('data-md-color-scheme');
    const logo = document.getElementById('epoch-logo');

    switch (scheme) {
      case 'slate':
        logo.src = '../images/EpochLogoDarkMode.png';
        break;
      default:
        logo.src = '../images/EpochLogoLightMode.png';
    }
  }

  // Initial update on page load
  window.addEventListener("load", updateLogo);

  // Update on theme change (MutationObserver)
  const observer = new MutationObserver(updateLogo);
  observer.observe(document.body, {
    attributes: true,
    attributeFilter: ['data-md-color-scheme']
  });
</script>

Welcome to the official documentation for **Epoch Engine** — a powerful, flexible, and modern **3D game engine** built to empower developers to bring their ideas to life.

Whether you're building a fast-paced action game, a stunning simulation, or a multiplayer experience, Epoch Engine gives you the tools and architecture to make it happen

---

## What is Epoch Engine?

**Epoch Engine** is a modular, cross-platform (desktop only) 3D game engine focused on:

- 🧩 **Modularity** – Use only what you need: rendering, physics, input, and more.
- 🖥️ **High Performance** – Designed with performance in mind for demanding real-time applications.
- 🔧 **Customizability** – Open-source, extensible, and built for developers who want full control.

!!! warning "Alpha Release Notice"
    **Epoch Engine** is currently in **active development**. An alpha version is expected in **November 2025**.   Contributions, feedback, and testing are welcome
---

## 📚 Documentation Overview

Here's what you’ll find in the docs:

- [Getting Started](getting-started.md): Install the engine and build your first game.
- [Core Concepts](../core-concepts.md): ECS, scene management, input, rendering, and more.
- [Modules](../modules.md): Learn how rendering, audio, physics, and scripting work.
- [API Reference](../API-reference.md): Detailed documentation of public classes and methods.
- [Tutorials](../tutorials.md): Step-by-step guides for common gameplay mechanics.
- [Contributing](../contributing.md): How to contribute to the engine itself.

---

## 🧭 Navigation Tips

Use the navigation bar to jump between topics, and the search bar at the top to find anything quickly.

Need help? Join the [community](https://discord.com) or open an issue on [GitHub](https://github.com/Windokk/Epoch).

---

*Epoch Engine is an open-source project licensed under GNU GPL v3.0.*

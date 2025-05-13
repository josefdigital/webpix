# Webpix 🖼️

**Webpix** is a lightweight CLI tool (written in C) that detects, compresses, and organizes PNG and JPEG images for use in websites — especially static sites like [josef.digital](https://josef.digital).

> ⚠️ Webpix is a **work in progress**. Currently supports PNG compression using `libpng`.

---

## ✨ Features

- ✅ Detects image type (PNG, JPEG)
- ✅ Compresses PNGs with `libpng` (lossless)
- 🚧 JPEG support coming soon
- ✅ Moves images from a `public/` folder to a `static/` folder
- 🛠 Built for developer workflows and static site optimization

---

## 📦 Requirements

- `libpng`
- (Optional) `libjpeg` or `libjpeg-turbo` for JPEGs
- `gcc` or `clang` (tested on macOS)
- `make`

Install dependencies (macOS example):

```bash
brew install libpng libjpeg-turbo
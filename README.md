
# 📦 wnpkg

**wnpkg** is a tiny Windows utility that packages a Node.js application into a portable `.exe` file.

## 🚀 Features

- Automatically wraps your Node.js script into an executable
- Includes `node.exe` inside the build
- Adds optional custom icon support (`icon.ico`)
- Generates a complete build directory (`build-last/`)
- Super lightweight, written in pure C with `gcc`

---

## 🛠️ Usage

```bash
wnpkg <project-folder>
```

This command will:

1. Copy `node.exe` and your `index.js` into a build folder.
2. Compile a small C launcher using `gcc`.
3. Output a single `.exe` file in `build-last/`.

### Example

```bash
wnpkg my-app
```

This will create:

```
build-last/
├── source/
│   ├── index.js      <-- Your app
│   └── node.exe      <-- Node runtime
├── my-app.exe          <-- Final executable
```

Double-click `shop.exe` and your Node app runs instantly.

---

## 🎨 Optional: Add Icon

Drop your custom `icon.ico` in the project folder.

If present, `wnpkg` will embed it in the final `.exe`.

---

## 📄 Requirements

- Windows (32 or 64 bit)
- [GCC / MinGW](https://www.mingw-w64.org/) installed and in PATH

---

## 💬 Config file (lbuilder)

You can add a custom icon for your project, this file is important for your project name. Create a file named `lbuilder` in your nodejs project, and add the options.

#### A example :
```txt
app-name;
icon.ico;
```

need be like this, you can change the entry values, and if you don't want to use a icon, you can simple add a "*" in `icon.ico` place.

---

## 🧠 Author Notes

This project was built for fun and utility — feel free to hack it, improve it or fork it.

---

## 🐧 Created by Leonardo Pinezi (a.k.a. Shinpi)

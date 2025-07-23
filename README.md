# 📦 wnpkg

**wnpkg** is a tiny utility that packages a Node.js application into a portable executable — works on **Windows** and **Linux**.

## 🚀 Features

- Automatically wraps your Node.js script into a standalone executable
- Includes the Node.js runtime (`node.exe` on Windows, `node` on Linux)
- Optional custom icon support (`icon.ico`)
- Generates a clean build structure in `wnpkg-build/`
- Super lightweight — written in pure C using `gcc`

---

## 🛠️ Usage

```
wnpkg <project-folder>
```

This command will:

1. Copy `node` (or `node.exe` on Windows) and your `index.js` into a build folder.
2. Compile a small C launcher using `gcc`.
3. Output a single executable:
   - On **Windows**: `my-app.exe`
   - On **Linux**: `my-app` (ELF binary without file extension)

### Example

```
wnpkg my-app
```

This will create the following structure:

```
wnpkg-build/
├── source/
│   ├── index.js         <-- Your Node.js app
│   └── node(.exe)       <-- Node.js runtime
├── my-app(.exe)         <-- Final executable
```

- On **Windows**, just double-click `my-app.exe` to run your app.
- On **Linux**, run it with:

```
./my-app
```

---

## 🎨 Optional: Custom Icon

To embed a custom icon in the Windows executable:

1. Place your `icon.ico` file in the project folder.
2. `wnpkg` will automatically detect and include it in the `.exe`.

---

## ⚙️ Optional: Configuration File (`wnpkg_config`)

You can customize the app name and icon by creating a file named `wnpkg_config` in your project folder. This file should contain:

```
app-name;
icon.ico;
```

- Replace `app-name` with the desired output name.
- If you don't want to use an icon, simply use a `*` instead:

```
my-app;
*;
```

---

## 📄 Requirements

- Windows or Linux (32 or 64 bit)
- [GCC / MinGW](https://www.mingw-w64.org/) installed and available in PATH

---

## 🧠 Author Notes

This project was built for fun and utility — feel free to hack it, improve it or fork it.

---

## 🐧 Created by Leonardo Pinezi (a.k.a. Shinpi)

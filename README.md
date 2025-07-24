# 📦 lnpkg

**lnpkg** is a tiny linux utility that packages a Node.js application into a linux executable.

## 🚀 Features

- Automatically wraps your Node.js script into a standalone executable
- Includes the Node.js runtime `node`
- Generates a clean build structure in `lnpkg-build/`
- Super lightweight — written in pure C using `gcc`

---

## 🛠️ Usage

```
lnpkg <project-folder>
```

This command will:

1. Copy `node` and your `index.js` into a build folder.
2. Compile a small C launcher using `gcc`.
3. Output a single executable:
   - On **Linux**: `my-app` (ELF binary without file extension)

### Example

```
lnpkg my-app
```

This will create the following structure:

```
lnpkg-build/
├── source/
│   ├── index.js         <-- Your Node.js app
│   └── node             <-- Node.js runtime
├── my-app               <-- Final executable
```

- On **Linux**, run it with:

```
./my-app
```

---

## ⚙️ Optional: Configuration File (`lnpkg_config`)


```
app-name;
```

- Replace `app-name` with the desired output name.

---

## 📄 Requirements

- Linux (32 or 64 bit)
- [Node](https://nodejs.org/pt) installed and available in PATH
- [GCC](https://gcc.gnu.org/) installed and available in PATH

---

## 🧠 Author Notes

This project was built for fun and utility — feel free to hack it, improve it or fork it.

---

## 🐧 Created by Leonardo Pinezi (a.k.a. Shinpi) & Aquiles Trindade (a.k.a TrindadeDev13)

#### Leonardo Pinezi :
- Make the original base code and created the project
- Started the project organization
- Windows 32/64bit port
- Fixed bugs

#### Aquiles Trindade :
- Organized all the entire code
- Added makefile
- Added colours and file functions
- Linux 32/64bits port
- ELF export
- Fixed bugs

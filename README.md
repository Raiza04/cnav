# 🧭 CNav (Command Navigation)

<p align="center">
  <img src="https://img.shields.io/badge/Language-C11-blue.svg" alt="Language C11" />
  <img src="https://img.shields.io/badge/Build-CMake-orange.svg" alt="Build CMake" />
  <img src="https://img.shields.io/badge/Database-SQLite3-003B57.svg" alt="Database SQLite3" />
  <img src="https://img.shields.io/badge/Platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey.svg" alt="Platform" />
  <img src="https://img.shields.io/badge/License-MIT-green.svg" alt="License MIT" />
</p>

<p align="center">
  <img src="assets/demo.gif" alt="CNav in action" width="400"/>
</p>

**CNav** is a lightweight, blazingly fast CLI tool written in C11 that transparently tracks your file usage habits directly from the terminal, accelerates your workflow, and lets you reopen files instantly using fuzzy searching.

By seamlessly integrating with your shell (Bash, Zsh, or PowerShell), CNav remembers which files you open, which programs you use to open them, and how often you do it. It builds a robust **frecency-based** (frequency + recency) SQLite database to act as your smart file navigator.

---

## ✨ Features

* **Cross-Platform:** Fully supports Linux, macOS (Bash/Zsh), and Windows (PowerShell).
* **Robust SQLite Backend:** Safely stores history in a local SQLite database (`cnav.db`). Built using the SQLite Amalgamation approach, so there are **no external dependencies** to install.
* **Smart Fuzzy Search:** Uses a custom prefix-aware **Levenshtein distance** algorithm. It tolerates typos and forgives incomplete search terms. 
* **Context-Aware Scoring:** Files located in your Current Working Directory (CWD) receive a score boost, ensuring highly relevant search results.
* **Transparent Tracking:** Operates silently in the background via intelligent shell hooks. It does not block your terminal, pollute your `stdout`, or break pipelines.
* **Success-Aware:** CNav only tracks commands that execute successfully. Typos won't pollute your database.
* **Smart Auto-completion:** CNav natively integrates with your shell to provide dynamic, real-time tab completion for the files stored in your database!

---

## 📦 Installation (Pre-compiled Binaries)

The easiest way to install CNav. These scripts will automatically download the latest release, place it in your local bin folder, and setup your shell hooks.

**🐧 For Linux & macOS (Bash/Zsh):**
```bash
curl -sSfL https://raw.githubusercontent.com/Raiza04/cnav/main/install.sh | bash

```

**🪟 For Windows (PowerShell):**

```powershell
Invoke-Expression (Invoke-WebRequest -Uri "https://raw.githubusercontent.com/Raiza04/cnav/main/install.ps1" -UseBasicParsing).Content

```

*After installation, restart your terminal.*

---

## ⚙️ Configuration (Telling CNav what to track)

CNav needs to know which tools you want to track (e.g., `vim`, `nano`, `code`).
Edit the `tools.txt` file located in:

* **Linux/macOS:** `~/.local/share/cnav/tools.txt`
* **Windows:** `%LOCALAPPDATA%\cnav\tools.txt` (or `~\AppData\Local\cnav\tools.txt`)

Add the commands you want to track (one per line):

```text
# Example tools.txt
vim
nano
code
notepad

```

---

## 🚀 Usage

Using CNav is incredibly simple because tracking happens completely in the background.

### 1. Tracking Files (Automatic)

Just use your terminal normally:

```bash
vim src/main.c
code README.md

```

If the command succeeds, CNav silently logs the file and the program used, updates the SQLite database, and increments the usage counter.

### 2. Opening Tracked Files

To quickly open a file, call `n` followed by a search term. CNav will find the best match and open it.

```bash
n main        # Might open src/main.c in vim
n read        # Might open README.md in code
n tb          # Might open tools.txt (Typo tolerance!)

```

### 3. Database Management

CNav provides built-in flags to manage your tracked history:

```bash
n --list      # View a formatted table of your tracked files
n --clean     # Remove entries of files that no longer exist on disk
n --purge     # Completely empty the database history
n -d test     # Interactively prompt to delete a specific entry (e.g., test.txt)

```

---

## 🛠️ Manual Build Instructions

If you want to compile CNav from source instead of using the installation scripts.

### Prerequisites

* CMake (>= 3.10)
* A C Compiler (GCC/Clang for Linux/macOS, MSVC for Windows)

### 1. Clone & Compile

```bash
git clone https://github.com/Raiza04/cnav.git
cd cnav

# On Linux/macOS:
./build.sh

# On Windows:
.\build.ps1

```

### 2. Manual Shell Setup

If you built manually, you must add CNav to your PATH and initialize the hooks.

**Linux / macOS (`~/.bashrc` or `~/.zshrc`):**

> ⚠️ **CRITICAL:** CNav initialization MUST happen before you define or source any aliases.

```bash
export PATH="$PATH:/path/to/cnav/build"
eval "$(n --init)"

```

**Windows (`$PROFILE`):**

```powershell
$env:PATH += ";C:\path\to\cnav\build\Release"
Invoke-Expression (n --init | Out-String)

```

---

## 📄 License

This project is open-source and available under the **MIT License**.

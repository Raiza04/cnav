# 🧭 CNav (Command Navigation)

**CNav** is a lightweight, blazingly fast CLI tool written in C that transparently tracks your file usage habits directly from the terminal. 

By seamlessly integrating with your Bash shell, CNav remembers which files you open, which programs you use to open them, and how often you do it—building the perfect foundation for a frecency-based (frequency + recency) file navigation system.

---

## ✨ Features

* **Transparent Tracking:** Operates silently in the background via intelligent Bash hooks. You just use your terminal normally (e.g., `vim test.txt`), and CNav does the rest.
* **Success-Aware:** CNav only tracks commands that execute successfully (Exit Code 0). Typos won't pollute your database.
* **Smart Path Resolution:** Properly resolves absolute and relative paths. It even supports tracking newly created files.
* **Customizable:** You decide which tools are tracked by simply adding them to a text file.
* **Lightning Fast:** Written in standard C (C11) with minimal overhead.

---

## 🚀 Usage

Using CNav is incredibly simple because it works completely in the background.

### 1. Tracking Files (Automatic)
Once configured, just use your terminal as you always do:
```bash
vim src/main.c
batcat README.md
nano /etc/fstab
```
If the command succeeds, CNav silently logs the file and the program used, increments the usage counter, and updates the timestamp.

### 2. Opening Tracked Files
To quickly open a file you frequently use, simply call `n` followed by a search term. CNav will find the best match in your database and open it using the exact program you originally used!
```bash
n main       # Might open src/main.c in vim
n read       # Might open README.md in batcat
```

---

## 🛠️ Prerequisites

* **OS:** Linux / Unix-like system
* **Compiler:** GCC or Clang
* **Build System:** CMake (>= 3.10)
* **Shell:** Bash

---

## 📦 Installation & Build

1. **Clone the repository:**
   ```bash
   git clone <your-repo-url>
   cd cnav
   ```

2. **Build the executable:**
   You can easily build the project using the provided shell script:
   ```bash
   ./build.sh
   ```
   *(Alternatively, you can manually create a build folder and run `cmake .. && make`)*

---

## ⚙️ Configuration

### 1. Setup PATH and Shell Hooks (Crucial!)
To make CNav accessible and allow it to intercept your commands, you need to update your `~/.bashrc`.

> ⚠️ **CRITICAL ORDER:** The CNav initialization (`eval "$(n --init)"`) **MUST** happen before you define or source any aliases (like your `~/.bash_aliases` file). Otherwise, your aliases will override the CNav hooks!

Add the following to your `~/.bashrc`:

```bash
# 1. Add CNav build directory to your PATH 
# (Replace with your actual path to the cnav/build folder)
export PATH="$PATH:/home/username/path/to/cnav/build"

# 2. Initialize CNav hooks FIRST
eval "$(n --init)"

# 3. Load aliases AFTER CNav
if [ -f ~/.bash_aliases ]; then
    . ~/.bash_aliases
fi
```
*(After adding this, restart your terminal or run `source ~/.bashrc`)*

**<span style="color:red">NOTE</span>**: Make sure that the path to the executable `n` is correct.

### 2. Define Tools to Track
CNav creates a configuration directory at `~/.local/share/cnav/`.

Edit the `tools.txt` file in this directory to include the commands you want to track (one per line):
```text
# Example ~/.local/share/cnav/tools.txt
vim
nano
cat
batcat
code
```

---

## 🗄️ Database Structure

CNav stores the tracking data in a simple CSV format at `~/.local/share/cnav/db.txt`.

**Format:**
```text
<Absolute Path>,<Filename>,<Program>,<Call Count>,<Unix Timestamp>
```

**Example entry:**
```text
/home/user/projects/main.c,main.c,vim,14,1787572621
```

---

## ⚠️ Troubleshooting

### Aliases not triggering CNav?
If you have an alias like `alias cat='batcat'`, the bash shell translates `cat` to `batcat` before CNav can hook into it.
* **Solution:** Ensure that the target of your alias (`batcat`) is listed in your `tools.txt`.

### "Command not found" for n?
* **Solution:** Double-check that the `export PATH=...` line in your `.bashrc` points exactly to the folder containing the compiled `n` binary.

---

## 📄 License

This project is open-source and available under the **MIT License**.

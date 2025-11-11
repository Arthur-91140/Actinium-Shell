# 🧪 ACTINIUM - A Modular and Customizable C++ Shell

<div align="center">

**A revolutionary C++ shell that allows you to modify command behavior on the fly**

[![Platform](https://img.shields.io/badge/Platform-Windows-blue)](https://github.com/Arthur-91140/Actinium-Shell)
[![Language](https://img.shields.io/badge/Language-C%2B%2B11-orange)](https://github.com/Arthur-91140/Actinium-Shell)
[![License](https://img.shields.io/badge/License-MIT-green)](LICENSE)
[![Status](https://img.shields.io/badge/Status-Alpha-yellow)](https://github.com/Arthur-91140/Actinium-Shell)

</div>

---

## 🌟 Overview

**ACTINIUM** is not just another shell — it's a **modular, dynamic, and hackable** command-line environment built in C++. Unlike traditional shells where commands are static binaries, ACTINIUM gives you **direct access to the source code of each command from within the terminal**, allowing you to modify, recompile, and reload them **without ever leaving the shell**.

Inspired by **Terry A. Davis** and his work on operating systems, particularly the JIT compilation concept from Holy C, Actinium brings the power of on-the-fly code modification to your terminal experience.

> *"Someone said that the smaller your duties, the better. Bad advice"* - **Terry A. Davis** (1969-2018)

---

## ✨ Key Features

### 🚀 Core Functionality
- ✅ **Basic Command Line Interface** with full UTF-8 support
- ✅ **File System Navigation** - `ls`, `cd`, `pwd` commands
- ✅ **External Command Execution** via PATH environment variables
- ✅ **Dynamic Module System** - Load/unload commands as DLL modules on-the-fly
- ✅ **Colored Output** - Beautiful, customizable terminal colors

### 🧠 Intelligent Features
- ✅ **Advanced Auto-Completion**
  - Tab completion for files, directories, and commands
  - Fuzzy matching with Levenshtein distance algorithm
  - Real-time suggestions while typing
  - Context-aware completions

- ✅ **Smart Command History**
  - Persistent history (1000+ commands)
  - Frequency-based ranking
  - Fuzzy search through history
  - Duplicate prevention

### 🔧 Developer-Friendly
- ✅ **On-the-Fly Command Modification**
  - Edit command source code from within the shell
  - Compile and reload instantly
  - No shell restart required
- ✅ **Module Creation System**
  - Generate command templates with `create [name]`
  - Built-in compilation with `compile [name]`
  - Hot-reload with `reload [name]`

---

## 📦 Requirements

- **C++ Compiler**: g++ supporting C++11 or higher
- **Operating System**: Windows (Linux/macOS support coming soon)
- **Git**: For cloning the repository
- **Optional**: nano.exe for the `edit` command

---

## 🛠️ Installation

### Quick Start

```bash
# Clone the repository
git clone https://github.com/Arthur-91140/Actinium-Shell.git
cd Actinium-Shell

# Compile the project
g++ -o actinium src/main.cpp src/command-processor.cpp src/input-handler.cpp src/history-manager.cpp src/auto-complete.cpp src/init.cpp src/config.cpp -I./src -static

# Run the shell
./actinium
```

### Building Individual Components

```bash
# Compile all source files together
g++ -o actinium \
    src/main.cpp \
    src/command-processor.cpp \
    src/input-handler.cpp \
    src/history-manager.cpp \
    src/auto-complete.cpp \
    src/init.cpp \
    src/config.cpp \
    -I./src \
    -static
```

---

## 🎯 Usage

### Basic Commands

```bash
# Launch the shell
./actinium

# Basic navigation
> pwd              # Show current directory
> ls [path]        # List directory contents
> cd [path]        # Change directory (supports ., .., absolute, relative paths)

# Greeting
> hello [name]     # Display welcome message

# Help and information
> help             # Show available commands
> version          # Display version information

# Exit
> exit             # Quit the shell
```

### Dynamic Command System

```bash
# Create a new command module
> create mycommand
# This generates: commands/mycommand/mycommand.cpp

# Edit the command source code
> edit mycommand
# Opens the source file in your editor

# Compile the module
> compile mycommand
# Compiles: commands/mycommand/mycommand.cpp -> mycommand.dll

# Reload the module (if already loaded)
> reload mycommand

# Use your new command!
> mycommand arg1 arg2
```

### Auto-Completion & History

```bash
# Tab completion
> cd Do[TAB]           # Completes to "Documents" if it exists
> my[TAB]              # Shows all commands starting with "my"

# History navigation
> [UP ARROW]           # Previous command
> [DOWN ARROW]         # Next command

# Real-time suggestions
# Just start typing and see suggestions appear automatically!
> hel                  # Suggests "hello" based on history
```

---

## 🏗️ Architecture

### Project Structure

```
Actinium-Shell/
├── src/
│   ├── include/                      # Header files
│   │   ├── command-processor.h       # Command execution engine
│   │   ├── input-handler.h           # Interactive input & UI
│   │   ├── history-manager.h         # Command history management
│   │   ├── auto-complete.h           # Completion system
│   │   ├── init.h                    # Initialization
│   │   └── config.h                  # Configuration constants
│   ├── main.cpp                      # Entry point
│   ├── command-processor.cpp         # Core command logic
│   ├── input-handler.cpp             # Input handling & display
│   ├── history-manager.cpp           # History storage & search
│   ├── auto-complete.cpp             # Completion engine
│   ├── init.cpp                      # Setup & initialization
│   └── config.cpp                    # Configuration values
├── commands/                          # Dynamic command modules
│   └── [command-name]/
│       ├── [command-name].cpp        # Command implementation
│       └── [command-name].dll        # Compiled module
├── asset-Actinium/                    # Assets and resources
├── command_history.txt                # Persistent command history
├── README.md                          # This file
├── ROADMAP.md                         # Detailed feature roadmap
├── LICENSE                            # MIT License
└── .gitignore
```

### Command Module Structure

Each command in ACTINIUM is an **independent DLL module** with a standardized interface:

```cpp
// commands/mycommand/mycommand.cpp
#include <windows.h>
#include <vector>
#include <string>
#include <iostream>

extern "C" __declspec(dllexport) void execute(const std::vector<std::string>& args) {
    // Your command implementation here
    std::cout << "Hello from mycommand!" << std::endl;

    // Access arguments
    for (size_t i = 0; i < args.size(); ++i) {
        std::cout << "Argument " << i << ": " << args[i] << std::endl;
    }
}
```

**Compile it:**
```bash
g++ -shared -o commands/mycommand/mycommand.dll commands/mycommand/mycommand.cpp -I./src
```

This architecture enables:
- ⚡ **Hot-Reloading** - Update commands without restarting
- 🔌 **Modularity** - Each command is independent
- 🛠️ **Customization** - Modify any command to suit your needs
- 🚀 **Dynamic Loading** - Commands load on-demand

---

## 🎨 Customization

### Modifying Built-in Commands

Want to change how `ls` works? Edit it!

```bash
> edit ls
# Modify the listing format, add colors, change sorting...
> compile ls
> reload ls
# Your changes are now live!
```

### Creating Custom Commands

```bash
# Generate a new command template
> create weather

# Edit it to fetch weather data
> edit weather

# Compile and use it
> compile weather
> weather Paris
```

Example weather command:
```cpp
extern "C" __declspec(dllexport) void execute(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Usage: weather <city>" << std::endl;
        return;
    }

    std::string city = args[0];
    // Add your weather API call here
    std::cout << "Weather for " << city << ": Sunny, 22°C" << std::endl;
}
```

---

## 🗺️ Roadmap

We have an ambitious roadmap for Actinium! See our [**detailed ROADMAP.md**](ROADMAP.md) for the complete feature list.

### High Priority (In Progress)
- [ ] Essential shell commands (`cat`, `echo`, `mkdir`, `rm`, `cp`, `mv`, `touch`, `grep`, `find`)
- [ ] Pipes and redirections (`|`, `>`, `>>`, `<`)
- [ ] Environment variables and aliases
- [ ] Cross-platform support (Linux, macOS)
- [ ] Package manager (SPM - Squirrel Package Manager)
- [ ] Integrated text editor

### Medium Priority
- [ ] Scripting support with `.acsh` files
- [ ] Control structures (if/else, loops, case)
- [ ] Syntax highlighting
- [ ] Job control (`&`, `fg`, `bg`, `jobs`)
- [ ] Multiple tabs/split screen
- [ ] Git integration

### Completed ✅
- ✅ Basic shell interface
- ✅ File system navigation
- ✅ Dynamic command system
- ✅ Auto-completion with fuzzy matching
- ✅ Command history with frequency tracking
- ✅ External command execution
- ✅ Module creation/editing/compilation

See [ROADMAP.md](ROADMAP.md) for the **complete list of 200+ planned features** organized into 20 development phases.

---

## 🤝 Contributing

Contributions are **welcome and encouraged**! Here's how you can help:

1. **Report Bugs** - Open an issue with details
2. **Suggest Features** - Share your ideas
3. **Submit Pull Requests** - Improve the codebase
4. **Create Modules** - Share custom commands
5. **Improve Documentation** - Help others learn

### Development Guidelines

```bash
# Fork the repository
# Create a feature branch
git checkout -b feature/amazing-feature

# Make your changes
# Commit with clear messages
git commit -m "Add amazing feature"

# Push to your fork
git push origin feature/amazing-feature

# Open a Pull Request
```

---

## 📚 Documentation

### Command Reference

| Command | Description | Usage |
|---------|-------------|-------|
| `hello [name]` | Display welcome message | `hello Arthur` |
| `pwd` | Print working directory | `pwd` |
| `cd [path]` | Change directory | `cd Documents` |
| `ls [path]` | List directory contents | `ls`, `ls ..` |
| `exit` | Exit the shell | `exit` |
| `help` | Show command list | `help` |
| `version` | Show version info | `version` |
| `create [name]` | Create new command module | `create mycommand` |
| `edit [name]` | Edit command source | `edit mycommand` |
| `compile [name]` | Compile command module | `compile mycommand` |
| `reload [name]` | Reload command module | `reload mycommand` |

### Configuration

Configuration settings are defined in `src/config.cpp`:

```cpp
const std::string HISTORY_FILENAME = "command_history.txt";
const int MAX_HISTORY_SIZE = 1000;
const int MAX_SUGGESTIONS = 5;
const int FUZZY_MATCH_THRESHOLD = 50;
```

Customize these values to adjust shell behavior.

---

## 🐛 Known Issues

- Windows-only (Linux/macOS ports in progress)
- External editor dependency for `edit` command
- Limited built-in commands (expanding rapidly)

Report issues at: [GitHub Issues](https://github.com/Arthur-91140/Actinium-Shell/issues)

---

## 📜 License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

You are free to:
- ✅ Use commercially
- ✅ Modify
- ✅ Distribute
- ✅ Private use

---

## 🙏 Acknowledgements

Special thanks to:

- **Terry A. Davis** (1969-2018) - For inspiration from TempleOS and Holy C's JIT compilation concept
- The **C++ community** - For excellent libraries and tools
- **Contributors** - Everyone who helps improve Actinium

---

## 👨‍💻 Author

**Arthur Pruvost Rivière**
- GitHub: [@Arthur-91140](https://github.com/Arthur-91140)
- Project: [Actinium-Shell](https://github.com/Arthur-91140/Actinium-Shell)

Developed with passion and love for innovative terminal experiences ^^

---

## 📞 Support

- 📖 [Documentation](ROADMAP.md)
- 🐛 [Issue Tracker](https://github.com/Arthur-91140/Actinium-Shell/issues)
- 💬 [Discussions](https://github.com/Arthur-91140/Actinium-Shell/discussions)

---

<div align="center">

**⭐ Star this project if you find it interesting! ⭐**

Made with ❤️ and C++

</div>

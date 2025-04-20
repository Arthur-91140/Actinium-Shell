# ACTINIUM - A Modular and Customizable C++ Shell

ACTINIUM is a C++ shell that allows you to modify command behavior on the fly. Unlike traditional shells, ACTINIUM gives you direct access to the source code of each command from within the terminal, allowing you to modify and recompile them without leaving the environment.

## Key Features

- **Basic command line interface** with UTF-8 support
- **File system navigation** with `ls`, `cd`, and `pwd` commands
- **External command execution** via environment variables
- **On-the-fly modification and recompilation of commands**
- **Modular architecture** for easy addition of new commands

## Requirements

- C++ compiler supporting C++11 or higher (g++ recommended)
- Windows system (for now, support for other platforms planned)
- Git for cloning the repository

## Installation

```bash
# Clone the repository
git clone https://github.com/your-username/actinium.git
cd actinium

# Compile the project
g++ -o actinium src/main.cpp src/command-processor.cpp src/init.cpp -I./src -static
```

## Usage

```bash
# Launch the shell
./actinium

# Basic commands
> hello [name]    # Display a welcome message
> pwd             # Show current directory
> ls [path]       # List directory contents
> cd [path]       # Change current directory
> exit            # Quit the shell

# To edit a command (coming soon)
> edit ls         # Open editor to modify the ls command
```

## Command Executable Structure

Each command in ACTINIUM is designed as an independent module that can be dynamically loaded:

```
commands/
├── ls/
│   ├── ls.cpp           # Implementation of the ls command
│   ├── ls.h             # Header with declarations
│   └── ls.dll           # Compiled dynamic library
├── cd/
│   ├── cd.cpp           # Implementation of the cd command
│   ├── cd.h             # Header with declarations
│   └── cd.dll           # Compiled dynamic library
└── ...                  # Other commands
```

Each command module must expose a standard `execute` function:

```cpp
// Standard signature for a command function
extern "C" __declspec(dllexport) void execute(const std::vector<std::string>& args);
```

This architecture allows ACTINIUM to:
1. Load commands on demand
2. Recompile a command without restarting the shell
3. Easily create new commands by simply adding modules

## Command Customization

One of the key aspects of ACTINIUM is the ability to customize commands:

1. Use `edit [command]` to open the command's source code
2. Modify the code according to your needs
3. Save, and the command will be automatically recompiled
4. Immediately use the modified version of the command

## Roadmap

- [ ] Implementation of the integrated text editor
- [ ] Dynamic command loading system
- [ ] Syntax highlighting support
- [ ] Version control system for modified commands
- [ ] Linux and macOS platform support
- [ ] Autocomplete for commands and paths
- [ ] Package manager integration for sharing commands

## Contributing

Contributions are welcome! Feel free to open an issue or submit a pull request.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgements
I would like to thank Terry A. Davis for his work on operating systems and the Saint C from
which I get the idea of the JIT compilation.
>" Someone said that the smaller your duties, the better. Bad advice" (Terry A. Davis 1969-2018)

## Author

Developed with passion love by [Arthur Pruvost Rivière](https://github.com/Arthur-91140) ^^.

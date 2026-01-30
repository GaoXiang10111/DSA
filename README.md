# C/C++ Template Library with Python ctypes Integration

This GitHub project is created to revive old C/C++ foundation library to faciliate C/C++ engine for ML/AI research developement. The non-coding part of this project, such as formating, folder layout, Makefile, License, documents, etc. are mostly created by VS Code AI agent. This is learning experience of coding with AI.  

To Do: 
1) Add more python interfaces for the DSA core library.

## === Below is AI generated contents === ##
## 📁 Project Structure

```
c_library_example/
├── .github/
│   └── copilot-instructions.md     # AI agent development guide
├── include/
│   └── mylib.h                      # Public C API header
├── src/
│   ├── mylib.c                      # C implementation
│   └── mylib.cpp                    # C++ implementation (classes, templates)
├── build/                           # Compilation outputs (git ignored)
│   ├── lib/
│   │   └── mylib.so|dll|dylib       # Compiled shared library
│   └── obj/
│       ├── mylib.o                  # C object file
│       └── mylib_cpp.o              # C++ object file
├── examples/
│   ├── mylib_wrapper.py             # Full Python wrapper (ctypes)
│   ├── cpp_example.py               # C++ features demo
│   ├── simple_example.py            # Minimal usage example
│   ├── test_mylib.py                # Unit tests (pytest)
│   └── QUICK_REFERENCE.py           # Code snippets
├── docs/
│   ├── README.md                    # Quick start guide
│   ├── START_HERE.md                # Complete getting started
│   ├── ADVANCED_USAGE.md            # Advanced techniques
│   ├── PROJECT_SUMMARY.md           # Project overview
│   ├── CPP_ARCHITECTURE.md          # C++/C integration deep dive
│   └── CPP_INTEGRATION_SUMMARY.md   # C++ additions summary
├── Makefile                         # Cross-platform build (primary)
├── build.bat                        # Windows build script
├── build.sh                         # Unix build script
└── INDEX.txt                        # File index
```

## 🚀 Quick Start

### 1. Build the Library

```bash
make or make all   # Build library and tests
make tests         # Build test executables only
make clean         # Clean
make help          # Show help
```

**Output**: Compiled library in `build/lib/`

### 2. Run Examples

```bash
# From project root
python examples/mylib_wrapper.py     # Full demonstration
python examples/cpp_example.py       # C++ features
python examples/simple_example.py    # Minimal example
```

### 3. Use in Your Code

```python
from examples.mylib_wrapper import MyLibrary

with MyLibrary() as lib:
    result = lib.add(5, 3)
    print(f"5 + 3 = {result}")
```

## 📚 Documentation

| File | Purpose |
|------|---------|
| [docs/START_HERE.md](docs/START_HERE.md) | Complete getting started guide |
| [docs/README.md](docs/README.md) | Quick reference |
| [docs/ADVANCED_USAGE.md](docs/ADVANCED_USAGE.md) | Advanced techniques |
| [docs/CPP_ARCHITECTURE.md](docs/CPP_ARCHITECTURE.md) | C++/C integration architecture |
| [.github/copilot-instructions.md](.github/copilot-instructions.md) | AI development guide |

## 🏗️ Architecture

### Dual Interfaces
- **C Interface** (`include/DSA/mylib.h`) - Simple, portable functions for C callers
- **C++ Implementation** (`src/`) - C++ implementation files (e.g., `src/mylib_cpp.cpp`) contain classes, templates and STL usage (hidden from C)
- **Python Binding** (`examples/mylib_wrapper.py`) - Pythonic ctypes wrapper

### Key Features
✓ Base and derived classes (OOP)
✓ Template classes with STL (`Vector<T>`)
✓ C++ exceptions converted to C return codes
✓ Static global object management
✓ Cross-platform (Windows DLL, Linux SO, macOS DYLIB)
✓ Automatic platform detection

## 🛠️ Build System

### Platform Detection
Makefile automatically detects:
- **Windows** → generates `mylib.dll`
- **Linux** → generates `mylib.so`
- **macOS** → generates `mylib.dylib`

### Compilation Order
1. `gcc -c src/mylib.c` → `build/obj/mylib.o` (pure C)
2. `g++ -c src/mylib.cpp` → `build/obj/mylib_cpp.o` (C++ with STL)
3. `g++ build/obj/*.o` → `build/lib/mylib.so` (C++ linker)

## 📦 Examples

### Simple Math
```python
from examples.mylib_wrapper import MyLibrary

with MyLibrary() as lib:
    print(lib.add(5, 3))      # 8
    print(lib.multiply(6, 7)) # 42
```

### Complex Numbers (C++ class)
```python
with MyLibrary() as lib:
    lib.set_complex_number(3.0, 4.0)
    mag = lib.get_complex_magnitude()  # 5.0
    lib.add_complex(1.0, 2.0)
    lib.multiply_complex(2.0, 3.0)
```

### Vector Template (C++ template)
```python
with MyLibrary() as lib:
    lib.vector_clear()
    lib.vector_push_double(1.5)
    lib.vector_push_double(2.5)
    total = lib.vector_sum()           # 4.0
    product = lib.vector_product()     # 3.75
```

## 🧪 Testing

```bash
# Run unit tests
pytest examples/test_mylib.py -v

# Minimal test
python examples/simple_example.py
```

## 🔧 Development

### Add New C Function

1. **Declare** in `include/DSA/mylib.h`:
   ```c
   int my_function(int a, int b);
   ```

2. **Implement** in `src/mylib.c`:
   ```c
   int my_function(int a, int b) {
       return a + b;
   }
   ```

3. **Bind** in `examples/mylib_wrapper.py`:
   ```python
   def my_function(self, a, b):
       return self.lib.my_function(ctypes.c_int(a), ctypes.c_int(b))
   ```

### Add New C++ Class

See [docs/CPP_ARCHITECTURE.md](docs/CPP_ARCHITECTURE.md) for detailed step-by-step guide.

## 📋 Key Files

| File | Lines | Purpose |
|------|-------|---------|
| [include/DSA/mylib.h](include/DSA/mylib.h) | 75 | C API declarations |
| [src/mylib.c](src/mylib.c) | 60 | C implementation |
| [src/mylib_cpp.cpp](src/mylib_cpp.cpp) | 280 | C++ implementation |
| [examples/mylib_wrapper.py](examples/mylib_wrapper.py) | 315 | Python ctypes wrapper |
| [Makefile](Makefile) | 100 | Build configuration |
| [docs/CPP_ARCHITECTURE.md](docs/CPP_ARCHITECTURE.md) | 200+ | Architecture guide |

## ⚙️ Requirements

- **Build**: GCC/G++ (or MinGW on Windows)
- **Python**: 3.7+ (no external dependencies for library, uses standard ctypes)
- **Testing**: pytest (optional, for unit tests)

## 📝 License

This is an example project for educational purposes.

## 🤖 AI Development

For AI agents or LLMs working on this project, see:
- [.github/copilot-instructions.md](.github/copilot-instructions.md) - Comprehensive development guide
- [docs/CPP_ARCHITECTURE.md](docs/CPP_ARCHITECTURE.md) - Architecture deep dive

## 📞 Structure Overview

This project demonstrates professional C/C++ library structure suitable for:
- Educational examples
- Foundation for larger projects
- Multi-language bindings (Python shown here)
- Cross-platform libraries

The organized structure makes it easy to extend with additional compilation units, languages, or components.

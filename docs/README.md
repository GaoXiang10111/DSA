# C Library Python Integration - Minimal Example

This project demonstrates how to create a C/C++ library and load it in Python using `ctypes`.

## Project Structure

```
c_library_example/
├── include/DSA/          # Public headers (mylib.h, SharedPtr.h, etc.)
├── src/                  # C/C++ sources (mylib.c, mylib_cpp.cpp)
├── Makefile              # Build configuration (cross-platform)
├── examples/             # Python wrappers and example scripts
└── build/                # Build outputs (bin, lib, obj, test)
```

## Quick Start

### 1. Build the Library

```bash
# Linux/macOS
make

# Windows (if make is installed)
mingw32-make

# Or compile directly
gcc -fPIC -shared -o mylib.dll mylib.c       # Windows
gcc -fPIC -shared -o mylib.so mylib.c        # Linux
gcc -fPIC -shared -o mylib.dylib mylib.c     # macOS
```

### 2. Use in Python

**Option A: Minimal approach**
```python
from simple_example import main
main()
```

**Option B: Full wrapper with context manager**
```python
from mylib_wrapper import MyLibrary

with MyLibrary() as lib:
    print(lib.add(5, 3))          # 8
    print(lib.get_version())       # "1.0.0"
    print(lib.sum_array([1,2,3]))  # 6
```

## API Reference

### C Library Functions

| Function | Parameters | Returns | Description |
|----------|-----------|---------|-------------|
| `init_library()` | - | int | Initialize library (0=success) |
| `cleanup_library()` | - | void | Cleanup resources |
| `add(a, b)` | int, int | int | Add two numbers |
| `subtract(a, b)` | int, int | int | Subtract two numbers |
| `multiply(a, b)` | int, int | int | Multiply two numbers |
| `get_version()` | - | char* | Get version string |
| `sum_array(arr, len)` | int*, int | int | Sum array elements |
| `get_status()` | - | int | Get initialization status |

### Python Wrapper Methods

```python
lib = MyLibrary()          # Create wrapper
lib.load()                 # Load library
lib.init()                 # Initialize
lib.add(a, b)              # Call functions
lib.cleanup()              # Cleanup
lib.unload()               # Unload library

# Or use context manager (recommended)
with MyLibrary() as lib:
    result = lib.add(5, 3)
```

## Cross-Platform Notes

- **Windows**: Generates `mylib.dll`
- **Linux**: Generates `mylib.so`
- **macOS**: Generates `mylib.dylib`

The Makefile automatically detects your platform and builds the correct library format.

## Common Issues

**"Failed to load library"**
- Make sure to run `make` first to compile the library
- Verify the DLL/SO file exists in the current directory

**"Function not found"**
- Check that the function is declared in `mylib.h` with `extern "C"`
- Verify the function is exported from the compiled library

**Type errors in Python**
- Set `restype` and `argtypes` before calling functions
- The wrapper class handles this automatically

# C/C++ Library with Python ctypes Integration - Complete Guide

## Overview

This project provides a **minimal, production-ready example** of:
- Writing a C library with a public API
- Compiling it to a shared library (DLL/SO)
- Loading and using it in Python via `ctypes`
- Proper resource management (init/cleanup)
- Cross-platform compatibility

## Files Created

| File | Purpose |
|------|---------|
| `mylib.h` | C header file with API declarations |
| `mylib.c` | C implementation with business logic |
| `Makefile` | Cross-platform build configuration |
| `build.bat` | Windows build script |
| `build.sh` | Linux/macOS build script |
| `mylib_wrapper.py` | Full-featured Python ctypes wrapper |
| `simple_example.py` | Minimal Python ctypes example |
| `README.md` | Quick start guide |
| `ADVANCED_USAGE.md` | This file - advanced techniques |

## Building the Library

### Method 1: Using Makefile (Recommended)

**Linux/macOS:**
```bash
make              # Build
make clean        # Clean build artifacts
make help         # Show help
```

**Windows (if make is installed):**
```bash
mingw32-make
# or
make
```

### Method 2: Using Build Scripts

**Windows:**
```cmd
build.bat         # Compile
build.bat clean   # Clean
```

**Linux/macOS:**
```bash
chmod +x build.sh
./build.sh        # Compile
./build.sh clean  # Clean
```

### Method 3: Direct Compilation

**Windows:**
```cmd
gcc -fPIC -shared -o mylib.dll mylib.c -Wall -Wextra -O2
```

**Linux:**
```bash
gcc -fPIC -shared -o mylib.so mylib.c -Wall -Wextra -O2
```

**macOS:**
```bash
gcc -fPIC -shared -o mylib.dylib mylib.c -Wall -Wextra -O2
```

## Python Integration

### Option 1: Full Wrapper (Recommended)

The `MyLibrary` class provides:
- Automatic platform detection
- Function signature management
- Context manager support
- Error handling

```python
from mylib_wrapper import MyLibrary

# Method 1: Manual control
lib = MyLibrary()
lib.load()
lib.init()
result = lib.add(5, 3)
lib.cleanup()
lib.unload()

# Method 2: Context manager (automatic cleanup)
with MyLibrary() as lib:
    result = lib.add(5, 3)
    print(f"5 + 3 = {result}")  # Output: 5 + 3 = 8
```

### Option 2: Minimal Direct Loading

For simple use cases:

```python
import ctypes
import sys

# Load library based on platform
if sys.platform == "win32":
    lib = ctypes.CDLL("mylib.dll")
elif sys.platform == "darwin":
    lib = ctypes.CDLL("mylib.dylib")
else:
    lib = ctypes.CDLL("mylib.so")

# Define function return type
lib.get_version.restype = ctypes.c_char_p

# Initialize and use
lib.init_library()
print(lib.add(5, 3))  # 8
lib.cleanup_library()
```

## API Reference

### C Library Functions

```c
int init_library();
void cleanup_library();
int add(int a, int b);
int subtract(int a, int b);
int multiply(int a, int b);
char* get_version();
int sum_array(int* arr, int length);
int get_status();
```

### Python Wrapper Methods

```python
class MyLibrary:
    def load() -> bool              # Load DLL/SO into memory
    def unload() -> bool            # Unload from memory
    
    def init() -> bool              # Initialize library
    def cleanup() -> None           # Cleanup resources
    
    def add(a: int, b: int) -> int
    def subtract(a: int, b: int) -> int
    def multiply(a: int, b: int) -> int
    def get_version() -> str
    def sum_array(arr: List[int]) -> int
    def get_status() -> int         # Returns 1 if initialized, 0 otherwise
    
    # Context manager support
    def __enter__() -> MyLibrary
    def __exit__()
```

## Advanced Usage

### 1. Working with Arrays

```python
import ctypes
from mylib_wrapper import MyLibrary

with MyLibrary() as lib:
    # Python list to C array
    arr = [1, 2, 3, 4, 5]
    c_arr = (ctypes.c_int * len(arr))(*arr)
    
    # Call function with array
    result = lib.sum_array(arr)
    print(f"Sum: {result}")  # Output: Sum: 15
```

### 2. Working with Strings

```python
with MyLibrary() as lib:
    version = lib.get_version()
    print(f"Version: {version}")  # Output: Version: 1.0.0
```

### 3. Custom Function Signatures

When adding new functions to the C library, define their signatures in Python:

```python
import ctypes

lib = ctypes.CDLL("mylib.dll")

# Define C function signature
# int process_data(int* data, int len, int* output)
lib.process_data.argtypes = [
    ctypes.POINTER(ctypes.c_int),
    ctypes.c_int,
    ctypes.POINTER(ctypes.c_int)
]
lib.process_data.restype = ctypes.c_int
```

### 4. Error Handling

```python
from mylib_wrapper import MyLibrary
import sys

try:
    with MyLibrary() as lib:
        result = lib.add(10, 20)
        print(f"Result: {result}")
except FileNotFoundError as e:
    print(f"Library not found: {e}")
    print("Please compile first: make")
    sys.exit(1)
except RuntimeError as e:
    print(f"Runtime error: {e}")
    sys.exit(1)
```

## Platform-Specific Notes

### Windows
- Compiles to `.dll` format
- `ctypes.CDLL()` automatically handles DLL loading
- Requires `gcc` (from MinGW or similar)

### Linux
- Compiles to `.so` format
- Standard location: `/usr/lib/` or `./`
- May need `LD_LIBRARY_PATH` for non-standard locations

### macOS
- Compiles to `.dylib` format
- Similar to Linux but with `.dylib` extension
- Requires Xcode command line tools

## Debugging Tips

### 1. Verify Library Export

**Linux/macOS:**
```bash
nm -D mylib.so | grep add    # Check exported symbols
```

**Windows:**
```cmd
dumpbin /exports mylib.dll   # Check exported functions
```

### 2. Test Library Directly

```python
import ctypes
import sys

try:
    if sys.platform == "win32":
        lib = ctypes.CDLL("mylib.dll")
    else:
        lib = ctypes.CDLL("./mylib.so")
    print("✓ Library loaded successfully")
    print(f"Functions: {dir(lib)}")
except OSError as e:
    print(f"✗ Failed to load: {e}")
```

### 3. Add Debug Output

Add `printf()` statements to C code:

```c
int add(int a, int b) {
    printf("DEBUG: add(%d, %d) called\n", a, b);
    return a + b;
}
```

## Performance Considerations

### 1. Minimize Function Call Overhead
- Batch operations in C when possible
- Pass data structures instead of individual values

### 2. Memory Management
- Allocate memory in C, release in C
- Use proper initialization/cleanup

### 3. Type Conversions
- Avoid implicit type conversions
- Use explicit ctypes conversion where needed

## Extending the Library

### Adding a New C Function

1. **Add to header** (`mylib.h`):
```c
int process(int value);
```

2. **Implement** (`mylib.c`):
```c
int process(int value) {
    return value * 2;
}
```

3. **Add to Python wrapper** (`mylib_wrapper.py`):
```python
def process(self, value):
    """Process a value."""
    return self.lib.process(ctypes.c_int(value))
```

4. **Recompile**:
```bash
make clean && make
```

## Common Pitfalls

| Issue | Solution |
|-------|----------|
| "No module named ctypes" | ctypes is built-in; check Python installation |
| "Cannot find library" | Verify `.dll/.so` exists in current directory |
| "Function not found" | Check function is exported with `extern "C"` |
| "Type error" | Set `restype` and `argtypes` for functions |
| "Segmentation fault" | Check pointer arguments and memory access |

## Testing

Run the provided examples to verify setup:

```bash
# Example 1: Comprehensive wrapper demo
python mylib_wrapper.py

# Example 2: Simple minimal example
python simple_example.py
```

Expected output:
```
✓ Library loaded
Version: 1.0.0
5 + 3 = 8
10 - 4 = 6
6 * 7 = 42
✓ Done!
```

## Security Considerations

⚠️ **Important:**
- Only load trusted DLL/SO files
- Validate all inputs from Python to C
- Be careful with pointer operations
- Test thoroughly before production use

## License

This example is provided as-is for educational purposes.

## Further Resources

- [ctypes Documentation](https://docs.python.org/3/library/ctypes.html)
- [GCC Manual](https://gcc.gnu.org/onlinedocs/)
- [C FFI Best Practices](https://cffi.readthedocs.io/)
- [ctypes Examples](https://github.com/topics/ctypes)

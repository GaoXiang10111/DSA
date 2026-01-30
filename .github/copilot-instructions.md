# Copilot Instructions: C/C++ Library with Python ctypes Integration

## Project Overview

This is a **cross-platform C/C++ library project** that demonstrates:
- C library implementation with public API exposed via `DSA/mylib.h`
- C++ classes and templates encapsulated internally in `src/mylib.cpp`, NOT exposed to C callers
- Dual interfaces: C functions (for C/Python) wrap C++ functionality (internal)
- Cross-platform compilation to shared libraries (DLL/SO/DYLIB) in `build/lib/`
- Python integration using ctypes (not C extensions or CFFI)
- Full lifecycle management with init/cleanup pattern
- Professional project organization suitable for larger workspaces, with project-specific header folders (DSA/, graph/, network/, etc.)

**Project Structure:**
```
DSA/             - Public C API header files (Data Structures & Algorithms)
src/             - C and C++ source code
build/lib/       - Compiled shared libraries
build/obj/       - Intermediate object files
examples/        - Example scripts and tests
docs/            - Documentation
Makefile         - Cross-platform build configuration
```

**Key Files:**
- `DSA/mylib.h` - Public C API declarations (only function signatures, no class definitions)
- `src/mylib.c` - C implementation for basic operations (init, math, arrays)
- `src/mylib.cpp` - C++ implementation with classes/templates + C wrapper functions
- `examples/mylib_wrapper.py` - ctypes wrapper class with context manager
- `Makefile` - Primary build system (compiles both .c and .cpp)
- `docs/CPP_ARCHITECTURE.md` - Deep dive into C++/C integration design

---

## Architecture & Design Patterns

### C Library Design
- **Header guards + extern "C"**: Ensures C linkage even when compiled as C++ (see `DSA/mylib.h` lines 3-6)
- **Initialization pattern**: `init_library()` / `cleanup_library()` required before/after use; `get_status()` tracks state
- **Global state**: Single static `is_initialized` flag manages library lifecycle (see `src/mylib.c` lines 5-6)
- **Array handling**: Raw pointer pattern (`int* arr, int length`) for arrays - no automatic memory management
- **String returns**: `get_version()` returns const string literal, NOT heap-allocated (caller doesn't free)

### C++ Integration (src/mylib.cpp)
- **Hidden classes**: C++ classes (`Number`, `ComplexNumber`, `Vector<T>`) defined only in src/mylib.cpp, NOT exposed to C
- **Wrapper functions**: Each C++ class/method exposed via `extern "C"` function that marshals data
  - Example: `ComplexNumber::add()` → C function `int add_complex(double, double)`
  - Return codes: 0 = success, 1 = exception (C doesn't have exceptions)
- **Static globals**: Single instances (`g_complex_cache`, `g_vector_cache`) managed internally
- **Template instantiation**: `Vector<double>` is the concrete template instantiation; wrapper functions work with this only
- **No class objects in C API**: C callers never see class definitions; they only call functions

### Python-C Boundary
- **ctypes (not C extensions)**: Simpler integration; no Python.h dependency
- **Function signatures configured**: `_configure_functions()` in `MyLibrary` sets `restype` and `argtypes` for ALL functions
  - Critical: Must include C++ wrapper functions here
- **Context manager pattern**: `MyLibrary` uses `__enter__`/`__exit__` for automatic load/cleanup
- **Platform detection**: `_find_library()` auto-detects DLL/SO/DYLIB based on `sys.platform`
- **Type conversion**: Python doubles/ints auto-convert via ctypes; complex data requires wrapper methods

---

## Build System

### Primary Build: `make` (All Platforms)
```bash
make           # Builds TARGET (mylib.dll|mylib.so|mylib.dylib) in build/lib/
make clean     # Removes build/ directory
make help      # Shows available targets
```

**Platform detection**: Makefile line 7 checks `$(OS)` for Windows_NT; `uname -s` for Unix variants.

**Compilation order** (Makefile lines 60-80):
1. `gcc -c src/mylib.c -Iinclude` → `build/obj/mylib.o` (pure C)
2. `g++ -c src/mylib.cpp -Iinclude -std=c++11` → `build/obj/mylib_cpp.o` (C++ with STL)
3. `g++ build/obj/mylib.o build/obj/mylib_cpp.o` → `build/lib/mylib.so` (C++ linker for both)

**Critical flags:**
- `-Iinclude` - Include path for header files
- `-fPIC` (Position Independent Code) - required on Linux/macOS
- `-shared` - creates dynamic library
- `-std=c++11` - enable C++11 features (needed for `Vector<T>`, `auto`)
- `-O2` - optimization; part of `CFLAGS_BASE`
- **Must use `g++` as linker** (not `gcc`) to link C++ STL symbols

---

## Development Workflows

### Testing
```bash
# Run pytest tests
pytest test_mylib.py -v

# Key test fixtures: See test_mylib.py line 12-19
# Creates lib instance, loads, inits, yields, then cleanup
@pytest.fixture
def lib():
    library = MyLibrary()
    library.load()
    library.init()
    yield library
    library.cleanup()
    library.unload()
```

### Running Examples
```bash
python examples/mylib_wrapper.py       # Full wrapper demo (main() function at EOF)
python examples/cpp_example.py         # C++ features demo (complex numbers, vectors)
python examples/simple_example.py      # Minimal ctypes usage
pytest examples/test_mylib.py -v      # Unit tests (pytest)
```

### Debugging
- **Library not found**: Check `build/lib/` contains `.dll`/`.so`/`.dylib`
- **Function missing**: Verify function is in `include/mylib.h` and exported from `src/mylib.c` (no `static` keyword)
- **Type errors**: Check `_configure_functions()` in `examples/mylib_wrapper.py` sets correct `restype` and `argtypes`

---

## Code Conventions

### C Library Additions
1. **Declare in `DSA/mylib.h`**: Add function signature inside `extern "C"` block
2. **Implement in `src/mylib.c`**: Write implementation, use static variables only if private
3. **Document init/cleanup**: If accessing shared state, modify `init_library()` or add new init function
4. **Return types**: Prefer simple types (int, char*) over structs; pass pointers for complex data

### C++ Class & Wrapper Additions
1. **Define class in `src/mylib.cpp`** (before wrapper functions):
   ```cpp
   class MyClass {
   private:
       // private members - hidden from C
   public:
       MyClass() { }
       void doSomething() { }
   };
   ```

2. **Create wrapper functions** (after class, inside `extern "C"`):
   ```cpp
   static MyClass g_my_object;  // singleton
   
   extern "C" {
       int my_class_do_something() {
           try {
               g_my_object.doSomething();
               return 0;
           } catch (...) {
               return 1;  // error
           }
       }
   }
   ```

3. **Declare in `DSA/mylib.h`** (inside `extern "C"`):
   ```c
   int my_class_do_something();  // NO class definition visible
   ```

4. **Update Python wrapper** in `examples/mylib_wrapper.py`:
   - Add to `_signatures` dict
   - Set `restype`/`argtypes` in `_configure_functions()`
   - Add wrapper method that calls the C function

### Python Wrapper Updates
1. **Add to `_signatures`** (line ~48): `'function_name': (None, [ctypes.c_int, ...], ctypes.c_int)`
2. **Configure in `_configure_functions()`**: Set `restype` and `argtypes`
3. **Add wrapper method**: Decode strings (`bytes` → `str`), convert arrays if needed

Example (new C++ function):
```c
// DSA/mylib.h
int add_complex(double real, double imag);

// examples/mylib_wrapper.py - add to _signatures
'add_complex': (None, [ctypes.c_double, ctypes.c_double], ctypes.c_int),

// examples/mylib_wrapper.py - add to _configure_functions()
self.lib.add_complex.argtypes = [ctypes.c_double, ctypes.c_double]
self.lib.add_complex.restype = ctypes.c_int

// examples/mylib_wrapper.py - add method
def add_complex(self, real, imag):
    result = self.lib.add_complex(ctypes.c_double(real), ctypes.c_double(imag))
    return result == 0
```

---

## Critical Context for AI Agents

- **No dynamic memory** in C: Strings are const literals; arrays are caller-allocated
- **Init must happen**: `init_library()` runs before any operation; tests verify via `get_status()`
- **Platform-specific output**: Build generates `.dll` (Windows), `.so` (Linux), `.dylib` (macOS)
- **ctypes auto-conversion**: Python lists/strings convert automatically for function arguments
- **Pytest fixture pattern**: Always use the `lib` fixture in tests to ensure proper init/cleanup lifecycle

---

## Key Reference Files
- **API contract**: [DSA/mylib.h](../DSA/mylib.h) - all exported functions
- **Build config**: [Makefile](../Makefile) - platform detection and compilation rules
- **C++ architecture**: [docs/CPP_ARCHITECTURE.md](../docs/CPP_ARCHITECTURE.md) - deep dive into C++/C integration design
- **Python wrapper**: [examples/mylib_wrapper.py](../examples/mylib_wrapper.py) - ctypes setup and function signatures
- **Test patterns**: [examples/test_mylib.py](../examples/test_mylib.py) - pytest fixtures and test structure
- **C++ examples**: [examples/cpp_example.py](../examples/cpp_example.py) - dedicated C++ feature demonstrations
- **Examples**: [examples/simple_example.py](../examples/simple_example.py) and [examples/mylib_wrapper.py](../examples/mylib_wrapper.py#L270) (main function)

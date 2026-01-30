"""
Header showing how the C/C++ library is architected with dual interfaces.

This demonstrates:
1. How C++ classes are hidden from C callers
2. How C wrapper functions expose C++ functionality to C
3. The separation of concerns between C and C++ code
"""

# ============================================================================
# C++ ARCHITECTURE OVERVIEW
# ============================================================================

# mylib.cpp contains:
# 
# ┌─ C++ IMPLEMENTATION LAYER (private to C) ──────────────────────────────┐
# │                                                                          │
# │  class Number (base class)                                             │
# │    └─ Abstract interface: getValue(), setValue(), getTypeName()       │
# │                                                                         │
# │  class RealNumber : public Number                                      │
# │    └─ Concrete: stores a double value                                 │
# │                                                                         │
# │  class ComplexNumber : public Number                                   │
# │    ├─ Stores: real + imaginary parts                                   │
# │    ├─ Methods: add(), multiply(), getValue() [magnitude]              │
# │    └─ Encapsulated: private members, public C++ interface            │
# │                                                                         │
# │  template<typename T> class Vector                                     │
# │    ├─ Generic container for any type T                               │
# │    ├─ Uses std::vector<T> internally                                 │
# │    └─ Methods: push(), get(), size(), clear()                        │
# │                                                                         │
# │  Static global objects:                                                │
# │    - g_complex_cache (stores one ComplexNumber)                       │
# │    - g_vector_cache (stores Vector<double> of values)                │
# │                                                                         │
# └─────────────────────────────────────────────────────────────────────────┘
#
# ┌─ C WRAPPER LAYER (exposed to C/Python) ────────────────────────────────┐
# │                                                                          │
# │  extern "C" functions that use the C++ objects:                        │
# │                                                                         │
# │  set_complex_number(real, imag)                                        │
# │    └─ Creates ComplexNumber(real, imag) and stores in g_complex_cache │
# │                                                                         │
# │  get_complex_magnitude()                                               │
# │    └─ Returns g_complex_cache.getValue() [magnitude]                 │
# │                                                                         │
# │  add_complex(real, imag)                                               │
# │    └─ g_complex_cache = g_complex_cache.add(ComplexNumber(real, imag))│
# │                                                                         │
# │  multiply_complex(real, imag)                                          │
# │    └─ g_complex_cache = g_complex_cache.multiply(...)                │
# │                                                                         │
# │  vector_push_double(value)                                             │
# │    └─ g_vector_cache.push(value)                                      │
# │                                                                         │
# │  vector_size() / vector_get_double(index) / vector_clear() / etc      │
# │    └─ Delegate to g_vector_cache methods                              │
# │                                                                         │
# └─────────────────────────────────────────────────────────────────────────┘

# ============================================================================
# KEY DESIGN PATTERNS
# ============================================================================

# 1. SEPARATION OF CONCERNS
#    - C++ code (.cpp) contains classes/templates with no C visibility
#    - C wrapper functions bridge C++ to C without exposing classes
#    - mylib.h declares ONLY the wrapper functions, not the classes

# 2. NO CLASS EXPOSURE TO C
#    - C callers cannot see ComplexNumber, Number, Vector classes
#    - Classes are fully encapsulated in mylib.cpp
#    - Only functions are exposed via extern "C"

# 3. STATIC GLOBAL OBJECTS
#    - g_complex_cache: Single ComplexNumber instance (singleton pattern)
#    - g_vector_cache: Single Vector<double> instance
#    - Managed internally; C callers use functions, not objects

# 4. WRAPPER FUNCTION PATTERN
#    - Each C++ class method → wrapper function
#    - Example: ComplexNumber.add() → int add_complex(double, double)
#    - Conversion: C++ exceptions → C return codes (0=success, 1=error)

# 5. TEMPLATE USAGE
#    - Vector<T> is instantiated once with T=double
#    - Wrapper functions work with concrete Vector<double> only
#    - Could extend to Vector<int>, Vector<float> if needed

# ============================================================================
# DATA FLOW EXAMPLE: Complex Number Operations
# ============================================================================

# C/Python Code:
#   mylib.set_complex_number(3.0, 4.0)
#   mag = mylib.get_complex_magnitude()
#
# Flow through layers:
#
#   Python ctypes calls set_complex_number(3.0, 4.0)
#        │
#        ├─→ C wrapper: int set_complex_number(double, double)
#        │       │
#        │       └─→ ComplexNumber(3.0, 4.0) [C++ constructor]
#        │           │
#        │           └─→ Store in g_complex_cache
#        │               │
#        │               └─→ Return 0 (success)
#        │
#   Python receives: 0
#
#   Python ctypes calls get_complex_magnitude()
#        │
#        ├─→ C wrapper: double get_complex_magnitude()
#        │       │
#        │       └─→ g_complex_cache.getValue()
#        │           │
#        │           └─→ C++ method: sqrt(real² + imag²)
#        │               │
#        │               └─→ Return 5.0
#        │
#   Python receives: 5.0

# ============================================================================
# COMPILATION: How C and C++ Merge
# ============================================================================

# Makefile:
#   1. Compile mylib.c with gcc    → mylib.o
#      - Pure C code
#      - References extern "C" functions from mylib.cpp
#
#   2. Compile mylib.cpp with g++  → mylib_cpp.o
#      - C++ code with classes, STL, templates
#      - extern "C" functions that call C++ classes internally
#
#   3. Link with g++ (C++ linker):  → mylib.so
#      - Links mylib.o + mylib_cpp.o
#      - C++ linker handles std:: library symbols
#      - Result: Single .so with both C and C++ code

# ============================================================================
# EXTENDING THE PROJECT
# ============================================================================

# To add a new C++ class:
#
# 1. Define class in mylib.cpp:
#    class NewClass { public: ... }
#
# 2. Create wrapper functions in mylib.cpp (after class):
#    extern "C" {
#        int create_newclass(...) { return ...; }
#        void destroy_newclass() { ... }
#        ...
#    }
#
# 3. Declare functions in mylib.h:
#    int create_newclass(...);
#    void destroy_newclass();
#    ...
#
# 4. Add Python wrapper methods in MyLibrary class:
#    def create_newclass(self, ...):
#        return self.lib.create_newclass(...)
#
# 5. Rebuild:
#    make clean && make

# ============================================================================
# COMPARING WITH OTHER APPROACHES
# ============================================================================

# Why ctypes + C wrapper instead of:
#
# 1. C Extension Module (Python.h):
#    ❌ More boilerplate, tighter coupling to Python
#    ✓ ctypes: Simpler, Python-agnostic, dynamic loading
#
# 2. CFFI:
#    ❌ Requires C parser, CFFI DSL
#    ✓ ctypes: Pure Python, no external dependencies
#
# 3. Direct .so loading:
#    ❌ Type mismatches, memory leaks likely
#    ✓ ctypes: Type-safe, automatic conversions
#
# 4. C++ direct without C wrapper:
#    ❌ C++ name mangling breaks C ABI
#    ✓ C wrapper + extern "C": Clean C ABI for all callers

# ============================================================================
# THREAD SAFETY NOTE
# ============================================================================

# Current implementation is NOT thread-safe because:
# - g_complex_cache is a single static global
# - g_vector_cache is a single static global
# - Multiple threads calling functions simultaneously will race
#
# To fix: Use thread-local storage (C++11 thread_local keyword):
#    thread_local ComplexNumber g_complex_cache;
#    thread_local Vector<double> g_vector_cache;
#
# Or: Use mutex locks around operations in the C wrapper functions.

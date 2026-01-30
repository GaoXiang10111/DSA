#!/usr/bin/env python3
"""
Quick Reference - C Library with Python ctypes

Location: c:\\Users\\xgao\\Library\\LLM\\c_library_example\\
"""

# ============================================================================
# QUICK START - Copy & Paste Ready
# ============================================================================

# OPTION 1: Recommended - Using Context Manager
from mylib_wrapper import MyLibrary

with MyLibrary() as lib:
    print(lib.add(5, 3))              # 8
    print(lib.multiply(6, 7))         # 42
    print(lib.get_version())          # "1.0.0"
    print(lib.sum_array([1,2,3,4,5])) # 15


# OPTION 2: Manual Control
from mylib_wrapper import MyLibrary

lib = MyLibrary()
lib.load()
lib.init()
print(lib.add(10, 20))
lib.cleanup()
lib.unload()


# OPTION 3: Minimal - Direct ctypes
import ctypes
import sys
from pathlib import Path

# Auto-detect library based on platform
if sys.platform == "win32":
    lib_name = "mylib.dll"
elif sys.platform == "darwin":
    lib_name = "mylib.dylib"
else:
    lib_name = "mylib.so"

lib_path = Path(__file__).parent.parent / "build" / "lib" / lib_name
lib = ctypes.CDLL(str(lib_path))
lib.init_library()
print(lib.add(5, 3))
lib.cleanup_library()


# ============================================================================
# COMMON OPERATIONS (using MyLibrary wrapper for convenience)
# ============================================================================

from mylib_wrapper import MyLibrary

with MyLibrary() as lib:
    # Arithmetic
    lib.add(10, 20)        # 30
    lib.subtract(10, 20)   # -10
    lib.multiply(3, 4)     # 12

    # Arrays
    lib.sum_array([1, 2, 3])       # 6

    # Status
    lib.get_version()      # "1.0.0"
    lib.get_status()       # 1 = initialized, 0 = not


# ============================================================================
# BUILD COMMANDS
# ============================================================================

"""
Windows:
  build.bat

Linux/macOS:
  chmod +x build.sh
  ./build.sh

Or directly:
  gcc -fPIC -shared -o mylib.dll mylib.c -Wall -Wextra -O2
  gcc -fPIC -shared -o mylib.so mylib.c -Wall -Wextra -O2
"""


# ============================================================================
# ERROR HANDLING
# ============================================================================

try:
    with MyLibrary() as lib:
        result = lib.add(5, 3)
except FileNotFoundError:
    print("Library not found. Run: build.bat")
except RuntimeError as e:
    print(f"Runtime error: {e}")


# ============================================================================
# ADDING NEW FUNCTIONS
# ============================================================================

"""
1. Add to mylib.h:
   int my_function(int x);

2. Add to mylib.c:
   int my_function(int x) {
       return x * 2;
   }

3. Add to mylib_wrapper.py:
   def my_function(self, x):
       return self.lib.my_function(ctypes.c_int(x))

4. Rebuild:
   make clean && make
"""


# ============================================================================
# FILES REFERENCE
# ============================================================================

FILES = {
    "mylib.h": "C header (API declaration)",
    "mylib.c": "C implementation",
    "mylib.dll": "Compiled library (Windows)",
    "mylib.so": "Compiled library (Linux)",
    "mylib.dylib": "Compiled library (macOS)",
    "Makefile": "Build configuration",
    "build.bat": "Windows build script",
    "build.sh": "Linux/macOS build script",
    "mylib_wrapper.py": "Full-featured Python wrapper",
    "simple_example.py": "Minimal ctypes example",
    "test_mylib.py": "Unit tests",
    "README.md": "Quick start",
    "ADVANCED_USAGE.md": "Advanced reference",
    "PROJECT_SUMMARY.md": "Complete overview",
}


# ============================================================================
# TROUBLESHOOTING
# ============================================================================

SOLUTIONS = {
    "Library not found": "Run build.bat or ./build.sh",
    "Module ctypes not found": "ctypes is built-in; check Python",
    "Function not found": "Check mylib.h and mylib.c",
    "Type error": "Verify ctypes argtypes/restype",
    "Segmentation fault": "Check pointer args and bounds",
}


# ============================================================================
# PLATFORM DETECTION
# ============================================================================

import sys

if sys.platform == "win32":
    lib_name = "mylib.dll"
    print("Windows detected")
elif sys.platform == "darwin":
    lib_name = "mylib.dylib"
    print("macOS detected")
else:
    lib_name = "mylib.so"
    print("Linux detected")


# ============================================================================
# CTYPES CHEAT SHEET
# ============================================================================

"""
Loading:
  lib = ctypes.CDLL("path/to/library")

Function signatures:
  lib.function.argtypes = [ctypes.c_int, ctypes.c_int]
  lib.function.restype = ctypes.c_int

Types:
  ctypes.c_int          # int
  ctypes.c_char_p       # char*
  ctypes.c_float        # float
  ctypes.c_double       # double
  ctypes.POINTER(type)  # type*
  type * length         # type[length]

Arrays:
  arr = (ctypes.c_int * 5)(1, 2, 3, 4, 5)
  lib.func.argtypes = [ctypes.POINTER(ctypes.c_int)]
"""


if __name__ == "__main__":
    print("C Library Quick Reference")
    print("=" * 60)
    print("\nFiles in this project:")
    for filename, description in FILES.items():
        print(f"  {filename:25} - {description}")
    print("\nCommon solutions:")
    for problem, solution in SOLUTIONS.items():
        print(f"  {problem:25} -> {solution}")
    print("\nQuick start:")
    print("  1. Run: build.bat (or ./build.sh)")
    print("  2. Run: python mylib_wrapper.py")
    print("  3. Check: ADVANCED_USAGE.md for detailed docs")

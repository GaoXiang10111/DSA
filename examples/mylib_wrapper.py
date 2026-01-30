"""
Python ctypes wrapper for mylib C library.

This module demonstrates how to load, use, and unload a C library
compiled as a DLL (Windows) or SO (Linux/macOS).
"""

import ctypes
import os
import sys
from pathlib import Path
from contextlib import contextmanager


class MyLibrary:
    """Wrapper for mylib C library using ctypes."""

    def __init__(self, lib_path=None):
        """
        Initialize the library wrapper.
        
        Args:
            lib_path: Path to the compiled library (.dll, .so, or .dylib).
                     If None, searches in current directory.
        """
        self.lib_path = lib_path or self._find_library()
        self.lib = None
        self._setup_functions()

    def _find_library(self):
        """Auto-detect library based on platform."""
        if sys.platform == "win32":
            lib_name = "mylib.dll"
        elif sys.platform == "darwin":
            lib_name = "mylib.dylib"
        else:  # Linux and others
            lib_name = "mylib.so"
        
        # Check in build/lib/ directory
        lib_path = Path(__file__).parent.parent / "build" / "lib" / lib_name
        if lib_path.exists():
            return str(lib_path.absolute())
        
        # Fallback: check current directory
        lib_path = Path(lib_name)
        if lib_path.exists():
            return str(lib_path.absolute())
        
        raise FileNotFoundError(
            f"Library '{lib_name}' not found. "
            f"Please compile using: make"
        )

    def _setup_functions(self):
        """Define function signatures before loading."""
        # We'll set this up after loading the library
        self._signatures = {
            'init_library': (None, [], ctypes.c_int),
            'cleanup_library': (None, [], None),
            'add': (None, [ctypes.c_int, ctypes.c_int], ctypes.c_int),
            'subtract': (None, [ctypes.c_int, ctypes.c_int], ctypes.c_int),
            'multiply': (None, [ctypes.c_int, ctypes.c_int], ctypes.c_int),
            'get_version': (None, [], ctypes.c_char_p),
            'sum_array': (None, [ctypes.POINTER(ctypes.c_int), ctypes.c_int], ctypes.c_int),
            'get_status': (None, [], ctypes.c_int),
            # C++ wrapped functions
            'set_complex_number': (None, [ctypes.c_double, ctypes.c_double], ctypes.c_int),
            'get_complex_magnitude': (None, [], ctypes.c_double),
            'add_complex': (None, [ctypes.c_double, ctypes.c_double], ctypes.c_int),
            'multiply_complex': (None, [ctypes.c_double, ctypes.c_double], ctypes.c_int),
            'get_number_type': (None, [], ctypes.c_char_p),
            'vector_push_double': (None, [ctypes.c_double], ctypes.c_int),
            'vector_size': (None, [], ctypes.c_int),
            'vector_get_double': (None, [ctypes.c_int], ctypes.c_double),
            'vector_clear': (None, [], None),
            'vector_sum': (None, [], ctypes.c_double),
            'vector_product': (None, [], ctypes.c_double),
        }

    def load(self):
        """Load the library into memory."""
        try:
            self.lib = ctypes.CDLL(self.lib_path)
            print(f"✓ Library loaded: {self.lib_path}")
            
            # Configure function signatures
            self._configure_functions()
            return True
        except OSError as e:
            print(f"✗ Failed to load library: {e}")
            return False

    def _configure_functions(self):
        """Set up ctypes function signatures."""
        if not self.lib:
            raise RuntimeError("Library not loaded. Call load() first.")
        
        # C functions
        self.lib.get_version.restype = ctypes.c_char_p
        self.lib.sum_array.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_int]
        self.lib.sum_array.restype = ctypes.c_int
        
        # C++ wrapped functions - complex numbers
        self.lib.set_complex_number.argtypes = [ctypes.c_double, ctypes.c_double]
        self.lib.set_complex_number.restype = ctypes.c_int
        self.lib.get_complex_magnitude.restype = ctypes.c_double
        self.lib.add_complex.argtypes = [ctypes.c_double, ctypes.c_double]
        self.lib.add_complex.restype = ctypes.c_int
        self.lib.multiply_complex.argtypes = [ctypes.c_double, ctypes.c_double]
        self.lib.multiply_complex.restype = ctypes.c_int
        self.lib.get_number_type.restype = ctypes.c_char_p
        
        # C++ wrapped functions - vector
        self.lib.vector_push_double.argtypes = [ctypes.c_double]
        self.lib.vector_push_double.restype = ctypes.c_int
        self.lib.vector_size.restype = ctypes.c_int
        self.lib.vector_get_double.argtypes = [ctypes.c_int]
        self.lib.vector_get_double.restype = ctypes.c_double
        self.lib.vector_sum.restype = ctypes.c_double
        self.lib.vector_product.restype = ctypes.c_double

    def unload(self):
        """Unload the library from memory."""
        if self.lib:
            try:
                self.lib = None
                print("✓ Library unloaded")
                return True
            except Exception as e:
                print(f"✗ Failed to unload library: {e}")
                return False

    def init(self):
        """Initialize library resources."""
        if not self.lib:
            raise RuntimeError("Library not loaded. Call load() first.")
        result = self.lib.init_library()
        return result == 0

    def cleanup(self):
        """Cleanup library resources."""
        if not self.lib:
            raise RuntimeError("Library not loaded. Call load() first.")
        self.lib.cleanup_library()

    def add(self, a, b):
        """Add two integers."""
        return self.lib.add(ctypes.c_int(a), ctypes.c_int(b))

    def subtract(self, a, b):
        """Subtract two integers."""
        return self.lib.subtract(ctypes.c_int(a), ctypes.c_int(b))

    def multiply(self, a, b):
        """Multiply two integers."""
        return self.lib.multiply(ctypes.c_int(a), ctypes.c_int(b))

    def get_version(self):
        """Get library version string."""
        version_bytes = self.lib.get_version()
        return version_bytes.decode('utf-8') if version_bytes else "Unknown"

    def sum_array(self, arr):
        """Sum an array of integers."""
        # Convert Python list to C array
        c_arr = (ctypes.c_int * len(arr))(*arr)
        return self.lib.sum_array(c_arr, ctypes.c_int(len(arr)))

    def get_status(self):
        """Get library status (1=initialized, 0=not initialized)."""
        if not self.lib:
            raise RuntimeError("Library not loaded. Call load() first.")
        return self.lib.get_status()

    # ===== C++ Features =====
    
    def set_complex_number(self, real, imag):
        """Set a complex number (real + imaginary)."""
        result = self.lib.set_complex_number(ctypes.c_double(real), ctypes.c_double(imag))
        return result == 0
    
    def get_complex_magnitude(self):
        """Get magnitude of the stored complex number."""
        return self.lib.get_complex_magnitude()
    
    def add_complex(self, real, imag):
        """Add a complex number to the stored one."""
        result = self.lib.add_complex(ctypes.c_double(real), ctypes.c_double(imag))
        return result == 0
    
    def multiply_complex(self, real, imag):
        """Multiply the stored complex number by another."""
        result = self.lib.multiply_complex(ctypes.c_double(real), ctypes.c_double(imag))
        return result == 0
    
    def get_number_type(self):
        """Get the type name of the stored number."""
        type_bytes = self.lib.get_number_type()
        return type_bytes.decode('utf-8') if type_bytes else "Unknown"
    
    def vector_push_double(self, value):
        """Push a double value to the vector."""
        result = self.lib.vector_push_double(ctypes.c_double(value))
        return result == 0
    
    def vector_size(self):
        """Get the current size of the vector."""
        return self.lib.vector_size()
    
    def vector_get_double(self, index):
        """Get value at index in the vector."""
        return self.lib.vector_get_double(ctypes.c_int(index))
    
    def vector_clear(self):
        """Clear the vector."""
        self.lib.vector_clear()
    
    def vector_sum(self):
        """Sum all values in the vector."""
        return self.lib.vector_sum()
    
    def vector_product(self):
        """Compute the product of all values in the vector."""
        return self.lib.vector_product()

    def __enter__(self):
        """Context manager entry."""
        self.load()
        self.init()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit."""
        self.cleanup()
        self.unload()


def main():
    """Example usage of the library."""
    print("=" * 60)
    print("C Library Integration with Python ctypes")
    print("=" * 60)
    print()

    # Example 1: Manual load/unload with C features
    print("Example 1: C Features (Manual load/unload)")
    print("-" * 60)
    mylib = MyLibrary()
    
    if not mylib.load():
        print("Failed to load library. Make sure to compile it first:")
        print("  make")
        return 1
    
    mylib.init()
    
    # Call library functions
    result_add = mylib.add(5, 3)
    result_sub = mylib.subtract(10, 4)
    result_mul = mylib.multiply(6, 7)
    version = mylib.get_version()
    status = mylib.get_status()
    
    print(f"Version: {version}")
    print(f"Status: {status} (1=initialized, 0=not)")
    print(f"5 + 3 = {result_add}")
    print(f"10 - 4 = {result_sub}")
    print(f"6 * 7 = {result_mul}")
    
    # Array operation
    arr = [1, 2, 3, 4, 5]
    arr_sum = mylib.sum_array(arr)
    print(f"sum({arr}) = {arr_sum}")
    
    print()

    # Example 2: C++ Features - Complex Numbers
    print("Example 2: C++ Complex Number Features")
    print("-" * 60)
    
    # Create complex number 3 + 4i
    mylib.set_complex_number(3.0, 4.0)
    mag = mylib.get_complex_magnitude()
    num_type = mylib.get_number_type()
    print(f"Set complex: 3 + 4i")
    print(f"Magnitude: {mag} (expected: 5.0)")
    print(f"Type: {num_type}")
    
    # Add complex number: (3+4i) + (1+2i) = 4+6i
    mylib.add_complex(1.0, 2.0)
    mag = mylib.get_complex_magnitude()
    print(f"After adding (1+2i): magnitude = {mag:.4f}")
    
    print()

    # Example 3: C++ Features - Vector Template
    print("Example 3: C++ Vector<double> Template Features")
    print("-" * 60)
    
    mylib.vector_clear()
    values = [1.5, 2.5, 3.5, 4.5, 5.5]
    for val in values:
        mylib.vector_push_double(val)
    
    print(f"Pushed values: {values}")
    print(f"Vector size: {mylib.vector_size()}")
    print(f"Vector sum: {mylib.vector_sum()}")
    print(f"Vector product: {mylib.vector_product():.4f}")
    
    mylib.cleanup()
    mylib.unload()
    print()

    # Example 4: Using context manager (recommended)
    print("Example 4: Context Manager (automatic cleanup)")
    print("-" * 60)
    try:
        with MyLibrary() as mylib:
            print(f"Version: {mylib.get_version()}")
            print(f"20 + 30 = {mylib.add(20, 30)}")
            print(f"Status: {mylib.get_status()}")
            
            # C++ features
            mylib.set_complex_number(1.0, 1.0)
            print(f"Complex 1+i magnitude: {mylib.get_complex_magnitude():.4f}")
    except FileNotFoundError as e:
        print(f"Error: {e}")
        return 1
    
    print()
    print("=" * 60)
    print("✓ All examples completed successfully!")
    print("=" * 60)
    return 0


if __name__ == "__main__":
    sys.exit(main())

"""
C++ Feature Examples

This module demonstrates the C++ features exposed through ctypes:
- Complex number operations (wrapped C++ ComplexNumber class)
- Vector template operations (wrapped C++ Vector<T> template)
"""

import ctypes
import sys
from pathlib import Path


def load_library():
    """Load C library based on platform."""
    if sys.platform == "win32":
        lib_name = "mylib.dll"
    elif sys.platform == "darwin":
        lib_name = "mylib.dylib"
    else:
        lib_name = "mylib.so"
    
    # Check in build/lib/ directory
    from pathlib import Path
    lib_path = Path(__file__).parent.parent / "build" / "lib" / lib_name
    if lib_path.exists():
        lib = ctypes.CDLL(str(lib_path.absolute()))
        print(f"✓ Loaded: {lib_name}")
        return lib
    
    # Fallback: check current directory
    lib_path = Path(lib_name)
    if lib_path.exists():
        lib = ctypes.CDLL(str(lib_path.absolute()))
        print(f"✓ Loaded: {lib_name}")
        return lib
    
    raise FileNotFoundError(f"Library '{lib_name}' not found in build/lib/ or current directory")


def setup_complex_functions(lib):
    """Configure complex number function signatures."""
    lib.set_complex_number.argtypes = [ctypes.c_double, ctypes.c_double]
    lib.set_complex_number.restype = ctypes.c_int
    
    lib.get_complex_magnitude.argtypes = []
    lib.get_complex_magnitude.restype = ctypes.c_double
    
    lib.add_complex.argtypes = [ctypes.c_double, ctypes.c_double]
    lib.add_complex.restype = ctypes.c_int
    
    lib.multiply_complex.argtypes = [ctypes.c_double, ctypes.c_double]
    lib.multiply_complex.restype = ctypes.c_int
    
    lib.get_number_type.argtypes = []
    lib.get_number_type.restype = ctypes.c_char_p


def setup_vector_functions(lib):
    """Configure vector function signatures."""
    lib.vector_push_double.argtypes = [ctypes.c_double]
    lib.vector_push_double.restype = ctypes.c_int
    
    lib.vector_size.argtypes = []
    lib.vector_size.restype = ctypes.c_int
    
    lib.vector_get_double.argtypes = [ctypes.c_int]
    lib.vector_get_double.restype = ctypes.c_double
    
    lib.vector_clear.argtypes = []
    lib.vector_clear.restype = None
    
    lib.vector_sum.argtypes = []
    lib.vector_sum.restype = ctypes.c_double
    
    lib.vector_product.argtypes = []
    lib.vector_product.restype = ctypes.c_double


def example_complex_numbers(lib):
    """Example: Complex number operations."""
    print("\n" + "=" * 60)
    print("C++ Complex Number Operations")
    print("=" * 60)
    
    # Create a complex number: 3 + 4i
    lib.set_complex_number(3.0, 4.0)
    print(f"Set complex: 3 + 4i")
    
    # Get magnitude: |3 + 4i| = sqrt(9 + 16) = 5
    mag = lib.get_complex_magnitude()
    print(f"Magnitude: {mag} (expected: 5.0)")
    
    # Get type name
    type_name = lib.get_number_type()
    print(f"Type: {type_name.decode('utf-8') if type_name else 'Unknown'}")
    
    # Add another complex number: (3 + 4i) + (1 + 2i) = 4 + 6i
    lib.add_complex(1.0, 2.0)
    mag = lib.get_complex_magnitude()
    print(f"After adding (1 + 2i): magnitude = {mag:.4f}")
    
    # Reset and demonstrate multiplication
    lib.set_complex_number(2.0, 3.0)  # 2 + 3i
    lib.multiply_complex(1.0, -1.0)   # multiply by (1 - i)
    mag = lib.get_complex_magnitude()
    print(f"(2+3i) * (1-i) magnitude = {mag:.4f}")


def example_vector_operations(lib):
    """Example: Vector template operations with doubles."""
    print("\n" + "=" * 60)
    print("C++ Vector<double> Template Operations")
    print("=" * 60)
    
    # Clear previous data
    lib.vector_clear()
    
    # Push some values
    values = [1.5, 2.5, 3.5, 4.5, 5.5]
    for val in values:
        lib.vector_push_double(val)
        print(f"Pushed {val}")
    
    # Get size
    size = lib.vector_size()
    print(f"\nVector size: {size}")
    
    # Access elements
    print("Vector elements:")
    for i in range(size):
        val = lib.vector_get_double(i)
        print(f"  v[{i}] = {val}")
    
    # Sum
    total = lib.vector_sum()
    print(f"\nSum: {total} (expected: {sum(values)})")
    
    # Product
    prod = lib.vector_product()
    expected_prod = 1.0
    for v in values:
        expected_prod *= v
    print(f"Product: {prod:.4f} (expected: {expected_prod:.4f})")


def example_mixed_operations(lib):
    """Example: Mix of C and C++ features."""
    print("\n" + "=" * 60)
    print("Mixed C and C++ Features")
    print("=" * 60)
    
    # Initialize library
    lib.init_library()
    
    # Setup version function
    lib.get_version.restype = ctypes.c_char_p
    
    # Use both C and C++ features
    print("\nC Features:")
    print(f"  5 + 3 = {lib.add(5, 3)}")
    version = lib.get_version()
    version_str = version.decode('utf-8') if isinstance(version, bytes) else version
    print(f"  Version: {version_str}")
    
    print("\nC++ Features:")
    lib.set_complex_number(1.0, 1.0)
    mag = lib.get_complex_magnitude()
    print(f"  Complex 1+i magnitude: {mag:.4f}")
    
    lib.vector_clear()
    for v in [10.0, 20.0, 30.0]:
        lib.vector_push_double(v)
    print(f"  Vector sum [10,20,30]: {lib.vector_sum()}")


def main():
    """Run all C++ examples."""
    lib = load_library()
    
    # Setup function signatures
    lib.init_library()
    setup_complex_functions(lib)
    setup_vector_functions(lib)
    
    # Run examples
    try:
        example_complex_numbers(lib)
        example_vector_operations(lib)
        example_mixed_operations(lib)
        
        print("\n" + "=" * 60)
        print("✓ All C++ examples completed successfully!")
        print("=" * 60)
    except Exception as e:
        print(f"Error: {e}")
        return 1
    finally:
        lib.cleanup_library()
    
    return 0


if __name__ == "__main__":
    sys.exit(main())

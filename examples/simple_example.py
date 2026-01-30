"""
Minimal ctypes example - Load and use C library directly.
This is a simpler alternative to the wrapper class.
"""

import ctypes
import sys
from pathlib import Path


def load_library():
    """Load C library based on platform."""
    # Determine library name based on OS
    if sys.platform == "win32":
        lib_name = "mylib.dll"
    elif sys.platform == "darwin":
        lib_name = "mylib.dylib"
    else:
        lib_name = "mylib.so"
    
    # Check in build/lib/ directory
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


def main():
    """Minimal example."""
    # Load library
    lib = load_library()
    
    # Initialize
    lib.init_library()
    
    # Call functions
    print(f"5 + 3 = {lib.add(5, 3)}")
    print(f"10 - 4 = {lib.subtract(10, 4)}")
    print(f"6 * 7 = {lib.multiply(6, 7)}")
    
    # Get version (needs restype definition)
    lib.get_version.restype = ctypes.c_char_p
    version = lib.get_version().decode('utf-8')
    print(f"Version: {version}")
    
    # Cleanup
    lib.cleanup_library()
    print("✓ Done!")


if __name__ == "__main__":
    main()

"""
Unit tests for mylib C library using pytest.

Install: pip install pytest
Run: pytest test_mylib.py -v
"""

import pytest
import sys
from pathlib import Path
from mylib_wrapper import MyLibrary


@pytest.fixture
def lib():
    """Fixture to provide initialized library for tests."""
    library = MyLibrary()
    library.load()
    library.init()
    yield library
    library.cleanup()
    library.unload()


class TestArithmetic:
    """Test basic arithmetic operations."""
    
    def test_add(self, lib):
        assert lib.add(2, 3) == 5
        assert lib.add(0, 0) == 0
        assert lib.add(-5, 3) == -2
        assert lib.add(-5, -3) == -8
    
    def test_subtract(self, lib):
        assert lib.subtract(5, 3) == 2
        assert lib.subtract(0, 0) == 0
        assert lib.subtract(-5, 3) == -8
        assert lib.subtract(3, 5) == -2
    
    def test_multiply(self, lib):
        assert lib.multiply(3, 4) == 12
        assert lib.multiply(0, 100) == 0
        assert lib.multiply(-3, 4) == -12
        assert lib.multiply(-3, -4) == 12


class TestLibraryState:
    """Test library initialization and state."""
    
    def test_initialization(self, lib):
        """Test library is properly initialized."""
        status = lib.get_status()
        assert status == 1, "Library should be initialized"
    
    def test_version(self, lib):
        """Test version retrieval."""
        version = lib.get_version()
        assert version == "1.0.0"
        assert isinstance(version, str)


class TestArrayOperations:
    """Test array operations."""
    
    def test_sum_array_basic(self, lib):
        assert lib.sum_array([1, 2, 3]) == 6
        assert lib.sum_array([0]) == 0
        assert lib.sum_array([]) == 0
    
    def test_sum_array_negative(self, lib):
        assert lib.sum_array([-1, -2, -3]) == -6
        assert lib.sum_array([-5, 5]) == 0
    
    def test_sum_array_large(self, lib):
        arr = list(range(1, 101))  # 1 to 100
        expected = sum(arr)
        assert lib.sum_array(arr) == expected


class TestContextManager:
    """Test context manager functionality."""
    
    def test_context_manager(self):
        """Test that context manager properly initializes and cleans up."""
        with MyLibrary() as lib:
            status = lib.get_status()
            assert status == 1
            result = lib.add(10, 20)
            assert result == 30
    
    def test_context_manager_exception_handling(self):
        """Test cleanup happens even on exception."""
        try:
            with MyLibrary() as lib:
                assert lib.get_status() == 1
                # Simulate error
                raise ValueError("Test error")
        except ValueError:
            pass
        # Library should still be cleaned up


class TestEdgeCases:
    """Test edge cases and boundary conditions."""
    
    def test_large_numbers(self, lib):
        """Test with large integer values."""
        result = lib.add(1000000, 2000000)
        assert result == 3000000
    
    def test_zero_operations(self, lib):
        """Test operations with zero."""
        assert lib.add(0, 100) == 100
        assert lib.subtract(100, 100) == 0
        assert lib.multiply(100, 0) == 0


class TestManualLoading:
    """Test manual load/unload without context manager."""
    
    def test_load_unload_cycle(self):
        """Test explicit load/unload cycle."""
        lib = MyLibrary()
        
        # Initially unloaded
        with pytest.raises(RuntimeError):
            lib.get_status()
        
        # Load and use
        assert lib.load() is True
        assert lib.init() is True
        assert lib.get_status() == 1
        
        # Cleanup
        lib.cleanup()
        lib.unload()
    
    def test_multiple_loads(self):
        """Test multiple load/unload cycles."""
        lib = MyLibrary()
        
        for _ in range(3):
            assert lib.load() is True
            assert lib.init() is True
            lib.cleanup()
            lib.unload()


if __name__ == "__main__":
    pytest.main([__file__, "-v"])

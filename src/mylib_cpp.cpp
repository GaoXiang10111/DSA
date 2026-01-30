#include "DSA/mylib.h"
#include <cmath>
#include <cstring>
#include <vector>
#include <memory>

// ============================================================================
// C++ Classes (not exposed to C callers)
// ============================================================================

/**
 * Base class for numeric values
 */
class Number {
public:
    virtual ~Number() = default;
    virtual double getValue() const = 0;
    virtual void setValue(double val) = 0;
    virtual const char* getTypeName() const = 0;
};

/**
 * Real number implementation
 */
class RealNumber : public Number {
private:
    double value;
public:
    explicit RealNumber(double val = 0.0) : value(val) {}
    
    double getValue() const override {
        return value;
    }
    
    void setValue(double val) override {
        value = val;
    }
    
    const char* getTypeName() const override {
        return "Real";
    }
};

/**
 * Complex number implementation: a + bi
 */
class ComplexNumber : public Number {
private:
    double real;
    double imag;
public:
    explicit ComplexNumber(double r = 0.0, double i = 0.0) 
        : real(r), imag(i) {}
    
    double getReal() const { return real; }
    double getImaginary() const { return imag; }
    
    double getValue() const override {
        // Return magnitude for virtual method
        return std::sqrt(real * real + imag * imag);
    }
    
    void setValue(double val) override {
        real = val;
        imag = 0.0;
    }
    
    const char* getTypeName() const override {
        return "Complex";
    }
    
    // Complex addition
    ComplexNumber add(const ComplexNumber& other) const {
        return ComplexNumber(real + other.real, imag + other.imag);
    }
    
    // Complex multiplication
    ComplexNumber multiply(const ComplexNumber& other) const {
        double r = real * other.real - imag * other.imag;
        double i = real * other.imag + imag * other.real;
        return ComplexNumber(r, i);
    }
};

/**
 * Generic Vector template for storing multiple values
 */
template<typename T>
class Vector {
private:
    std::vector<T> data;
public:
    Vector() = default;
    explicit Vector(int size) : data(size) {}
    
    void push(const T& value) {
        data.push_back(value);
    }
    
    T get(int index) const {
        if (index >= 0 && index < (int)data.size()) {
            return data[index];
        }
        return T();
    }
    
    int size() const {
        return data.size();
    }
    
    void clear() {
        data.clear();
    }
};

// ============================================================================
// Global C++ objects (managed internally)
// ============================================================================

// Note: These are default-constructed and don't require explicit init
static ComplexNumber g_complex_cache(0.0, 0.0);
static Vector<double> g_vector_cache;

// ============================================================================
// C Wrapper Functions (exposed to C code)
// ============================================================================

/**
 * Create and store a complex number (real + imaginary)
 */
extern "C" {
    int set_complex_number(double real, double imag) {
        try {
            g_complex_cache = ComplexNumber(real, imag);
            return 0;  // success
        } catch (...) {
            return 1;  // error
        }
    }
    
    /**
     * Get magnitude of complex number
     */
    double get_complex_magnitude() {
        return g_complex_cache.getValue();
    }
    
    /**
     * Add two complex numbers and store result
     * First number is cached, second is provided
     */
    int add_complex(double real2, double imag2) {
        try {
            ComplexNumber c2(real2, imag2);
            g_complex_cache = g_complex_cache.add(c2);
            return 0;
        } catch (...) {
            return 1;
        }
    }
    
    /**
     * Multiply two complex numbers and store result
     */
    int multiply_complex(double real2, double imag2) {
        try {
            ComplexNumber c2(real2, imag2);
            g_complex_cache = g_complex_cache.multiply(c2);
            return 0;
        } catch (...) {
            return 1;
        }
    }
    
    /**
     * Get the type name of current cached number
     */
    const char* get_number_type() {
        return g_complex_cache.getTypeName();
    }
    
    /**
     * Vector operations: push a double value
     */
    int vector_push_double(double value) {
        try {
            g_vector_cache.push(value);
            return 0;
        } catch (...) {
            return 1;
        }
    }
    
    /**
     * Vector operations: get size
     */
    int vector_size() {
        return g_vector_cache.size();
    }
    
    /**
     * Vector operations: get value at index
     */
    double vector_get_double(int index) {
        return g_vector_cache.get(index);
    }
    
    /**
     * Vector operations: clear
     */
    void vector_clear() {
        g_vector_cache.clear();
    }
    
    /**
     * Vector operations: compute sum of all values
     */
    double vector_sum() {
        double sum = 0.0;
        for (int i = 0; i < g_vector_cache.size(); ++i) {
            sum += g_vector_cache.get(i);
        }
        return sum;
    }
    
    /**
     * Vector operations: compute product of all values
     */
    double vector_product() {
        if (g_vector_cache.size() == 0) return 0.0;
        double prod = 1.0;
        for (int i = 0; i < g_vector_cache.size(); ++i) {
            prod *= g_vector_cache.get(i);
        }
        return prod;
    }
}

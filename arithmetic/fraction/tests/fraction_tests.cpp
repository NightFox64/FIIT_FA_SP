#include "../include/fraction.h"
#include "../include/big_int.h"
#include <cassert>
#include <iostream>
#include <sstream>
#include <limits>

void test_arithmetic() {
    std::cout << "Testing arithmetic operations...\n";
    
    fraction f1(1, 2);
    fraction f2(1, 3);
    
    assert((f1 + f2) == fraction(5, 6));
    assert((f1 + fraction(1, 1)) == fraction(3, 2));
    
    assert(f1 - f2 == fraction(1, 6));
    assert(f2 - f1 == fraction(-1, 6));
    
    assert(f1 * f2 == fraction(1, 6));
    assert(f1 * fraction(3, 1) == fraction(3, 2));
    
    assert(f1 / f2 == fraction(3, 2));
    assert(f2 / f1 == fraction(2, 3));
    
    try {
        fraction f3 = f1 / fraction(0, 1);
        assert(false && "Expected division by zero exception");
    } catch (const std::invalid_argument&) {}
    
    std::cout << "Arithmetic tests passed!\n\n";
}

void test_comparisons() {
    std::cout << "Testing comparison operations...\n";
    
    fraction f1(1, 2);
    fraction f2(1, 3);
    fraction f3(-1, 2);
    fraction f4(2, 4);
    
    assert(f1 > f2);
    assert(f2 < f1);
    assert(f1 == f4);
    assert(f1 != f2);
    assert(f3 < f1);
    assert(f1 >= f4);
    assert(f2 <= f1);
    
    std::cout << "Comparison tests passed!\n\n";
}

void test_io_operations() {
    std::cout << "Testing I/O operations...\n";
    
    fraction f1(3, 4);
    std::ostringstream oss;
    oss << f1;
    assert(oss.str() == "3/4");
    
    fraction f2;
    std::istringstream iss("5/8");
    iss >> f2;
    assert(f2 == fraction(5, 8));
    
    fraction f3;
    std::istringstream invalid_iss("3/0");
    try {
        invalid_iss >> f3;
    } catch (const std::invalid_argument&) {}
    
    std::cout << "I/O tests passed!\n\n";
}

void test_trigonometric() {
    std::cout << "Testing trigonometric functions...\n";
    
    fraction pi_4(785398, 1000000);
    fraction epsilon(1, 1000000);
    
    fraction sin_pi_4 = pi_4.sin(epsilon);
    assert(sin_pi_4 > fraction(7071, 10000) && sin_pi_4 < fraction(7072, 10000));
    
    fraction cos_pi_4 = pi_4.cos(epsilon);
    assert(cos_pi_4 > fraction(7071, 10000) && cos_pi_4 < fraction(7072, 10000));
    
    fraction tan_pi_4 = pi_4.tg(epsilon);
    assert(tan_pi_4 > fraction(9999, 10000) && tan_pi_4 < fraction(10001, 10000));
    
    std::cout << "Trigonometric tests passed!\n\n";
}

void test_exponential() {
    std::cout << "Testing exponential and logarithmic functions...\n";
    
    fraction epsilon(1, 1000000);
    
    fraction e(271828, 100000);
    fraction ln_e = e.ln(epsilon);
    assert(ln_e > fraction(899999, 1000000) && ln_e < fraction(1100001, 1000000));
    std::cout << "passed\n";
    
    fraction hundred(100, 1);
    fraction log10_100 = hundred.lg(epsilon);
    assert(log10_100 > fraction(1999999, 1000000) && log10_100 < fraction(2000001, 1000000));
    std::cout << "passed\n";
    
    std::cout << "Exponential tests passed!\n\n";
}

int main() {
    try {
        test_arithmetic();
        test_comparisons();
        test_io_operations();
        test_trigonometric();
        test_exponential();
        
        std::cout << "All fraction tests passed successfully!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << "\n";
        return 1;
    }
}
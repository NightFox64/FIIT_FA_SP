#include "../include/fraction.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <numeric>

void fraction::optimise()
{
    if (_denominator == big_int(0))
    {
        throw std::invalid_argument("Denominator cannot be zero");
    }

    big_int gcd_val = gcd(_numerator.abs(), _denominator.abs());
    _numerator /= gcd_val;
    _denominator /= gcd_val;

    if (_denominator < big_int(0))
    {
        _numerator = big_int(0) -_numerator;
        _denominator = big_int(0) - _denominator;
    }
}

big_int fraction::gcd(big_int a, big_int b) const
{
    if (a == 0)
        return b;
    if (b == 0)
        return a;

    a = a.abs();
    b = b.abs();

    while (b != 0)
    {
        big_int temp = b;
        b = a % b;
        a = temp;
    }

    return a;
}

template<std::convertible_to<big_int> f, std::convertible_to<big_int> s>
fraction::fraction(f &&numerator, s &&denominator)
    : _numerator(std::forward<f>(numerator)), _denominator(std::forward<s>(denominator))
{
    if (_denominator == big_int(0))
    {
        throw std::invalid_argument("Denominator cannot be zero");
    }
    optimise();
}

fraction::fraction(pp_allocator<big_int::value_type> alloc)
    : _numerator(0, alloc), _denominator(1, alloc)
{}

fraction fraction::operator-() const
{
    return fraction(big_int(0) - _numerator, _denominator);
}

fraction fraction::abs() const
{
    return fraction(_numerator.abs(), _denominator.abs());
}

fraction &fraction::operator+=(fraction const &other) &
{
    _numerator = _numerator * other._denominator + other._numerator * _denominator;
    _denominator *= other._denominator;
    optimise();
    return *this;
}

fraction fraction::operator+(fraction const &other) const
{
    fraction result(*this);
    return result += other;
}

fraction &fraction::operator-=(fraction const &other) &
{
    _numerator = _numerator * other._denominator - other._numerator * _denominator;
    _denominator *= other._denominator;
    optimise();
    return *this;
}

fraction fraction::operator-(fraction const &other) const
{
    fraction result(*this);
    return result -= other;
}

fraction &fraction::operator*=(fraction const &other) &
{
    _numerator *= other._numerator;
    _denominator *= other._denominator;
    optimise();
    return *this;
}

fraction fraction::operator*(fraction const &other) const
{
    fraction result(*this);
    return result *= other;
}

fraction &fraction::operator/=(fraction const &other) &
{
    if (other._numerator == big_int(0))
    {
        throw std::invalid_argument("Division by zero");
    }
    _numerator *= other._denominator;
    _denominator *= other._numerator;
    optimise();
    return *this;
}

fraction fraction::operator/(fraction const &other) const
{
    fraction result(*this);
    return result /= other;
}

bool fraction::operator==(fraction const &other) const noexcept
{
    return _numerator == other._numerator && _denominator == other._denominator;
}

std::partial_ordering fraction::operator<=>(const fraction& other) const noexcept
{
    big_int lhs = _numerator * other._denominator;
    big_int rhs = other._numerator * _denominator;
    return lhs <=> rhs;
}

std::ostream &operator<<(std::ostream &stream, fraction const &obj)
{
    stream << obj._numerator << "/" << obj._denominator.abs();
    return stream;
}

std::istream &operator>>(std::istream &stream, fraction &obj)
{
    std::string input;
    stream >> input;

    size_t slash_pos = input.find('/');
    if (slash_pos == std::string::npos) {
        try {
            big_int numerator(input);
            obj = fraction(numerator, 1);
            return stream;
        } catch (...) {
            stream.setstate(std::ios::failbit);
            return stream;
        }
    }

    std::string num_str = input.substr(0, slash_pos);
    std::string denom_str = input.substr(slash_pos + 1);

    try {
        big_int numerator(num_str);
        big_int denominator(denom_str);

        if (denominator == big_int(0)) {
            stream.setstate(std::ios::failbit);
            return stream;
        }

        obj = fraction(numerator, denominator);
    } catch (...) {
        stream.setstate(std::ios::failbit);
    }

    return stream;
}

std::string fraction::to_string() const
{
    return _numerator.to_string() + "/" + _denominator.abs().to_string();
}

fraction fraction::sin(fraction const &epsilon) const
{
    fraction term = *this;
    fraction sum = term;
    fraction prev_sum;
    big_int n(1);
    
    do {
        prev_sum = sum;
        term = (term * (*this) * (*this)) / fraction((n + big_int(1)) * (n + big_int(2)), 1);
        term = -term;
        sum += term;
        n += big_int(2);
    } while ((sum - prev_sum).abs() > epsilon);
    
    return sum;
}


fraction fraction::cos(fraction const &epsilon) const
{
    fraction term(1, 1);
    fraction sum = term;
    fraction prev_sum;
    big_int n(0);
    
    do {
        prev_sum = sum;
        term = (term * (*this) * (*this)) / fraction((n + big_int(1)) * (n + big_int(2)), 1);
        term = -term;
        sum += term;
        n += big_int(2);
    } while ((sum - prev_sum).abs() > epsilon);
    
    return sum;
}

fraction fraction::tg(fraction const &epsilon) const
{
    fraction cos_val = cos(epsilon);
    if (cos_val == fraction(0))
    {
        throw std::domain_error("Tangent is undefined for this angle");
    }
    return sin(epsilon) / cos_val;
}

fraction fraction::ctg(fraction const &epsilon) const
{
    fraction sin_val = sin(epsilon);
    if (sin_val == fraction(0))
    {
        throw std::domain_error("Cotangent is undefined for this angle");
    }
    return cos(epsilon) / sin_val;
}

fraction fraction::sec(fraction const &epsilon) const
{
    fraction cos_val = cos(epsilon);
    if (cos_val == fraction(0))
    {
        throw std::domain_error("Secant is undefined for this angle");
    }
    return fraction(1, 1) / cos_val;
}

fraction fraction::cosec(fraction const &epsilon) const
{
    fraction sin_val = sin(epsilon);
    if (sin_val == fraction(0))
    {
        throw std::domain_error("Cosecant is undefined for this angle");
    }
    return fraction(1, 1) / sin_val;
}

fraction fraction::pow(size_t degree) const
{
    if (degree == 0)
    {
        return fraction(1, 1);
    }
    
    fraction result(1, 1);
    fraction base(*this);
    
    while (degree > 0)
    {
        if (degree % 2 == 1)
        {
            result *= base;
        }
        base *= base;
        degree /= 2;
    }
    
    return result;
}

fraction fraction::root(size_t degree, fraction const &epsilon) const
{
    if (degree == 0)
    {
        throw std::invalid_argument("Zero root is undefined");
    }
    
    if (_numerator < big_int(0) && degree % 2 == 0)
    {
        throw std::domain_error("Even root of negative number");
    }
    
    fraction guess = (*this).abs();
    fraction prev_guess;
    fraction power;
    
    do
    {
        prev_guess = guess;
        power = guess.pow(degree - 1);
        guess = fraction(degree - 1, 1) * guess + (*this) / power;
        guess /= fraction(degree, 1);
    } while ((guess - prev_guess).abs() > epsilon);
    
    return guess;
}

fraction fraction::ln(fraction const &epsilon) const
{
    if (*this <= fraction(0, 1))
    {
        throw std::domain_error("Natural logarithm of non-positive number");
    }

    fraction x = *this;
    fraction result(0, 1);
    big_int power_shift(0);

    while (x > fraction(2, 1))
    {
        x /= fraction(2, 1);
        power_shift += big_int(1);
    }

    while (x < fraction(1, 2))
    {
        x *= fraction(2, 1);
        power_shift -= big_int(1);
    }

    fraction y = (x - fraction(1, 1)) / (x + fraction(1, 1));
    fraction y_squared = y * y;
    fraction term = y;
    fraction sum = term;
    big_int n(1);
    fraction prev_sum;
    int max_iterations = 1000;
    int iterations = 0;

    do {
        prev_sum = sum;
        term = (term * y_squared * fraction(big_int(2) * n - 1, 1)) / fraction(big_int(2) * n + 1, 1);
        sum += term;
        n += big_int(1);
        iterations++;
        
        if (iterations > 1 && (sum - prev_sum).abs() <= epsilon) {
            break;
        }
    } while (iterations < max_iterations && term.abs() > epsilon);

    fraction ln2 = ln_of_2(epsilon);
    
    return fraction(2, 1) * sum + fraction(power_shift, 1) * ln2;
}

fraction fraction::lg(fraction const &epsilon) const
{
    return ln(epsilon) / ln_of_10(epsilon);
}

fraction fraction::log2(fraction const &epsilon) const
{
    return ln(epsilon) / ln_of_2(epsilon);
}

fraction fraction::ln_of_2(fraction const &epsilon) const
{
    fraction sum(0);
    fraction term(1, 1);
    big_int n(1);
    int max_iterations = 10000;
    int iterations = 0;
    
    fraction x(1, 2);
    fraction x_power = x;
    
    do {
        term = x_power / fraction(n, 1);
        sum += term;
        x_power *= x;
        n += big_int(1);
        iterations++;
    } while (iterations < max_iterations && term.abs() > epsilon);
    
    return sum;
}

fraction fraction::ln_of_10(fraction const &epsilon) const
{
    fraction y = fraction(9, 1) / fraction(11, 1);
    fraction y_squared = y * y;
    fraction term = y;
    fraction sum = term;
    big_int n(1);

    do {
        term = (term * y_squared * fraction(big_int(2) * n - 1, 1)) / fraction(big_int(2) * n + 1, 1);
        sum += term;
        n += big_int(1);
    } while (term.abs() > epsilon);

    return fraction(2, 1) * sum;
}
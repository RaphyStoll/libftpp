// File: tests/test_stringutils_class.cpp
// C++98 - Tests unitaires pour libftpp::StringUtils
//
// Dépendances :
// - pad_line(std::string) (fourni par libftpp.hpp)
// - class libftpp::StringUtils (dans StringUtils.hpp)

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <limits>

#include "../include/libftpp.hpp"
#include "../include/StringUtils.hpp"

using libftpp::StringUtils;

// Prototypes des tests
static void test_1_trim_functions(int& ok, int& ko);
static void test_2_split_basic(int& ok, int& ko);
static void test_3_split_limits(int& ok, int& ko);
static void test_4_case_conversion(int& ok, int& ko);
static void test_5_iequals(int& ok, int& ko);
static void test_6_parse_numbers(int& ok, int& ko);

// Helpers
static void title(const std::string& t) {
    std::cout << pad_line(t) << "\n";
}

static void expect_(bool cond, const std::string& msg, int& ok, int& ko) {
    if (cond) {
        ++ok;
        std::cout << "  [OK] " << msg << "\n";
    } else {
        ++ko;
        std::cout << "  [KO] " << msg << "\n";
    }
}

template <typename T>
static std::string to_str(T val) {
    std::ostringstream oss;
    oss << val;
    return oss.str();
}

int main() {
    int ok = 0;
    int ko = 0;

    std::cout << pad_line("StringUtils tests (C++98)") << "\n\n";

    test_1_trim_functions(ok, ko);
    test_2_split_basic(ok, ko);
    test_3_split_limits(ok, ko);
    test_4_case_conversion(ok, ko);
    test_5_iequals(ok, ko);
    test_6_parse_numbers(ok, ko);

    std::cout << "\n" << pad_line("Résumé") << "\n";
    std::cout << "OK: " << ok << "\n";
    std::cout << "KO: " << ko << "\n\n";

    return (ko == 0) ? 0 : 1;
}

// ===================== Tests =====================

// 1) Trim (ltrim, rtrim, trim)
static void test_1_trim_functions(int& ok, int& ko) {
    title("1) Trim functions (ltrim, rtrim, trim)");

    // ltrim
    expect_(StringUtils::ltrim("   abc") == "abc", "ltrim('   abc') == 'abc'", ok, ko);
    expect_(StringUtils::ltrim("abc   ") == "abc   ", "ltrim('abc   ') == 'abc   '", ok, ko);
    expect_(StringUtils::ltrim("   ") == "", "ltrim('   ') == ''", ok, ko);
    expect_(StringUtils::ltrim("") == "", "ltrim('') == ''", ok, ko);

    // rtrim
    expect_(StringUtils::rtrim("abc   ") == "abc", "rtrim('abc   ') == 'abc'", ok, ko);
    expect_(StringUtils::rtrim("   abc") == "   abc", "rtrim('   abc') == '   abc'", ok, ko);
    expect_(StringUtils::rtrim("   ") == "", "rtrim('   ') == ''", ok, ko);
    expect_(StringUtils::rtrim("") == "", "rtrim('') == ''", ok, ko);

    // trim
    expect_(StringUtils::trim("   abc   ") == "abc", "trim('   abc   ') == 'abc'", ok, ko);
    expect_(StringUtils::trim("abc") == "abc", "trim('abc') == 'abc'", ok, ko);
    expect_(StringUtils::trim("   ") == "", "trim('   ') == ''", ok, ko);
    expect_(StringUtils::trim("\t\n abc \r") == "abc", "trim handles \\t\\n\\r", ok, ko);

    std::cout << "\n";
}

// 2) Split basic
static void test_2_split_basic(int& ok, int& ko) {
    title("2) Split basic");

    std::vector<std::string> v;

    // Normal case
    v = StringUtils::split("a,b,c", ',');
    expect_(v.size() == 3 && v[0] == "a" && v[1] == "b" && v[2] == "c", "split('a,b,c', ',')", ok, ko);

    // Empty tokens
    v = StringUtils::split(",,a,,", ',');
    // Expected: "", "", "a", "", "" -> 5 elements
    expect_(v.size() == 5, "split(',,a,,', ',') size == 5", ok, ko);
    expect_(v[0].empty() && v[1].empty() && v[2] == "a", "check empty tokens", ok, ko);

    // No delimiter
    v = StringUtils::split("hello", ',');
    expect_(v.size() == 1 && v[0] == "hello", "split('hello', ',') -> ['hello']", ok, ko);

    // Empty string
    v = StringUtils::split("", ',');
    expect_(v.size() == 1 && v[0].empty(), "split('', ',') -> ['']", ok, ko);

    std::cout << "\n";
}

// 3) Split limits
static void test_3_split_limits(int& ok, int& ko) {
    title("3) Split with limits");

    std::vector<std::string> v;

    // Limit = 2
    v = StringUtils::split("a,b,c,d", ',', 2);
    expect_(v.size() == 2, "split('a,b,c,d', ',', 2) size == 2", ok, ko);
    expect_(v[0] == "a" && v[1] == "b,c,d", "limit 2 -> ['a', 'b,c,d']", ok, ko);

    // Limit = 1
    v = StringUtils::split("a,b,c", ',', 1);
    expect_(v.size() == 1 && v[0] == "a,b,c", "limit 1 -> ['a,b,c']", ok, ko);

    // Limit > parts
    v = StringUtils::split("a,b", ',', 5);
    expect_(v.size() == 2, "limit 5 on 'a,b' -> size 2", ok, ko);

    std::cout << "\n";
}

// 4) Case conversion
static void test_4_case_conversion(int& ok, int& ko) {
    title("4) Case conversion (toLower, toUpper)");

    expect_(StringUtils::toLower("ABC") == "abc", "toLower('ABC') == 'abc'", ok, ko);
    expect_(StringUtils::toLower("AbC12") == "abc12", "toLower('AbC12') == 'abc12'", ok, ko);
    
    expect_(StringUtils::toUpper("abc") == "ABC", "toUpper('abc') == 'ABC'", ok, ko);
    expect_(StringUtils::toUpper("aBc12") == "ABC12", "toUpper('aBc12') == 'ABC12'", ok, ko);

    std::cout << "\n";
}

// 5) iequals
static void test_5_iequals(int& ok, int& ko) {
    title("5) iequals (case insensitive compare)");

    expect_(StringUtils::iequals("abc", "ABC"), "iequals('abc', 'ABC') -> true", ok, ko);
    expect_(StringUtils::iequals("AbC", "aBc"), "iequals('AbC', 'aBc') -> true", ok, ko);
    expect_(!StringUtils::iequals("abc", "abcd"), "iequals('abc', 'abcd') -> false", ok, ko);
    expect_(!StringUtils::iequals("abc", "abd"), "iequals('abc', 'abd') -> false", ok, ko);
    expect_(StringUtils::iequals("", ""), "iequals('', '') -> true", ok, ko);

    std::cout << "\n";
}

// 6) Parse numbers
static void test_6_parse_numbers(int& ok, int& ko) {
    title("6) Parse numbers (uint, size_t)");

    unsigned int u;
    size_t s;

    // Valid uint
    expect_(StringUtils::parse_uint("123", u) && u == 123, "parse_uint('123')", ok, ko);
    expect_(StringUtils::parse_uint("0", u) && u == 0, "parse_uint('0')", ok, ko);
    
    // Invalid uint
    expect_(!StringUtils::parse_uint("-1", u), "parse_uint('-1') -> false", ok, ko);
    expect_(!StringUtils::parse_uint("12a", u), "parse_uint('12a') -> false", ok, ko);
    expect_(!StringUtils::parse_uint("", u), "parse_uint('') -> false", ok, ko);

    // Overflow uint (assuming 32-bit uint usually, but let's test logic)
    // Max uint is usually 4294967295
    // Let's try a definitely too big number
    expect_(!StringUtils::parse_uint("99999999999999999999", u), "parse_uint(huge) -> false", ok, ko);

    // Valid size_t
    expect_(StringUtils::parse_size_t("1024", s) && s == 1024, "parse_size_t('1024')", ok, ko);
    
    std::cout << "\n";
}

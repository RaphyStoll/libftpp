// File: tests/test_httprequest_class.cpp
// C++98 - Tests unitaires pour libftpp::HttpRequest
//
// Dépendances :
// - pad_line(std::string) (fourni par libftpp.hpp)
// - class libftpp::HttpRequest (dans HttpRequest.hpp)

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

#include "../include/libftpp.hpp"
#include "../include/HttpRequest.hpp"

using libftpp::HttpRequest;

// Prototypes des tests
static void test_1_request_line(int& ok, int& ko);
static void test_2_headers_parsing(int& ok, int& ko);
static void test_3_headers_access(int& ok, int& ko);
static void test_4_content_length(int& ok, int& ko);
static void test_5_clear_reset(int& ok, int& ko);

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

int main() {
    int ok = 0;
    int ko = 0;

    std::cout << pad_line("HttpRequest tests (C++98)") << "\n\n";

    test_1_request_line(ok, ko);
    test_2_headers_parsing(ok, ko);
    test_3_headers_access(ok, ko);
    test_4_content_length(ok, ko);
    test_5_clear_reset(ok, ko);

    std::cout << "\n" << pad_line("Résumé") << "\n";
    std::cout << "OK: " << ok << "\n";
    std::cout << "KO: " << ko << "\n\n";

    return (ko == 0) ? 0 : 1;
}

// ===================== Tests =====================

// 1) Request Line Parsing
static void test_1_request_line(int& ok, int& ko) {
    title("1) Request Line Parsing");
    HttpRequest req;

    // Valid
    expect_(req.parseRequestLine("GET /index.html HTTP/1.1"), "Valid: GET /index.html HTTP/1.1", ok, ko);
    expect_(req.getMethod() == "GET", "Method is GET", ok, ko);
    expect_(req.getTarget() == "/index.html", "Target is /index.html", ok, ko);
    expect_(req.getVersion() == "HTTP/1.1", "Version is HTTP/1.1", ok, ko);

    // Valid with extra spaces
    expect_(req.parseRequestLine("  POST   /api/data   HTTP/1.0  "), "Valid with spaces: POST /api/data HTTP/1.0", ok, ko);
    expect_(req.getMethod() == "POST", "Method is POST", ok, ko);
    expect_(req.getTarget() == "/api/data", "Target is /api/data", ok, ko);
    expect_(req.getVersion() == "HTTP/1.0", "Version is HTTP/1.0", ok, ko);

    // Invalid formats
    expect_(!req.parseRequestLine("GET /"), "Invalid: missing version", ok, ko);
    expect_(!req.parseRequestLine("GET"), "Invalid: missing target and version", ok, ko);
    expect_(!req.parseRequestLine(""), "Invalid: empty string", ok, ko);
    expect_(!req.parseRequestLine("   "), "Invalid: spaces only", ok, ko);
    expect_(!req.parseRequestLine("GET / HTTP/1.1 EXTRA"), "Invalid: too many parts", ok, ko);
    expect_(!req.parseRequestLine("GET / NOTHTTP"), "Invalid: bad version prefix", ok, ko);

    std::cout << "\n";
}

// 2) Headers Parsing
static void test_2_headers_parsing(int& ok, int& ko) {
    title("2) Headers Parsing");
    HttpRequest req;

    // Valid
    expect_(req.parseHeaderLine("Host: example.com"), "Valid: Host: example.com", ok, ko);
    expect_(req.parseHeaderLine("User-Agent: MyClient/1.0"), "Valid: User-Agent", ok, ko);
    
    // Trimming
    expect_(req.parseHeaderLine("Accept:   text/html  "), "Valid with spaces: Accept", ok, ko);
    expect_(req.getHeader("Accept") == "text/html", "Value is trimmed", ok, ko);

    // Invalid
    expect_(!req.parseHeaderLine("InvalidHeader"), "Invalid: no colon", ok, ko);
    expect_(!req.parseHeaderLine(": value"), "Invalid: empty key", ok, ko);
    
    // Empty value is valid? Usually yes.
    expect_(req.parseHeaderLine("Empty-Val:"), "Valid: Empty-Val:", ok, ko);
    expect_(req.getHeader("Empty-Val") == "", "Value is empty string", ok, ko);

    std::cout << "\n";
}

// 3) Headers Access (Case Insensitivity)
static void test_3_headers_access(int& ok, int& ko) {
    title("3) Headers Access (Case Insensitivity)");
    HttpRequest req;

    req.parseHeaderLine("Content-Type: application/json");

    expect_(req.hasHeader("Content-Type"), "hasHeader(Content-Type) true", ok, ko);
    expect_(req.hasHeader("content-type"), "hasHeader(content-type) true", ok, ko);
    expect_(req.hasHeader("CONTENT-TYPE"), "hasHeader(CONTENT-TYPE) true", ok, ko);
    
    expect_(req.getHeader("Content-Type") == "application/json", "getHeader(Content-Type)", ok, ko);
    expect_(req.getHeader("content-type") == "application/json", "getHeader(content-type)", ok, ko);

    expect_(!req.hasHeader("Host"), "hasHeader(Host) false", ok, ko);
    expect_(req.getHeader("Host", "default") == "default", "getHeader(Host) returns default", ok, ko);

    std::cout << "\n";
}

// 4) Content-Length Helper
static void test_4_content_length(int& ok, int& ko) {
    title("4) Content-Length Helper");
    HttpRequest req;
    size_t len = 0;

    // Not present
    expect_(!req.contentLength(len), "contentLength() false when missing", ok, ko);

    // Valid
    req.parseHeaderLine("Content-Length: 1234");
    expect_(req.contentLength(len), "contentLength() true when present", ok, ko);
    expect_(len == 1234, "len == 1234", ok, ko);

    // Invalid (not a number)
    req.parseHeaderLine("Content-Length: 12a34"); // Overwrite
    expect_(!req.contentLength(len), "contentLength() false when invalid number", ok, ko);

    std::cout << "\n";
}

// 5) Clear / Reset
static void test_5_clear_reset(int& ok, int& ko) {
    title("5) Clear / Reset");
    HttpRequest req;

    req.parseRequestLine("GET / HTTP/1.1");
    req.parseHeaderLine("Host: localhost");

    expect_(!req.getMethod().empty(), "Before clear: method not empty", ok, ko);
    expect_(req.hasHeader("Host"), "Before clear: has Host", ok, ko);

    req.clear();

    expect_(req.getMethod().empty(), "After clear: method empty", ok, ko);
    expect_(req.getTarget().empty(), "After clear: target empty", ok, ko);
    expect_(req.getVersion().empty(), "After clear: version empty", ok, ko);
    expect_(!req.hasHeader("Host"), "After clear: no Host header", ok, ko);

    std::cout << "\n";
}

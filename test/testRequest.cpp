#include "../include/StringUtils.hpp"
#include "RequestParser.hpp"
#include <iostream>
#include <string>

static int g_passed = 0;
static int g_failed = 0;

void printRequest(http::Request &req) {
  std::cout << "  Method: " << req.getMethod() << std::endl;
  std::cout << "  Path: " << req.getPath() << std::endl;
  std::cout << "  Version: " << req.getHttpVersion() << std::endl;
  std::cout << "  Query: " << req.getQueryString() << std::endl;
  std::cout << "  Host: " << req.getHeader("Host") << std::endl;
  std::cout << "  Content-Type: " << req.getHeader("Content-Type") << std::endl;
  std::cout << "  Content-Length: " << req.getHeader("Content-Length")
            << std::endl;
  if (req.getBodySize() > 0) {
    std::cout << "  Body (" << req.getBodySize() << " bytes): " << req.getBody()
              << std::endl;
  }
  std::cout << std::endl;
}

void testPass(const std::string &name) {
  std::cout << "✓ " << name << std::endl;
  g_passed++;
}

void testFail(const std::string &name) {
  std::cout << "✗ " << name << std::endl;
  g_failed++;
}

// =============================================================================
// BASIC REQUEST PARSING TESTS
// =============================================================================

void testSimpleGet() {
  std::cout << "=== Test: Simple GET ===" << std::endl;
  http::RequestParser parser;
  std::string request = "GET /index.html HTTP/1.1\r\n"
                        "Host: www.example.com\r\n"
                        "User-Agent: Mozilla/5.0\r\n"
                        "\r\n";

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::COMPLETE) {
    testPass("Simple GET parsing complete");
    printRequest(parser.getRequest());
  } else {
    testFail("Simple GET parsing failed");
  }
}

void testSimpleGetHttp10() {
  std::cout << "=== Test: Simple GET HTTP/1.0 ===" << std::endl;
  http::RequestParser parser;
  std::string request = "GET /index.html HTTP/1.0\r\n"
                        "\r\n"; // Note: Host not required in HTTP/1.0

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::COMPLETE) {
    testPass("HTTP/1.0 GET without Host header accepted");
    printRequest(parser.getRequest());
  } else {
    testFail("HTTP/1.0 GET should not require Host header");
  }
}

void testHttp11RequiresHost() {
  std::cout << "=== Test: HTTP/1.1 requires Host header ===" << std::endl;
  http::RequestParser parser;
  std::string request = "GET /index.html HTTP/1.1\r\n"
                        "\r\n"; // Missing Host header!

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::ERROR && parser.getErrorCode() == 400) {
    testPass("HTTP/1.1 without Host correctly rejected (400)");
  } else {
    testFail("HTTP/1.1 without Host should return 400 error");
  }
}

// =============================================================================
// QUERY STRING PARSING TESTS
// =============================================================================

void testQueryStringParsing() {
  std::cout << "=== Test: Query string parsing ===" << std::endl;
  http::RequestParser parser;
  std::string request = "GET /search?q=hello&page=2&sort=asc HTTP/1.1\r\n"
                        "Host: example.com\r\n"
                        "\r\n";

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::COMPLETE) {
    http::Request &req = parser.getRequest();
    if (req.getPath() == "/search" &&
        req.getQueryString() == "q=hello&page=2&sort=asc") {
      testPass("Query string correctly parsed");
      printRequest(req);
    } else {
      testFail("Query string parsing incorrect");
      std::cout << "  Expected path: /search, got: " << req.getPath()
                << std::endl;
      std::cout << "  Expected query: q=hello&page=2&sort=asc, got: "
                << req.getQueryString() << std::endl;
    }
  } else {
    testFail("Query string request parsing failed");
  }
}

// =============================================================================
// BODY PARSING TESTS (Content-Length)
// =============================================================================

void testPostWithBody() {
  std::cout << "=== Test: POST with Content-Length body ===" << std::endl;
  http::RequestParser parser;
  std::string body = "{\"name\":\"John\",\"age\":30}";
  std::string request = "POST /api/users HTTP/1.1\r\n"
                        "Host: localhost:8080\r\n"
                        "Content-Type: application/json\r\n"
                        "Content-Length: " +
                        libftpp::str::StringUtils::itos(body.size()) +
                        "\r\n"
                        "\r\n" +
                        body;

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::COMPLETE) {
    http::Request &req = parser.getRequest();
    if (req.getBody() == body) {
      testPass("POST body correctly parsed");
      printRequest(req);
    } else {
      testFail("POST body content mismatch");
      std::cout << "  Expected: " << body << std::endl;
      std::cout << "  Got: " << req.getBody() << std::endl;
    }
  } else {
    testFail("POST with body parsing failed");
  }
}

void testPostBodyInChunks() {
  std::cout << "=== Test: POST body arriving in network chunks ==="
            << std::endl;
  http::RequestParser parser;
  std::string body = "This is the request body content";

  std::string headers = "POST /upload HTTP/1.1\r\n"
                        "Host: example.com\r\n"
                        "Content-Length: " +
                        libftpp::str::StringUtils::itos(body.size()) +
                        "\r\n"
                        "\r\n";

  // Send headers first
  http::RequestParser::State state =
      parser.parse(headers.c_str(), headers.size());
  if (state != http::RequestParser::PARSING_BODY_LENGTH) {
    testFail("Should be waiting for body after headers");
    return;
  }

  // Send body in two chunks
  std::string chunk1 = body.substr(0, 15);
  std::string chunk2 = body.substr(15);

  parser.parse(chunk1.c_str(), chunk1.size());
  state = parser.parse(chunk2.c_str(), chunk2.size());

  if (state == http::RequestParser::COMPLETE &&
      parser.getRequest().getBody() == body) {
    testPass("Chunked network body correctly assembled");
    printRequest(parser.getRequest());
  } else {
    testFail("Chunked network body parsing failed");
  }
}

void testPostWithoutContentLength() {
  std::cout << "=== Test: POST without Content-Length (should fail 411) ==="
            << std::endl;
  http::RequestParser parser;
  std::string request = "POST /api/data HTTP/1.1\r\n"
                        "Host: localhost\r\n"
                        "\r\n";

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::ERROR && parser.getErrorCode() == 411) {
    testPass(
        "POST without Content-Length correctly rejected (411 Length Required)");
  } else {
    testFail("POST without Content-Length should return 411");
    std::cout << "  Got error code: " << parser.getErrorCode() << std::endl;
  }
}

// =============================================================================
// CHUNKED TRANSFER ENCODING TESTS (HTTP/1.1 only)
// =============================================================================

void testChunkedEncoding() {
  std::cout << "=== Test: Chunked Transfer-Encoding (HTTP/1.1) ==="
            << std::endl;
  http::RequestParser parser;
  std::string request = "POST /upload HTTP/1.1\r\n"
                        "Host: example.com\r\n"
                        "Transfer-Encoding: chunked\r\n"
                        "\r\n"
                        "7\r\n"
                        "Mozilla\r\n"
                        "9\r\n"
                        "Developer\r\n"
                        "7\r\n"
                        "Network\r\n"
                        "0\r\n"
                        "\r\n";

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::COMPLETE) {
    http::Request &req = parser.getRequest();
    std::string expectedBody = "MozillaDeveloperNetwork";
    if (req.getBody() == expectedBody) {
      testPass("Chunked encoding correctly parsed");
      printRequest(req);
    } else {
      testFail("Chunked body content mismatch");
      std::cout << "  Expected: " << expectedBody << std::endl;
      std::cout << "  Got: " << req.getBody() << std::endl;
    }
  } else {
    testFail("Chunked encoding parsing failed");
    std::cout << "  Error code: " << parser.getErrorCode() << std::endl;
  }
}

void testChunkedEncodingHttp10Rejected() {
  std::cout << "=== Test: Chunked encoding rejected for HTTP/1.0 ==="
            << std::endl;
  http::RequestParser parser;
  std::string request = "POST /upload HTTP/1.0\r\n"
                        "Transfer-Encoding: chunked\r\n"
                        "\r\n"
                        "5\r\n"
                        "Hello\r\n"
                        "0\r\n"
                        "\r\n";

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::ERROR && parser.getErrorCode() == 400) {
    testPass("Chunked encoding correctly rejected for HTTP/1.0");
  } else {
    testFail("Chunked encoding should be rejected for HTTP/1.0");
  }
}

void testChunkedIncrementalParsing() {
  std::cout << "=== Test: Chunked encoding with incremental data ==="
            << std::endl;
  http::RequestParser parser;

  std::string part1 = "POST /data HTTP/1.1\r\n"
                      "Host: test.com\r\n"
                      "Transfer-Encoding: chunked\r\n"
                      "\r\n"
                      "5\r\n"
                      "Hel"; // Partial chunk

  std::string part2 = "lo\r\n"; // Rest of chunk

  std::string part3 = "6\r\n"
                      "World!\r\n"
                      "0\r\n"
                      "\r\n";

  parser.parse(part1.c_str(), part1.size());
  parser.parse(part2.c_str(), part2.size());
  http::RequestParser::State state = parser.parse(part3.c_str(), part3.size());

  if (state == http::RequestParser::COMPLETE) {
    std::string expectedBody = "HelloWorld!";
    if (parser.getRequest().getBody() == expectedBody) {
      testPass("Incremental chunked parsing correct");
      printRequest(parser.getRequest());
    } else {
      testFail("Incremental chunked body mismatch");
    }
  } else {
    testFail("Incremental chunked parsing failed");
  }
}

// =============================================================================
// INCREMENTAL PARSING TESTS
// =============================================================================

void testIncrementalParsing() {
  std::cout << "=== Test: Incremental header parsing ===" << std::endl;
  http::RequestParser parser;
  std::string chunk1 = "GET /test HTTP/1.1\r\n";
  std::string chunk2 = "Host: example.com\r\n";
  std::string chunk3 = "Connection: keep-alive\r\n\r\n";

  parser.parse(chunk1.c_str(), chunk1.size());
  parser.parse(chunk2.c_str(), chunk2.size());
  http::RequestParser::State state =
      parser.parse(chunk3.c_str(), chunk3.size());

  if (state == http::RequestParser::COMPLETE) {
    testPass("Incremental parsing complete");
    printRequest(parser.getRequest());
  } else {
    testFail("Incremental parsing failed");
  }
}

// =============================================================================
// ERROR HANDLING TESTS
// =============================================================================

void testInvalidHttpVersion() {
  std::cout << "=== Test: Invalid HTTP version ===" << std::endl;
  http::RequestParser parser;
  std::string request = "GET / HTTP/2.0\r\n\r\n";

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::ERROR) {
    testPass("Invalid HTTP version correctly rejected");
  } else {
    testFail("Should have rejected HTTP/2.0");
  }
}

void testMalformedRequestLine() {
  std::cout << "=== Test: Malformed request line ===" << std::endl;
  http::RequestParser parser;
  std::string request = "INVALID REQUEST\r\n\r\n";

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::ERROR) {
    testPass("Malformed request correctly rejected");
  } else {
    testFail("Should have rejected malformed request");
  }
}

void testMalformedHeader() {
  std::cout << "=== Test: Malformed header (no colon) ===" << std::endl;
  http::RequestParser parser;
  std::string request = "GET / HTTP/1.1\r\n"
                        "Host: example.com\r\n"
                        "InvalidHeaderNoColon\r\n"
                        "\r\n";

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::ERROR && parser.getErrorCode() == 400) {
    testPass("Malformed header correctly rejected");
  } else {
    testFail("Should have rejected header without colon");
  }
}

// =============================================================================
// KEEP-ALIVE TESTS
// =============================================================================

void testKeepAliveHttp11() {
  std::cout << "=== Test: HTTP/1.1 default keep-alive ===" << std::endl;
  http::RequestParser parser;
  std::string request = "GET / HTTP/1.1\r\n"
                        "Host: example.com\r\n"
                        "\r\n";

  parser.parse(request.c_str(), request.size());

  if (parser.getRequest().keepAlive()) {
    testPass("HTTP/1.1 defaults to keep-alive");
  } else {
    testFail("HTTP/1.1 should default to keep-alive");
  }
}

void testKeepAliveHttp10() {
  std::cout << "=== Test: HTTP/1.0 default close ===" << std::endl;
  http::RequestParser parser;
  std::string request = "GET / HTTP/1.0\r\n"
                        "\r\n";

  parser.parse(request.c_str(), request.size());

  if (!parser.getRequest().keepAlive()) {
    testPass("HTTP/1.0 defaults to close");
  } else {
    testFail("HTTP/1.0 should default to close");
  }
}

void testHttp10ExplicitKeepAlive() {
  std::cout << "=== Test: HTTP/1.0 explicit keep-alive ===" << std::endl;
  http::RequestParser parser;
  std::string request = "GET / HTTP/1.0\r\n"
                        "Connection: keep-alive\r\n"
                        "\r\n";

  parser.parse(request.c_str(), request.size());

  if (parser.getRequest().keepAlive()) {
    testPass("HTTP/1.0 with Connection: keep-alive works");
  } else {
    testFail("HTTP/1.0 with explicit keep-alive should keep connection");
  }
}

void testHttp11ExplicitClose() {
  std::cout << "=== Test: HTTP/1.1 explicit close ===" << std::endl;
  http::RequestParser parser;
  std::string request = "GET / HTTP/1.1\r\n"
                        "Host: example.com\r\n"
                        "Connection: close\r\n"
                        "\r\n";

  parser.parse(request.c_str(), request.size());

  if (!parser.getRequest().keepAlive()) {
    testPass("HTTP/1.1 with Connection: close works");
  } else {
    testFail("HTTP/1.1 with explicit close should close connection");
  }
}

// =============================================================================
// CASE-INSENSITIVE HEADER TESTS
// =============================================================================

void testCaseInsensitiveHeaders() {
  std::cout << "=== Test: Case-insensitive header names ===" << std::endl;
  http::RequestParser parser;
  std::string request = "GET / HTTP/1.1\r\n"
                        "HOST: example.com\r\n"
                        "content-type: text/html\r\n"
                        "Content-LENGTH: 0\r\n"
                        "\r\n";

  parser.parse(request.c_str(), request.size());
  http::Request &req = parser.getRequest();

  bool hostOk = (req.getHeader("host") == "example.com" &&
                 req.getHeader("Host") == "example.com" &&
                 req.getHeader("HOST") == "example.com");

  bool contentTypeOk = (req.getHeader("Content-Type") == "text/html");
  bool contentLengthOk = (req.getHeader("content-length") == "0");

  if (hostOk && contentTypeOk && contentLengthOk) {
    testPass("Headers are case-insensitive");
  } else {
    testFail("Headers should be case-insensitive");
  }
}

// =============================================================================
// MAIN
// =============================================================================

int main() {
  std::cout << "\n========================================" << std::endl;
  std::cout << "   HTTP REQUEST PARSER TEST SUITE" << std::endl;
  std::cout << "========================================\n" << std::endl;

  // Basic parsing
  testSimpleGet();
  testSimpleGetHttp10();
  testHttp11RequiresHost();

  // Query string
  testQueryStringParsing();

  // Body parsing (Content-Length)
  testPostWithBody();
  testPostBodyInChunks();
  testPostWithoutContentLength();

  // Chunked encoding (HTTP/1.1)
  testChunkedEncoding();
  testChunkedEncodingHttp10Rejected();
  testChunkedIncrementalParsing();

  // Incremental parsing
  testIncrementalParsing();

  // Error handling
  testInvalidHttpVersion();
  testMalformedRequestLine();
  testMalformedHeader();

  // Keep-alive behavior
  testKeepAliveHttp11();
  testKeepAliveHttp10();
  testHttp10ExplicitKeepAlive();
  testHttp11ExplicitClose();

  // Case-insensitive headers
  testCaseInsensitiveHeaders();

  // Summary
  std::cout << "\n========================================" << std::endl;
  std::cout << "   TEST RESULTS" << std::endl;
  std::cout << "========================================" << std::endl;
  std::cout << "Passed: " << g_passed << std::endl;
  std::cout << "Failed: " << g_failed << std::endl;
  std::cout << "Total:  " << (g_passed + g_failed) << std::endl;
  std::cout << "========================================\n" << std::endl;

  return (g_failed > 0) ? 1 : 0;
}

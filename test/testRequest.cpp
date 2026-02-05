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

void testSimpleDelete() {
  std::cout << "=== Test: Simple DELETE ===" << std::endl;
  http::RequestParser parser;
  std::string request = "DELETE /api/users/123 HTTP/1.1\r\n"
                        "Host: api.example.com\r\n"
                        "\r\n";

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::COMPLETE &&
      parser.getRequest().getMethod() == "DELETE") {
    testPass("DELETE method parsing complete");
    printRequest(parser.getRequest());
  } else {
    testFail("DELETE method parsing failed");
  }
}

void testHttp10Rejected() {
  std::cout << "=== Test: HTTP/1.0 rejected with 505 ===" << std::endl;
  http::RequestParser parser;
  std::string request = "GET /index.html HTTP/1.0\r\n"
                        "Host: example.com\r\n"
                        "\r\n";

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::ERROR && parser.getErrorCode() == 505) {
    testPass("HTTP/1.0 correctly rejected with 505 HTTP Version Not Supported");
  } else {
    testFail("HTTP/1.0 should be rejected with 505");
    std::cout << "  Got error code: " << parser.getErrorCode() << std::endl;
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
  std::string request = "GET / HTTP/2.0\r\n"
                        "Host: example.com\r\n"
                        "\r\n";

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::ERROR && parser.getErrorCode() == 505) {
    testPass("Invalid HTTP version correctly rejected (505)");
  } else {
    testFail("Should have rejected HTTP/2.0 with 505");
    std::cout << "  Got error code: " << parser.getErrorCode() << std::endl;
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

void testUnsupportedMethod() {
  std::cout << "=== Test: Unsupported method (PUT) ===" << std::endl;
  http::RequestParser parser;
  std::string request = "PUT /resource HTTP/1.1\r\n"
                        "Host: example.com\r\n"
                        "\r\n";

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::ERROR && parser.getErrorCode() == 501) {
    testPass("Unsupported method correctly rejected (501 Not Implemented)");
  } else {
    testFail("PUT method should be rejected with 501");
    std::cout << "  Got error code: " << parser.getErrorCode() << std::endl;
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
// PATH TRAVERSAL TESTS
// =============================================================================

void testPathTraversalMiddle() {
  std::cout << "=== Test: Path traversal /../ rejected ===" << std::endl;
  http::RequestParser parser;
  std::string request = "GET /foo/../etc/passwd HTTP/1.1\r\n"
                        "Host: example.com\r\n"
                        "\r\n";

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::ERROR && parser.getErrorCode() == 400) {
    testPass("Path traversal /../ correctly rejected");
  } else {
    testFail("Path traversal /../ should be rejected with 400");
    std::cout << "  Got error code: " << parser.getErrorCode() << std::endl;
  }
}

void testPathTraversalEnd() {
  std::cout << "=== Test: Path traversal /.. at end rejected ===" << std::endl;
  http::RequestParser parser;
  std::string request = "GET /foo/bar/.. HTTP/1.1\r\n"
                        "Host: example.com\r\n"
                        "\r\n";

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::ERROR && parser.getErrorCode() == 400) {
    testPass("Path traversal /.. at end correctly rejected");
  } else {
    testFail("Path traversal /.. at end should be rejected with 400");
    std::cout << "  Got error code: " << parser.getErrorCode() << std::endl;
  }
}

void testPathTraversalStart() {
  std::cout << "=== Test: Path traversal ../ at start rejected ===" << std::endl;
  http::RequestParser parser;
  std::string request = "GET ../etc/passwd HTTP/1.1\r\n"
                        "Host: example.com\r\n"
                        "\r\n";

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::ERROR && parser.getErrorCode() == 400) {
    testPass("Path traversal ../ at start correctly rejected");
  } else {
    testFail("Path traversal ../ at start should be rejected with 400");
    std::cout << "  Got error code: " << parser.getErrorCode() << std::endl;
  }
}

// =============================================================================
// CONTENT-LENGTH VALIDATION TESTS
// =============================================================================

void testMultipleContentLengthSameValue() {
  std::cout << "=== Test: Multiple Content-Length with same value ===" << std::endl;
  http::RequestParser parser;
  std::string request = "POST /api HTTP/1.1\r\n"
                        "Host: example.com\r\n"
                        "Content-Length: 5\r\n"
                        "Content-Length: 5\r\n"
                        "\r\n"
                        "hello";

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::COMPLETE &&
      parser.getRequest().getBody() == "hello") {
    testPass("Multiple identical Content-Length values accepted");
  } else {
    testFail("Multiple identical Content-Length values should be accepted");
    std::cout << "  Got error code: " << parser.getErrorCode() << std::endl;
  }
}

void testMultipleContentLengthDifferentValues() {
  std::cout << "=== Test: Multiple Content-Length with different values ===" << std::endl;
  http::RequestParser parser;
  std::string request = "POST /api HTTP/1.1\r\n"
                        "Host: example.com\r\n"
                        "Content-Length: 5\r\n"
                        "Content-Length: 10\r\n"
                        "\r\n"
                        "hello";

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::ERROR && parser.getErrorCode() == 400) {
    testPass("Conflicting Content-Length values correctly rejected");
  } else {
    testFail("Conflicting Content-Length values should be rejected with 400");
  }
}

void testContentLengthCommaSeparated() {
  std::cout << "=== Test: Content-Length with comma-separated identical values ===" << std::endl;
  http::RequestParser parser;
  std::string request = "POST /api HTTP/1.1\r\n"
                        "Host: example.com\r\n"
                        "Content-Length: 5, 5, 5\r\n"
                        "\r\n"
                        "hello";

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::COMPLETE &&
      parser.getRequest().getBody() == "hello") {
    testPass("Comma-separated identical Content-Length values accepted");
  } else {
    testFail("Comma-separated identical Content-Length should be accepted");
    std::cout << "  Got error code: " << parser.getErrorCode() << std::endl;
  }
}

void testContentLengthNonNumeric() {
  std::cout << "=== Test: Content-Length with non-numeric value ===" << std::endl;
  http::RequestParser parser;
  std::string request = "POST /api HTTP/1.1\r\n"
                        "Host: example.com\r\n"
                        "Content-Length: abc\r\n"
                        "\r\n";

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::ERROR && parser.getErrorCode() == 400) {
    testPass("Non-numeric Content-Length correctly rejected");
  } else {
    testFail("Non-numeric Content-Length should be rejected with 400");
  }
}

// =============================================================================
// TRANSFER-ENCODING TESTS
// =============================================================================

void testTransferEncodingAndContentLengthConflict() {
  std::cout << "=== Test: Transfer-Encoding + Content-Length conflict ===" << std::endl;
  http::RequestParser parser;
  std::string request = "POST /api HTTP/1.1\r\n"
                        "Host: example.com\r\n"
                        "Transfer-Encoding: chunked\r\n"
                        "Content-Length: 100\r\n"
                        "\r\n"
                        "5\r\n"
                        "hello\r\n"
                        "0\r\n"
                        "\r\n";

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::ERROR && parser.getErrorCode() == 400) {
    testPass("Transfer-Encoding + Content-Length conflict correctly rejected");
  } else {
    testFail("Transfer-Encoding + Content-Length should be rejected with 400");
    std::cout << "  Got error code: " << parser.getErrorCode() << std::endl;
  }
}

void testUnsupportedTransferEncoding() {
  std::cout << "=== Test: Unsupported Transfer-Encoding (gzip) ===" << std::endl;
  http::RequestParser parser;
  std::string request = "POST /api HTTP/1.1\r\n"
                        "Host: example.com\r\n"
                        "Transfer-Encoding: gzip\r\n"
                        "\r\n";

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::ERROR && parser.getErrorCode() == 501) {
    testPass("Unsupported Transfer-Encoding correctly rejected (501)");
  } else {
    testFail("Unsupported Transfer-Encoding should be rejected with 501");
    std::cout << "  Got error code: " << parser.getErrorCode() << std::endl;
  }
}

void testMultipleTransferEncodings() {
  std::cout << "=== Test: Multiple Transfer-Encodings rejected ===" << std::endl;
  http::RequestParser parser;
  std::string request = "POST /api HTTP/1.1\r\n"
                        "Host: example.com\r\n"
                        "Transfer-Encoding: gzip, chunked\r\n"
                        "\r\n";

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::ERROR && parser.getErrorCode() == 400) {
    testPass("Multiple Transfer-Encodings correctly rejected");
  } else {
    testFail("Multiple Transfer-Encodings should be rejected with 400");
    std::cout << "  Got error code: " << parser.getErrorCode() << std::endl;
  }
}

// =============================================================================
// HEADER VALIDATION TESTS
// =============================================================================

void testHeaderFoldingRejected() {
  std::cout << "=== Test: Obsolete header folding rejected ===" << std::endl;
  http::RequestParser parser;
  std::string request = "GET / HTTP/1.1\r\n"
                        "Host: example.com\r\n"
                        "X-Custom-Header: value\r\n"
                        " continued\r\n"
                        "\r\n";

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::ERROR && parser.getErrorCode() == 400) {
    testPass("Obsolete header folding correctly rejected");
  } else {
    testFail("Obsolete header folding should be rejected with 400");
  }
}

void testInvalidHeaderName() {
  std::cout << "=== Test: Invalid header name (contains space) ===" << std::endl;
  http::RequestParser parser;
  std::string request = "GET / HTTP/1.1\r\n"
                        "Host: example.com\r\n"
                        "Invalid Header: value\r\n"
                        "\r\n";

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::ERROR && parser.getErrorCode() == 400) {
    testPass("Invalid header name correctly rejected");
  } else {
    testFail("Header name with space should be rejected with 400");
  }
}

void testHostHeaderWithWhitespace() {
  std::cout << "=== Test: Host header with embedded whitespace rejected ===" << std::endl;
  http::RequestParser parser;
  std::string request = "GET / HTTP/1.1\r\n"
                        "Host: example .com\r\n"
                        "\r\n";

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::ERROR && parser.getErrorCode() == 400) {
    testPass("Host header with embedded whitespace correctly rejected");
  } else {
    testFail("Host with embedded whitespace should be rejected with 400");
    std::cout << "  Got error code: " << parser.getErrorCode() << std::endl;
  }
}

void testDuplicateHeadersCombined() {
  std::cout << "=== Test: Duplicate headers combined with comma ===" << std::endl;
  http::RequestParser parser;
  std::string request = "GET / HTTP/1.1\r\n"
                        "Host: example.com\r\n"
                        "Accept: text/html\r\n"
                        "Accept: application/json\r\n"
                        "\r\n";

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::COMPLETE) {
    std::string accept = parser.getRequest().getHeader("Accept");
    if (accept == "text/html, application/json") {
      testPass("Duplicate headers correctly combined");
    } else {
      testFail("Duplicate headers should be combined with comma");
      std::cout << "  Got Accept: " << accept << std::endl;
    }
  } else {
    testFail("Request with duplicate headers should be accepted");
  }
}

// =============================================================================
// RESET/KEEP-ALIVE TESTS
// =============================================================================

void testParserReset() {
  std::cout << "=== Test: Parser reset for keep-alive ===" << std::endl;
  http::RequestParser parser;

  // First request
  std::string request1 = "GET /first HTTP/1.1\r\n"
                         "Host: example.com\r\n"
                         "\r\n";
  parser.parse(request1.c_str(), request1.size());

  if (parser.getRequest().getPath() != "/first") {
    testFail("First request should have path /first");
    return;
  }

  // Reset and parse second request
  parser.reset();

  std::string request2 = "POST /second HTTP/1.1\r\n"
                         "Host: example.com\r\n"
                         "Content-Length: 4\r\n"
                         "\r\n"
                         "test";
  http::RequestParser::State state =
      parser.parse(request2.c_str(), request2.size());

  if (state == http::RequestParser::COMPLETE &&
      parser.getRequest().getPath() == "/second" &&
      parser.getRequest().getMethod() == "POST" &&
      parser.getRequest().getBody() == "test") {
    testPass("Parser reset works correctly for keep-alive");
  } else {
    testFail("Parser reset should allow parsing new request");
    std::cout << "  Path: " << parser.getRequest().getPath() << std::endl;
    std::cout << "  Method: " << parser.getRequest().getMethod() << std::endl;
  }
}

// =============================================================================
// URI VALIDATION TESTS
// =============================================================================

void testEmptyUri() {
  std::cout << "=== Test: Empty URI rejected ===" << std::endl;
  http::RequestParser parser;
  std::string request = "GET  HTTP/1.1\r\n"
                        "Host: example.com\r\n"
                        "\r\n";

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::ERROR) {
    testPass("Empty URI correctly rejected");
  } else {
    testFail("Empty URI should be rejected");
  }
}

void testAbsoluteFormUri() {
  std::cout << "=== Test: Absolute-form URI parsing ===" << std::endl;
  http::RequestParser parser;
  std::string request = "GET http://example.com/path/to/resource HTTP/1.1\r\n"
                        "Host: example.com\r\n"
                        "\r\n";

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::COMPLETE &&
      parser.getRequest().getPath() == "/path/to/resource") {
    testPass("Absolute-form URI correctly parsed to path");
  } else {
    testFail("Absolute-form URI should extract path");
    std::cout << "  Path: " << parser.getRequest().getPath() << std::endl;
  }
}

void testAbsoluteFormUriNoPath() {
  std::cout << "=== Test: Absolute-form URI with no path ===" << std::endl;
  http::RequestParser parser;
  std::string request = "GET http://example.com HTTP/1.1\r\n"
                        "Host: example.com\r\n"
                        "\r\n";

  http::RequestParser::State state =
      parser.parse(request.c_str(), request.size());

  if (state == http::RequestParser::COMPLETE &&
      parser.getRequest().getPath() == "/") {
    testPass("Absolute-form URI with no path defaults to /");
  } else {
    testFail("Absolute-form URI with no path should default to /");
    std::cout << "  Path: " << parser.getRequest().getPath() << std::endl;
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
  std::cout << "\n--- Basic Parsing Tests ---\n" << std::endl;
  testSimpleGet();
  testSimpleDelete();
  testHttp10Rejected();
  testHttp11RequiresHost();

  // Query string
  std::cout << "\n--- Query String Tests ---\n" << std::endl;
  testQueryStringParsing();

  // Body parsing (Content-Length)
  std::cout << "\n--- Body Parsing Tests (Content-Length) ---\n" << std::endl;
  testPostWithBody();
  testPostBodyInChunks();
  testPostWithoutContentLength();

  // Chunked encoding (HTTP/1.1)
  std::cout << "\n--- Chunked Encoding Tests ---\n" << std::endl;
  testChunkedEncoding();
  testChunkedIncrementalParsing();

  // Incremental parsing
  std::cout << "\n--- Incremental Parsing Tests ---\n" << std::endl;
  testIncrementalParsing();

  // Error handling
  std::cout << "\n--- Error Handling Tests ---\n" << std::endl;
  testInvalidHttpVersion();
  testMalformedRequestLine();
  testMalformedHeader();
  testUnsupportedMethod();

  // Keep-alive behavior
  std::cout << "\n--- Keep-Alive Tests ---\n" << std::endl;
  testKeepAliveHttp11();
  testHttp11ExplicitClose();
  testParserReset();

  // Case-insensitive headers
  std::cout << "\n--- Case-Insensitive Header Tests ---\n" << std::endl;
  testCaseInsensitiveHeaders();

  // Path traversal
  std::cout << "\n--- Path Traversal Tests ---\n" << std::endl;
  testPathTraversalMiddle();
  testPathTraversalEnd();
  testPathTraversalStart();

  // Content-Length validation
  std::cout << "\n--- Content-Length Validation Tests ---\n" << std::endl;
  testMultipleContentLengthSameValue();
  testMultipleContentLengthDifferentValues();
  testContentLengthCommaSeparated();
  testContentLengthNonNumeric();

  // Transfer-Encoding validation
  std::cout << "\n--- Transfer-Encoding Tests ---\n" << std::endl;
  testTransferEncodingAndContentLengthConflict();
  testUnsupportedTransferEncoding();
  testMultipleTransferEncodings();

  // Header validation
  std::cout << "\n--- Header Validation Tests ---\n" << std::endl;
  testHeaderFoldingRejected();
  testInvalidHeaderName();
  testHostHeaderWithWhitespace();
  testDuplicateHeadersCombined();

  // URI validation
  std::cout << "\n--- URI Validation Tests ---\n" << std::endl;
  testEmptyUri();
  testAbsoluteFormUri();
  testAbsoluteFormUriNoPath();

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

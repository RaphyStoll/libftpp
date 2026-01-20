#include "RequestParser.hpp"
#include <iostream>
#include <string>

void printRequest(const http::Request &req)
{
	std::cout << "Method: " << req.getMethod() << std::endl;
	std::cout << "Path: " << req.getPath() << std::endl;
	std::cout << "Host: " << req.getHeader("Host") << std::endl;
	std::cout << "Content-Type: " << req.getHeader("Content-Type") << std::endl;
	std::cout << "Content-Length: " << req.getHeader("Content-Length") << std::endl;
	std::cout << std::endl;
}

int main()
{
	// Test 1: Simple GET request
	std::cout << "=== Test 1: Simple GET ===" << std::endl;
	{
		http::RequestParser parser;
		std::string request =
			"GET /index.html HTTP/1.1\r\n"
			"Host: www.example.com\r\n"
			"User-Agent: Mozilla/5.0\r\n"
			"\r\n";

		http::RequestParser::State state = parser.parse(request.c_str(), request.size());

		if (state == http::RequestParser::COMPLETE)
		{
			std::cout << "✓ Parsing complete!" << std::endl;
			printRequest(parser.getRequest());
		}
		else if (state == http::RequestParser::ERROR)
		{
			std::cout << "✗ Parsing error!" << std::endl;
		}
	}

	// Test 2: POST request with multiple headers
	std::cout << "=== Test 2: POST with headers ===" << std::endl;
	{
		http::RequestParser parser;
		std::string request =
			"POST /api/data HTTP/1.1\r\n"
			"Host: localhost:8080\r\n"
			"Content-Type: application/json\r\n"
			"Content-Length: 27\r\n"
			"Accept: */*\r\n"
			"\r\n";

		http::RequestParser::State state = parser.parse(request.c_str(), request.size());

		if (state == http::RequestParser::COMPLETE)
		{
			std::cout << "✓ Parsing complete!" << std::endl;
			printRequest(parser.getRequest());
		}
		else
		{
			std::cout << "✗ Parsing error!" << std::endl;
		}
	}

	// Test 3: Incremental parsing (simulating network chunks)
	std::cout << "=== Test 3: Incremental parsing ===" << std::endl;
	{
		http::RequestParser parser;
		std::string chunk1 = "GET /test HTTP/1.1\r\n";
		std::string chunk2 = "Host: example.com\r\n";
		std::string chunk3 = "Connection: keep-alive\r\n\r\n";

		parser.parse(chunk1.c_str(), chunk1.size());
		parser.parse(chunk2.c_str(), chunk2.size());
		http::RequestParser::State state = parser.parse(chunk3.c_str(), chunk3.size());

		if (state == http::RequestParser::COMPLETE)
		{
			std::cout << "✓ Incremental parsing complete!" << std::endl;
			printRequest(parser.getRequest());
		}
		else
		{
			std::cout << "✗ Parsing incomplete or error!" << std::endl;
		}
	}

	// Test 4: Invalid HTTP version
	std::cout << "=== Test 4: Invalid version ===" << std::endl;
	{
		http::RequestParser parser;
		std::string request = "GET / HTTP/2.0\r\n\r\n";

		http::RequestParser::State state = parser.parse(request.c_str(), request.size());

		if (state == http::RequestParser::ERROR)
		{
			std::cout << "✓ Correctly rejected invalid version!" << std::endl;
		}
		else
		{
			std::cout << "✗ Should have rejected HTTP/2.0!" << std::endl;
		}
	}

	// Test 5: Malformed request line
	std::cout << "=== Test 5: Malformed request ===" << std::endl;
	{
		http::RequestParser parser;
		std::string request = "INVALID REQUEST\r\n\r\n";

		http::RequestParser::State state = parser.parse(request.c_str(), request.size());

		if (state == http::RequestParser::ERROR)
		{
			std::cout << "✓ Correctly rejected malformed request!" << std::endl;
		}
		else
		{
			std::cout << "✗ Should have rejected malformed request!" << std::endl;
		}
	}

	return 0;
}

Missing / Potential Issues

### Critical (Security/Correctness)

1. **Content-Length + Transfer-Encoding conflict**
    - If both are present, `Transfer-Encoding` takes precedence and `Content-Length` MUST be ignored (RFC 7230 §3.3.3)
    - Your code checks `Transfer-Encoding` first which is correct, but you should **remove/ignore** `Content-Length` to prevent request smuggling attacks

2. **Multiple/Invalid Content-Length values**
    - Multiple `Content-Length` headers with different values = 400 error
    - Your code doesn't check for duplicates

3. **Method validation**
    - You accept any method string. Consider validating against allowed methods (GET, POST, DELETE, etc.) and returning 405 or 501

4. **URI validation**
    - No validation of URI format (e.g., invalid characters, path traversal `/../`)
    - No length limit on URI specifically (414 URI Too Long)

5. **Header name validation**
    - Header names shouldn't contain spaces, colons (except the delimiter), or control characters
    - `Content -Length: 100` should be rejected

### Important

6. **Obsolete line folding (LWS)**
    - Headers can span multiple lines if continuation starts with space/tab:
        ```/dev/null/example.txt#L1-2
        Header: value
                 continued
        ```
    - RFC 7230 says to reject or replace with single space. You currently don't handle this.

7. **Transfer-Encoding values other than chunked**
    - If `Transfer-Encoding` contains anything other than `chunked` (e.g., `gzip, chunked`), you should handle or reject it
    - Only `chunked` must be last; unknown encodings = 501

8. **Case-insensitive header names**
    - `Content-Length` and `content-length` should be treated the same
    - Check if your `Request::getHeader()` handles this

9. **Duplicate headers**
    - Multiple headers with same name should be combined with comma (except `Set-Cookie`)
    - Does `setHeader()` overwrite or append?

10. **Empty header values**
    - `Header-Name:` (empty value) is technically valid

### Nice to Have

11. **Expect: 100-continue**
    - Client may send this header expecting a `100 Continue` response before sending body
    - Not handling it works but is inefficient for large uploads

12. **Connection header**
    - `Connection: close` means close after response
    - Useful for your event loop to know when to close

13. **Absolute-form URI**
    - Proxies receive `GET http://example.com/path HTTP/1.1`
    - If you're not a proxy, you can ignore this

14. **Trailer headers after chunked body**
    - You skip trailers but don't parse them. That's acceptable unless you need specific trailers.

15. **Max header count/size**
    - Limit number of headers to prevent DoS
    - Limit individual header line length

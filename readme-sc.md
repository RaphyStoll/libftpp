# RFC learning path

## Learn RFC
### Important RFCs
#### HTTP/1.0
- RFC 1945 - for context, no need to study hard
#### HTTP/1.1
- **7230 : Message Syntax & Routing**
- 7231 : Semantics & Content
- 7232 : Conditional Requests
- 7233 : Range Requests
- 7234 : Caching
- 7235 : Authentication

### Read RFC
RFCs are not tutorials - read them like specs:

1. Read the structure first
- Abstract
- Section titles
- ABNF grammar blocks

2. Focus on “MUST / SHOULD / MAY”

3. Ignore edge cases at first
- Obsolete features
- Rare headers
- Weird compatibility rules

4. Rewrite rules in your own words
- Example:
“Header field names are case-insensitive”
→ Convert header names to lowercase internally

## What to Understand for parsing
### HTTP request structure
*RFC 7230*
```
<method> <request-target> <HTTP-version>\r\n`
<header-field-name>: <field-value>\r\n
...
\r\n
[message-body]
```
I must understand:
- Line endings are CRLF (`\r\n`)
- Headers end at the empty line
- Request body length is defined by:
  - `Content-Length`
  - OR `Transfer-Encoding: chunked`

### Request line
*Example :*

`GET /index.html HTTP/1.1`

You must parse:
- Method (token)
- Target (path / query)
- Version (HTTP/1.0 or HTTP/1.1)

Validation rules:
- Spaces separate elements
- Exactly 3 components
- Unsupported methods → 405
- Invalid syntax → 400

### Headers
Key rules:
- Header names are case-insensitive
- Whitespace around : is allowed
- Duplicate headers may exist
- Order does not matter

Important headers for a basic server:
- Host **MANDATORY in HTTP/1.1**
- Content-Length
- Transfer-Encoding
- Connection

### Body parsing
1. If Transfer-Encoding: chunked → chunk parser
2. Else if Content-Length → read N bytes
3. Else → no body

## Help by nginx
### Read source
Focus
- ngx_http_parse_request_line
- ngx_http_parse_header_line

## Practical learing
### Learn by observing
- curl -v http://example.com
- Browser DevTools → Network tab
- nc or telnet to send raw requests

`printf "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n" | nc localhost 8080`

### use ABNF as guidance
*RFC 7230*
```
method = token
request-line = method SP request-target SP HTTP-version CRLF
```

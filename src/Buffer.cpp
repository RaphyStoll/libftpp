#include "../include/Buffer.hpp"

using libftpp::Buffer::Buffer;

void Buffer::compact_if_needed() {
	const size_t kMin = 8 * 1024; 
	
	if (_off == 0)
		return;

	if (_off >= kMin || _off * 2 >= _buf.size()) {
		_buf.erase(0, _off);
		_off = 0;
	}
}

Buffer::Buffer(size_t max_bytes) : _buf(), _off(0)
	,_max_bytes(max_bytes), _overflow(false) {}

Buffer::~Buffer() {}

Buffer::Buffer(const Buffer& other) : _buf(other._buf), _off(other._off)
	, _max_bytes(other._max_bytes), _overflow(other._overflow){}

Buffer& Buffer::operator=(const Buffer& other) {
	if (this != &other) {
		_buf = other._buf;
		_off = other._off;
		_max_bytes = other._max_bytes;
		_overflow = other._overflow;
	}
	return *this;
}

bool Buffer::empty() const {
	return size() == 0;
}

size_t Buffer::size() const {
	if (_off >= _buf.size())
		return 0;
	return _buf.size() - _off;
}

void Buffer::clear() {
	_buf.clear();
	_off = 0;
	_overflow = false;
}

// penser a size() > 0 avant de send(fd, buf.data(), buf.size(), 0);
// si pas fait risque de segfault si data return NULL
// fix possible return string vide "" (comme en C++ 17)
// mais en 17 "" est garanti null-termined et pas en 98
// deplus "" = std::string alors que la c'est un char*
const char* Buffer::data() const {
	if (size() == 0)
		return NULL;
	return &_buf[0] + _off;
}

void Buffer::setMax(size_t max_bytes) {
	_max_bytes = max_bytes;
	if (_max_bytes != 0 && size() > _max_bytes)
		_overflow = true;
}

size_t Buffer::max() const {
	return _max_bytes;
}

bool Buffer::overflow() const {
	return _overflow;
}

void Buffer::resetOverflow() {
	_overflow = false;
}

void Buffer::append(const char* p, size_t n) {
	if (!p || n == 0)
		return;

	if (_max_bytes != 0) {
	size_t cur = size();
		if (n > _max_bytes - cur) {
			_overflow = true;
			return;
		}
	}
	_buf.append(p, n);
}

void Buffer::append(const std::string& s) {
	if (s.empty())
		return;
	append(s.data(), s.size());
}

void Buffer::consume(size_t n) {
	if (n == 0)
		return;

	if (n >= size()) {
		clear();
		return;
	}

	_off += n;
	compact_if_needed();
}

bool Buffer::take(size_t n, std::string& out) {
	if (n > size())
		return false;

	if (n == 0) {
		out.clear();
		return true;
	}

	out.assign(data(), n);
	consume(n);
	return true;
}

bool Buffer::peek(size_t n, std::string& out) const {
	if (n > size())
		return false;

	if (n == 0) {
		out.clear();
		return true;
	}

	out.assign(data(), n);
	return true;
}

size_t Buffer::find(const std::string& needle) const {
	if (needle.empty())
		return 0;

	if (size() < needle.size())
		return std::string::npos;

	size_t pos = _buf.find(needle, _off);
	if (pos == std::string::npos)
		return std::string::npos;

	return pos - _off;
}

bool Buffer::readLineCRLF(std::string& out) {
	size_t pos = find("\r\n");
	if (pos == std::string::npos)
		return false;

	out.assign(data(), pos);
	consume(pos + 2);
	return true;
}

bool Buffer::readUntilCRLFCRLF(std::string& out) {
	size_t pos = find("\r\n\r\n");
	if (pos == std::string::npos)
		return false;

	out.assign(data(), pos);
	consume(pos + 4);
	return true;
}

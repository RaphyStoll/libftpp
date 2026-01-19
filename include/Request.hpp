#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <map>
#include <string>

namespace http {

class Request {
private:
	std::string _method;
	std::string _path;
  std::string _version;
	std::map<std::string, std::string> _headers;
	std::string _body;

public:
	Request();
	~Request();

	std::string getHeader(const std::string &name) const;
	std::string getMethod() const;
	std::string getPath() const;
	std::string getBody() const;

	void setMethod(const std::string &method);
	void setPath(const std::string &path);
	void setHeader(const std::string &name, const std::string &value);
	void setBody(const std::string &body);
};

} // namespace http

#endif

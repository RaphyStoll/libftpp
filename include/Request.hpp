#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <cstddef>
#include <map>
#include <string>

namespace webserv
{
	namespace http
	{
		class Request
		{
		private:
			std::string _method;
			std::string _path;
			std::string _queryString;
			std::string _httpVersion;
			std::map<std::string, std::string> _headers;
			std::string _body;

		public:
			Request();
			~Request();

			std::string getMethod() const;
			std::string getPath() const;
			std::string getQueryString() const;
			std::string getHttpVersion() const;
			std::string getHeader(const std::string &name) const;
			std::string getBody() const;

			void setMethod(const std::string &method);
			void setPath(const std::string &path);
			void setQueryString(const std::string &query);
			void setHttpVersion(const std::string &version);
			void setHeader(const std::string &name, const std::string &value);
			void setBody(const std::string &body);

			void appendBody(const std::string &data);
			size_t getBodySize() const;

			bool keepAlive() const
			{
				std::string connection = getHeader("Connection");
				if (_httpVersion == "HTTP/1.1")
					return (connection != "close");
				else
					return (connection == "keep-alive");
			}
		};
	} // namespace http
}
#endif

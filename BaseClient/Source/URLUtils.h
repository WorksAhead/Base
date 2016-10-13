#ifndef URLUTILS_HEADER_
#define URLUTILS_HEADER_

#include "KVMap.h"

#include <sstream>
#include <string>
#include <map>
#include <utility>

#include <stdlib.h>

inline void percentEncode(std::string& s)
{
	std::string ss;
	for (std::size_t i = 0; i < s.size(); ++i) {
		const char c = s[i];
		if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
			ss += c;
		}
		else if (c == '-' || c == '_' || c == '.' || c == '~') {
			ss += c;
		}
		else if (c == ' ') {
			ss += '+';
		}
		else {
			char buf[8];
			sprintf(buf, "%%%02X", (unsigned char)c);
			ss += buf;
		}
	}
	s.swap(ss);
}

inline bool percentDecode(std::string& s)
{
	std::string ss;
	for (std::size_t i = 0; i < s.size(); ++i) {
		if (s[i] == '%') {
			if (i + 3 <= s.size()) {
				int value;
				std::istringstream is(s.substr(i + 1, 2));
				if (is >> std::hex >> value) {
					ss += (char)value;
					i += 2;
				}
				else {
					return false;
				}
			}
			else {
				return false;
			}
		}
		else if (s[i] == '+') {
			ss += ' ';
		}
		else {
			ss += s[i];
		}
	}
	s.swap(ss);
	return true;
}

inline bool parseUrl(const std::string& url, std::string& path, KVMap& args)
{
	std::string str = url;
	std::string::size_type pos;
	if ((pos = str.find('?')) != std::string::npos) {
		std::string sub = str.substr(pos + 1);
		for (size_t i = 0; i < sub.size(); ++i) {
			if (sub[i] == '&') {
				sub[i] = ' ';
			}
		}
		std::istringstream iss(sub);
		std::string kv;
		while (iss >> kv) {
			std::string::size_type p = kv.find('=');
			if (p == std::string::npos) {
				return false;
			}
			std::string k = kv.substr(0, p);
			std::string v = kv.substr(p + 1);
			if (k.empty() || v.empty()) {
				continue;
			}
			if (!percentDecode(k) || !percentDecode(v)) {
				return false;
			}
			args[k] = v;
		}
		str.erase(pos);
	}
	path = str;
	return true;
}

class URLQuery {
public:
	URLQuery()
	{
	}

	URLQuery(const std::string& url) : url_(url)
	{
	}

	URLQuery& args(const KVMap& kvm)
	{
		for (auto it = kvm.begin(); it != kvm.end(); ++it) {
			arg(it->first, it->second);
		}
		return *this;
	}

	template<typename T>
	URLQuery& arg(const std::string& name, const T& v)
	{
		std::ostringstream oss;
		oss << v;
		std::string str = oss.str();
		percentEncode(str);
		m_[name] = str;
		return *this;
	}

	URLQuery& erase(const std::string& name)
	{
		m_.erase(name);
		return *this;
	}

	std::string str() const
	{
		std::string q;
		for (auto it = m_.begin(); it != m_.end(); ++it) {
			if (it != m_.begin()) {
				q += '&';
			}
			q += it->first;
			q += '=';
			q += it->second;
		}

		if (url_.size() && q.size()) {
			return url_ + "?" + q;
		}
		else if (url_.size()) {
			return url_;
		}
		else {
			return q;
		}
	}

private:
	std::string url_;
	std::map<std::string, std::string> m_;
};

inline std::ostream& operator<<(std::ostream& os, const URLQuery& q)
{
	os << q.str();
	return os;
}

#endif // URLUTILS_HEADER_


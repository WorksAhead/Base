#ifndef BASESERVER_PATHUTILS_HEADER_
#define BASESERVER_PATHUTILS_HEADER_

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

inline std::string normalizePath(const boost::filesystem::path& path)
{
#if WIN32
	std::string s = path.string();
	if (s.size() >= 260 && !boost::starts_with(s, R"(\\?\)")) {
		s = R"(\\?\)" + boost::filesystem::absolute(s).string();
	}
	boost::replace_all(s, "/", "\\");
	return s;
#else
	return path.string();
#endif
}

#endif // BASESERVER_PATHUTILS_HEADER_


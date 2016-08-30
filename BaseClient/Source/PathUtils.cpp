#include "PathUtils.h"

#include <boost/algorithm/string.hpp>

#include <assert.h>

#if WIN32
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#endif

std::string makeSafePath(const boost::filesystem::path& path)
{
#if WIN32
	std::string s = path.string();
	boost::replace_all(s, "/", "\\");
	if (path.is_absolute() && !boost::starts_with(s, R"(\\?\)")) {
		if (PathIsUNCA(s.c_str())) {
			assert(boost::starts_with(s, R"(\\)"));
			s = R"(\\?\UNC\)" + s.substr(2);
		}
		else {
			s = R"(\\?\)" + s;
		}
	}
	return s;
#else
	return path.string();
#endif
}


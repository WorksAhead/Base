#ifndef PATHUTILS_HEADER_
#define PATHUTILS_HEADER_

#include <boost/filesystem.hpp>

#include <string>

std::string makeSafePath(const boost::filesystem::path&);

#endif // PATHUTILS_HEADER_


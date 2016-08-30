#ifndef BASESERVER_PATHUTILS_HEADER_
#define BASESERVER_PATHUTILS_HEADER_

#include <boost/filesystem.hpp>

#include <string>

std::string makeSafePath(const boost::filesystem::path&);

#endif // BASESERVER_PATHUTILS_HEADER_


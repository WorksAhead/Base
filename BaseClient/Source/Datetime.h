#ifndef DATETIME_HEADER_
#define DATETIME_HEADER_

#include <string>
#include <time.h>

inline std::string timeToString(time_t t)
{
	char buffer[32];
	strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&t));
	return std::string(buffer);
}

inline std::string getCurrentTimeString()
{
	time_t t;
	time(&t);
	return timeToString(t);
}

#endif // DATETIME_HEADER_


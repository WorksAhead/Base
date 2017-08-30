#ifndef SCRIPT_HEADER_
#define SCRIPT_HEADER_

#include "Context.h"

#include <kaguya.hpp>

class Script {
public:
	Script();
	~Script();

	bool loadFromString(const std::string& code);
	bool loadFromServer(ContextPtr context, const char* uniformInfo);

	kaguya::State& state()
	{
		return state_;
	}

private:
	kaguya::State state_;
};

#endif // SCRIPT_HEADER_


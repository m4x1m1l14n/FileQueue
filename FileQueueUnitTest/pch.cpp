// pch.cpp: source file corresponding to the pre-compiled header

#include "pch.h"

// When you are using pre-compiled headers, this source file is necessary for compilation to succeed.
std::string gen_random(const size_t len)
{
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	std::string ret;

	ret.resize(len);

	for (size_t i = 0; i < len; ++i)
	{
		ret[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	return ret;
}

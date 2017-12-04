#pragma once

#include "../Exception.h"

class ExceptionNetwork : public Exception
{
public:
	ExceptionNetwork(const char* msg) : Exception(msg) {}
};
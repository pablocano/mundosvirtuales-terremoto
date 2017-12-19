#pragma once

#include "../Exception.h"

/// <summary>
/// Network Exception.
/// </summary>
class ExceptionNetwork : public Exception
{
public:
	ExceptionNetwork(const char* msg) : Exception(msg) {}
};
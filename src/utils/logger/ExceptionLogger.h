#pragma once

#include "../Exception.h"

/// <summary>
/// Logger Exception.
/// </summary>
class ExceptionLogger : public Exception
{
public:
	ExceptionLogger(const char* msg) : Exception(msg) {}
};
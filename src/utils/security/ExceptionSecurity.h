#pragma once

#include "../Exception.h"

/// <summary>
/// Security Exception
/// </summary>
class ExceptionSecurity : public Exception
{
public:
	ExceptionSecurity(const char* msg) : Exception(msg) {}
};
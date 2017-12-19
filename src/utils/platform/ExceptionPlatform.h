#pragma once

#include "../Exception.h"

/// <summary>
/// Plataform Exception.
/// </summary>
class ExceptionPlatform : public Exception
{
public:
	ExceptionPlatform(const char* msg) : Exception(msg) {}
};
#pragma once

#include "../utils/Exception.h"

/// <summary>
/// Server Exception.
/// </summary>
class ExceptionServer : public Exception
{
public:
	ExceptionServer(const char* msg) : Exception(msg) {}
};
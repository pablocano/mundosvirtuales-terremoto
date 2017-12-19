#pragma once

#include "../Exception.h"

/// <summary>
/// Concurrency Exception.
/// </summary>
class ExceptionConcurrency : public Exception
{
public:
	ExceptionConcurrency(const char* msg) : Exception(msg) {}
};
#pragma once

#include "../Exception.h"

/// <summary>
/// Communication Exception.
/// </summary>
class ExceptionCommunication : public Exception
{
public:
	ExceptionCommunication(const char* msg) : Exception(msg) {}
};
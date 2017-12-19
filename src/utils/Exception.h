#pragma once

#include <exception>
#include <string>

/// <summary>
/// Base class for handling exception.
/// </summary>
class Exception : public std::exception
{
private:
	
	std::string m_msg; /* Last message of error. */
	
public:
	/// <summary>
	/// Default Constructor.
	/// </summary>
	Exception() : m_msg() {}
	
	/// <summary>
	/// Constructor.
	/// </summary>
	/// <param name="msg">Message of error.</param>
	Exception(std::string msg) : m_msg(msg) {}

	/// <summary>
	/// Constructor.
	/// </summary>
	/// <param name="msg">Message of error.</param>
	Exception(const char* msg) : m_msg(msg) {}
	
	/// <summary>
	/// Getter of last error.
	/// </summary>
	/// <returns>Returns a string with the last error.</returns>
	std::string getMessage() const { return m_msg; }
};
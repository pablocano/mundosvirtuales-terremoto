#pragma once

#include <exception>
#include <string>

class Exception : public std::exception
{
private:
	
	std::string m_msg;
	
public:
	Exception() : m_msg() {}
	
	Exception(std::string msg) : m_msg(msg) {}

	Exception(const char* msg) : m_msg(msg) {}
	
	std::string getMessage() const { return m_msg; }
};
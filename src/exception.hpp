#pragma once
#include <exception>
#include <string>

namespace KeegMake {

class Token;

class BaseException : public std::exception {
public:
    BaseException(const std::string& error_msg)
        : m_error_msg(error_msg)
    {
    }

    const char* what() const throw() { return m_error_msg.c_str(); }
    const std::string m_error_msg;
};

class WrongLiteralType : public BaseException {
public:
    // TODO: Add real text description to this!
    WrongLiteralType(const std::string& type)
        : BaseException(type)
    {
    }

private:
    const std::string m_type;
};

class ParseError : public BaseException {
public:
    // TODO: Add real text description to this!
    ParseError(const std::string& type)
        : BaseException(type)
    {
    }

private:
    const std::string m_type;
};
} // namespace KeegMake

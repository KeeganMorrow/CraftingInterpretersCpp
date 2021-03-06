#pragma once
#include <exception>
#include <string>

namespace lox
{
class Token;

class BaseException : public std::exception
{
public:
    explicit BaseException(std::string error_msg) : m_error_msg(std::move(error_msg)) {}

    [[nodiscard]] const char* what() const noexcept override { return m_error_msg.c_str(); }

private:
    const std::string m_error_msg;
};

class WrongLiteralType : public BaseException
{
public:
    // TODO: Add real text description to this!
    explicit WrongLiteralType(const std::string& type) : BaseException(type) {}

private:
    const std::string m_type;
};

class ParseError : public BaseException
{
public:
    // TODO: Add real text description to this!
    explicit ParseError(const std::string& type) : BaseException(type) {}

private:
    const std::string m_type;
};
}  // namespace lox

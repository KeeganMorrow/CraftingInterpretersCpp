#include "literal.hpp"

namespace lox
{
const std::string& literalValTypeToStr(LiteralValType type)
{
    switch (type)
    {
    case LiteralValType::String:
        return LiteralValTypeStr_String;
    case LiteralValType::Bool:
        return LiteralValTypeStr_Bool;
    case LiteralValType::Number:
        return LiteralValTypeStr_Number;
    default:
        throw WrongLiteralType("?");
    }
}

[[nodiscard]] LiteralValType LiteralVal::type() const
{
    if (std::holds_alternative<std::string>(m_value))
    {
        return LiteralValType::String;
    }
    if (std::holds_alternative<bool>(m_value))
    {
        return LiteralValType::Bool;
    }
    if (std::holds_alternative<double>(m_value))
    {
        return LiteralValType::Number;
    }
    if (std::holds_alternative<NilLiteral>(m_value))
    {
        return LiteralValType::Nil;
    }
    spdlog::error("LiteralVal type() requested but is invalid, index is {}", m_value.index());
    // TODO: Use better exception
    throw(std::exception());
}

[[nodiscard]] std::string LiteralVal::repr() const
{
    if (const auto *pstr(std::get_if<std::string>(&m_value)); pstr)
    {
        return *pstr;
    }
    if (const auto *pdoub(std::get_if<double>(&m_value)); pdoub)
    {
        return std::to_string(*pdoub);
    }
    if (const auto *pbool(std::get_if<bool>(&m_value)); pbool)
    {
        if (*pbool)
        {
            return "true";
        }
        return "false";
    }
    if (const auto *pnil(std::get_if<NilLiteral>(&m_value)); pnil)
    {
        return "nil";
    }

    spdlog::error("LiteralVal repr() requested but is invalid, index is this is {:p}",
                  (void *)this);
    throw(std::exception());
}
}  // namespace lox

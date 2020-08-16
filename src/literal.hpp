#pragma once
#include <memory>
#include <utility>
#include <variant>

#include "exception.hpp"

namespace Lox
{
enum class LiteralValType
{
    String,
    Bool,
    Number,
    Nil
};
class LiteralVal;

class NilLiteral
{
public:
    bool operator==(const NilLiteral &other) const
    {
        (void)other;
        return true;
    }
    friend LiteralVal;

protected:
    NilLiteral() = default;
};

using LiteralVariant = std::variant<double, bool, std::string, NilLiteral>;

class LiteralVal
{
public:
    LiteralVal(std::string value) : m_value(value) {}
    LiteralVal(double value) : m_value(value) {}
    LiteralVal(bool value) : m_value(value) {}
    LiteralVal() : m_value(NilLiteral()) {}
    LiteralVal(const LiteralVal &) = default;

    bool operator==(const LiteralVal &other) const { return m_value == other.m_value; }

    bool operator!=(const LiteralVal &other) const { return m_value == other.m_value; }

    [[nodiscard]] LiteralValType type() const
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
        // TODO: Use better exception
        throw(std::exception());
    }

    [[nodiscard]] std::string repr() const
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
        throw(std::exception());
    }
    template <typename T>
    friend T getLiteral(LiteralVal &val);

protected:
    LiteralVariant m_value;
};

template <typename T>
T getLiteral(LiteralVal &val)
{
    return std::get<T>(val.m_value);
}

std::string literalRepresent(const LiteralVal &literal);

const std::string LiteralValTypeStr_String{"String"};
const std::string LiteralValTypeStr_Bool{"Bool"};
const std::string LiteralValTypeStr_Number{"Number"};

const std::string &LiteralValTypeToStr(LiteralVal &);

}  // namespace Lox

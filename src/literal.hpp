#pragma once
#include <spdlog/spdlog.h>

#include <memory>
#include <utility>
#include <variant>

#include "exception.hpp"

namespace lox
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

    NilLiteral() = default;
    NilLiteral(const NilLiteral &other) = default;
};

using LiteralVariant = std::variant<double, bool, std::string, NilLiteral>;

class LiteralVal
{
public:
    explicit LiteralVal(std::string value) : m_value(value) {}
    explicit LiteralVal(double value) : m_value(value) {}
    explicit LiteralVal(bool value) : m_value(value) {}
    LiteralVal() : m_value(NilLiteral()) {}
    LiteralVal(const LiteralVal &other) = default;
    LiteralVal(LiteralVal &&other) = default;

    bool operator==(const LiteralVal &other) const { return m_value == other.m_value; }

    bool operator!=(const LiteralVal &other) const { return m_value == other.m_value; }

    [[nodiscard]] LiteralValType type() const;

    [[nodiscard]] std::string repr() const;
    template <typename T>
    friend T getLiteral(const LiteralVal &val);

protected:
    // NOLINTNEXTLINE
    LiteralVariant m_value;
};

template <typename T>
T getLiteral(const LiteralVal &val)
{
    return std::get<T>(val.m_value);
}

std::string literalRepresent(const LiteralVal &literal);

const std::string LiteralValTypeStr_String{"String"};
const std::string LiteralValTypeStr_Bool{"Bool"};
const std::string LiteralValTypeStr_Number{"Number"};

const std::string &literalValTypeToStr(LiteralVal &);

}  // namespace lox

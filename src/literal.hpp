#pragma once
#include "exception.hpp"
#include <memory>
#include <utility>

namespace KeegMake {
enum class LiteralValType {
    String,
    Bool,
    Number,
    None
};

const std::string LiteralValTypeStr_String {"String"};
const std::string LiteralValTypeStr_Bool {"Bool"};
const std::string LiteralValTypeStr_Number {"Bool"};

const std::string& LiteralValTypeToStr(LiteralValType type);

class LiteralVal {
public:
    LiteralVal(LiteralValType type)
        : m_type(type)
    {
    }

    LiteralVal(const LiteralVal& other) = delete;
    virtual ~LiteralVal() = default;

    virtual std::unique_ptr<LiteralVal> clone() const = 0;

    [[nodiscard]] virtual std::string literal_str() const
    {
        throw WrongLiteralType(LiteralValTypeToStr(m_type));
    }
    [[nodiscard]] virtual bool literal_bool() const
    {
        throw WrongLiteralType(LiteralValTypeToStr(m_type));
    }
    [[nodiscard]] virtual double literal_num() const
    {
        throw WrongLiteralType(LiteralValTypeToStr(m_type));
    }

    [[nodiscard]] virtual std::string repr() const = 0;

protected:
    const LiteralValType m_type;
};

class BoolLiteralVal : public LiteralVal {
public:
    BoolLiteralVal(bool literal)
        : LiteralVal(LiteralValType::Bool)
        , m_literal(literal)
    {
    }

    virtual ~BoolLiteralVal() = default;

    std::unique_ptr<LiteralVal> clone() const override
    {
        return std::make_unique<BoolLiteralVal>(m_literal);
    }

    [[nodiscard]] std::string repr() const override
    {
        return (m_literal) ? "true" : "false";
    }

private:
    const bool m_literal;
};

class NumberLiteralVal : public LiteralVal {
public:
    NumberLiteralVal(double literal)
        : LiteralVal(LiteralValType::Number)
        , m_literal(literal)
    {
    }

    virtual ~NumberLiteralVal() = default;

    std::unique_ptr<LiteralVal> clone() const override
    {
        return std::make_unique<NumberLiteralVal>(m_literal);
    }

    [[nodiscard]] std::string repr() const override
    {
        return std::to_string(m_literal);
    }

private:
    const double m_literal;
};

class StringLiteralVal : public LiteralVal {
public:
    StringLiteralVal(const std::string& literal)
        : LiteralVal(LiteralValType::String)
        , m_literal(literal)
    {
    }

    virtual ~StringLiteralVal() = default;

    std::unique_ptr<LiteralVal> clone() const override
    {
        return std::make_unique<StringLiteralVal>(m_literal);
    }

    StringLiteralVal(const StringLiteralVal& other) = delete;

    [[nodiscard]] std::string repr() const override { return m_literal; }

private:
    const std::string m_literal;
};

class NoneLiteralVal : public LiteralVal {
public:
    NoneLiteralVal()
        : LiteralVal(LiteralValType::None)
    {
    }

    virtual ~NoneLiteralVal() = default;

    std::unique_ptr<LiteralVal> clone() const override
    {
        return std::make_unique<NoneLiteralVal>();
    }

    NoneLiteralVal(const NoneLiteralVal& other) = delete;

    [[nodiscard]] std::string repr() const override { return "None"; }
};

} // namespace KeegMake

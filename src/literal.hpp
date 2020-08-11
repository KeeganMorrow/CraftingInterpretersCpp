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

    LiteralVal(const LiteralVal& other) = delete;
    virtual ~LiteralVal() = default;

    [[nodiscard]]virtual std::unique_ptr<LiteralVal> clone() const = 0;
    [[nodiscard]]virtual LiteralValType type() const = 0;

    [[nodiscard]] virtual std::string literal_str() const
    {
        throw WrongLiteralType(LiteralValTypeToStr(type()));
    }
    [[nodiscard]] virtual bool literal_bool() const
    {
        throw WrongLiteralType(LiteralValTypeToStr(type()));
    }
    [[nodiscard]] virtual double literal_num() const
    {
        throw WrongLiteralType(LiteralValTypeToStr(type()));
    }

    [[nodiscard]] virtual std::string repr() const = 0;

protected:
    LiteralVal() = default;
};

class BoolLiteralVal : public LiteralVal {
public:
    BoolLiteralVal(bool literal) : m_literal(literal)
    {
    }

    ~BoolLiteralVal() override = default;

    [[nodiscard]]std::unique_ptr<LiteralVal> clone() const override
    {
        return std::make_unique<BoolLiteralVal>(m_literal);
    }

    [[nodiscard]] LiteralValType type() const override
    {
        return LiteralValType::Bool;
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
        : m_literal(literal)
    {
    }

    ~NumberLiteralVal() override = default;

    [[nodiscard]]std::unique_ptr<LiteralVal> clone() const override
    {
        return std::make_unique<NumberLiteralVal>(m_literal);
    }

    [[nodiscard]] LiteralValType type() const override
    {
        return LiteralValType::Number;
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
    StringLiteralVal(std::string literal)
        : m_literal(std::move(literal))
    {
    }

    ~StringLiteralVal() override = default;

    [[nodiscard]]std::unique_ptr<LiteralVal> clone() const override
    {
        return std::make_unique<StringLiteralVal>(m_literal);
    }

    [[nodiscard]] LiteralValType type() const override
    {
        return LiteralValType::String;
    }

    StringLiteralVal(const StringLiteralVal& other) = delete;

    [[nodiscard]] std::string repr() const override { return m_literal; }

private:
    const std::string m_literal;
};

class NoneLiteralVal : public LiteralVal {
public:
    NoneLiteralVal() = default;

    ~NoneLiteralVal() override = default;

    [[nodiscard]]std::unique_ptr<LiteralVal> clone() const override
    {
        return std::make_unique<NoneLiteralVal>();
    }

    [[nodiscard]] LiteralValType type() const override
    {
        return LiteralValType::None;
    }

    NoneLiteralVal(const NoneLiteralVal& other) = delete;

    [[nodiscard]] std::string repr() const override { return "None"; }
};

} // namespace KeegMake

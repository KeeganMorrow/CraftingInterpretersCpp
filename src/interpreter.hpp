#pragma once
#include "exception.hpp"
#include "expression_ast.hpp"
namespace KeegMake
{
class RuntimeError : BaseException
{
public:
    RuntimeError(const Token& token, std::string error_msg)
        : BaseException(std::move(error_msg)), m_token(token)
    {
    }

private:
    const std::string m_error_msg;
    const Token& m_token;
};

// TODO: Use string for now, need some kind of lox data object type
class Interpreter : public Expression::VisitorLiteralVal
{
public:
    [[nodiscard]] std::unique_ptr<LiteralVal> evaluate(const Expression& expression) const;

private:
    [[nodiscard]] std::unique_ptr<LiteralVal> visitBinary(const Binary& expression) const override;
    [[nodiscard]] std::unique_ptr<LiteralVal> visitGrouping(
        const Grouping& expression) const override;
    [[nodiscard]] std::unique_ptr<LiteralVal> visitLiteral(
        const Literal& expression) const override;
    [[nodiscard]] std::unique_ptr<LiteralVal> visitUnary(const Unary& expression) const override;

    [[nodiscard]] std::unique_ptr<LiteralVal> isTruthy(const LiteralVal& lval) const;

    void checkNumberOperand(const Token& token, const LiteralVal& operand);
};

}  // namespace KeegMake

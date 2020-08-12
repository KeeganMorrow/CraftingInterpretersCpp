#pragma once
#include "exception.hpp"
#include "expression_ast.hpp"
namespace KeegMake
{
class RuntimeError : public BaseException
{
public:
    RuntimeError(const Token& token, std::string error_msg)
        : BaseException(std::move(error_msg)), m_token(token)
    {
    }

    [[nodiscard]] const Token& token() const { return m_token; };

private:
    const std::string m_error_msg;
    const Token& m_token;
};

// TODO: Use string for now, need some kind of lox data object type
class Interpreter : public Expression::VisitorLiteralVal
{
public:
    [[nodiscard]] std::unique_ptr<LiteralVal> evaluate(
        const Expression::Expression& expression) const;

private:
    [[nodiscard]] std::unique_ptr<LiteralVal> visitBinary(
        const Expression::Binary& expression) const override;
    [[nodiscard]] std::unique_ptr<LiteralVal> visitGrouping(
        const Expression::Grouping& expression) const override;
    [[nodiscard]] std::unique_ptr<LiteralVal> visitLiteral(
        const Expression::Literal& expression) const override;
    [[nodiscard]] std::unique_ptr<LiteralVal> visitUnary(
        const Expression::Unary& expression) const override;

    [[nodiscard]] static std::unique_ptr<LiteralVal> isTruthy(const LiteralVal& lval);

    static void checkNumberOperand(const Token& token, const LiteralVal& operand);
    static void checkNumberOperands(const Token& token, const LiteralVal& left,
                                    const LiteralVal& right);
};

}  // namespace KeegMake

#pragma once
#include "exception.hpp"
#include "expression_ast.hpp"
#include "statement_ast.hpp"
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
class Interpreter : public ExpressionVisitorLiteralVal, public StatementVisitorVoid
{
public:
    Interpreter() = default;
    [[nodiscard]] std::unique_ptr<LiteralVal> evaluate(const Expression& expression) const;

    void interpret(std::vector<std::unique_ptr<const Statement>>&& program);

private:
    void execute(std::unique_ptr<const Statement>&& statement) { statement->accept(*this); }
    void visitStatementExpression(const StatementExpression& statement) const override;
    void visitStatementPrint(const StatementPrint& statement) const override;
    void visitStatementVariable(const StatementVariable& statement) const override;

    [[nodiscard]] std::unique_ptr<LiteralVal> visitExpressionBinary(
        const ExpressionBinary& expression) const override;
    [[nodiscard]] std::unique_ptr<LiteralVal> visitExpressionGrouping(
        const ExpressionGrouping& expression) const override;
    [[nodiscard]] std::unique_ptr<LiteralVal> visitExpressionLiteral(
        const ExpressionLiteral& expression) const override;
    [[nodiscard]] std::unique_ptr<LiteralVal> visitExpressionUnary(
        const ExpressionUnary& expression) const override;
    [[nodiscard]] std::unique_ptr<LiteralVal> visitExpressionVariable(
        const ExpressionVariable& expression) const override;

    [[nodiscard]] static std::unique_ptr<LiteralVal> isTruthy(const LiteralVal& lval);

    static void checkNumberOperand(const Token& token, const LiteralVal& operand);
    static void checkNumberOperands(const Token& token, const LiteralVal& left,
                                    const LiteralVal& right);
};

}  // namespace KeegMake

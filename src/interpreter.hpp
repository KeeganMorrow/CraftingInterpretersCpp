#pragma once
#include <utility>

#include "environment.hpp"
#include "exception.hpp"
#include "expression_ast.hpp"
#include "statement_ast.hpp"
namespace lox
{
class RuntimeError : public BaseException
{
public:
    RuntimeError(Token token, std::string error_msg)
        : BaseException(std::move(error_msg)), m_token(std::move(token))
    {
    }

    [[nodiscard]] const Token& token() const { return m_token; };

private:
    const std::string m_error_msg;
    const Token m_token;
};

// TODO: Use string for now, need some kind of lox data object type
class Interpreter : public ExpressionVisitorLiteralVal, public StatementVisitorVoid
{
public:
    Interpreter()
        : m_global_environment(std::make_unique<Environment>()),
          m_environment(m_global_environment.get())
    {
    }
    [[nodiscard]] std::unique_ptr<LiteralVal> evaluate(Expression* expression);

    void interpret(std::vector<std::unique_ptr<Statement>>&& program);

private:
    // TODO Why do we need to transfer ownership of the environment? Fix this
    void execute(std::unique_ptr<Statement>&& statement) { statement->accept(*this); }
    void executeBlock(std::vector<std::unique_ptr<Statement>>& statements,
                      Environment& environment);

    void visitStatementBlock(StatementBlock& statement) override;
    void visitStatementExpression(StatementExpression& statement) override;
    void visitStatementPrint(StatementPrint& statement) override;
    void visitStatementVariable(StatementVariable& statement) override;

    [[nodiscard]] std::unique_ptr<LiteralVal> visitExpressionAssign(
        ExpressionAssign& expression) override;
    [[nodiscard]] std::unique_ptr<LiteralVal> visitExpressionBinary(
        ExpressionBinary& expression) override;
    [[nodiscard]] std::unique_ptr<LiteralVal> visitExpressionGrouping(
        ExpressionGrouping& expression) override;
    [[nodiscard]] std::unique_ptr<LiteralVal> visitExpressionLiteral(
        ExpressionLiteral& expression) override;
    [[nodiscard]] std::unique_ptr<LiteralVal> visitExpressionUnary(
        ExpressionUnary& expression) override;
    [[nodiscard]] std::unique_ptr<LiteralVal> visitExpressionVariable(
        ExpressionVariable& expression) override;

    [[nodiscard]] static std::unique_ptr<LiteralVal> isTruthy(const LiteralVal& lval);

    static void checkNumberOperand(const Token& token, const LiteralVal& operand);
    static void checkNumberOperands(const Token& token, const LiteralVal& left,
                                    const LiteralVal& right);

    std::unique_ptr<Environment> m_global_environment;
    Environment* m_environment;
};

}  // namespace lox

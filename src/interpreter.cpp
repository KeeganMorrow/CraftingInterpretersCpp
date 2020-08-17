#include "interpreter.hpp"

#include <spdlog/spdlog.h>

#include <cassert>
namespace lox
{
std::unique_ptr<LiteralVal> Interpreter::evaluate(const Expression* expression)
{
    if (expression != nullptr)
    {
        return expression->accept(*this);
    }
    // TODO: I think this is the right thing to do, not sure though
    spdlog::error("Evaluating a nullptr expression, wtf?");
    return nullptr;
}

void Interpreter::interpret(std::vector<std::unique_ptr<const Statement>>&& program)
{
    try
    {
        for (auto& statement : program)
        {
            execute(std::move(statement));
        }
    }
    catch (RuntimeError& error)
    {
        spdlog::error(error.what());
        spdlog::error("Error found on line {} token {}", error.token().line(),
                      error.token().lexeme());
    }
}
void Interpreter::visitStatementExpression(const StatementExpression& statement)
{
    (void)evaluate(statement.expression());
}

void Interpreter::visitStatementPrint(const StatementPrint& statement)
{
    auto value = evaluate(statement.expression());
    spdlog::info(value->repr());
}

void Interpreter::visitStatementVariable(const StatementVariable& statement)
{
    std::unique_ptr<LiteralVal> value;
    assert(statement.name() != nullptr);
    if (statement.initializer() != nullptr)
    {
        value = evaluate(statement.initializer());
    }

    m_environment->define(statement.name()->lexeme(), std::move(value));
}

[[nodiscard]] std::unique_ptr<LiteralVal> Interpreter::visitExpressionAssign(
    const ExpressionAssign& expression)
{
    auto value = evaluate(expression.value());

    // Make a new copy of value here so that we can return the original
    m_environment->assign(*(expression.name()), std::make_unique<LiteralVal>(*value));
    return value;
}

std::unique_ptr<LiteralVal> Interpreter::visitExpressionBinary(const ExpressionBinary& expression)
{
    auto right = evaluate(expression.right());
    auto left = evaluate(expression.left());

    switch (expression.token()->type())
    {
    case TokenType::MINUS:
    {
        checkNumberOperands(*expression.token(), *left, *right);
        auto result = getLiteral<double>(*left) - getLiteral<double>(*right);
        return std::make_unique<LiteralVal>(result);
    }
    case TokenType::SLASH:
    {
        checkNumberOperands(*expression.token(), *left, *right);
        auto result = getLiteral<double>(*left) / getLiteral<double>(*right);
        return std::make_unique<LiteralVal>(result);
    }
    case TokenType::STAR:
    {
        checkNumberOperands(*expression.token(), *left, *right);
        auto result = getLiteral<double>(*left) * getLiteral<double>(*right);
        return std::make_unique<LiteralVal>(result);
    }
    case TokenType::PLUS:
    {
        if (left->type() == LiteralValType::Number && right->type() == LiteralValType::Number)
        {
            auto result = getLiteral<double>(*left) + getLiteral<double>(*right);
            return std::make_unique<LiteralVal>(result);
        }
        if (left->type() == LiteralValType::String && right->type() == LiteralValType::String)
        {
            auto result = getLiteral<std::string>(*left) + getLiteral<std::string>(*right);
            return std::make_unique<LiteralVal>(result);
        }
        throw(RuntimeError(*expression.token(), "Operands must be two numbers or two strings."));
    }
    case TokenType::GREATER:
    {
        checkNumberOperands(*expression.token(), *left, *right);
        bool result = (getLiteral<double>(*left) > getLiteral<double>(*right));
        return std::make_unique<LiteralVal>(result);
    }
    case TokenType::GREATER_EQUAL:
    {
        checkNumberOperands(*expression.token(), *left, *right);
        bool result = (getLiteral<double>(*left) >= getLiteral<double>(*right));
        return std::make_unique<LiteralVal>(result);
    }
    case TokenType::LESS:
    {
        checkNumberOperands(*expression.token(), *left, *right);
        bool result = (getLiteral<double>(*left) < getLiteral<double>(*right));
        return std::make_unique<LiteralVal>(result);
    }
    case TokenType::LESS_EQUAL:
    {
        checkNumberOperands(*expression.token(), *left, *right);
        bool result = (getLiteral<double>(*left) <= getLiteral<double>(*right));
        return std::make_unique<LiteralVal>(result);
    }
    case TokenType::BANG_EQUAL:
    {
        return std::make_unique<LiteralVal>(*left == *right);
    }
    case TokenType::EQUAL_EQUAL:
        return std::make_unique<LiteralVal>(*left != *right);
    default:
        break;
    }
    return nullptr;
}

std::unique_ptr<LiteralVal> Interpreter::visitExpressionGrouping(
    const ExpressionGrouping& expression)
{
    return evaluate(expression.expression());
}

std::unique_ptr<LiteralVal> Interpreter::visitExpressionLiteral(const ExpressionLiteral& expression)
{
    // TODO : Check against nullptr. Not sure what to do if we see one at the moment
    return std::make_unique<LiteralVal>(*expression.value());
}

std::unique_ptr<LiteralVal> Interpreter::visitExpressionUnary(const ExpressionUnary& expression)
{
    auto right = evaluate(expression.right());

    switch (expression.token()->type())
    {
    case TokenType::MINUS:
        checkNumberOperand(*expression.token(), *right);
        return std::make_unique<LiteralVal>(-(getLiteral<double>(*right)));
    case TokenType::BANG:
        return isTruthy(*right);
    default:
        break;
    }

    return nullptr;
}

std::unique_ptr<LiteralVal> Interpreter::visitExpressionVariable(
    const ExpressionVariable& expression)
{
    const auto& varname = expression.name();
    spdlog::debug("Reading variable {}", varname->lexeme());
    auto val = m_environment->get(*varname);
    return std::make_unique<LiteralVal>(val);
}
std::unique_ptr<LiteralVal> Interpreter::isTruthy(const LiteralVal& lval)
{
    bool result = true;
    if (lval.type() == LiteralValType::Nil)
    {
        result = false;
    }
    return std::make_unique<LiteralVal>(result);
}

void Interpreter::checkNumberOperand(const Token& token, const LiteralVal& operand)
{
    if (operand.type() == LiteralValType::Number)
    {
        return;
    }
    throw RuntimeError(token, "Operand must be a number.");
}

void Interpreter::checkNumberOperands(const Token& token, const LiteralVal& left,
                                      const LiteralVal& right)
{
    if (left.type() == LiteralValType::Number && right.type() == LiteralValType::Number)
    {
        return;
    }
    throw RuntimeError(token, "Operands must be a number.");
}

}  // namespace lox

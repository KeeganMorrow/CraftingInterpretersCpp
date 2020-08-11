#include "interpreter.hpp"

#include <spdlog/spdlog.h>
namespace KeegMake
{
std::unique_ptr<LiteralVal> Interpreter::evaluate(const Expression& expression) const
{
    (void)expression;

    return expression.accept(*this);
}

std::unique_ptr<LiteralVal> Interpreter::visitBinary(const Binary& expression) const
{
    auto right = evaluate(expression.right());
    auto left = evaluate(expression.left());

    switch (expression.token().type())
    {
    case TokenType::MINUS:
    {
        auto result = getLiteral<double>(*left) - getLiteral<double>(*right);
        return std::make_unique<LiteralVal>(result);
    }
    case TokenType::SLASH:
    {
        auto result = getLiteral<double>(*left) / getLiteral<double>(*right);
        return std::make_unique<LiteralVal>(result);
    }
    case TokenType::STAR:
    {
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
        break;
    }
    case TokenType::GREATER:
    {
        bool result = (getLiteral<double>(*left) > getLiteral<double>(*right));
        return std::make_unique<LiteralVal>(result);
    }
    case TokenType::GREATER_EQUAL:
    {
        bool result = (getLiteral<double>(*left) >= getLiteral<double>(*right));
        return std::make_unique<LiteralVal>(result);
    }
    case TokenType::LESS:
    {
        bool result = (getLiteral<double>(*left) < getLiteral<double>(*right));
        return std::make_unique<LiteralVal>(result);
    }
    case TokenType::LESS_EQUAL:
    {
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

std::unique_ptr<LiteralVal> Interpreter::visitGrouping(const Grouping& expression) const
{
    return evaluate(expression.expression());
}

std::unique_ptr<LiteralVal> Interpreter::visitLiteral(const Literal& expression) const
{
    // TODO : Check against nullptr. Not sure what to do if we see one at the moment
    return std::make_unique<LiteralVal>(expression.value());
}

std::unique_ptr<LiteralVal> Interpreter::visitUnary(const Unary& expression) const
{
    auto right = evaluate(expression.right());

    switch (expression.token().type())
    {
    case TokenType::MINUS:
        return std::make_unique<LiteralVal>(-(getLiteral<double>(*right)));
    case TokenType::BANG:
        return isTruthy(*right);
    default:
        break;
    }

    return nullptr;
}
[[nodiscard]] std::unique_ptr<LiteralVal> Interpreter::isTruthy(const LiteralVal& lval) const
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

}  // namespace KeegMake

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
        double result = left->literal_num() - right->literal_num();
        return std::make_unique<NumberLiteralVal>(result);
    }
    case TokenType::SLASH:
    {
        double result = left->literal_num() / right->literal_num();
        return std::make_unique<NumberLiteralVal>(result);
    }
    case TokenType::STAR:
    {
        double result = left->literal_num() * right->literal_num();
        return std::make_unique<NumberLiteralVal>(result);
    }
    case TokenType::PLUS:
    {
        if (left->type() == LiteralValType::Number && right->type() == LiteralValType::Number)
        {
            double result = left->literal_num() + right->literal_num();
            return std::make_unique<NumberLiteralVal>(result);
        }
        if (left->type() == LiteralValType::String && right->type() == LiteralValType::String)
        {
            auto result_str = left->literal_str() + right->literal_str();
            return std::make_unique<StringLiteralVal>(result_str);
        }
        break;
    }
    case TokenType::GREATER:
    {
        bool result = (left->literal_num() > right->literal_num());
        return std::make_unique<BoolLiteralVal>(result);
    }
    case TokenType::GREATER_EQUAL:
    {
        bool result = (left->literal_num() >= right->literal_num());
        return std::make_unique<BoolLiteralVal>(result);
    }
    case TokenType::LESS:
    {
        bool result = (left->literal_num() < right->literal_num());
        return std::make_unique<BoolLiteralVal>(result);
    }
    case TokenType::LESS_EQUAL:
    {
        bool result = (left->literal_num() <= right->literal_num());
        return std::make_unique<BoolLiteralVal>(result);
    }
    case TokenType::BANG_EQUAL:
        // TODO: Add Not equal
    case TokenType::EQUAL_EQUAL:
        return isEqual(*left, *right);
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
    return expression.value().clone();
}

std::unique_ptr<LiteralVal> Interpreter::visitUnary(const Unary& expression) const
{
    auto right = evaluate(expression.right());

    switch (expression.token().type())
    {
    case TokenType::MINUS:
        return std::make_unique<NumberLiteralVal>(-(right->literal_num()));
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
    if (lval.type() == LiteralValType::None)
    {
        result = false;
    }
    return std::make_unique<BoolLiteralVal>(result);
}

[[nodiscard]] std::unique_ptr<LiteralVal> Interpreter::isEqual(const LiteralVal& a,
                                                               const LiteralVal& b) const
{
    bool result;
    if (a.type() == LiteralValType::None)
    {
        if (b.type() == LiteralValType::None)
        {
            result = true;
        }
        else
        {
            result = false;
        }
        return std::make_unique<BoolLiteralVal>(result);
    }
// TODO: Implement other types for equality. It is a bit trickier
    return nullptr;
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

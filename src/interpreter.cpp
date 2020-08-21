#include "interpreter.hpp"

#include <spdlog/spdlog.h>

#include <cassert>
namespace lox
{
std::unique_ptr<LiteralVal> Interpreter::evaluate(Expression* expression)
{
    if (expression != nullptr)
    {
        return expression->accept(*this);
    }
    // TODO: I think this is the right thing to do, not sure though
    spdlog::error("Evaluating a nullptr expression, wtf?");
    return nullptr;
}

void Interpreter::interpret(std::vector<std::unique_ptr<Statement>>&& program)
{
    try
    {
        for (auto& statement : program)
        {
            if (statement != nullptr)
            {
                execute(*statement);
            }
            else
            {
                spdlog::error("Null statement found in program");
            }
        }
    }
    catch (RuntimeError& error)
    {
        spdlog::error(error.what());
        spdlog::error("Error found on line {} token {}", error.token().line(),
                      error.token().lexeme());
    }
}

void Interpreter::executeBlock(std::vector<std::unique_ptr<Statement>>& statements,
                               Environment& environment)
{
    auto* previous_env = m_environment;
    try
    {
        m_environment = &environment;

        for (auto& statement : statements)
        {
            if (statement != nullptr)
            {
                execute(*statement);
            }
            else
            {
                spdlog::error("Null statement found in block");
            }
        }

        m_environment = previous_env;
    }
    catch (RuntimeError& error)
    {
        // Even if exception occurs we need to restore the old env
        m_environment = previous_env;
    }
}

void Interpreter::visitStatementBlock(StatementBlock& statement)
{
    auto* statements = statement.getStatements();
    if (statements != nullptr)
    {
        Environment env(m_environment);
        executeBlock(*statements, env);
    }
}

void Interpreter::visitStatementExpression(StatementExpression& statement)
{
    (void)evaluate(statement.getExpression());
}

void Interpreter::visitStatementIf(StatementIf& statement)
{
    auto result = evaluate(statement.getCondition());
    if (result != nullptr && isTruthy(*result))
    {
        auto* thenbranch = statement.getthenBranch();
        if (thenbranch != nullptr)
        {
            execute(*thenbranch);
        }
        else
        {
            spdlog::error("If statement found with null then branch");
        }
    }
    else if (statement.getelseBranch() != nullptr)
    {
        execute(*statement.getelseBranch());
    }
}

void Interpreter::visitStatementPrint(StatementPrint& statement)
{
    auto value = evaluate(statement.getExpression());
    spdlog::info(value->repr());
}

void Interpreter::visitStatementVariable(StatementVariable& statement)
{
    std::unique_ptr<LiteralVal> value;
    assert(statement.getName() != nullptr);
    if (statement.getInitializer() != nullptr)
    {
        value = evaluate(statement.getInitializer());
    }

    m_environment->define(statement.getName().lexeme(), std::move(value));
}

[[nodiscard]] std::unique_ptr<LiteralVal> Interpreter::visitExpressionAssign(
    ExpressionAssign& expression)
{
    auto value = evaluate(expression.getValue());

    // Make a new copy of value here so that we can return the original
    m_environment->assign(expression.getName(), std::make_unique<LiteralVal>(*value));
    return value;
}

std::unique_ptr<LiteralVal> Interpreter::visitExpressionBinary(ExpressionBinary& expression)
{
    auto right = evaluate(expression.getRight());
    auto left = evaluate(expression.getLeft());

    switch (expression.getToken().type())
    {
    case TokenType::MINUS:
    {
        checkNumberOperands(expression.getToken(), *left, *right);
        auto result = getLiteral<double>(*left) - getLiteral<double>(*right);
        return std::make_unique<LiteralVal>(result);
    }
    case TokenType::SLASH:
    {
        checkNumberOperands(expression.getToken(), *left, *right);
        auto result = getLiteral<double>(*left) / getLiteral<double>(*right);
        return std::make_unique<LiteralVal>(result);
    }
    case TokenType::STAR:
    {
        checkNumberOperands(expression.getToken(), *left, *right);
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
        throw(RuntimeError(expression.getToken(), "Operands must be two numbers or two strings."));
    }
    case TokenType::GREATER:
    {
        checkNumberOperands(expression.getToken(), *left, *right);
        bool result = (getLiteral<double>(*left) > getLiteral<double>(*right));
        return std::make_unique<LiteralVal>(result);
    }
    case TokenType::GREATER_EQUAL:
    {
        checkNumberOperands(expression.getToken(), *left, *right);
        bool result = (getLiteral<double>(*left) >= getLiteral<double>(*right));
        return std::make_unique<LiteralVal>(result);
    }
    case TokenType::LESS:
    {
        checkNumberOperands(expression.getToken(), *left, *right);
        bool result = (getLiteral<double>(*left) < getLiteral<double>(*right));
        return std::make_unique<LiteralVal>(result);
    }
    case TokenType::LESS_EQUAL:
    {
        checkNumberOperands(expression.getToken(), *left, *right);
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
        spdlog::error("Unrecognized binary operator {}", expression.getToken().repr());
        break;
    }
    return nullptr;
}

std::unique_ptr<LiteralVal> Interpreter::visitExpressionLogical(ExpressionLogical& expression)
{
    auto left = evaluate(expression.getLeft());
    switch (expression.getToken().type())
    {
    case TokenType::OR:
        if (isTruthy(*left))
        {
            return left;
        }
        break;
    case TokenType::AND:
        if (!isTruthy(*left))
        {
            return left;
        }
        break;
    default:
        spdlog::error("Unrecognized binary operator {}", expression.getToken().repr());
        break;
    }
    return evaluate(expression.getRight());
}

std::unique_ptr<LiteralVal> Interpreter::visitExpressionGrouping(ExpressionGrouping& expression)
{
    return evaluate(expression.getExpression());
}

std::unique_ptr<LiteralVal> Interpreter::visitExpressionLiteral(ExpressionLiteral& expression)
{
    // TODO : Check against nullptr. Not sure what to do if we see one at the moment
    return std::make_unique<LiteralVal>(expression.getValue());
}

std::unique_ptr<LiteralVal> Interpreter::visitExpressionUnary(ExpressionUnary& expression)
{
    auto right = evaluate(expression.getExpression());

    switch (expression.getToken().type())
    {
    case TokenType::MINUS:
        checkNumberOperand(expression.getToken(), *right);
        return std::make_unique<LiteralVal>(-(getLiteral<double>(*right)));
    case TokenType::BANG:
        return std::make_unique<LiteralVal>(isTruthy(*right));
    default:
        break;
    }

    return nullptr;
}

std::unique_ptr<LiteralVal> Interpreter::visitExpressionVariable(ExpressionVariable& expression)
{
    const auto& varname = expression.getName();
    spdlog::debug("Reading variable {}", varname.lexeme());
    auto val = m_environment->get(varname);
    return std::make_unique<LiteralVal>(val);
}
bool Interpreter::isTruthy(const LiteralVal& lval)
{
    bool result = true;
    if (lval.type() == LiteralValType::Nil)
    {
        result = false;
    }
    if (lval.type() == LiteralValType::Bool)
    {
        result = getLiteral<bool>(lval);
    }
    return result;
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

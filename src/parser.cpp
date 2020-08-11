#include "parser.hpp"

#include <spdlog/spdlog.h>
namespace KeegMake
{
std::unique_ptr<Expression> Parser::parse()
{
    try
    {
        return expression();
    }
    catch (ParseError& error)
    {
        spdlog::error("Parse error encountered {}", error.what());
        return nullptr;
    }
}
void Parser::synchronize()
{
    advance();

    while (!isAtEnd())
    {
        if (previous()->type() == TokenType::SEMICOLON)
        {
            return;
        }

        switch (peek()->type())
        {
            case TokenType::CLASS:
            case TokenType::FUN:
            case TokenType::VAR:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::PRINT:
            case TokenType::RETURN:
                return;
            default:
                break;
        }
        advance();
    }
}

std::unique_ptr<Expression> Parser::expression() { return equality(); }

std::unique_ptr<Expression> Parser::equality()
{
    auto expr = comparison();

    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL}))
    {
        auto oper = previous();
        auto right = comparison();
        expr = std::make_unique<Binary>(std::move(expr), std::move(oper), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> Parser::comparison()
{
    auto expr = addition();

    while (match(
        {TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL}))
    {
        auto oper = previous();
        auto right = addition();
        expr = std::make_unique<Binary>(std::move(expr), std::move(oper), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> Parser::addition()
{
    auto expr = multiplication();

    while (match({TokenType::MINUS, TokenType::PLUS}))
    {
        auto oper = previous();
        auto right = multiplication();
        expr = std::make_unique<Binary>(std::move(expr), std::move(oper), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> Parser::multiplication()
{
    auto expr = unary();

    while (match({TokenType::SLASH, TokenType::STAR}))
    {
        auto oper = previous();
        auto right = unary();
        expr = std::make_unique<Binary>(std::move(expr), std::move(oper), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> Parser::unary()
{
    if (match({TokenType::BANG, TokenType::MINUS}))
    {
        auto oper = previous();
        auto right = unary();
        return std::make_unique<Unary>(std::move(oper), std::move(right));
    }

    return primary();
}

std::unique_ptr<Expression> Parser::primary()
{
    if (match({TokenType::FALSE}))
    {
        return std::make_unique<Literal>(std::make_unique<BoolLiteralVal>(false));
    }
    if (match({TokenType::TRUE}))
    {
        return std::make_unique<Literal>(std::make_unique<BoolLiteralVal>(true));
    }
    if (match({TokenType::NIL}))
    {
        return std::make_unique<Literal>(std::make_unique<NoneLiteralVal>());
    }

    if (match({TokenType::NUMBER, TokenType::STRING}))
    {
        return std::make_unique<Literal>(previous()->literal().clone());
    }

    if (match({TokenType::LEFT_PAREN}))
    {
        auto expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<Grouping>(std::move(expr));
    }

    throw(error(peek(), "Expect expression."));
}

bool Parser::match(std::vector<TokenType>&& types)
{
    for (auto type : types)
    {
        if (check(type))
        {
            advance();
            return true;
        }
    }
    return false;
}

bool Parser::check(TokenType type)
{
    if (isAtEnd())
    {
        return false;
    }
    return peek()->type() == type;
}
std::unique_ptr<Token> Parser::advance()
{
    if (!isAtEnd())
    {
        m_current++;
    }
    return previous();
}

bool Parser::isAtEnd() { return peek()->type() == TokenType::END_OF_FILE; }

std::unique_ptr<Token> Parser::peek() { return std::make_unique<Token>(m_tokens.at(m_current)); }

std::unique_ptr<Token> Parser::previous()
{
    return std::make_unique<Token>(m_tokens.at(m_current - 1));
}

std::unique_ptr<Token> Parser::consume(TokenType type, const std::string& message)
{
    if (check(type))
    {
        return advance();
    }

    throw(error(peek(), message));
}

ParseError Parser::error(std::unique_ptr<Token> token, const std::string& message)
{
    // TODO Feed this up to other logging function and make it like original Java
    if (token->type() == TokenType::END_OF_FILE)
    {
        spdlog::error(" line {} at end {}", token->line(), message);
    }
    else
    {
        spdlog::error(" {} at '{}' {}", token->line(), token->lexeme(), message);
    }

    return ParseError(message);
}
}  // namespace KeegMake

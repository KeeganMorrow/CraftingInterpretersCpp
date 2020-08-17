#include "parser.hpp"

#include <spdlog/spdlog.h>
namespace lox
{
std::vector<std::unique_ptr<const Statement>> Parser::parse()
{
    std::vector<std::unique_ptr<const Statement>> statements;
    while (!isAtEnd())
    {
        auto dec = declaration();
        if (dec != nullptr)
        {
            statements.emplace_back(std::move(dec));
        }
    }
    return statements;
}

void Parser::synchronize()
{
    advance();

    while (!isAtEnd())
    {
        if (previous().type() == TokenType::SEMICOLON)
        {
            return;
        }

        switch (peek().type())
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

std::unique_ptr<Statement> Parser::declaration()
{
    try
    {
        if (match({TokenType::VAR}))
        {
            return varDeclaration();
        }
        return statement();
    }
    catch (ParseError& error)
    {
        synchronize();
        return nullptr;
    }
}

std::unique_ptr<Statement> Parser::statement()
{
    if (match({TokenType::PRINT}))
    {
        return printStatement();
    }

    return expressionStatement();
}

std::unique_ptr<Statement> Parser::printStatement()
{
    auto expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    return std::make_unique<StatementPrint>(std::move(expr));
}

std::unique_ptr<Statement> Parser::expressionStatement()
{
    auto expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_unique<StatementExpression>(std::move(expr));
}

std::unique_ptr<Statement> Parser::varDeclaration()
{
    auto name = consume(TokenType::IDENTIFIER, "Expect variable name.");
    std::unique_ptr<Expression> initializer;
    if (match({TokenType::EQUAL}))
    {
        initializer = expression();
    }

    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_unique<StatementVariable>(std::make_unique<Token>(name),
                                               std::move(initializer));
}

std::unique_ptr<Expression> Parser::expression() { return assignment(); }

std::unique_ptr<Expression> Parser::assignment()
{
    auto expr = equality();

    if (match({TokenType::EQUAL}))
    {
        auto equals = previous();
        auto value = assignment();

        if (auto* varexpr = dynamic_cast<ExpressionVariable*>(expr.get()))
        {
            const auto* name = varexpr->name();
            spdlog::debug("Found expression variable {}!", name->repr());
            return std::make_unique<ExpressionAssign>(std::make_unique<Token>(*name),
                                                      std::move(value));
        }

        throw error(equals, "Invalid assignment target");
    }
    return expr;
}

std::unique_ptr<Expression> Parser::equality()
{
    auto expr = comparison();

    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL}))
    {
        auto oper = previous();
        auto right = comparison();
        expr = std::make_unique<ExpressionBinary>(std::move(expr), std::make_unique<Token>(oper),
                                                  std::move(right));
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
        expr = std::make_unique<ExpressionBinary>(std::move(expr), std::make_unique<Token>(oper),
                                                  std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> Parser::addition()
{
    auto expr = multiplication();

    while (match({TokenType::MINUS, TokenType::PLUS}))
    {
        spdlog::debug("Combining additions...");
        auto oper = previous();
        auto right = multiplication();
        expr = std::make_unique<ExpressionBinary>(std::move(expr), std::make_unique<Token>(oper),
                                                  std::move(right));
    }

    spdlog::debug("Additions combined!");
    return expr;
}

std::unique_ptr<Expression> Parser::multiplication()
{
    auto expr = unary();

    while (match({TokenType::SLASH, TokenType::STAR}))
    {
        auto oper = previous();
        auto right = unary();
        expr = std::make_unique<ExpressionBinary>(std::move(expr), std::make_unique<Token>(oper),
                                                  std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> Parser::unary()
{
    if (match({TokenType::BANG, TokenType::MINUS}))
    {
        auto oper = previous();
        auto right = unary();
        return std::make_unique<ExpressionUnary>(std::make_unique<Token>(oper), std::move(right));
    }

    return primary();
}

std::unique_ptr<Expression> Parser::primary()
{
    if (match({TokenType::FALSE}))
    {
        spdlog::debug("Found primary expression false");
        return std::make_unique<ExpressionLiteral>(std::make_unique<LiteralVal>(false));
    }
    if (match({TokenType::TRUE}))
    {
        spdlog::debug("Found primary expression true");
        return std::make_unique<ExpressionLiteral>(std::make_unique<LiteralVal>(true));
    }
    if (match({TokenType::NIL}))
    {
        spdlog::debug("Found primary expression nil");
        return std::make_unique<ExpressionLiteral>(std::make_unique<LiteralVal>());
    }

    if (match({TokenType::NUMBER, TokenType::STRING}))
    {
        spdlog::debug("Found primary expression string or number {}", previous().repr());
        return std::make_unique<ExpressionLiteral>(
            std::make_unique<LiteralVal>(previous().literal()));
    }

    if (match({TokenType::IDENTIFIER}))
    {
        spdlog::debug("Found primary expression identifier");
        return std::make_unique<ExpressionVariable>(std::make_unique<Token>(previous()));
    }

    if (match({TokenType::LEFT_PAREN}))
    {
        spdlog::debug("Found primary expression left paren");
        auto expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<ExpressionGrouping>(std::move(expr));
    }

    throw(error(peek(), "Expect expression."));
}

bool Parser::match(const std::vector<TokenType>&& types)
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
    return peek().type() == type;
}
const Token& Parser::advance()
{
    if (!isAtEnd())
    {
        m_current++;
    }
    return previous();
}

bool Parser::isAtEnd() const { return peek().type() == TokenType::END_OF_FILE; }

const Token& Parser::peek() const { return m_tokens.at(m_current); }

const Token& Parser::previous() const { return m_tokens.at(m_current - 1); }

const Token& Parser::consume(TokenType type, const std::string& message)
{
    if (check(type))
    {
        return advance();
    }

    throw(error(peek(), message));
}

ParseError Parser::error(const Token& token, const std::string& message)
{
    // TODO Feed this up to other logging function and make it like original Java
    if (token.type() == TokenType::END_OF_FILE)
    {
        spdlog::error(" line {} at end {}", token.line(), message);
    }
    else
    {
        spdlog::error(" {} at '{}' {}", token.line(), token.lexeme(), message);
    }

    return ParseError(message);
}
}  // namespace lox

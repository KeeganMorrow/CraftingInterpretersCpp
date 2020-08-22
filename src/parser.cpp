#include "parser.hpp"

#include <spdlog/spdlog.h>

#include "literal.hpp"
namespace lox
{
std::vector<std::unique_ptr<Statement>> Parser::parse()
{
    std::vector<std::unique_ptr<Statement>> statements;
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

std::unique_ptr<std::vector<std::unique_ptr<Statement>>> Parser::block()
{
    auto statements = std::make_unique<std::vector<std::unique_ptr<Statement>>>();

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())
    {
        statements->emplace_back(declaration());
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return statements;
}

std::unique_ptr<Statement> Parser::statement()
{
    if (match({TokenType::IF}))
    {
        return ifStatement();
    }
    if (match({TokenType::PRINT}))
    {
        return printStatement();
    }
    if (match({TokenType::WHILE}))
    {
        return whileStatement();
    }
    if (match({TokenType::FOR}))
    {
        return forStatement();
    }
    if (match({TokenType::LEFT_BRACE}))
    {
        return std::make_unique<StatementBlock>(block());
    }

    return expressionStatement();
}

std::unique_ptr<Statement> Parser::ifStatement()
{
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    auto condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");
    auto then_branch = statement();
    std::unique_ptr<Statement> else_branch;
    if (match({TokenType::ELSE}))
    {
        else_branch = statement();
    }
    return std::make_unique<StatementIf>(std::move(condition), std::move(then_branch),
                                         std::move(else_branch));
}

std::unique_ptr<Statement> Parser::printStatement()
{
    auto expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    return std::make_unique<StatementPrint>(std::move(expr));
}

std::unique_ptr<Statement> Parser::whileStatement()
{
    consume(TokenType::LEFT_PAREN, "Expect '(' after while.");
    auto expr = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after while.");
    auto body = statement();
    return std::make_unique<StatementWhile>(std::move(expr), std::move(body));
}

std::unique_ptr<Statement> Parser::forStatement()
{
    consume(TokenType::LEFT_PAREN, "Expect '(' after for.");
    std::unique_ptr<Statement> initializer;
    if (match({TokenType::SEMICOLON}))
    {
        initializer = nullptr;
    }
    else if (match({TokenType::VAR}))
    {
        initializer = varDeclaration();
    }
    else
    {
        initializer = expressionStatement();
    }

    std::unique_ptr<Expression> condition;
    if (!check(TokenType::SEMICOLON))
    {
        condition = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

    std::unique_ptr<Expression> increment;
    if (!check(TokenType::RIGHT_PAREN))
    {
        increment = expression();
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after for loop clauses.");

    auto body = statement();
    if (increment != nullptr)
    {
        // Statement block is the for loop body followed by the increment
        auto statements = std::make_unique<std::vector<std::unique_ptr<Statement>>>();
        statements->emplace_back(std::move(body));
        statements->emplace_back(std::make_unique<StatementExpression>(std::move(increment)));
        body = std::make_unique<StatementBlock>(std::move(statements));
    }

    if (condition == nullptr)
    {
        condition = std::make_unique<ExpressionLiteral>(LiteralVal(true));
    }

    body = std::make_unique<StatementWhile>(std::move(condition), std::move(body));

    if (initializer != nullptr)
    {
        auto statements = std::make_unique<std::vector<std::unique_ptr<Statement>>>();
        statements->emplace_back(std::move(initializer));
        statements->emplace_back(std::move(body));
        body = std::make_unique<StatementBlock>(std::move(statements));
    }
    return body;
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
    return std::make_unique<StatementVariable>(name, std::move(initializer));
}

std::unique_ptr<Expression> Parser::expression() { return assignment(); }

std::unique_ptr<Expression> Parser::assignment()
{
    auto expr = logicalOr();

    if (match({TokenType::EQUAL}))
    {
        auto equals = previous();
        auto value = assignment();

        if (auto* varexpr = dynamic_cast<ExpressionVariable*>(expr.get()))
        {
            auto name = varexpr->getName();
            spdlog::debug("Found expression variable {}!", name.repr());
            return std::make_unique<ExpressionAssign>(name, std::move(value));
        }

        throw error(equals, "Invalid assignment target");
    }
    return expr;
}

std::unique_ptr<Expression> Parser::logicalOr()
{
    auto expr = logicalAnd();

    if (match({TokenType::OR}))
    {
        auto token = previous();
        auto right = logicalAnd();
        expr = std::make_unique<ExpressionLogical>(std::move(expr), token, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expression> Parser::logicalAnd()
{
    auto expr = equality();
    if (match({TokenType::AND}))
    {
        auto token = previous();
        auto right = equality();
        expr = std::make_unique<ExpressionLogical>(std::move(expr), token, std::move(right));
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
        expr = std::make_unique<ExpressionBinary>(std::move(expr), oper, std::move(right));
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
        expr = std::make_unique<ExpressionBinary>(std::move(expr), oper, std::move(right));
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
        expr = std::make_unique<ExpressionBinary>(std::move(expr), oper, std::move(right));
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
        expr = std::make_unique<ExpressionBinary>(std::move(expr), oper, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> Parser::unary()
{
    if (match({TokenType::BANG, TokenType::MINUS}))
    {
        auto oper = previous();
        auto right = unary();
        return std::make_unique<ExpressionUnary>(oper, std::move(right));
    }

    return primary();
}

std::unique_ptr<Expression> Parser::primary()
{
    if (match({TokenType::FALSE}))
    {
        spdlog::debug("Found primary expression false");
        return std::make_unique<ExpressionLiteral>(LiteralVal(false));
    }
    if (match({TokenType::TRUE}))
    {
        spdlog::debug("Found primary expression true");
        return std::make_unique<ExpressionLiteral>(LiteralVal(true));
    }
    if (match({TokenType::NIL}))
    {
        spdlog::debug("Found primary expression nil");
        return std::make_unique<ExpressionLiteral>(LiteralVal());
    }

    if (match({TokenType::NUMBER, TokenType::STRING}))
    {
        spdlog::debug("Found primary expression string or number {}", previous().repr());
        return std::make_unique<ExpressionLiteral>(previous().literal());
    }

    if (match({TokenType::IDENTIFIER}))
    {
        spdlog::debug("Found primary expression identifier");
        return std::make_unique<ExpressionVariable>(previous());
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

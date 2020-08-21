#pragma once
#include <memory>
#include <vector>

#include "expression_ast.hpp"
#include "statement_ast.hpp"
#include "token.hpp"

namespace lox
{
class Parser
{
public:
    explicit Parser(std::vector<Token>&& tokens) : m_tokens(tokens) {}

    std::vector<std::unique_ptr<Statement>> parse();

private:
    void synchronize();

    std::unique_ptr<std::vector<std::unique_ptr<Statement>>> block();
    std::unique_ptr<Statement> declaration();

    std::unique_ptr<Statement> statement();
    std::unique_ptr<Statement> ifStatement();
    std::unique_ptr<Statement> printStatement();
    std::unique_ptr<Statement> expressionStatement();
    std::unique_ptr<Statement> varDeclaration();

    std::unique_ptr<Expression> expression();

    std::unique_ptr<Expression> assignment();
    std::unique_ptr<Expression> equality();
    std::unique_ptr<Expression> logicalOr();
    std::unique_ptr<Expression> logicalAnd();
    std::unique_ptr<Expression> comparison();
    std::unique_ptr<Expression> addition();
    std::unique_ptr<Expression> multiplication();
    std::unique_ptr<Expression> unary();
    std::unique_ptr<Expression> primary();
    bool match(const std::vector<TokenType>&& types);
    bool check(TokenType type);

    [[nodiscard]] bool isAtEnd() const;
    const Token& advance();
    [[nodiscard]] const Token& peek() const;
    [[nodiscard]] const Token& previous() const;
    const Token& consume(TokenType type, const std::string& message);

    static ParseError error(const Token& token, const std::string& message);

    const std::vector<Token> m_tokens;
    int m_current = 0;
};
}  // namespace lox

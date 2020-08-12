#pragma once
#include <expressions.hpp>
#include <memory>
#include <vector>

#include "expression_ast.hpp"
#include "statement_ast.hpp"
#include "token.hpp"

namespace KeegMake
{
class Parser
{
public:
    Parser(std::vector<Token>&& tokens) : m_tokens(tokens) {}

    std::vector<std::unique_ptr<const Statement>> parse();

private:
    void synchronize();

    std::unique_ptr<Statement> declaration();

    std::unique_ptr<Statement> statement();
    std::unique_ptr<Statement> printStatement();
    std::unique_ptr<Statement> expressionStatement();
    std::unique_ptr<Statement> varDeclaration();


    std::unique_ptr<Expression> expression();

    std::unique_ptr<Expression> equality();
    std::unique_ptr<Expression> comparison();
    std::unique_ptr<Expression> addition();
    std::unique_ptr<Expression> multiplication();
    std::unique_ptr<Expression> unary();
    std::unique_ptr<Expression> primary();
    bool match(std::vector<TokenType>&& types);
    bool check(TokenType type);

    bool isAtEnd() const;
    std::unique_ptr<Token> advance();
    [[nodiscard]] std::unique_ptr<Token> peek() const;
    [[nodiscard]] std::unique_ptr<Token> previous() const;
    std::unique_ptr<Token> consume(TokenType type, const std::string& message);

    static ParseError error(std::unique_ptr<Token> token, const std::string& message);

    const std::vector<Token> m_tokens;
    int m_current = 0;
};
}  // namespace KeegMake

#pragma once
#include "expression_ast.hpp"
#include "parser.hpp"
#include "token.hpp"
#include <expressions.hpp>
#include <memory>
#include <vector>

namespace KeegMake {

class Parser {
public:
    Parser(std::vector<Token>&& tokens)
        : m_tokens(tokens)
    {
    }

    std::unique_ptr<Expression> parse();

private:
    void synchronize();

    std::unique_ptr<Expression> expression();

    std::unique_ptr<Expression> equality();
    std::unique_ptr<Expression> comparison();
    std::unique_ptr<Expression> addition();
    std::unique_ptr<Expression> multiplication();
    std::unique_ptr<Expression> unary();
    std::unique_ptr<Expression> primary();
    bool match(std::vector<TokenType>&& types);
    bool check(TokenType type);

    bool isAtEnd();
    std::unique_ptr<Token> advance();
    std::unique_ptr<Token> peek();
    std::unique_ptr<Token> previous();

    static ParseError error(std::unique_ptr<Token> token, const std::string& message);

    std::unique_ptr<Token> consume(TokenType type, const std::string &message);

    const std::vector<Token> m_tokens;
    int m_current = 0;
};
} //namespace KeegMake

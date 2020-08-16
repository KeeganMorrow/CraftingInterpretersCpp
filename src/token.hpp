#pragma once
#include <spdlog/fmt/fmt.h>

#include <string>
#include <type_traits>

#include "exception.hpp"
#include "literal.hpp"
namespace lox
{
enum class TokenType
{
    // Single character tokens
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,

    // One or two character tokens.
    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    // Literals.
    IDENTIFIER,
    STRING,
    NUMBER,

    // Keywords.
    AND,
    CLASS,
    ELSE,
    FALSE,
    FUN,
    FOR,
    IF,
    NIL,
    OR,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    TRUE,
    VAR,
    WHILE,

    END_OF_FILE
};

class Token
{
public:
    Token(TokenType type, std::string lexeme, std::unique_ptr<LiteralVal> literal, int line)
        : m_type(type), m_lexeme(std::move(lexeme)), m_line(line), m_literal(std::move(literal))
    {
    }
    Token(const Token& other)
        : m_type(other.m_type),
          m_lexeme(other.m_lexeme),
          m_line(other.m_line),
          m_literal(std::make_unique<LiteralVal>(*other.m_literal))
    {
    }
    [[nodiscard]] TokenType type() const { return m_type; }
    [[nodiscard]] std::string lexeme() const { return m_lexeme; }
    [[nodiscard]] int line() const { return m_line; }
    [[nodiscard]] const LiteralVal& literal() const { return *m_literal; };

    [[nodiscard]] std::string repr() const
    {
        if (m_literal)
        {
            return fmt::format("TokenType: {}, lexeme: {}, literal: {}", m_type, m_lexeme,
                               m_literal->repr());
        }
        return fmt::format("TokenType: {}, lexeme: {}, literal: N/A", m_type, m_lexeme);
    }

private:
    TokenType m_type;
    const std::string m_lexeme;
    const int m_line;
    std::unique_ptr<const LiteralVal> m_literal;
};

}  // namespace lox

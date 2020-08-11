#pragma once
#include "exception.hpp"
#include "literal.hpp"
#include <spdlog/fmt/fmt.h>
#include <string>
#include <type_traits>
namespace KeegMake {

enum class TokenType {
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

class Token {
public:
    Token(TokenType type, const std::string&& lexeme,
        std::unique_ptr<const LiteralVal>&& literal, int line)
        : m_type(type)
        , m_lexeme(lexeme)
        , m_line(line)
        , m_literal(std::move(literal))
    {
    }
    Token(const Token& other)
        : m_type(other.m_type)
        , m_lexeme(other.m_lexeme)
        , m_line(other.m_line)
        , m_literal(other.m_literal->clone())
    {
    }
    [[nodiscard]] TokenType type() const { return m_type; }
    [[nodiscard]] std::string lexeme() const { return m_lexeme; }
    [[nodiscard]] int line() const { return m_line; }
    [[nodiscard]] const LiteralVal& literal() const { return *m_literal; };

    [[nodiscard]] std::string repr() const
    {
        return fmt::format("TokenType: {}, lexeme: {}, literal: {}", m_type,
            m_lexeme, m_literal->repr());
    }

private:
    TokenType m_type;
    const std::string m_lexeme;
    const int m_line;
    std::unique_ptr<const LiteralVal> m_literal;
};

} // namespace KeegMake

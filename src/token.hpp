#pragma once
#include <string>
#include <type_traits>
#include "exception.hpp"
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
    Token(TokenType type, const std::string&& lexeme, int line)
        : m_type(type)
        , m_lexeme(lexeme)
        , m_line(line)
    {
    }
    [[nodiscard]] TokenType type() const { return m_type; }
    [[nodiscard]] std::string lexeme() const { return m_lexeme; }
    [[nodiscard]] int line() const { return m_line; }

    [[nodiscard]] virtual std::string literal_str() const { throw MissingLiteralException(line(), lexeme());}
    [[nodiscard]] virtual bool literal_bool() const { throw MissingLiteralException(line(), lexeme());}
    [[nodiscard]] virtual double literal_num() const { throw MissingLiteralException(line(), lexeme());}

private:
    TokenType m_type;
    const std::string m_lexeme;
    const int m_line;
};

class NumberToken : public Token
{
    public:
        NumberToken(double value, const std::string&& lexeme, int line)
            : Token(TokenType::NUMBER, std::move(lexeme), line)
            , m_literal(value)
        {
        }

    double literal_num() const override {return m_literal;}

    private:
    const double m_literal;
};

class BoolToken : public Token
{
    public:
        BoolToken(bool value, const std::string&& lexeme, int line)
            : Token(TokenType::NUMBER, std::move(lexeme), line)
            , m_literal(value)
        {
        }
    [[nodiscard]] bool literal_bool() const override {return m_literal;}

    private:
    const bool m_literal;
};
class StringToken : public Token
{
    public:
        StringToken(const std::string&& value, const std::string&& lexeme, int line)
            : Token(TokenType::NUMBER, std::move(lexeme), line)
            , m_literal(value)
        {
        }
    [[nodiscard]] std::string literal_str() const override {return m_literal;}

    private:
    const std::string m_literal;
};
} //namespace KeegMake

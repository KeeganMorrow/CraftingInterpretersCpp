#include "scanner.hpp"

#include <spdlog/spdlog.h>

#include <string>

#include "application.hpp"

namespace lox
{
Scanner::Scanner(const std::string& source) : m_source(source) {}

std::vector<Token>& Scanner::scanTokens()
{
    while (!isAtEnd())
    {
        m_start = m_current;
        scanToken();
    };

    m_tokens.emplace_back(
        Token{TokenType::END_OF_FILE, std::string(), std::make_unique<LiteralVal>(), m_line});
    return m_tokens;
}

bool Scanner::isAtEnd() { return m_current >= (int)m_source.length(); }

void Scanner::scanToken()
{
    char c = advance();
    switch (c)
    {
    case '(':
        addToken(TokenType::LEFT_PAREN);
        break;
    case ')':
        addToken(TokenType::RIGHT_PAREN);
        break;
    case '{':
        addToken(TokenType::LEFT_BRACE);
        break;
    case '}':
        addToken(TokenType::RIGHT_BRACE);
        break;
    case ',':
        addToken(TokenType::COMMA);
        break;
    case '.':
        addToken(TokenType::DOT);
        break;
    case '-':
        addToken(TokenType::MINUS);
        break;
    case '+':
        addToken(TokenType::PLUS);
        break;
    case ';':
        addToken(TokenType::SEMICOLON);
        break;
    case '*':
        addToken(TokenType::STAR);
        break;
    case '!':
        addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
        break;
    case '=':
        addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        break;
    case '<':
        addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        break;
    case '>':
        addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
        break;
    case '/':
        if (match('/'))
        {
            // A comment goes until the end of the line.
            while (peek() != '\n' && !isAtEnd())
            {
                advance();
            }
        }
        else
        {
            addToken(TokenType::SLASH);
        }
        break;
    case '"':
        string();
        break;
    case ' ':
    case '\r':
    case '\t':
        // Ignore whitespace.
        break;

    case '\n':
        m_line++;
        break;
    default:
        if (std::isdigit(c) != 0)
        {
            number();
        }
        else if (std::isalpha(c) != 0)
        {
            identifier();
        }
        else
        {
            Application::error(m_line, "Unexpected character");
        }
        break;
    }
}

char Scanner::advance()
{
    m_current++;
    return m_source.at(m_current - 1);
}

void Scanner::addToken(TokenType type)
{
    auto text = m_source.substr(m_start, m_current - m_start);
    spdlog::debug("Adding a token with lexeme {} literal N/A start {} current {}", text, m_start,
                  m_current);
    m_tokens.emplace_back(Token{type, text, std::make_unique<LiteralVal>(), m_line});
}

void Scanner::addToken(TokenType type, std::string& literal)
{
    auto text = m_source.substr(m_start, m_current - m_start);
    spdlog::debug("Adding a token with lexeme {} literal {} start {} current {}", text, literal,
                  m_start, m_current);
    m_tokens.emplace_back(
        Token{type, std::move(text), std::make_unique<LiteralVal>(literal), m_line});
}

void Scanner::addToken(TokenType type, bool literal)
{
    auto text = m_source.substr(m_start, m_current - m_start);
    spdlog::debug("Adding a token with lexeme {} literal {} start {} current {}", text, literal,
                  m_start, m_current);
    m_tokens.emplace_back(
        Token{type, std::move(text), std::make_unique<LiteralVal>(literal), m_line});
}

void Scanner::addToken(TokenType type, double literal)
{
    auto text = m_source.substr(m_start, m_current - m_start);
    spdlog::debug("Adding a token with lexeme {} literal {} start {} current {}", text, literal,
                  m_start, m_current);
    m_tokens.emplace_back(
        Token{type, std::move(text), std::make_unique<LiteralVal>(literal), m_line});
}

bool Scanner::match(char expected)
{
    if (isAtEnd())
    {
        return false;
    }
    if (m_source.at(m_current) != expected)
    {
        return false;
    }

    m_current++;
    return true;
}
char Scanner::peek()
{
    if (isAtEnd())
    {
        return '\0';
    }
    return m_source.at(m_current);
}

char Scanner::peekNext()
{
    if (m_current + 1 >= (int)m_source.length())
    {
        return '\0';
    }
    return m_source.at(m_current + 1);
}

void Scanner::string()
{
    while (peek() != '"' && !isAtEnd())
    {
        if (peek() == '\n')
        {
            m_line++;
        }
        advance();
    }

    if (isAtEnd())
    {
        Application::error(m_line, "Unterminated string");
        return;
    }

    // The closing ".
    advance();

    // Trim those quotes
    std::string value = m_source.substr(m_start + 1, m_current - m_start - 2);
    addToken(TokenType::STRING, value);
}

void Scanner::number()
{
    while (std::isdigit(peek()) != 0)
    {
        advance();
    }

    // Look for a fractional part
    if (peek() == '.' && std::isdigit(peekNext()) != 0)
    {
        // Consume the "."
        advance();

        while (std::isdigit(peek()) != 0)
        {
            advance();
        }
    }

    auto numstr = m_source.substr(m_start, m_current - m_start);
    auto val = std::stod(numstr);
    addToken(TokenType::NUMBER, (double)val);
}

void Scanner::identifier()
{
    while (std::isalnum(peek()) != 0)
    {
        advance();
    }

    auto text = m_source.substr(m_start, m_current - m_start);

    TokenType type;
    try
    {
        type = Keywords.at(text);
    }
    catch (std::out_of_range& e)
    {
        type = TokenType::IDENTIFIER;
    }

    addToken(type, text);
}

const std::map<std::string, TokenType> Scanner::Keywords = {
    {"and", TokenType::AND},     {"class", TokenType::CLASS},   {"else", TokenType::ELSE},
    {"false", TokenType::FALSE}, {"for", TokenType::FOR},       {"fun", TokenType::FUN},
    {"if", TokenType::IF},       {"nil", TokenType::NIL},       {"or", TokenType::OR},
    {"print", TokenType::PRINT}, {"return", TokenType::RETURN}, {"super", TokenType::SUPER},
    {"this", TokenType::THIS},   {"true", TokenType::TRUE},     {"var", TokenType::VAR},
    {"while", TokenType::WHILE}};

}  // namespace lox

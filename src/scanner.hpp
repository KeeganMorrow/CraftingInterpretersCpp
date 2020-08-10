#pragma once
#include "token.hpp"
#include <map>
#include <string>
#include <vector>
namespace KeegMake {

class Token;

class Scanner {
public:
    Scanner(const std::string&& source);

    std::vector<Token> scanTokens();

    // Delete undesired constructors (Allow move, not copy or assign)
    Scanner(const Scanner&) = delete;
    Scanner& operator=(const Scanner&) = delete;

private:
    bool isAtEnd();
    void scanToken();
    char advance();
    void addToken(TokenType type, const std::string& literal);
    void addToken(TokenType type, bool literal);
    void addToken(TokenType type, double literal);
    void addToken(TokenType type);
    bool match(char expected);
    char peek();
    char peekNext();

    void string();
    void number();
    void identifier();

    const std::string m_source {};
    std::vector<Token> m_tokens {};
    int m_start { 0 };
    int m_current { 0 };
    int m_line { 1 };

    static const std::map<std::string, TokenType> Keywords;
};

} // namespace KeegMake

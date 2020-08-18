#pragma once

#include <map>
#include <string>

#include "literal.hpp"
#include "token.hpp"

namespace lox
{
class Environment
{
public:
    explicit Environment(Environment *enclosing = nullptr) : m_enclosing(enclosing) {}
    void define(std::string name, std::unique_ptr<LiteralVal> value);
    void assign(const Token &token, std::unique_ptr<LiteralVal> value);

    [[nodiscard]] LiteralVal get(const Token &token) const;

private:
    std::map<std::string, std::unique_ptr<LiteralVal>> m_values;
    Environment *m_enclosing;
};

}  // namespace lox

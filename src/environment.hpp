#pragma once

#include <map>
#include <string>

#include "literal.hpp"
#include "token.hpp"

namespace Lox
{

class Environment
{
public:
    Environment() = default;
    void define(std::string name, std::unique_ptr<LiteralVal> value);

    LiteralVal &get(const Token &token);

private:
    std::map<std::string, std::unique_ptr<LiteralVal>> m_values;
};

}  // namespace Lox

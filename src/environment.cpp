#include "environment.hpp"

#include <spdlog/spdlog.h>

#include <utility>

#include "interpreter.hpp"

namespace lox
{
void Environment::define(std::string name, std::unique_ptr<LiteralVal> value)
{
    spdlog::debug("Defining variable {} with value {}", name, value->repr());
    m_values.emplace(std::move(name), std::move(value));
}

void Environment::assign(const Token &token, std::unique_ptr<LiteralVal> value)
{
    spdlog::debug("Assigning variable {} value {}", token.repr(), value->repr());
    try
    {
        auto &result = m_values.at(token.lexeme());
        result = std::move(value);
    }
    catch (std::out_of_range &e)
    {
        throw RuntimeError(token, "Undefined variable " + token.lexeme() + ".");
    }
}

LiteralVal Environment::get(const Token &token) const
{
    spdlog::debug("Reading variable {}", token.repr());
    try
    {
        const auto &result = m_values.at(token.lexeme());
        return *result;
    }
    catch (std::out_of_range &e)
    {
        throw RuntimeError(token, "Undefined variable " + token.lexeme() + ".");
    }
}
}  // namespace lox

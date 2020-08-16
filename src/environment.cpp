#include "environment.hpp"

#include <spdlog/spdlog.h>

#include <utility>

#include "interpreter.hpp"

namespace lox
{
void Environment::define(std::string name, std::unique_ptr<LiteralVal> value)
{
    spdlog::debug("Defining variable {}", name);
    m_values.emplace(std::move(name), std::move(value));
}

LiteralVal Environment::get(const Token &token)
{
    spdlog::debug("Reading variable {}", token.repr());
    try
    {
        auto &result = m_values.at(token.lexeme());
        return *result;
    }
    catch (std::out_of_range &e)
    {
        throw RuntimeError(token, "Undefined variable " + token.lexeme() + ".");
    }
}
}  // namespace lox

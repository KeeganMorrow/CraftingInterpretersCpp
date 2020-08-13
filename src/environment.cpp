#include "environment.hpp"
#include "interpreter.hpp"
#include <utility>
#include <spdlog/spdlog.h>

namespace KeegMake
{

void Environment::define(std::string name, std::unique_ptr<LiteralVal> value)
{
    m_values.emplace(std::move(name), std::move(value));
}

LiteralVal&::get(const Token &token)
{
    spdlog::info("Reading variable {}", token.repr());
    try{
        return *(m_values[token.lexeme()]);
    }catch(std::out_of_range &e)
    {
        throw RuntimeError(token, "Undefined variable " + token.lexeme() + ".");
    }

}
}  // namespace KeegMake

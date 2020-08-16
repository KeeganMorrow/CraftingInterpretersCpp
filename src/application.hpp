#pragma once
#include <string>
#include <vector>

#include "interpreter.hpp"

namespace lox
{
class Application
{
public:
    explicit Application(std::vector<std::string>&& args) : m_args(std::move(args)) {}
    int start();

    // Delete undesired constructors (Allow move, not copy or assign)
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    int run(const std::string& source);
    int runFile(const std::string& filepath);
    int runPrompt();

    static void error(int line, const std::string& message);
    static void runtimeError(const RuntimeError& error);
    static void report(int line, const std::string& where, const std::string& message);

private:
    bool m_hadError{false};
    const std::vector<std::string> m_args;
    Interpreter m_interpreter;
};
}  // namespace lox

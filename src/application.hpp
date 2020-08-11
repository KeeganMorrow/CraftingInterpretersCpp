#pragma once
#include "interpreter.hpp"
#include <string>
#include <vector>

namespace KeegMake {

class Application {
public:
    Application(std::vector<std::string>&& args)
        : m_args(std::move(args))
    {
    }
    int run();

    // Delete undesired constructors (Allow move, not copy or assign)
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

private:
    const std::vector<std::string> m_args;
    Interpreter m_interpreter;
};
} // namespace KeegMake

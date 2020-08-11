#pragma once
#include <string>

namespace KeegMake
{
class Environment
{
public:
    Environment() = default;

    int runFile(const std::string& filepath);
    int run(const std::string& source);
    int runPrompt();

    static void error(int line, const std::string& message);
    static void report(int line, const std::string& where, const std::string& message);

    // Delete undesired constructors (Allow move, not copy or assign)
    Environment(const Environment&) = delete;
    Environment& operator=(const Environment&) = delete;

private:
    bool m_hadError{false};
};
}  // namespace KeegMake

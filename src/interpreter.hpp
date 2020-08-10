#pragma once
#include <string>

namespace KeegMake {

class Interpreter {
public:
    Interpreter() = default;

    int runFile(const std::string& filepath);
    int run(const std::string& source);
    int runPrompt();

    static void error(int line, const std::string& message);
    static void report(int line, const std::string& where,
        const std::string& message);

    // Delete undesired constructors (Allow move, not copy or assign)
    Interpreter(const Interpreter&) = delete;
    Interpreter& operator=(const Interpreter&) = delete;

private:
    bool m_hadError {false};
};
} // namespace KeegMake

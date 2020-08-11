#include "interpreter.hpp"
#include "application.hpp"
#include "ast_visitor.hpp"
#include "parser.hpp"
#include "scanner.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <spdlog/spdlog.h>

namespace KeegMake {

int Interpreter::run(const std::string& source)
{
    Scanner scanner(std::move(source));
    auto tokens = scanner.scanTokens();
    Parser parser(std::move(tokens));
    auto expression = parser.parse();
    if (expression)
        spdlog::info("{}", AstPrinter().print(*expression));
    else {
        spdlog::warn("No valid expression after parsing, see previous output");
    }

    for (auto& token : tokens) {
        spdlog::info("Found token {}", token.repr());
    }
    return 0;
}

int Interpreter::runPrompt()
{
    while (true) {
        std::cout << "> ";
        std::string line;
        std::getline(std::cin, line);
        if (line == "exit") {
            break;
        }
        run(line);
        m_hadError = false;
    }
    return 0;
}

int Interpreter::runFile(const std::string& filepath)
{
    int status {0};
    spdlog::info("Opening file {}", filepath);

    std::filesystem::path path {filepath};
    std::ifstream file;
    try {
        std::ostringstream buf;
        file.open(path);
        if (!file.fail()) {
            buf << file.rdbuf();
            run(buf.str());
            if (m_hadError) {
                status = 5;
            }
        } else {
            spdlog::error("Error opening file {}", filepath);
        }
    } catch (std::ifstream::failure& e) {
        spdlog::error("Failed operating on file '{}'", e.what(),
            e.code().value());
    }

    return status;
}

void Interpreter::error(int line, const std::string& message)
{
    report(line, "", message);
}

void Interpreter::report(int line, const std::string& where,
    const std::string& message)
{
    spdlog::error("[line {}] Error {}: {}", line, where, message);
}

} // namespace KeegMake

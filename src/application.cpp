#include "application.hpp"

#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>

#include "ast_visitor.hpp"
#include "exception.hpp"
#include "parser.hpp"
#include "scanner.hpp"

namespace lox
{
const int EXIT_RESULT_OK = 0;
const int EXIT_RESULT_PARSE_ERROR = 1;

int Application::start()
{
    int status{0};

    if (std::getenv("VERBOSE") != nullptr)
    {
        spdlog::set_level(spdlog::level::trace);
    }

    if (m_args.size() == 1)
    {
        status = runPrompt();
    }
    else if (m_args.size() == 2)
    {
        status = runFile(m_args[1]);
    }
    else
    {
        spdlog::warn("Wrong number of args! Booo {}", m_args.size());
        status = 1;
    }
    return status;
}

int Application::run(const std::string& source)
{
    int result{EXIT_RESULT_OK};
    Scanner scanner(source);
    auto tokens = scanner.scanTokens();

    for (auto& token : tokens)
    {
        spdlog::debug("Found token {}", token.repr());
    }

    Parser parser(std::move(tokens));
    auto program = parser.parse();
    m_interpreter.interpret(std::move(program));

    return result;
}

int Application::runPrompt()
{
    while (true)
    {
        std::cout << "> ";
        std::string line;
        std::getline(std::cin, line);
        if (line == "exit")
        {
            break;
        }
        run(line);
        m_hadError = false;
    }
    return 0;
}

int Application::runFile(const std::string& filepath)
{
    int status{0};
    spdlog::debug("Opening file {}", filepath);

    std::filesystem::path path{filepath};
    std::ifstream file;
    try
    {
        std::ostringstream buf;
        file.open(path);
        if (!file.fail())
        {
            buf << file.rdbuf();
            auto result = run(buf.str());
            if (m_hadError || result != 0)
            {
                spdlog::error("Error reading file {}", filepath);
                status = EXIT_RESULT_PARSE_ERROR;
            }
        }
        else
        {
            spdlog::error("Error opening file {}", filepath);
        }
    }
    catch (std::ifstream::failure& e)
    {
        spdlog::error("Failed operating on file '{}'", e.what(), e.code().value());
    }

    return status;
}

void Application::error(int line, const std::string& message) { report(line, "", message); }

void Application::runtimeError(const RuntimeError& error)
{
    spdlog::error("[line {}] {} (Operator {})", error.token().line(), error.what(),
                  error.token().lexeme());
}

void Application::report(int line, const std::string& where, const std::string& message)
{
    spdlog::error("[line {}] Error {}: {}", line, where, message);
}
}  // namespace lox

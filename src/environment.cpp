#include "environment.hpp"

#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>
#include <iostream>

#include "application.hpp"
#include "ast_visitor.hpp"
#include "parser.hpp"
#include "scanner.hpp"

namespace KeegMake
{
const int EXIT_RESULT_OK = 0;
const int EXIT_RESULT_PARSE_ERROR = 1;

int Environment::run(const std::string& source)
{
    int result{EXIT_RESULT_OK};
    Scanner scanner(source);
    auto tokens = scanner.scanTokens();

    for (auto& token : tokens)
    {
        spdlog::info("Found token {}", token.repr());
    }

    Parser parser(std::move(tokens));
    auto expression = parser.parse();
    if (expression)
    {
        spdlog::info("{}", AstPrinter().print(*expression));
    }
    else
    {
        m_hadError = true;
        spdlog::warn("No valid expression after parsing, see previous output");
        result = EXIT_RESULT_PARSE_ERROR;
    }

    return result;
}

int Environment::runPrompt()
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

int Environment::runFile(const std::string& filepath)
{
    int status{0};
    spdlog::info("Opening file {}", filepath);

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
            if (m_hadError || result)
            {
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

void Environment::error(int line, const std::string& message) { report(line, "", message); }

void Environment::report(int line, const std::string& where, const std::string& message)
{
    spdlog::error("[line {}] Error {}: {}", line, where, message);
}

}  // namespace KeegMake

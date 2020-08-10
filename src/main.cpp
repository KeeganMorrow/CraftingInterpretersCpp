#include "application.hpp"
#include "ast_visitor.hpp"
#include "expressions.hpp"
#include "literal.hpp"
#include <memory>
#include <spdlog/spdlog.h>

using namespace KeegMake;

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    std::vector<std::string> args;
    for (int i = 0; i < argc; i++) {
        args.emplace_back(argv[i]);
    }
    KeegMake::Application application(std::move(args));

    std::unique_ptr<const Expression> expr = std::make_unique<Binary>(
        std::make_unique<Unary>(
            std::make_unique<Token>(TokenType::MINUS, "-",
                LiteralVal(LiteralValType::None), 1),
            std::make_unique<Literal>(
                std::make_unique<NumberLiteralVal>(123.4))),
        std::make_unique<Token>(TokenType::STAR, "*",
            LiteralVal(LiteralValType::None), 1),
        std::make_unique<Grouping>(std::make_unique<Literal>(
            std::make_unique<NumberLiteralVal>(45.67))));

    spdlog::info(AstPrinter().print(*expr.get()));

    return application.run();
}

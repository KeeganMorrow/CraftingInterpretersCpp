#include <spdlog/spdlog.h>

#include <memory>

#include "application.hpp"
#include "ast_visitor.hpp"
#include "literal.hpp"

using namespace Lox;

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    std::vector<std::string> args;
    // This is a bit pointless, but avoids
    // performance-inefficient-vector-operation
    // May want to disable this check, see if it is useful longterm
    args.reserve(argc);
    for (int i = 0; i < argc; i++)
    {
        args.emplace_back(argv[i]);
    }
    Lox::Application application(std::move(args));
    return application.start();
}

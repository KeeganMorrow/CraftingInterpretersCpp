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
    // This is a bit pointless, but avoids
    // performance-inefficient-vector-operation
    // May want to disable this check, see if it is useful longterm
    args.reserve(argc);
    for (int i = 0; i < argc; i++) {
        args.emplace_back(argv[i]);
    }
    KeegMake::Application application(std::move(args));
    return application.run();
}

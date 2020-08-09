#include "application.hpp"
#include <spdlog/spdlog.h>

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    std::vector<std::string> args;
    for(int i = 0; i < argc; i++)
    {
        args.emplace_back(argv[i]);
    }
    KeegMake::Application application(std::move(args));

    return application.run();
}

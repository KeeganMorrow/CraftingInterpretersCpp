#include "application.hpp"

#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>
#include <iostream>

namespace KeegMake
{
int Application::run()
{
    int status{0};

    if (m_args.size() == 1)
    {
        status = m_environment.runPrompt();
    }
    else if (m_args.size() == 2)
    {
        status = m_environment.runFile(m_args[1]);
    }
    else
    {
        spdlog::warn("Wrong number of args! Booo {}", m_args.size());
        status = 1;
    }
    return status;
}

}  // namespace KeegMake

#include <soulng/util/Process.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Process.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/TextUtils.hpp>
#include <iostream>

using namespace soulng::util;

struct InitDone
{
    InitDone()
    {
        soulng::util::Init();
    }
    ~InitDone()
    {
        soulng::util::Done();
    }
};

int main()
{
    InitDone initDone;
    std::string errors;
    try
    {
        std::string command = "cmd /C \"C:\\work\\temp\\koe.bat\"";
        Process process(command, Process::Redirections::none);
        process.WaitForExit();
        int exitCode = process.ExitCode();
        if (exitCode != 0)
        {
            throw std::runtime_error("compile command returned exit code " + std::to_string(exitCode));
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << ": " << errors << std::endl;
        return 1;
    }
    return 0;
}
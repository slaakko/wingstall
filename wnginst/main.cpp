#include <soulng/rex/Match.hpp>
#include <soulng/util/BinaryStreamReader.hpp>
#include <soulng/util/BinaryStreamWriter.hpp>
#include <soulng/util/FileStream.hpp>
#include <soulng/util/BufferedStream.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/FileUtil.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <stdint.h>

std::string version()
{
    return "1.1.0";
}

void PrintHelp()
{
    std::cout << "wnginst file installation utility version " << version() << std::endl;
    std::cout << std::endl;
    std::cout << "usage: wnginst [options] (SOURCEFILE DESTFILE | SOURCEFILES... DIR)" << std::endl;
    std::cout << std::endl;
    std::cout << "copies SOURCEFILE to DESTFILE or SOURCEFILES to DIR for each file if:" << std::endl;
    std::cout << "- destination file does not exist or" << std::endl;
    std::cout << "- destination file is older than source file or" << std::endl;
    std::cout << "- forced" << std::endl;
    std::cout << std::endl;
    std::cout << "wild cards ok for SOURCEFILES" << std::endl;
    std::cout << std::endl;
    std::cout << "options:" << std::endl;
    std::cout << std::endl;
    std::cout << "--verbose | -v:" << std::endl;
    std::cout << "  be verbose" << std::endl;
    std::cout << std::endl;
    std::cout << "--help | -h:" << std::endl;
    std::cout << "  print help and exit" << std::endl;
    std::cout << std::endl;
    std::cout << "--force | -f:" << std::endl;
    std::cout << "  force copy although source file is older than destination file" << std::endl;
    std::cout << std::endl;
    std::cout << "--dir | -d" << std::endl;
    std::cout << "  make destination directory" << std::endl;
    std::cout << std::endl;
}

using namespace soulng::rex;
using namespace soulng::util;
using namespace soulng::unicode;

void InitApplication()
{
    soulng::util::Init();
}


int main(int argc, const char** argv)
{
    try
    {
        InitApplication();
        bool verbose = false;
        bool force = false;
        bool makeDir = false;
        std::vector<std::string> paths;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (StartsWith(arg, "--"))
            {
                if (arg == "--verbose")
                {
                    verbose = true;
                }
                else if (arg == "--force")
                {
                    force = true;
                }
                else if (arg == "--make-dir")
                {
                    makeDir = true;
                }
                else if (arg == "--help")
                {
                    PrintHelp();
                    return 1;
                }
                else
                {
                    throw std::runtime_error("unknown option '" + arg + "'");
                }
            }
            else if (StartsWith(arg, "-"))
            {
                std::string options = arg.substr(1);
                for (char o : options)
                {
                    switch (o)
                    {
                    case 'v':
                    {
                        verbose = true;
                        break;
                    }
                    case 'h':
                    {
                        PrintHelp();
                        return 1;
                    }
                    case 'f':
                    {
                        force = true;
                        break;
                    }
                    case 'd':
                    {
                        makeDir = true;
                        break;
                    }
                    default:
                    {
                        throw std::runtime_error("unknown option '-" + std::string(1, o) + "'");
                    }
                    }
                }
            }
            else
            {
                std::string path = GetFullPath(arg);
                if (boost::filesystem::exists(path) || i == argc - 1 && makeDir)
                {
                    paths.push_back(path);
                }
                else
                {
                    std::string directory = Path::GetDirectoryName(path);
                    if (boost::filesystem::exists(directory))
                    {
                        std::string fileMask = Path::GetFileName(path);
                        boost::filesystem::directory_iterator it(directory);
                        while (it != boost::filesystem::directory_iterator())
                        {
                            boost::filesystem::directory_entry entry(*it);
                            if (boost::filesystem::is_regular_file(entry.path()))
                            {
                                std::string fileName = Path::GetFileName(entry.path().generic_string());
                                if (FilePatternMatch(ToUtf32(fileName), ToUtf32(fileMask)))
                                {
                                    std::string path = Path::Combine(directory, fileName);
                                    paths.push_back(path);
                                }
                            }
                            ++it;
                        }
                    }
                    else
                    {
                        if (verbose)
                        {
                            std::cout << "source directory '" + directory + "' does not exist" << std::endl;
                        }
                    }
                }
            }
        }
        if (paths.size() < 2)
        {
            PrintHelp();
            return 1;
        }
        if (paths.size() == 2 && boost::filesystem::is_regular_file(paths.front()) && !makeDir)
        {
            std::string source = GetFullPath(paths.front());
            std::string dest = GetFullPath(paths.back());
            CopyFile(source, dest, force, makeDir, verbose);
        }
        else if (boost::filesystem::is_directory(paths.back()) || makeDir)
        {
            std::string destDir = paths.back();
            for (int i = 0; i < paths.size() - 1; ++i)
            {
                std::string source = GetFullPath(paths[i]);
                std::string dest = GetFullPath(Path::Combine(destDir, Path::GetFileName(source)));
                CopyFile(source, dest, force, makeDir, verbose);
            }
        }
        else
        {
            PrintHelp();
            return 1;
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}

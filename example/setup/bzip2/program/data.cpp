#include "data.hpp"

namespace setup {

std::string PackageResourceName()
{
    return "example_package";
}

int64_t UncompressedPackageSize()
{
    return 7500135;
}

std::string UnicodeDBResourceName()
{
    return "wng_unicode_db";
}

int64_t UncompressedUnicodeDBSize()
{
    return 53233116;
}

std::string SetupIconResourceName()
{
    return "setup_icon";
}

std::string AppName()
{
    return "Example";
}

std::string AppVersion()
{
    return "1.0.0";
}

int Compression()
{
    return 2; // bzip2
}

std::string DefaultContainingDirPath()
{
    return "C:/";
}

std::string InstallDirName()
{
    return "example";
}


} // setup


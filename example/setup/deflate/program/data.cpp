#include "data.hpp"

namespace setup {

std::string PackageResourceName()
{
    return "example_package";
}

int64_t UncompressedPackageSize()
{
    return 6499769;
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
    return 1; // deflate
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


#include "data.hpp"

namespace setup {

std::string PackageResourceName()
{
    return "wingstall.package";
}

int64_t UncompressedPackageSize()
{
    return 795978818;
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
    return "Wingstall";
}

std::string AppVersion()
{
    return "2.0.0";
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
    return "wingstall";
}


} // setup


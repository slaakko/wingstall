#include <soulng/util/FileStream.hpp>
#include <soulng/util/BufferedStream.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <stdexcept>

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

bool FilesEqual(const std::string& leftFilePath, const std::string& rightFilePath)
{
    if (!boost::filesystem::exists(leftFilePath))
    {
        std::cout << "left file '" << leftFilePath << "' does not exist" << std::endl;
        return false;
    }
    if (!boost::filesystem::exists(rightFilePath))
    {
        std::cout << "right file '" << rightFilePath << "' does not exist" << std::endl;
        return false;
    }
    uintmax_t leftFileSize = boost::filesystem::file_size(leftFilePath);
    uintmax_t rightFileSize = boost::filesystem::file_size(rightFilePath);
    if (leftFileSize != rightFileSize)
    {
        std::cout << "file '" << leftFilePath << "' and '" << rightFilePath << "' sizes differ (" << leftFileSize << " != " << rightFileSize << ")" << std::endl;
        return false;
    }
    FileStream leftFile(leftFilePath, OpenMode::read | OpenMode::binary);
    BufferedStream leftBufStream(leftFile);
    FileStream rightFile(rightFilePath, OpenMode::read | OpenMode::binary);
    BufferedStream rightBufStream(rightFile);
    int64_t n = leftFileSize;
    for (int64_t i = 0; i < n; ++i)
    {
        int lx = leftBufStream.ReadByte();
        if (lx == -1)
        {
            std::cout << "unexpected end of file '" << leftFilePath << std::endl;
            return false;
        }
        int rx = rightBufStream.ReadByte();
        if (rx == -1)
        {
            std::cout << "unexpected end of file '" << rightFilePath << std::endl;
            return false;
        }
        if (lx != rx)
        {
            std::cout << "file '" << leftFilePath << "' and '" << rightFilePath << "' contents differ (byte " << i << ": " << lx << " != " << rx << ")" << std::endl;
            return false;
        }
    }
    return true;
}

void CompareDirectories(const std::string& leftDir, const std::string& rightDir, int& numDifferentDirectories, int& numDifferentFiles)
{
    bool leftExists = true;
    if (!boost::filesystem::exists(leftDir))
    {
        std::cout << "left directory '" << leftDir << "' does not exist" << std::endl;
        ++numDifferentDirectories;
        leftExists = false;
    }
    if (!boost::filesystem::exists(rightDir))
    {
        std::cout << "right directory '" << rightDir << "' does not exist" << std::endl;
        ++numDifferentDirectories;
    }
    if (leftExists)
    {
        boost::system::error_code ec;
        boost::filesystem::directory_iterator it(leftDir, ec);
        if (ec)
        {
            throw std::runtime_error("could not iterate directory '" + leftDir + "': " + PlatformStringToUtf8(ec.message()));
        }
        while (it != boost::filesystem::directory_iterator())
        {
            if (!it->path().filename_is_dot() && !it->path().filename_is_dot_dot())
            {
                std::string name = it->path().filename().generic_string();
                if (boost::filesystem::is_directory(it->path()))
                {
                    CompareDirectories(Path::Combine(leftDir, name), Path::Combine(rightDir, name), numDifferentDirectories, numDifferentFiles);
                }
                else if (boost::filesystem::is_regular_file(it->path()))
                {
                    if (!FilesEqual(Path::Combine(leftDir, name), Path::Combine(rightDir, name)))
                    {
                        ++numDifferentFiles;
                    }
                }
            }
            ++it;
        }
    }
}

int main(int argc, const char** argv)
{
    InitDone initDone;
    try
    {
        std::string left;
        std::string right;
        if (argc <= 2)
        {
            throw std::runtime_error("usage: filecompare LEFT-PATH RIGHT-PATH");
        }
        for (int i = 1; i < argc; ++i)
        {
            if (i == 1)
            { 
                left = GetFullPath(argv[i]);
            }
            else if (i == 2)
            {
                right = GetFullPath(argv[i]);
            }
        }
        if (!boost::filesystem::exists(left))
        {
            std::cout << "left path '" << left << "' does not exist" << std::endl;
            return 1;
        }
        if (!boost::filesystem::exists(right))
        {
            std::cout << "right path '" << right << "' does not exist" << std::endl;
            return 1;
        }
        boost::filesystem::file_status leftStatus = boost::filesystem::status(left);
        boost::filesystem::file_status rightStatus = boost::filesystem::status(right);
        if (leftStatus.type() == boost::filesystem::file_type::regular_file &&
            rightStatus.type() == boost::filesystem::file_type::regular_file)
        {
            if (FilesEqual(left, right))
            {
                std::cout << "files '" << left << "' and '" << right << "' are equal" << std::endl;
            }
        }
        else if (leftStatus.type() == boost::filesystem::file_type::directory_file && 
            rightStatus.type() == boost::filesystem::file_type::directory_file)
        {
            int numDifferentDirectories = 0;
            int numDifferentFiles = 0;
            CompareDirectories(left, right, numDifferentDirectories, numDifferentFiles);
            if (numDifferentDirectories == 0 && numDifferentFiles == 0)
            {
                std::cout << "all equal" << std::endl;
            }
            else
            {
                std::cout << numDifferentDirectories << " directories differ" << std::endl;
                std::cout << numDifferentFiles << " files differ" << std::endl;
            }
        }
        else
        {
            throw std::runtime_error("both arguments must be regular files or directories");
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
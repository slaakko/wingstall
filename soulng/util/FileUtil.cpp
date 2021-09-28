// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/FileUtil.hpp>
#include <soulng/util/FileStream.hpp>
#include <soulng/util/BufferedStream.hpp>
#include <soulng/util/BinaryStreamReader.hpp>
#include <soulng/util/BinaryStreamWriter.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <boost/filesystem.hpp>
#include <iostream>

namespace soulng { namespace util {

void CoutCopyFileObserver::WriteLine(const std::string& line)
{
    std::cout << line << std::endl;
}

void CopyFile(const std::string& source, const std::string& dest, bool force, bool makeDir, bool verbose)
{
    CoutCopyFileObserver observer;
    CopyFile(source, dest, force, makeDir, verbose, &observer);
}

void CopyFile(const std::string& source, const std::string& dest, bool force, bool makeDir, bool verbose, CopyFileObserver* observer)
{
    if (!boost::filesystem::exists(source))
    {
        if (verbose && observer)
        {
            observer->WriteLine("source file '" + source + "' does not exist");
        }
        return;
    }
    if (makeDir)
    {
        boost::system::error_code ec;
        boost::filesystem::create_directories(Path::GetDirectoryName(dest), ec);
        if (ec)
        {
            throw std::runtime_error("could not create directory '" + dest + "': " + PlatformStringToUtf8(ec.message()));
        }
    }
    if (force || !boost::filesystem::exists(dest) || boost::filesystem::last_write_time(source) > boost::filesystem::last_write_time(dest))
    {
        int64_t size = boost::filesystem::file_size(source);
        if (observer)
        {
            observer->FileCopyProgress(0, size);
        }
        {
            FileStream sourceFile(source, OpenMode::read | OpenMode::binary);
            BufferedStream bufferedSource(sourceFile);
            BinaryStreamReader reader(bufferedSource);
            FileStream destFile(dest, OpenMode::write | OpenMode::binary);
            BufferedStream bufferedDest(destFile);
            BinaryStreamWriter writer(bufferedDest);
            for (int64_t i = 0; i < size; ++i)
            {
                uint8_t x = reader.ReadByte();
                writer.Write(x);
                if (observer)
                {
                    observer->FileCopyProgress(i, size);
                }
            }
        }
        boost::system::error_code ec;
        boost::filesystem::last_write_time(dest, boost::filesystem::last_write_time(source), ec);
        if (ec)
        {
            throw std::runtime_error("could not set write time of file '" + dest + "': " + PlatformStringToUtf8(ec.message()));
        }
        if (verbose && observer)
        {
            observer->WriteLine(source + " -> " + dest);
        }
    }
}

} } // namespace soulng::util

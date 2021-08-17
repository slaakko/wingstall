// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wingpackage/uninstall_exe_file.hpp>
#include <wingpackage/package.hpp>
#include <wing/FileUtil.hpp>
#include <soulng/util/BinaryReader.hpp>
#include <soulng/util/FileStream.hpp>
#include <soulng/util/BufferedStream.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>

namespace wingstall { namespace wingpackage {

using namespace wing;
using namespace soulng::unicode;

UninstallExeFile::UninstallExeFile() : File(NodeKind::uninstall_exe_file, "uninstall.exe")
{
}

void UninstallExeFile::SetInfo()
{
    std::string filePath = Path::Combine(Path::Combine(WingstallRoot(), "bin"), Name());
    SetSize(boost::filesystem::file_size(filePath));
    SetTime(boost::filesystem::last_write_time(filePath));
}

void UninstallExeFile::WriteIndex(BinaryStreamWriter& writer)
{
    File::WriteIndex(writer);
}

void UninstallExeFile::ReadIndex(BinaryStreamReader& reader)
{
    File::ReadIndex(reader);
}

void UninstallExeFile::WriteData(BinaryStreamWriter& writer)
{
    std::string filePath = GetFullPath(Path::Combine(Path::Combine(WingstallRoot(), "bin"), Name()));
    FileStream fileStream(filePath, OpenMode::read | OpenMode::binary);
    BufferedStream bufferedStream(fileStream);
    int64_t size = Size();
    for (int64_t i = 0; i < size; ++i)
    {
        int x = bufferedStream.ReadByte();
        if (x == -1)
        {
            throw std::runtime_error("unexpected end of file '" + filePath + "'");
        }
        else
        {
            uint8_t b = static_cast<uint8_t>(x);
            writer.Write(b);
        }
    }
    Package* package = GetPackage();
    if (package)
    {
        package->IncrementFileContentPosition(size);
    }
}

void UninstallExeFile::ReadData(BinaryStreamReader& reader)
{
    Package* package = GetPackage();
    if (package)
    {
        package->SetFile(this);
        package->CheckInterrupted();
    }
    std::string filePath = Path::Combine(GetTargetRootDir(), Name());
    {
        FileStream fileStream(filePath, OpenMode::write | OpenMode::binary);
        BufferedStream bufferedStream(fileStream);
        int64_t n = Size();
        for (int64_t i = 0; i < n; ++i)
        {
            uint8_t x = reader.ReadByte();
            bufferedStream.Write(x);
        }
    }
    boost::system::error_code ec;
    boost::filesystem::last_write_time(filePath, Time(), ec);
    if (ec)
    {
        throw std::runtime_error("could not set write time of file '" + filePath + "': " + PlatformStringToUtf8(ec.message()));
    }
    if (package)
    {
        package->IncrementFileContentPosition(Size());
    }
}

void UninstallExeFile::Uninstall()
{
    Package* package = GetPackage();
    if (package)
    {
        package->SetFile(this);
        package->CheckInterrupted();
    }
    Node::Uninstall();
    if (Rename())
    {
        ScheduleToBeRemoved();
    }
    if (package)
    {
        package->IncrementFileIndex();
    }
}

bool UninstallExeFile::Rename()
{
    Package* package = GetPackage();
    if (package)
    {
        std::string newName = Path::ChangeExtension(Name(), ".rmv");
        try
        {
            std::string fromPath = Path::Combine(package->TargetRootDir(), Name());
            std::string toPath = Path::Combine(package->TargetRootDir(), newName);
            MoveFile(fromPath, toPath);
            SetName(newName);
        }
        catch (const std::exception& ex)
        {
            package->LogError("could not rename file '" + Name() + "' to '" + newName + "': " + ex.what());
            return false;
        }
    }
    return true;
}

void UninstallExeFile::ScheduleToBeRemoved()
{
    Package* package = GetPackage();
    if (package)
    {
        std::string filePath = Path::Combine(package->TargetRootDir(), Name());
        try
        {
            RemoveOnReboot(MakeNativePath(filePath));
        }
        catch (const std::exception& ex)
        {
            package->LogError("could not schedule file '" + filePath + "' to be removed on reboot: " + ex.what());
        }
    }
}

} } // namespace wingstall::wingpackage

// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wingpackage/uninstall_bin_file.hpp>
#include <wingpackage/package.hpp>
#include <soulng/util/BinaryReader.hpp>
#include <soulng/util/FileStream.hpp>
#include <soulng/util/BufferedStream.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>

namespace wingstall { namespace wingpackage {

using namespace soulng::unicode;

UninstallBinFile::UninstallBinFile() : File(NodeKind::uninstall_bin_file, "uninstall.bin")
{
}

void UninstallBinFile::WriteIndex(BinaryStreamWriter& writer)
{
}

void UninstallBinFile::ReadIndex(BinaryStreamReader& reader)
{
}

void UninstallBinFile::WriteData(BinaryStreamWriter& writer)
{
}

void UninstallBinFile::ReadData(BinaryStreamReader& reader)
{
}

void UninstallBinFile::Uninstall()
{
    Package* package = GetPackage();
    if (package)
    {
        package->SetFile(this);
        package->CheckInterrupted();
    }
    Node::Uninstall();
    Remove();
    if (package)
    {
        package->IncrementFileIndex();
    }
}

void UninstallBinFile::Remove()
{
    Package* package = GetPackage();
    if (package)
    {
        std::string filePath = Path::Combine(package->TargetRootDir(), Name());
        boost::system::error_code ec;
        boost::filesystem::remove(MakeNativeBoostPath(filePath), ec);
        if (ec)
        {
            package->LogError("could not remove '" + filePath + "': " + PlatformStringToUtf8(ec.message()));
        }
    }
}

} } // namespace wingstall::wingpackage

// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wingpackage/file.hpp>
#include <wingpackage/package.hpp>
#include <soulng/util/BinaryReader.hpp>
#include <soulng/util/FileStream.hpp>
#include <soulng/util/BufferedStream.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Sha1.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Time.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>

namespace wingstall { namespace wingpackage {

using namespace soulng::unicode;

File::File() : Node(NodeKind::file), size(0), time(), flags(FileFlags::none)
{
}

File::File(const std::string& name_) : Node(NodeKind::file, name_), size(0), time(), flags(FileFlags::none)
{
}

File::File(NodeKind nodeKind_, const std::string& name_) : Node(nodeKind_, name_), size(0), time(), flags(FileFlags::none)
{
}

void File::WriteIndex(BinaryStreamWriter& writer)
{
    Node::WriteIndex(writer);
    writer.Write(static_cast<uint64_t>(size));
    writer.WriteTime(time);
    writer.Write(hash);
    writer.Write(static_cast<uint8_t>(flags));
    Package* package = GetPackage();
    if (package)
    {
        package->IncrementFileContentSize(size);
    }
}

void File::ReadIndex(BinaryStreamReader& reader)
{
    Node::ReadIndex(reader);
    Package* package = GetPackage();
    if (package)
    {
        package->CheckInterrupted();
        package->IncrementFileCount();
    }
    size = reader.ReadULong();
    time = reader.ReadTime();
    hash = reader.ReadUtf8String();
    flags = static_cast<FileFlags>(reader.ReadByte());
    if (package)
    {
        package->IncrementFileContentSize(size);
    }
}

void File::WriteData(BinaryStreamWriter& writer)
{
    Sha1 sha1;
    std::string filePath = Path(GetSourceRootDir());
    FileStream fileStream(filePath, OpenMode::read | OpenMode::binary);
    BufferedStream bufferedStream(fileStream);
    int64_t n = size;
    for (int64_t i = 0; i < n; ++i)
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
            sha1.Process(b);
        }
    }
    hash = sha1.GetDigest();
    writer.Write(hash);
    Package* package = GetPackage();
    if (package)
    {
        package->IncrementFileContentPosition(size);
    }
}

void File::ReadData(BinaryStreamReader& reader)
{
    Package* package = GetPackage();
    if (package)
    {
        package->SetFile(this);
        package->CheckInterrupted();
    }
    std::string filePath = Path(GetTargetRootDir());
    bool exists = boost::filesystem::exists(MakeNativeBoostPath(filePath));
    SetFlag(FileFlags::exists, exists);
    {
        FileStream fileStream(filePath, OpenMode::write | OpenMode::binary);
        BufferedStream bufferedStream(fileStream);
        int64_t n = size;
        for (int64_t i = 0; i < n; ++i)
        {
            uint8_t x = reader.ReadByte();
            bufferedStream.Write(x);
        }
    }
    boost::system::error_code ec;
    boost::filesystem::last_write_time(MakeNativeBoostPath(filePath), time, ec);
    if (ec)
    {
        throw std::runtime_error("could not set write time of file '" + filePath + "': " + PlatformStringToUtf8(ec.message()));
    }
    hash = reader.ReadUtf8String();
    if (package)
    {
        package->IncrementFileContentPosition(size);
    }
}

std::string File::ComputeHash() const
{
    std::string filePath = Path(GetTargetRootDir());
    if (!boost::filesystem::exists(MakeNativeBoostPath(filePath)))
    {
        throw std::runtime_error("file '" + filePath + "' does not exist");
    }
    Sha1 sha1;
    FileStream fileStream(filePath, OpenMode::read | OpenMode::binary);
    BufferedStream bufferedStream(fileStream);
    int64_t n = size;
    for (int64_t i = 0; i < n; ++i)
    {
        int x = bufferedStream.ReadByte();
        if (x == -1)
        {
            throw std::runtime_error("unexpected end of file '" + filePath + "'");
        }
        uint8_t b = static_cast<uint8_t>(x);
        sha1.Process(b);
    }
    std::string h = sha1.GetDigest();
    return h;
}

void File::SetFlag(FileFlags flag, bool value) 
{ 
    if (value) flags = flags | flag; 
    else flags = flags & ~flag; 
}

bool File::Changed() const
{
    std::string filePath = Path(GetTargetRootDir());
    if (boost::filesystem::exists(MakeNativeBoostPath(filePath)))
    {
        if (size != boost::filesystem::file_size(MakeNativeBoostPath(filePath)))
        {
            return true;
        }
        std::string h = ComputeHash();
        if (h != hash)
        {
            return true;
        }
        return false;
    }
    else
    {
        return false;
    }
}

void File::Remove()
{
    Package* package = GetPackage();
    if (package)
    {
        try
        {
            std::string filePath = Path(GetTargetRootDir());
            boost::system::error_code ec;
            boost::filesystem::remove(MakeNativeBoostPath(filePath), ec);
            if (ec)
            {
                throw std::runtime_error("could not remove file '" + filePath + "': " + PlatformStringToUtf8(ec.message()));
            }
        }
        catch (const std::exception& ex)
        {
            package->LogError(ex.what());
        }
    }
    else
    {
        throw std::runtime_error("package not set");
    }
}

void File::Uninstall()
{
    Package* package = GetPackage();
    if (package)
    {
        package->SetFile(this);
        package->CheckInterrupted();
    }
    Node::Uninstall();
    if (!GetFlag(FileFlags::exists) && !Changed())
    {
        Remove();
    }
    if (package)
    {
        package->IncrementFileIndex();
    }
}

sngxml::dom::Element* File::ToXml() const
{
    sngxml::dom::Element* element = new sngxml::dom::Element(U"file");
    element->SetAttribute(U"name", ToUtf32(Name()));
    element->SetAttribute(U"size", ToUtf32(std::to_string(size)));
    element->SetAttribute(U"time", ToUtf32(TimeToString(time)));
    element->SetAttribute(U"hash", ToUtf32(hash));
    return element;
}

} } // namespace wingstall::wingpackage

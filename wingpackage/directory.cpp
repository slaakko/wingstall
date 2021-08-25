// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wingpackage/directory.hpp>
#include <wingpackage/package.hpp>
#include <wingpackage/file.hpp>
#include <wingpackage/path_matcher.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Time.hpp>
#include <boost/filesystem.hpp>

namespace wingstall { namespace wingpackage {

using namespace soulng::unicode;

Directory::Directory() : Node(NodeKind::directory), time(), flags(DirectoryFlags::none)
{
}

Directory::Directory(const std::string& name_) : Node(NodeKind::directory, name_), time(), flags(DirectoryFlags::none)
{
}

Directory::Directory(PathMatcher& pathMatcher, const std::string& name, std::time_t time_, sngxml::dom::Element* element) : Node(NodeKind::directory), time(time_), flags(DirectoryFlags::none)
{
    if (name.empty())
    {
        std::string lineColStr;
        if (element)
        {
            lineColStr = " line " + std::to_string(element->GetSourcePos().line) + ", column " + std::to_string(element->GetSourcePos().col);
        }
        throw std::runtime_error("directory element has no 'name' attribute in package XML document '" + pathMatcher.XmlFilePath() + "'" + lineColStr);
    }
    SetName(name);
    pathMatcher.BeginDirectory(Name(), element);
    if (time == std::time_t())
    {
        time = boost::filesystem::last_write_time(MakeNativeBoostPath(pathMatcher.CurrentDir()));
    }
    std::vector<DirectoryInfo> directories = pathMatcher.Directories();
    for (const auto& directoryInfo : directories)
    {
        Directory* directory = new Directory(pathMatcher, directoryInfo.name, directoryInfo.time, directoryInfo.element);
        AddDirectory(directory);
    }
    std::vector<FileInfo> files = pathMatcher.Files();
    for (const auto& fileInfo : files)
    {
        File* file = new File();
        file->SetName(fileInfo.name);
        file->SetSize(fileInfo.size);
        file->SetTime(fileInfo.time);
        AddFile(file);
    }
    pathMatcher.EndDirectory();
}

void Directory::AddDirectory(Directory* directory)
{
    directory->SetParent(this);
    directories.push_back(std::unique_ptr<Directory>(directory));
}

void Directory::AddFile(File* file)
{
    file->SetParent(this);
    files.push_back(std::unique_ptr<File>(file));
}

void Directory::WriteIndex(BinaryStreamWriter& writer)
{
    Node::WriteIndex(writer);
    writer.Write(time);
    writer.Write(static_cast<uint8_t>(flags));
    int32_t numDirectories = directories.size();
    writer.Write(numDirectories);
    for (int32_t i = 0; i < numDirectories; ++i)
    {
        Directory* directory = directories[i].get();
        wingpackage::WriteIndex(directory, writer);
    }
    int32_t numFiles = files.size();
    writer.Write(numFiles);
    for (int i = 0; i < numFiles; ++i)
    {
        File* file = files[i].get();
        wingpackage::WriteIndex(file, writer);
    }
}

void Directory::ReadIndex(BinaryStreamReader& reader)
{
    Node::ReadIndex(reader);
    Package* package = GetPackage();
    if (package)
    {
        package->SetComponent(this);
        package->CheckInterrupted();
    }
    time = reader.ReadTime();
    flags = static_cast<DirectoryFlags>(reader.ReadByte());
    int32_t numDirectories = reader.ReadInt();
    for (int32_t i = 0; i < numDirectories; ++i)
    {
        Directory* directory = BeginReadDirectory(reader);
        AddDirectory(directory);
        directory->ReadIndex(reader);
    }
    int32_t numFiles = reader.ReadInt();
    for (int32_t i = 0; i < numFiles; ++i)
    {
        File* file = BeginReadFile(reader);
        AddFile(file);
        file->ReadIndex(reader);
    }
}

int Directory::Level() const
{
    Node* parent = Parent();
    if (parent && parent->Kind() == NodeKind::directory)
    {
        return static_cast<Directory*>(parent)->Level() + 1;
    }
    else
    {
        return 0;
    }
}

void Directory::SetFlag(DirectoryFlags flag, bool value)
{
    if (value) flags = flags | flag;
    else flags = flags & ~flag;
}

void Directory::WriteData(BinaryStreamWriter& writer)
{
    for (const auto& directory : directories)
    {
        directory->WriteData(writer);
    }
    for (const auto& file : files)
    {
        file->WriteData(writer);
    }
}

void Directory::ReadData(BinaryStreamReader& reader)
{
    Package* package = GetPackage();
    if (package)
    {
        package->SetComponent(this);
        package->CheckInterrupted();
    }
    std::string directoryPath = Path(GetTargetRootDir());
    bool exists = boost::filesystem::exists(MakeNativeBoostPath(directoryPath));
    SetFlag(DirectoryFlags::exists, exists);
    boost::system::error_code ec;
    boost::filesystem::create_directories(MakeNativeBoostPath(directoryPath), ec);
    if (ec)
    {
        throw std::runtime_error("could not create directory '" + directoryPath + "': " + PlatformStringToUtf8(ec.message()));
    }
    for (const auto& directory : directories)
    {
        directory->ReadData(reader);
    }
    for (const auto& file : files)
    {
        file->ReadData(reader);
    }
    if (!exists)
    {
        boost::filesystem::last_write_time(MakeNativeBoostPath(directoryPath), time, ec);
        if (ec)
        {
            throw std::runtime_error("could not set write time of directory '" + directoryPath + "': " + PlatformStringToUtf8(ec.message()));
        }
    }
}

bool Directory::HasDirectoriesOrFiles() const
{
    Package* package = GetPackage();
    if (package)
    {
        try
        {
            std::string directoryPath = Path(GetTargetRootDir());
            if (boost::filesystem::exists(MakeNativeBoostPath(directoryPath)))
            {
                boost::system::error_code ec;
                boost::filesystem::directory_iterator it(directoryPath, ec);
                if (ec)
                {
                    throw std::runtime_error("could not iterate directory '" + directoryPath + "': " + PlatformStringToUtf8(ec.message()));
                }
                while (it != boost::filesystem::directory_iterator())
                {
                    if (boost::filesystem::is_directory(it->status()))
                    {
                        if (!it->path().filename_is_dot() && !it->path().filename_is_dot_dot())
                        {
                            return true;
                        }
                    }
                    else if (boost::filesystem::is_regular_file(it->status()))
                    {
                        return true;
                    }
                    ++it;
                }
                return false;
            }
            else
            {
                return false;
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
    return false;
}

void Directory::Remove()
{
    Package* package = GetPackage();
    if (package)
    {
        try
        {
            std::string directoryPath = Path(GetTargetRootDir());
            boost::system::error_code ec;
            boost::filesystem::remove(MakeNativeBoostPath(directoryPath), ec);
            if (ec)
            {
                throw std::runtime_error("could not remove directory '" + directoryPath + "': " + PlatformStringToUtf8(ec.message()));
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

void Directory::Uninstall()
{
    Package* package = GetPackage();
    if (package)
    {
        package->SetComponent(this);
        package->CheckInterrupted();
    }
    Node::Uninstall();
    for (const auto& file : files)
    {
        file->Uninstall();
    }
    for (const auto& directory : directories)
    {
        directory->Uninstall();
    }
    if (!GetFlag(DirectoryFlags::exists) && !HasDirectoriesOrFiles())
    {
        Remove();
    }
}

sngxml::dom::Element* Directory::ToXml() const
{
    sngxml::dom::Element* element = new sngxml::dom::Element(U"directory");
    element->SetAttribute(U"name", ToUtf32(Name()));
    element->SetAttribute(U"time", ToUtf32(TimeToString(time)));
    for (const auto& directory : directories)
    {
        sngxml::dom::Element* child = directory->ToXml();
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(child));
    }
    for (const auto& file : files)
    {
        sngxml::dom::Element* child = file->ToXml();
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(child));
    }
    return element;
}

} } // namespace wingstall::wingpackage
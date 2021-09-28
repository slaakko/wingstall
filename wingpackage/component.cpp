// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wingpackage/component.hpp>
#include <wingpackage/package.hpp>
#include <wingpackage/directory.hpp>
#include <wingpackage/file.hpp>
#include <wingpackage/path_matcher.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>

namespace wingstall { namespace wingpackage {
    
using namespace soulng::unicode;

std::string CompressionStr(Compression compression)
{
    switch (compression)
    {
        case Compression::none: return "none";
        case Compression::deflate: return "deflate";
        case Compression::bzip2: return "bzip2";
    }
    return std::string();
}

Compression ParseCompressionStr(const std::string& compressionStr)
{
    if (compressionStr == "none") return Compression::none;
    else if (compressionStr == "deflate") return Compression::deflate;
    else if (compressionStr == "bzip2") return Compression::bzip2;
    else throw std::runtime_error("invalid compression name");
}

Component::Component() : Node(NodeKind::component)
{
}

Component::Component(const std::string& name_) : Node(NodeKind::component, name_)
{
}

Component::Component(NodeKind nodeKind_, const std::string& name_) : Node(nodeKind_, name_)
{
}

Component::Component(PathMatcher& pathMatcher, sngxml::dom::Element* element) : Node(NodeKind::component)
{
    std::u32string nameAttr = element->GetAttribute(U"name");
    if (!nameAttr.empty())
    {
        SetName(ToUtf8(nameAttr));
    }
    else
    {
        throw std::runtime_error("component element has no 'name' attribute in package XML document '" + pathMatcher.XmlFilePath() + "' line " + 
            std::to_string(element->GetSourcePos().line) + ", column " + std::to_string(element->GetSourcePos().col));
    }
    pathMatcher.BeginFiles(element);
    std::vector<FileInfo> fileInfos = pathMatcher.Files();
    for (const auto& fileInfo : fileInfos)
    {
        File* file = new File(fileInfo.name);
        file->SetSize(fileInfo.size);
        file->SetTime(fileInfo.time);
        AddFile(file);
    }
    pathMatcher.EndFiles();
    std::unique_ptr<sngxml::xpath::XPathObject> directoryObject = sngxml::xpath::Evaluate(U"directory", element);
    if (directoryObject)
    {
        if (directoryObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(directoryObject.get());
            int n = nodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                sngxml::dom::Node* node = (*nodeSet)[i];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    std::u32string nameAttr = element->GetAttribute(U"name");
                    if (!nameAttr.empty())
                    {
                        std::time_t time = std::time_t();
                        Directory* directory = new Directory(pathMatcher, ToUtf8(nameAttr), time, element);
                        AddDirectory(directory);
                    }
                    else
                    {
                        throw std::runtime_error("directory element has no 'name' attribute in package XML document '" + pathMatcher.XmlFilePath() + "' line " + 
                            std::to_string(element->GetSourcePos().line) + ", column " + std::to_string(element->GetSourcePos().col));
                    }
                }
            }
        }
    }
}

void Component::RunCommands()
{
}

void Component::CreateInstallationInfo()
{
}

void Component::RemoveInstallationInfo()
{
}

void Component::AddDirectory(Directory* directory)
{
    directory->SetParent(this);
    directories.push_back(std::unique_ptr<Directory>(directory));
}

void Component::AddFile(File* file)
{
    file->SetParent(this);
    files.push_back(std::unique_ptr<File>(file));
}

void Component::Write(Streams& streams)
{
}

void Component::Read(Streams& streams)
{
}

void Component::WriteIndex(BinaryStreamWriter& writer)
{
    Package* package = GetPackage();
    if (package)
    {
        package->CheckInterrupted();
    }
    Node::WriteIndex(writer);
    int32_t numDirectories = directories.size();
    writer.Write(numDirectories);
    for (int32_t i = 0; i < numDirectories; ++i)
    {
        Directory* directory = directories[i].get();
        wingpackage::WriteIndex(directory, writer);
    }
    int32_t numFiles = files.size();
    writer.Write(numFiles);
    for (int32_t i = 0; i < numFiles; ++i)
    {
        File* file = files[i].get();
        wingpackage::WriteIndex(file, writer);
    }
}

void Component::ReadIndex(BinaryStreamReader& reader)
{
    Node::ReadIndex(reader);
    Package* package = GetPackage();
    if (package)
    {
        package->SetComponent(this);
        package->CheckInterrupted();
    }
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

void Component::WriteData(BinaryStreamWriter& writer)
{
    Package* package = GetPackage();
    if (package)
    {
        package->CheckInterrupted();
    }
    for (const auto& directory : directories)
    {
        directory->WriteData(writer);
    }
    for (const auto& file : files)
    {
        file->WriteData(writer);
    }
}

void Component::ReadData(BinaryStreamReader& reader)
{
    Package* package = GetPackage();
    if (package)
    {
        package->SetComponent(this);
        package->CheckInterrupted();
    }
    for (const auto& directory : directories)
    {
        directory->ReadData(reader);
    }
    std::string directoryPath = GetTargetRootDir();
    bool exists = boost::filesystem::exists(MakeNativeBoostPath(directoryPath));
    boost::system::error_code ec;
    boost::filesystem::create_directories(MakeNativeBoostPath(directoryPath), ec);
    if (ec)
    {
        throw std::runtime_error("could not create directory '" + directoryPath + "': " + PlatformStringToUtf8(ec.message()));
    }
    for (const auto& file : files)
    {
        file->ReadData(reader);
    }
}

void Component::Uninstall()
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
}

sngxml::dom::Element* Component::ToXml() const
{
    sngxml::dom::Element* element = new sngxml::dom::Element(U"component");
    element->SetAttribute(U"name", ToUtf32(Name()));
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

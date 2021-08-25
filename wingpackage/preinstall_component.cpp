// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wingpackage/preinstall_component.hpp>
#include <wingpackage/package.hpp>
#include <wingpackage/file.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/FileStream.hpp>
#include <soulng/util/BufferedStream.hpp>
#include <soulng/util/DeflateStream.hpp>
#include <soulng/util/BZip2Stream.hpp>
#include <soulng/util/Process.hpp>
#include <soulng/util/TextUtils.hpp>
#include <boost/filesystem.hpp>
#include <iostream>

namespace wingstall { namespace wingpackage {

using namespace soulng::unicode;

PreinstallComponent::PreinstallComponent() : Component(NodeKind::preinstall_component, "preinstall")
{
}

PreinstallComponent::PreinstallComponent(PathMatcher& pathMatcher, sngxml::dom::Element* element)
{
    std::unique_ptr<sngxml::xpath::XPathObject> includeObject = sngxml::xpath::Evaluate(U"include", element);
    if (includeObject)
    {
        if (includeObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(includeObject.get());
            int n = nodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                sngxml::dom::Node* node = (*nodeSet)[i];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* includeElement = static_cast<sngxml::dom::Element*>(node);
                    std::u32string filePathAttr = includeElement->GetAttribute(U"filePath");
                    if (!filePathAttr.empty())
                    {
                        std::string filePath = GetFullPath(Path::Combine(pathMatcher.SourceRootDir(), ToUtf8(filePathAttr)));
                        FileInfo fileInfo(filePath, boost::filesystem::file_size(MakeNativeBoostPath(filePath)), boost::filesystem::last_write_time(MakeNativeBoostPath(filePath)));
                        fileInfos.push_back(fileInfo);
                        File* file = new File(Path::GetFileName(filePath));
                        file->SetSize(fileInfo.size);
                        file->SetTime(fileInfo.time);
                        AddFile(file);
                    }
                    else
                    {
                        throw std::runtime_error("preinstall/include element has no 'filePath' attribute in package XML document '" + pathMatcher.XmlFilePath() + "' line " + 
                            std::to_string(includeElement->GetSourcePos().line) + ", column " + std::to_string(includeElement->GetSourcePos().col));
                    }
                }
            }
        }
    }
    std::unique_ptr<sngxml::xpath::XPathObject> runObject = sngxml::xpath::Evaluate(U"run", element);
    if (runObject)
    {
        if (runObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        { 
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(runObject.get());
            int n = nodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                sngxml::dom::Node* node = (*nodeSet)[i];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    std::u32string commandAttr = element->GetAttribute(U"command");
                    if (!commandAttr.empty())
                    {
                        commands.push_back(ToUtf8(commandAttr));
                    }
                    else
                    {
                        throw std::runtime_error("preinstall/run element has no 'command' attribute in package XML document '" + pathMatcher.XmlFilePath() + "' line " + 
                            std::to_string(element->GetSourcePos().line) + ", column " + std::to_string(element->GetSourcePos().col));
                    }
                }
            }
        }
    }
}

void PreinstallComponent::Write(Streams& streams)
{
    BinaryStreamWriter writer(streams.Back());
    WriteIndex(writer);
    WriteData(writer);
}

void PreinstallComponent::Read(Streams& streams)
{
    BinaryStreamReader reader(streams.Back());
    ReadIndex(reader);
    ReadData(reader);
}

void PreinstallComponent::RunCommands()
{
    Package* package = GetPackage();
    if (package)
    {
        try
        {
            int n = commands.size();
            for (int i = 0; i < n; ++i)
            {
                const std::string& command = commands[i];
                int exitCode = 0;
                try
                {
                    Process process(command, Process::Redirections::none);
                    process.WaitForExit();
                    exitCode = process.ExitCode();
                }
                catch (const std::exception& ex)
                {
                    throw std::runtime_error("error running preinstall command '" + command + "': " + ex.what());
                }
                if (exitCode != 0)
                {
                    throw std::runtime_error("error: preinstall command '" + command + "' returned nonzero exit code " + std::to_string(exitCode));
                }
            }
        }
        catch (const std::exception&)
        {
            RemovePreinstallDir();
            throw;
        }
    }
    else
    {
        throw std::runtime_error("package not set");
    }
    RemovePreinstallDir();
}

void PreinstallComponent::AddFile(File* file)
{
    file->SetParent(this);
    files.push_back(std::unique_ptr<File>(file));
}

void PreinstallComponent::WriteIndex(BinaryStreamWriter& writer)
{
    Component::WriteIndex(writer);
    int32_t numFiles = files.size();
    writer.Write(numFiles);
    for (int32_t i = 0; i < numFiles; ++i)
    {
        File* file = files[i].get();
        wingpackage::WriteIndex(file, writer);
    }
    int32_t numCommands = commands.size();
    writer.Write(numCommands);
    for (int32_t i = 0; i < numCommands; ++i)
    {
        writer.Write(commands[i]);
    }
}

void PreinstallComponent::ReadIndex(BinaryStreamReader& reader)
{
    Component::ReadIndex(reader);
    std::string preinstallDir;
    Package* package = GetPackage();
    if (package)
    {
        package->SetComponent(this);
        package->CheckInterrupted();
        preinstallDir = package->PreinstallDir();
        if (preinstallDir.empty())
        {
            throw std::runtime_error("preinstall directory not set");
        }
        boost::system::error_code ec;
        boost::filesystem::create_directories(MakeNativeBoostPath(preinstallDir), ec);
        if (ec)
        {
            throw std::runtime_error("could not create preinstall directory '" + preinstallDir + "': " + PlatformStringToUtf8(ec.message()));
        }
    }
    else
    {
        throw std::runtime_error("package not set");
    }
    int32_t numFiles = reader.ReadInt();
    for (int32_t i = 0; i < numFiles; ++i)
    {
        File* file = wingpackage::BeginReadFile(reader);
        AddFile(file);
        file->ReadIndex(reader);
    }
    int32_t numCommands = reader.ReadInt();
    for (int32_t i = 0; i < numCommands; ++i)
    {
        std::string command = reader.ReadUtf8String();
        commands.push_back(package->ExpandPath(command));
    }
}

void PreinstallComponent::WriteData(BinaryStreamWriter& writer)
{
    int32_t n = files.size();
    for (int32_t i = 0; i < n; ++i)
    {
        const FileInfo& fileInfo = fileInfos[i];
        std::string filePath = fileInfo.name; // file info 'name' contains actually full file path
        FileStream file(filePath, OpenMode::read | OpenMode::binary);
        BufferedStream bufferedFile(file);
        BinaryStreamReader reader(bufferedFile);
        int64_t size = fileInfo.size;
        for (int64_t i = 0; i < size; ++i)
        {
            uint8_t x = reader.ReadByte();
            writer.Write(x);
        }
    }
}

void PreinstallComponent::ReadData(BinaryStreamReader& reader)
{
    Package* package = GetPackage();
    std::string preinstallDir;
    if (package)
    {
        package->CheckInterrupted();
        preinstallDir = package->PreinstallDir();
    }
    else
    {
        throw std::runtime_error("package not set");
    }
    int32_t n = files.size();
    for (int32_t i = 0; i < n; ++i)
    {
        File* file = files[i].get();
        std::string filePath = Path::Combine(preinstallDir, file->Name());
        {
            FileStream fileStream(filePath, OpenMode::write | OpenMode::binary);
            BufferedStream bufferedStream(fileStream);
            int64_t size = file->Size();
            for (int64_t i = 0; i < size; ++i)
            {
                uint8_t x = reader.ReadByte();
                bufferedStream.Write(x);
            }
        }
    }
}

void PreinstallComponent::RemovePreinstallDir()
{
    try
    {
        Package* package = GetPackage();
        if (package)
        {
            boost::filesystem::remove_all(MakeNativeBoostPath(package->PreinstallDir()));
        }
    }
    catch (...)
    {
    }
}

} } // namespace wingstall::wingpackage

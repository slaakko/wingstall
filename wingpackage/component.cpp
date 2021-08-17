// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wingpackage/component.hpp>
#include <wingpackage/package.hpp>
#include <wingpackage/directory.hpp>
#include <wingpackage/path_matcher.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/Unicode.hpp>

namespace wingstall { namespace wingpackage {
    
using namespace soulng::unicode;

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
        throw std::runtime_error("component element has no 'name' attribute in package XML document '" + pathMatcher.XmlFilePath() + "'");
    }
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
                        throw std::runtime_error("directory element has no 'name' attribute in package XML document '" + pathMatcher.XmlFilePath() + "'");
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

void Component::Write(Streams& streams)
{
}

void Component::Read(Streams& streams)
{
}

void Component::WriteIndex(BinaryStreamWriter& writer)
{
    Node::WriteIndex(writer);
    int32_t numDirectories = directories.size();
    writer.Write(numDirectories);
    for (int32_t i = 0; i < numDirectories; ++i)
    {
        Directory* directory = directories[i].get();
        wingpackage::WriteIndex(directory, writer);
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
}

void Component::WriteData(BinaryStreamWriter& writer)
{
    for (const auto& directory : directories)
    {
        directory->WriteData(writer);
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
    return element;
}

} } // namespace wingstall::wingpackage

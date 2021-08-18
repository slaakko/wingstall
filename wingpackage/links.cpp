// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wingpackage/links.hpp>
#include <wingpackage/package.hpp>
#include <wingpackage/path_matcher.hpp>
#include <wing/Shell.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

namespace wingstall { namespace wingpackage {

using namespace wing;
using namespace soulng::unicode;

Link::Link() : Node(NodeKind::link, "link"), iconIndex(0)
{
}

Link::Link(const std::string& linkFilePath_, const std::string& path_, const std::string& arguments_, const std::string& workingDirectory_, 
    const std::string& description_, const std::string& iconPath_, int iconIndex_) : 
    Node(NodeKind::link, "link"), 
    linkFilePath(linkFilePath_), path(path_), arguments(arguments_), workingDirectory(workingDirectory_), description(description_), iconPath(iconPath_), iconIndex(iconIndex_)
{
}

Link::Link(PathMatcher& pathMatcher, sngxml::dom::Element* element) : Node(NodeKind::link, "link"), iconIndex(0)
{
    std::u32string linkFilePathAttr = element->GetAttribute(U"linkFilePath");
    if (!linkFilePathAttr.empty())
    {
        linkFilePath = ToUtf8(linkFilePathAttr);
    }
    else
    {
        throw std::runtime_error("link element has no 'linkFilePath' attribute in package XML document '" + pathMatcher.XmlFilePath() + "'");
    }
    std::u32string pathAttr = element->GetAttribute(U"path");
    if (!pathAttr.empty())
    {
        path = ToUtf8(pathAttr);
    }
    else
    {
        throw std::runtime_error("link element has no 'path' attribute in package XML document '" + pathMatcher.XmlFilePath() + "'");
    }
    std::u32string argumentsAttr = element->GetAttribute(U"arguments");
    if (!argumentsAttr.empty())
    {
        arguments = ToUtf8(argumentsAttr);
    }
    std::u32string workingDirectoryAttr = element->GetAttribute(U"workingDirectory");
    if (!workingDirectoryAttr.empty())
    {
        workingDirectory = ToUtf8(workingDirectoryAttr);
    }
    std::u32string descriptionAttr = element->GetAttribute(U"description");
    if (!descriptionAttr.empty())
    {
        description = ToUtf8(descriptionAttr);
    }
    std::u32string iconPathAttr = element->GetAttribute(U"iconPath");
    if (!iconPathAttr.empty())
    {
        iconPath = ToUtf8(iconPathAttr);
    }
    std::u32string iconIndexAttr = element->GetAttribute(U"iconIndex");
    if (!iconIndexAttr.empty())
    {
        iconIndex = boost::lexical_cast<int>(ToUtf8(iconIndexAttr));
    }
}

void Link::SetOld(Link* old_)
{
    old.reset(old_);
    old->SetParent(this);
}

void Link::WriteIndex(BinaryStreamWriter& writer)
{
    Node::WriteIndex(writer);
    writer.Write(linkFilePath);
    writer.Write(expandedLinkFilePath);
    writer.Write(path);
    writer.Write(expandedPath);
    writer.Write(arguments);
    writer.Write(workingDirectory);
    writer.Write(expandedWorkingDirectory);
    writer.Write(description);
    writer.Write(iconPath);
    writer.Write(expandedIconPath);
    writer.Write(iconIndex);
    bool hasOld = old != nullptr;
    writer.Write(hasOld);
    if (hasOld)
    {
        old->WriteIndex(writer);
    }
}

void Link::ReadIndex(BinaryStreamReader& reader)
{
    Node::ReadIndex(reader);
    linkFilePath = reader.ReadUtf8String();
    expandedLinkFilePath = reader.ReadUtf8String();
    path = reader.ReadUtf8String();
    expandedPath = reader.ReadUtf8String();
    arguments = reader.ReadUtf8String();
    workingDirectory = reader.ReadUtf8String();
    expandedWorkingDirectory = reader.ReadUtf8String();
    description = reader.ReadUtf8String();
    iconPath = reader.ReadUtf8String();
    expandedIconPath = reader.ReadUtf8String();
    iconIndex = reader.ReadInt();
    bool hasOld = reader.ReadBool();
    if (hasOld)
    {
        SetOld(new Link());
        old->ReadIndex(reader);
    }
}

sngxml::dom::Element* Link::ToXml() const
{
    sngxml::dom::Element* element = new sngxml::dom::Element(U"link");
    element->SetAttribute(U"linkFilePath", ToUtf32(linkFilePath));
    element->SetAttribute(U"path", ToUtf32(path));
    element->SetAttribute(U"arguments", ToUtf32(arguments));
    element->SetAttribute(U"workingDirecory", ToUtf32(workingDirectory));
    element->SetAttribute(U"description", ToUtf32(description));
    element->SetAttribute(U"iconPath", ToUtf32(iconPath));
    element->SetAttribute(U"iconIndex", ToUtf32(std::to_string(iconIndex)));
    return element;
}

void Link::Create(const std::string& expandedLinkFilePath, const std::string& expandedPath, const std::string& expandedWorkingDirectory, const std::string& expandedIconPath)
{
    try
    {
        std::string directoryPath = Path::GetDirectoryName(GetFullPath(expandedLinkFilePath));
        boost::system::error_code ec;
        boost::filesystem::create_directories(MakeNativeBoostPath(directoryPath), ec);
        if (ec)
        {
            throw std::runtime_error("could not create directory '" + directoryPath + "': " + PlatformStringToUtf8(ec.message()));
        }
        CreateShellLink(expandedLinkFilePath, expandedPath, arguments, expandedWorkingDirectory, description, expandedIconPath, iconIndex);
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("could not create link '" + expandedLinkFilePath + "': " + ex.what());
    }
}

void Link::Install()
{
    Package* package = GetPackage();
    if (package)
    {
        package->CheckInterrupted();
        expandedLinkFilePath = package->ExpandPath(linkFilePath);
        expandedPath = package->ExpandPath(path);
        expandedWorkingDirectory.clear();
        if (!workingDirectory.empty())
        {
            expandedWorkingDirectory = package->ExpandPath(workingDirectory);
        }
        expandedIconPath.clear();
        if (!iconPath.empty())
        {
            expandedIconPath = package->ExpandPath(iconPath);
        }
        if (boost::filesystem::exists(MakeNativeBoostPath(expandedLinkFilePath)))
        {
            try
            {
                std::string p;
                std::string args;
                std::string wdir;
                std::string desc;
                std::string iconP;
                int index = 0;
                GetShellLinkData(expandedLinkFilePath, p, args, wdir, desc, iconP, index);
                SetOld(new Link(expandedLinkFilePath, p, args, wdir, desc, iconP, index));
            }
            catch (const std::exception& ex)
            {
                package->LogError("could not get old link data from file '" + expandedLinkFilePath + "': " + ex.what());
            }
        }
        Create(expandedLinkFilePath, expandedPath, expandedWorkingDirectory, expandedIconPath);
    }
    else
    {
        throw std::runtime_error("package not set");
    }
}

void Link::Uninstall()
{
    Node::Uninstall();
    Package* package = GetPackage();
    if (package)
    {
        package->CheckInterrupted();
        try
        {
            if (boost::filesystem::exists(MakeNativeBoostPath(expandedLinkFilePath)))
            {
                boost::system::error_code ec;
                boost::filesystem::remove(MakeNativeBoostPath(expandedLinkFilePath), ec);
                if (ec)
                {
                    throw std::runtime_error(PlatformStringToUtf8(ec.message()));
                }
            }
        }
        catch (const std::exception& ex)
        {
            package->LogError("could not remove link file '" + expandedLinkFilePath + "': " + ex.what());
        }
        try
        {
            if (old)
            {
                old->Create(old->LinkFilePath(), old->Path(), old->WorkingDirectory(), old->IconPath());
            }
        }
        catch (const std::exception& ex)
        {
            package->LogError("could not recreate old link '" + old->LinkFilePath() + "': " + ex.what());
        }
    }
    else
    {
        throw std::runtime_error("package not set");
    }
}

LinkDirectory::LinkDirectory() : Node(NodeKind::linkDirectory, "linkDirectory"), flags(LinkDirectoryFlags::none)
{
}

LinkDirectory::LinkDirectory(const std::string& path_) : Node(NodeKind::linkDirectory, "linkDirectory"), path(path_), flags(LinkDirectoryFlags::none)
{
}

LinkDirectory::LinkDirectory(PathMatcher& pathMatcher, sngxml::dom::Element* element) : Node(NodeKind::linkDirectory, "linkDirectory"), flags(LinkDirectoryFlags::none)
{
    std::u32string pathAttr = element->GetAttribute(U"path");
    if (!pathAttr.empty())
    {
        path = ToUtf8(pathAttr);
    }
    else
    {
        throw std::runtime_error("link directory element has no 'path' attribute in package XML document '" + pathMatcher.XmlFilePath() + "'");
    }
}

void LinkDirectory::WriteIndex(BinaryStreamWriter& writer)
{
    Node::WriteIndex(writer);
    writer.Write(path);
    writer.Write(expandedPath);
    writer.Write(static_cast<uint8_t>(flags));
}

void LinkDirectory::ReadIndex(BinaryStreamReader& reader)
{
    Node::ReadIndex(reader);
    path = reader.ReadUtf8String();
    expandedPath = reader.ReadUtf8String();
    flags = static_cast<LinkDirectoryFlags>(reader.ReadByte());
}

sngxml::dom::Element* LinkDirectory::ToXml() const
{
    sngxml::dom::Element* element = new sngxml::dom::Element(U"linkDirectory");
    element->SetAttribute(U"path", ToUtf32(path));
    return element;
}

void LinkDirectory::SetFlag(LinkDirectoryFlags flag, bool value)
{
    if (value) flags = flags | flag;
    else flags = flags & ~flag;
}

void LinkDirectory::Create()
{
    boost::system::error_code ec;
    boost::filesystem::create_directories(MakeNativeBoostPath(expandedPath), ec);
    if (ec)
    {
        throw std::runtime_error("could not create directory '" + expandedPath + "': " + PlatformStringToUtf8(ec.message()));
    }
}

void LinkDirectory::Remove()
{
    Package* package = GetPackage();
    if (package)
    {
        try
        {
            boost::system::error_code ec;
            boost::filesystem::remove(MakeNativeBoostPath(expandedPath), ec);
            if (ec)
            { 
                throw std::runtime_error("could not remove directory '" + expandedPath + "': " + PlatformStringToUtf8(ec.message()));
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

void LinkDirectory::Install()
{
    Package* package = GetPackage();
    if (package)
    {
        package->CheckInterrupted();
        expandedPath = package->ExpandPath(path);
        std::u16string expPath = ToUtf16(expandedPath);
        boost::filesystem::path p((const wchar_t*)expPath.c_str());
        SetFlag(LinkDirectoryFlags::exists, boost::filesystem::exists(p));
        if (!GetFlag(LinkDirectoryFlags::exists))
        {
            Create();
        }
    }
    else
    {
        throw std::runtime_error("package not set");
    }
}

void LinkDirectory::Uninstall()
{
    Node::Uninstall();
    Package* package = GetPackage();
    if (package)
    {
        package->CheckInterrupted();
        if (!GetFlag(LinkDirectoryFlags::exists))
        {
            Remove();
        }
    }
}

Links::Links() : Node(NodeKind::links, "links")
{
}

Links::Links(PathMatcher& pathMatcher, sngxml::dom::Element* element) : Node(NodeKind::links, "links")
{
    std::unique_ptr<sngxml::xpath::XPathObject> linkDirectoryObject = sngxml::xpath::Evaluate(U"linkDirectory", element);
    if (linkDirectoryObject)
    {
        if (linkDirectoryObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(linkDirectoryObject.get());
            int n = nodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                sngxml::dom::Node* node = (*nodeSet)[i];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    LinkDirectory* linkDirectory = new LinkDirectory(pathMatcher, element);
                    AddLinkDirectory(linkDirectory);
                }
            }
        }
    }
    std::unique_ptr<sngxml::xpath::XPathObject> linkObject = sngxml::xpath::Evaluate(U"link", element);
    if (linkObject)
    {
        if (linkObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(linkObject.get());
            int n = nodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                sngxml::dom::Node* node = (*nodeSet)[i];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    Link* link = new Link(pathMatcher, element);
                    AddLink(link);
                }
            }
        }
    }
}

void Links::AddLinkDirectory(LinkDirectory* linkDirectory)
{
    linkDirectory->SetParent(this);
    linkDirectories.push_back(std::unique_ptr<LinkDirectory>(linkDirectory));
}

void Links::AddLink(Link* link)
{
    link->SetParent(this);
    links.push_back(std::unique_ptr<Link>(link));
}

sngxml::dom::Element* Links::ToXml() const
{
    sngxml::dom::Element* element = new sngxml::dom::Element(U"links");
    for (const auto& linkDirectory : linkDirectories)
    {
        sngxml::dom::Element* child = linkDirectory->ToXml();
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(child));
    }
    for (const auto& link : links)
    {
        sngxml::dom::Element* child = link->ToXml();
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(child));
    }
    return element;
}

void Links::WriteIndex(BinaryStreamWriter& writer)
{
    Node::WriteIndex(writer);
    int32_t numLinkDirectories = linkDirectories.size();
    writer.Write(numLinkDirectories);
    for (int32_t i = 0; i < numLinkDirectories; ++i)
    {
        LinkDirectory* linkDirectory = linkDirectories[i].get();
        linkDirectory->WriteIndex(writer);
    }
    int32_t numLinks = links.size();
    writer.Write(numLinks);
    for (int32_t i = 0; i < numLinks; ++i)
    {
        Link* link = links[i].get();
        link->WriteIndex(writer);
    }
}

void Links::ReadIndex(BinaryStreamReader& reader)
{
    Node::ReadIndex(reader);
    int32_t numLinkDirectories = reader.ReadInt();
    for (int32_t i = 0; i < numLinkDirectories; ++i)
    {
        LinkDirectory* linkDirectory = new LinkDirectory();
        AddLinkDirectory(linkDirectory);
        linkDirectory->ReadIndex(reader);
    }
    int32_t numLinks = reader.ReadInt();
    for (int32_t i = 0; i < numLinks; ++i)
    {
        Link* link = new Link();
        AddLink(link);
        link->ReadIndex(reader);
    }
}

void Links::Install()
{
    Package* package = GetPackage();
    if (package)
    {
        package->CheckInterrupted();
        for (const auto& linkDirectory : linkDirectories)
        {
            linkDirectory->Install();
        }
        for (const auto& link : links)
        {
            link->Install();
        }
    }
    else
    {
        throw std::runtime_error("package not set");
    }
}

void Links::Uninstall()
{
    Node::Uninstall();
    Package* package = GetPackage();
    if (package)
    {
        package->CheckInterrupted();
        for (const auto& link : links)
        {
            link->Uninstall();
        }
        for (const auto& linkDirectory : linkDirectories)
        {
            linkDirectory->Uninstall();
        }
    }
    else
    {
        throw std::runtime_error("package not set");
    }
}

} } // namespace wingstall::wingpackage

// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/links.hpp>
#include <package_editor/error.hpp>
#include <wing/ImageList.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/lexical_cast.hpp>

namespace wingstall { namespace package_editor {

using namespace soulng::unicode;

Links::Links() : Node(NodeKind::links, std::string())
{
}

Links::Links(const std::string& packageXMLFilePath, sngxml::dom::Element* element) : Node(NodeKind::links, std::string())
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
                    LinkDirectory* linkDirectory = new LinkDirectory(packageXMLFilePath, element);
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
                    Link* link = new Link(packageXMLFilePath, element);
                    AddLink(link);
                }
            }
        }
    }
}

TreeViewNode* Links::ToTreeViewNode(TreeView* view)
{
    TreeViewNode* node = new TreeViewNode("Links");
    SetTreeViewNode(node);
    node->SetData(this);
    ImageList* imageList = view->GetImageList();
    if (imageList)
    {
        node->SetImageIndex(imageList->GetImageIndex("links.bitmap"));
    }
    for (const auto& linkDirectory : linkDirectories)
    {
        node->AddChild(linkDirectory->ToTreeViewNode(view));
    }
    for (const auto& link : links)
    {
        node->AddChild(link->ToTreeViewNode(view));
    }
    return node;
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

LinkDirectory::LinkDirectory() : Node(NodeKind::linkDirectory, std::string())
{
}

LinkDirectory::LinkDirectory(const std::string& packageXMLFilePath, sngxml::dom::Element* element) : Node(NodeKind::linkDirectory, std::string())
{
    std::u32string pathAttr = element->GetAttribute(U"path");
    if (!pathAttr.empty())
    {
        path = ToUtf8(pathAttr);
    }
    else
    {
        throw PackageXMLException("'linkDirectory' element has no 'path' attribute", packageXMLFilePath, element);
    }
}

TreeViewNode* LinkDirectory::ToTreeViewNode(TreeView* view)
{
    TreeViewNode* node = new TreeViewNode(path);
    SetTreeViewNode(node);
    node->SetData(this);
    ImageList* imageList = view->GetImageList();
    if (imageList)
    {
        node->SetImageIndex(imageList->GetImageIndex("linked.folder.closed.bitmap"));
    }
    return node;
}

Link::Link() : Node(NodeKind::link, std::string()), iconIndex(0)
{
}

Link::Link(const std::string& packageXMLFilePath, sngxml::dom::Element* element) : Node(NodeKind::link, std::string()), iconIndex(0)
{
    std::u32string linkFilePathAttr = element->GetAttribute(U"linkFilePath");
    if (!linkFilePathAttr.empty())
    {
        linkFilePath = ToUtf8(linkFilePathAttr);
    }
    else
    {
        throw PackageXMLException("'link' element has no 'linkFilePath' attribute", packageXMLFilePath, element);
    }
    std::u32string pathAttr = element->GetAttribute(U"path");
    if (!pathAttr.empty())
    {
        path = ToUtf8(pathAttr);
    }
    else
    {
        throw PackageXMLException("'link' element has no 'path' attribute", packageXMLFilePath, element);
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

TreeViewNode* Link::ToTreeViewNode(TreeView* view)
{
    TreeViewNode* node = new TreeViewNode(linkFilePath);
    SetTreeViewNode(node);
    node->SetData(this);
    ImageList* imageList = view->GetImageList();
    if (imageList)
    {
        node->SetImageIndex(imageList->GetImageIndex("shortcut.bitmap"));
    }
    return node;
}

} } // wingstall::package_editor


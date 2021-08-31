// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_LINKS_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_LINKS_INCLUDED
#include <package_editor/node.hpp>
#include <sngxml/dom/Element.hpp>

namespace wingstall { namespace package_editor {

class LinkDirectory;
class Link;

class Links : public Node
{
public:
    Links();
    Links(const std::string& packageXMLFilePath, sngxml::dom::Element* element);
    TreeViewNode* ToTreeViewNode(TreeView* view);
    std::string ImageName() const override { return "links.bitmap"; }
    void AddLinkDirectory(LinkDirectory* linkDirectory);
    void AddLink(Link* link);
private:
    std::vector<std::unique_ptr<LinkDirectory>> linkDirectories;
    std::vector<std::unique_ptr<Link>> links;
};

class LinkDirectory : public Node
{
public:
    LinkDirectory();
    LinkDirectory(const std::string& packageXMLFilePath, sngxml::dom::Element* element);
    TreeViewNode* ToTreeViewNode(TreeView* view);
private:
    std::string path;
};

class Link : public Node
{
public:
    Link();
    Link(const std::string& packageXMLFilePath, sngxml::dom::Element* element);
    TreeViewNode* ToTreeViewNode(TreeView* view);
private:
    std::string linkFilePath;
    std::string path;
    std::string arguments;
    std::string workingDirectory;
    std::string description;
    std::string iconPath;
    int iconIndex;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_LINKS_INCLUDED

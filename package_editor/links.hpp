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
class LinkDirectories;
class Shortcuts;
class Shortcut;

class Links : public Node
{
public:
    Links();
    Links(const std::string& packageXMLFilePath, sngxml::dom::Element* element);
    TreeViewNode* ToTreeViewNode(TreeView* view);
    Control* CreateView(ImageList* imageList) override;
    std::string ImageName() const override { return "links.bitmap"; }
private:
    std::unique_ptr<LinkDirectories> linkDirectories;
    std::unique_ptr<Shortcuts> shortcuts;
};

class LinkDirectories : public Node
{
public:
    LinkDirectories();
    void AddLinkDirectory(LinkDirectory* linkDirectory);
    TreeViewNode* ToTreeViewNode(TreeView* view);
    Control* CreateView(ImageList* imageList) override;
    std::string ImageName() const override { return "linked.folder.closed.bitmap"; }
    int Count() const override;
    int IndexOf(const Node* child) const override;
    Node* GetNode(int index) const override;
    std::unique_ptr<Node> RemoveChild(int index) override;
    void InsertBefore(int index, std::unique_ptr<Node>&& child);
    void InsertAfter(int index, std::unique_ptr<Node>&& child);
private:
    std::vector<std::unique_ptr<LinkDirectory>> linkDirectories;
};

class LinkDirectory : public Node
{
public:
    LinkDirectory();
    LinkDirectory(const std::string& packageXMLFilePath, sngxml::dom::Element* element);
    TreeViewNode* ToTreeViewNode(TreeView* view);
    std::string ImageName() const override { return"linked.folder.closed.bitmap"; }
    void SetData(ListViewItem* item, ImageList* imageList) override;
private:
    std::string path;
};

class Shortcuts : public Node
{
public:
    Shortcuts();
    void AddShortcut(Shortcut* shortcut);
    TreeViewNode* ToTreeViewNode(TreeView* view);
    Control* CreateView(ImageList* imageList) override;
    std::string ImageName() const override { return "shortcut.bitmap"; }
    int Count() const override;
    int IndexOf(const Node* child) const override;
    Node* GetNode(int index) const override;
    std::unique_ptr<Node> RemoveChild(int index) override;
    void InsertBefore(int index, std::unique_ptr<Node>&& child);
    void InsertAfter(int index, std::unique_ptr<Node>&& child);
private:
    std::vector<std::unique_ptr<Shortcut>> shortcuts;
};

class Shortcut : public Node
{
public:
    Shortcut();
    Shortcut(const std::string& packageXMLFilePath, sngxml::dom::Element* element);
    TreeViewNode* ToTreeViewNode(TreeView* view);
    std::string ImageName() const override { return"shortcut.bitmap"; }
    void SetData(ListViewItem* item, ImageList* imageList) override;
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

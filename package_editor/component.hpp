// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_COMPONENT_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_COMPONENT_INCLUDED
#include <package_editor/directory.hpp>
#include <package_editor/file.hpp>

namespace wingstall { namespace package_editor {

class Component;

class Components : public Node
{
public:
    Components();
    void AddComponent(Component* component);
    TreeViewNode* ToTreeViewNode(TreeView* view);
    Control* CreateView(ImageList* imageList) override;
    std::string ImageName() const override { return "components.bitmap"; }
    int Count() const override;
    int IndexOf(const Node* child) const override;
    Node* GetNode(int index) const override;
    std::unique_ptr<Node> RemoveChild(int index) override;
    void InsertBefore(int index, std::unique_ptr<Node>&& child);
    void InsertAfter(int index, std::unique_ptr<Node>&& child);
private:
    std::vector<std::unique_ptr<Component>> components;
};

class Component : public Node
{
public:
    Component();
    Component(const std::string& packageXMLFilePath, sngxml::dom::Element* element);
    TreeViewNode* ToTreeViewNode(TreeView* view);
    Control* CreateView(ImageList* imageList) override;
    void AddDirectory(Directory* directory);
    void AddFile(File* file);
    std::string ImageName() const override { return "component.bitmap"; }
    int Count() const override;
    int IndexOf(const Node* child) const override;
    Node* GetNode(int index) const override;
    std::unique_ptr<Node> RemoveChild(int index) override;
    void InsertBefore(int index, std::unique_ptr<Node>&& child);
    void InsertAfter(int index, std::unique_ptr<Node>&& child);
    bool CanMoveUp(const Node* child) const override;
    bool CanMoveDown(const Node* child) const override;
private:
    std::vector<std::unique_ptr<Directory>> directories;
    std::vector<std::unique_ptr<File>> files;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_COMPONENT_INCLUDED

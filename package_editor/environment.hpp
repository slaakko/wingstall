// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_ENVIRONMENT_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_ENVIRONMENT_INCLUDED
#include <package_editor/node.hpp>
#include <sngxml/dom/Element.hpp>

namespace wingstall { namespace package_editor {

class EnvironmentVariable;
class PathDirectory;

class Environment : public Node
{
public:
    Environment();
    Environment(const std::string& packageXMLFilePath, sngxml::dom::Element* element);
    TreeViewNode* ToTreeViewNode(TreeView* view);
    Control* CreateView(ImageList* imageList) override;
    std::string ImageName() const override { return "environment.bitmap"; }
    void AddEnvironentVariable(EnvironmentVariable* environmentVariable);
    void AddPathDirectory(PathDirectory* pathDirectory);
    int Count() const override;
    int IndexOf(const Node* child) const override;
    Node* GetNode(int index) const override;
    std::unique_ptr<Node> RemoveChild(int index) override;
    void InsertBefore(int index, std::unique_ptr<Node>&& child);
    void InsertAfter(int index, std::unique_ptr<Node>&& child);
    bool CanMoveUp(const Node* child) const override;
    bool CanMoveDown(const Node* child) const override;
    bool HasEnvironmentVariable(const std::string& name) const;
    bool HasPathDirectory(const std::string& value) const;
    bool CanAdd() const override { return true; }
    void AddNew(NodeKind kind) override;
    void AddAddNewMenuItems(ContextMenu* contextMenu, std::vector<std::unique_ptr<ClickAction>>& clickActions) override;
private:
    std::vector<std::unique_ptr<EnvironmentVariable>> environmentVariables;
    std::vector<std::unique_ptr<PathDirectory>> pathDirectories;
};

class EnvironmentVariable : public Node
{
public:
    EnvironmentVariable();
    EnvironmentVariable(const std::string& packageXMLFilePath, sngxml::dom::Element* element);
    const std::string& Value() const { return value; }
    void SetValue(const std::string& value_);
    std::string ImageName() const override { return "environment.var.bitmap"; }
    void SetData(ListViewItem* item, ImageList* imageList) override;
    TreeViewNode* ToTreeViewNode(TreeView* view);
    bool Leaf() const override { return true; }
    bool CanOpen() const override { return false; }
    bool CanEdit() const override { return true; }
    void Edit() override;
    void ExecuteDefaultAction() override { Edit(); }
private:
    std::string value;
};

class PathDirectory : public Node
{
public:
    PathDirectory();
    PathDirectory(const std::string& packageXMLFilePath, sngxml::dom::Element* element);
    const std::string& Value() const { return value; }
    void SetValue(const std::string& value_);
    std::string ImageName() const override { return "path.directory.bitmap"; }
    void SetData(ListViewItem* item, ImageList* imageList) override;
    TreeViewNode* ToTreeViewNode(TreeView* view);
    bool Leaf() const override { return true; }
    bool CanOpen() const override { return false; }
    bool CanEdit() const override { return true; }
    void Edit() override;
    void ExecuteDefaultAction() override { Edit(); }
private:
    std::string value;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_ENVIRONMENT_INCLUDED


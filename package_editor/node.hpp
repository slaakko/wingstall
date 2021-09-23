// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_NODE_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_NODE_INCLUDED
#include <wing/ImageList.hpp>
#include <wing/ListView.hpp>
#include <wing/TreeView.hpp>
#include <wing/Menu.hpp>

namespace wingstall {namespace package_editor {

using namespace wing;

enum class NodeKind : int
{
    package, properties, components, component, directory, file, rules, rule, content, environment, environmentVariable, pathDirectory, links, linkDirectories, linkDirectory, shortcuts, shortcut,
    engineVariables, engineVariable
};

enum class MenuItemsKind 
{
    allMenuItems, newMenuItems
};

class Package;
class PackageContentView;
class MainWindow;

class Node
{
public:
    Node(NodeKind kind_, const std::string& name_);
    virtual ~Node();
    NodeKind Kind() const { return kind; }
    const std::string& Name() const { return name; }
    void SetName(const std::string& name_);
    Node* Parent() const { return parent; }
    void SetParent(Node* parent_) { parent = parent_; }
    MainWindow* GetMainWindow();
    TreeViewNode* GetTreeViewNode() const { return treeViewNode; }
    void SetTreeViewNode(TreeViewNode* treeViewNode_) { treeViewNode = treeViewNode_; }
    void Explore();
    void ViewContent();
    void Open();
    void Select();
    virtual void DefaultAction() { return Open(); }
    virtual Package* GetPackage() const;
    virtual Control* CreateView(ImageList* imageList);
    virtual std::string ImageName() const;
    virtual bool CanDisable() const { return false; }
    virtual bool IsDisabled() const { return false; }
    virtual void SetData(ListViewItem* item, ImageList* imageList);
    virtual int Count() const;
    virtual int IndexOf(const Node* child) const;
    virtual Node* GetNode(int index) const;
    virtual bool HasNode(const std::string& name) const;
    virtual void AddNew(NodeKind kind);
    virtual void Edit();
    virtual std::unique_ptr<Node> RemoveChild(int index);
    virtual void InsertBefore(int index, std::unique_ptr<Node>&& child);
    virtual void InsertAfter(int index, std::unique_ptr<Node>&& child);
    virtual bool CanOpen() const { return true; }
    virtual bool CanAdd() const { return false; }
    virtual bool CanEdit() const { return false; }
    virtual bool CanMoveUp(const Node* child) const;
    virtual bool CanMoveDown(const Node* child) const;
    bool CanRemove() const;
    bool CanMoveUp() const;
    bool CanMoveDown() const;
    void Remove();
    void MoveUp();
    void MoveDown();
    void AddMenuItems(ContextMenu* contextMenu, std::vector<std::unique_ptr<ClickAction>>& clickActions, MenuItemsKind menuItemsKind);
    virtual void AddAddNewMenuItems(ContextMenu* contextMenu, std::vector<std::unique_ptr<ClickAction>>& clickActions);
private:
    NodeKind kind;
    std::string name;
    Node* parent;
    TreeViewNode* treeViewNode;
    ListViewItem* listViewItem;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_NODE_INCLUDED

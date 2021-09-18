// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_NODE_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_NODE_INCLUDED
#include <wing/ImageList.hpp>
#include <wing/ListView.hpp>
#include <wing/TreeView.hpp>

namespace wingstall {namespace package_editor {

using namespace wing;

enum class NodeKind : int
{
    package, properties, components, component, directory, file, rules, rule, content, environment, environmentVariable, pathDirectory, links, linkDirectories, linkDirectory, shortcuts, shortcut,
    engineVariables, engineVariable
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
    MainWindow* GetMainWindow() const;
    TreeViewNode* GetTreeViewNode() const { return treeViewNode; }
    void SetTreeViewNode(TreeViewNode* treeViewNode_) { treeViewNode = treeViewNode_; }
    void Explore();
    void ViewContent();
    void Open();
    virtual Package* GetPackage() const;
    virtual Control* CreateView(ImageList* imageList);
    virtual std::string ImageName() const;
    virtual bool CanDisable() const { return false; }
    virtual bool IsDisabled() const { return false; }
    virtual void SetData(ListViewItem* item, ImageList* imageList);
private:
    NodeKind kind;
    std::string name;
    Node* parent;
    TreeViewNode* treeViewNode;
    PackageContentView* view;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_NODE_INCLUDED

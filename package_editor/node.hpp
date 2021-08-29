// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_NODE_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_NODE_INCLUDED
#include <wing/TreeView.hpp>

namespace wingstall {namespace package_editor {

using namespace wing;

enum class NodeKind : int
{
    package, properties, components, component, directory, file, rules, rule, content, environment, links
};

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
    TreeViewNode* GetTreeViewNode() const { return treeViewNode; }
    void SetTreeViewNode(TreeViewNode* treeViewNode_) { treeViewNode = treeViewNode_; }
private:
    NodeKind kind;
    std::string name;
    Node* parent;
    TreeViewNode* treeViewNode;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_NODE_INCLUDED

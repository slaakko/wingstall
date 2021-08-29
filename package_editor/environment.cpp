// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/environment.hpp>

namespace wingstall { namespace package_editor {

Environment::Environment() : Node(NodeKind::environment, std::string())
{
}

TreeViewNode* Environment::ToTreeViewNode(TreeView* view)
{
    TreeViewNode* node = new TreeViewNode("Environment");
    SetTreeViewNode(node);
    node->SetData(this);
    node->SetImageIndex(view->GetImageIndex("environment.bitmap"));
    return node;
}

} } // wingstall::package_editor

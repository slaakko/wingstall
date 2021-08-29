// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/links.hpp>

namespace wingstall { namespace package_editor {

Links::Links() : Node(NodeKind::links, std::string())
{
}

TreeViewNode* Links::ToTreeViewNode(TreeView* view)
{
    TreeViewNode* node = new TreeViewNode("Links");
    SetTreeViewNode(node);
    node->SetData(this);
    node->SetImageIndex(view->GetImageIndex("links.bitmap"));
    return node;
}

} } // wingstall::package_editor


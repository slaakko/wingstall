// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_LINKS_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_LINKS_INCLUDED
#include <package_editor/node.hpp>

namespace wingstall { namespace package_editor {

class Links : public Node
{
public:
    Links();
    TreeViewNode* ToTreeViewNode(TreeView* view);
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_LINKS_INCLUDED

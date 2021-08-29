// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_ENVIRONMENT_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_ENVIRONMENT_INCLUDED
#include <package_editor/node.hpp>
#include <sngxml/dom/Element.hpp>

namespace wingstall { namespace package_editor {

class Environment : public Node
{
public:
    Environment();
    TreeViewNode* ToTreeViewNode(TreeView* view);

};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_ENVIRONMENT_INCLUDED

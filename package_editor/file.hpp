// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_FILE_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_FILE_INCLUDED
#include <package_editor/node.hpp>
#include <sngxml/dom/Element.hpp>
#include <wing/TreeView.hpp>

namespace wingstall { namespace package_editor {

using namespace wing;

class File : public Node
{
public:
    File();
    File(const std::string& packageXMLFilePath, sngxml::dom::Element* element);
    TreeViewNode* ToTreeViewNode(TreeView* view);
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_FILE_INCLUDED

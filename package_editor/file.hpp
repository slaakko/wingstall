// =================================
// Copyright (c) 2022 Seppo Laakko
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
    File(const std::string& name_);
    File(const std::string& packageXMLFilePath, sngxml::dom::Element* element);
    sngxml::dom::Element* ToXml() const;
    std::string ImageName() const override { return "file.bitmap"; }
    bool Leaf() const override { return true; }
    bool CanOpen() const override { return false; }
    TreeViewNode* ToTreeViewNode(TreeView* view);
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_FILE_INCLUDED

// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/file.hpp>
#include <package_editor/error.hpp>
#include <soulng/util/Unicode.hpp>

namespace wingstall { namespace package_editor {

using namespace soulng::unicode;

File::File() : Node(NodeKind::file, std::string())
{
}

File::File(const std::string& packageXMLFilePath, sngxml::dom::Element* element) : Node(NodeKind::file, std::string())
{
    std::u32string nameAttr = element->GetAttribute(U"name");
    if (!nameAttr.empty())
    {
        SetName(ToUtf8(nameAttr));
    }
    else
    {
        throw PackageXMLException("'file' element has no 'name' attribute", packageXMLFilePath, element);
    }
}

TreeViewNode* File::ToTreeViewNode(TreeView* view)
{
    TreeViewNode* node = new TreeViewNode(Name());
    SetTreeViewNode(node);
    node->SetData(this);
    node->SetImageIndex(view->GetImageIndex("file.bitmap"));
    return node;
}

} } // wingstall::package_editor
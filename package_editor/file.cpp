// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/file.hpp>
#include <package_editor/error.hpp>
#include <wing/ImageList.hpp>
#include <soulng/util/Unicode.hpp>

namespace wingstall { namespace package_editor {

using namespace soulng::unicode;

File::File(const std::string& name_) : Node(NodeKind::file, name_)
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

sngxml::dom::Element* File::ToXml() const
{
    sngxml::dom::Element* element = new sngxml::dom::Element(U"file");
    element->SetAttribute(U"name", ToUtf32(Name()));
    return element;
}

TreeViewNode* File::ToTreeViewNode(TreeView* view)
{
    TreeViewNode* node = new TreeViewNode(Name());
    SetTreeViewNode(node);
    node->SetData(this);
    ImageList* imageList = view->GetImageList();
    if (imageList)
    {
        node->SetImageIndex(imageList->GetImageIndex("file.bitmap"));
    }
    return node;
}

} } // wingstall::package_editor
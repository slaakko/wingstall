// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/node.hpp>

namespace wingstall { namespace package_editor {

Node::Node(NodeKind kind_, const std::string& name_) : kind(kind_), name(name_), parent(nullptr), treeViewNode(nullptr)
{
}

Node::~Node()
{
}

void Node::SetName(const std::string& name_)
{
    name = name_;
}

Package* Node::GetPackage() const
{
    if (parent)
    {
        return parent->GetPackage();
    }
    return nullptr;
}

Control* Node::CreateView(ImageList* imageList)
{
    return nullptr;
}

std::string Node::ImageName() const
{
    return std::string();
}

void Node::SetData(ListViewItem* item, ImageList* imageList)
{
    item->SetData(this);
    item->SetColumnValue(0, Name());
    std::string imageName = ImageName();
    if (!imageName.empty())
    {
        item->SetImageIndex(imageList->GetImageIndex(imageName));
        if (CanDisable())
        {
            item->SetDisabledImageIndex(imageList->GetDisabledImageIndex(imageName));
            if (IsDisabled())
            {
                item->SetState(ListViewItemState::disabled);
            }
        }
    }
}

} } // wingstall::package_editor

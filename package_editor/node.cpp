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

} } // wingstall::package_editor

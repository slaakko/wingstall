// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wingpackage/node.hpp>

namespace wingstall { namespace wingpackage {

Node::Node(NodeKind kind_) : kind(kind_), parent(nullptr)
{
}

Node::Node(NodeKind kind_, const std::string& name_) : kind(kind_), name(name_), parent(nullptr)
{
}

Node::~Node()
{
}

void Node::SetName(const std::string& name_)
{
    name = name_;
}

std::string Node::FullName() const
{
    std::string fullName;
    if (parent)
    {
        fullName = parent->FullName();
    }
    if (!fullName.empty())
    {
        fullName.append(1, '.');
    }
    fullName.append(name);
    return fullName;
}

std::string Node::Path() const
{
    std::string path;
    if (parent && parent->Kind() == NodeKind::directory)
    {
        path.append(parent->Path());
    }
    if (!path.empty())
    {
        path.append(1, '/');
    }
    path.append(name);
    return path;
}

std::string Node::Path(const std::string& root) const
{
    std::string path(root);
    if (!path.empty() && path.back() != '/')
    {
        path.append(1, '/');
    }
    path.append(Path());
    return path;
}

Package* Node::GetPackage() const
{
    if (parent)
    {
        return parent->GetPackage();
    }
    else
    {
        return nullptr;
    }
}

const std::string& Node::GetSourceRootDir() const
{
    static std::string emptySourceRootDir;
    if (parent)
    {
        return parent->GetSourceRootDir();
    }
    return emptySourceRootDir;
}

const std::string& Node::GetTargetRootDir() const
{
    static std::string emptyTargetRootDir;
    if (parent)
    {
        return parent->GetTargetRootDir();
    }
    return emptyTargetRootDir;
}

void Node::WriteIndex(BinaryStreamWriter& writer)
{
    writer.Write(name);
}

void Node::ReadIndex(BinaryStreamReader& reader)
{
    name = reader.ReadUtf8String();
}

void Node::WriteData(BinaryStreamWriter& writer)
{
}

void Node::ReadData(BinaryStreamReader& reader)
{
}

} } // namespace wingstall::wingpackage

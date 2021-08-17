// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wingpackage/node.hpp>
#include <wingpackage/component.hpp>
#include <wingpackage/directory.hpp>
#include <wingpackage/file.hpp>
#include <wingpackage/environment.hpp>
#include <wingpackage/variable.hpp>
#include <wingpackage/links.hpp>
#include <wingpackage/preinstall_component.hpp>
#include <wingpackage/uninstall_component.hpp>
#include <wingpackage/uninstall_exe_file.hpp>
#include <wingpackage/uninstall_bin_file.hpp>
#include <wingpackage/installation_component.hpp>

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

void Node::Uninstall()
{
}

Node* CreateNode(NodeKind kind)
{
    switch (kind)
    {
        case NodeKind::component: return new Component();
        case NodeKind::directory: return new Directory();
        case NodeKind::file: return new File();
        case NodeKind::environment: return new Environment();
        case NodeKind::environmentVariable: return new EnvironmentVariable();
        case NodeKind::pathDirectory: return new PathDirectory();
        case NodeKind::variables: return new Variables();
        case NodeKind::variable: return new Variable();
        case NodeKind::links: return new Links();
        case NodeKind::link: return new Link();
        case NodeKind::preinstall_component: return new PreinstallComponent();
        case NodeKind::uninstall_component: return new UninstallComponent();
        case NodeKind::uninstall_exe_file: return new UninstallExeFile();
        case NodeKind::uninstall_bin_file: return new UninstallBinFile();
        case NodeKind::installation_component: return new InstallationComponent();
    }
    return nullptr;
}

void WriteIndex(Node* node, BinaryStreamWriter& writer)
{
    NodeKind kind = node->Kind();
    writer.Write(static_cast<uint8_t>(kind));
    node->WriteIndex(writer);
}

Component* BeginReadComponent(BinaryStreamReader& reader)
{
    NodeKind kind = static_cast<NodeKind>(reader.ReadByte());
    std::unique_ptr<Node> node(CreateNode(kind));
    if (node)
    {
        if (node->Kind() == NodeKind::component || node->Kind() == NodeKind::uninstall_component || node->Kind() == NodeKind::preinstall_component || node->Kind() == NodeKind::installation_component)
        {
            return static_cast<Component*>(node.release());
        }
        else
        {
            throw std::runtime_error("component node expected");
        }
    }
    else
    {
        throw std::runtime_error("component node expected");
    }
}

Directory* BeginReadDirectory(BinaryStreamReader& reader)
{
    NodeKind kind = static_cast<NodeKind>(reader.ReadByte());
    std::unique_ptr<Node> node(CreateNode(kind));
    if (node)
    {
        if (node->Kind() == NodeKind::directory)
        {
            return static_cast<Directory*>(node.release());
        }
        else
        {
            throw std::runtime_error("directory node expected");
        }
    }
    else
    {
        throw std::runtime_error("directory node expected");
    }
}

File* BeginReadFile(BinaryStreamReader& reader)
{
    NodeKind kind = static_cast<NodeKind>(reader.ReadByte());
    std::unique_ptr<Node> node(CreateNode(kind));
    if (node)
    {
        if (node->Kind() == NodeKind::file || node->Kind() == NodeKind::uninstall_exe_file || node->Kind() == NodeKind::uninstall_bin_file)
        {
            return static_cast<File*>(node.release());
        }
        else
        {
            throw std::runtime_error("file node expected");
        }
    }
    else
    {
        throw std::runtime_error("file node expected");
    }
}

} } // namespace wingstall::wingpackage

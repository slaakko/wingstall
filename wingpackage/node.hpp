// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_WINGPACKAGE_NODE_INCLUDED
#define WINGSTALL_WINGPACKAGE_NODE_INCLUDED
#include <wingpackage/api.hpp>
#include <sngxml/dom/Element.hpp>
#include <soulng/util/BinaryStreamWriter.hpp>
#include <soulng/util/BinaryStreamReader.hpp>
#include <memory>
#include <vector>

namespace wingstall { namespace wingpackage {

using namespace soulng::util;

enum class NodeKind : uint8_t
{
    package, component, directory, file, environment, environmentVariable, pathDirectory, variables, variable, links, linkDirectory, link, preinstall_component, 
    uninstall_component, uninstall_exe_file, uninstall_bin_file, installation_component
};

class Component;
class Directory;
class File;

class Package;

class Node
{
public:
    Node(NodeKind kind_);
    Node(NodeKind kind_, const std::string& name_);
    virtual ~Node();
    NodeKind Kind() const { return kind; }
    const std::string& Name() const { return name; }
    void SetName(const std::string& name_);
    std::string FullName() const;
    std::string Path() const;
    std::string Path(const std::string& root) const;
    Node* Parent() const { return parent; }
    void SetParent(Node* parent_) { parent = parent_; }
    virtual Package* GetPackage() const;
    virtual const std::string& GetSourceRootDir() const;
    virtual const std::string& GetTargetRootDir() const;
    virtual void WriteIndex(BinaryStreamWriter& writer);
    virtual void ReadIndex(BinaryStreamReader& reader);
    virtual void WriteData(BinaryStreamWriter& writer);
    virtual void ReadData(BinaryStreamReader& reader);
    virtual void Uninstall();
    virtual sngxml::dom::Element* ToXml() const = 0;
private:
    NodeKind kind;
    std::string name;
    Node* parent;
};

Node* CreateNode(NodeKind kind);
void WriteIndex(Node* node, BinaryStreamWriter& writer);
Component* BeginReadComponent(BinaryStreamReader& reader);
Directory* BeginReadDirectory(BinaryStreamReader& reader);
File* BeginReadFile(BinaryStreamReader& reader);

} } // namespace wingstall::wingpackage

#endif // WINGSTALL_WINGPACKAGE_NODE_INCLUDED

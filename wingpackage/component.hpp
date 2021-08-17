// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_WINGPACKAGE_COMPONENT_INCLUDED
#define WINGSTALL_WINGPACKAGE_COMPONENT_INCLUDED
#include <wingpackage/node.hpp>
#include <sngxml/dom/Element.hpp>
#include <soulng/util/FileStream.hpp>

namespace wingstall { namespace wingpackage {

using namespace soulng::util;

class Directory;
class PathMatcher;

enum class Compression : uint8_t
{
    none, deflate, bzip2
};

class Component : public Node
{
public:
    Component();
    Component(const std::string& name_);
    Component(NodeKind nodeKind_, const std::string& name_);
    Component(PathMatcher& pathMatcher, sngxml::dom::Element* element);
    virtual void RunCommands();
    virtual void CreateInstallationInfo();
    virtual void RemoveInstallationInfo();
    const std::vector<std::unique_ptr<Directory>>& Directories() const { return directories; }
    void AddDirectory(Directory* directory);
    virtual void Write(Streams& streams);
    virtual void Read(Streams& streams);
    void WriteIndex(BinaryStreamWriter& writer) override;
    void ReadIndex(BinaryStreamReader& reader) override;
    void WriteData(BinaryStreamWriter& writer) override;
    void ReadData(BinaryStreamReader& reader) override;
    void Uninstall() override;
    sngxml::dom::Element* ToXml() const override;
private:
    std::vector<std::unique_ptr<Directory>> directories;
};

} } // namespace wingstall::wingpackage

#endif // WINGSTALL_WINGPACKAGE_COMPONENT_INCLUDED

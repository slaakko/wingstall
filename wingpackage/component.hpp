// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_WINGPACKAGE_COMPONENT_INCLUDED
#define WINGSTALL_WINGPACKAGE_COMPONENT_INCLUDED
#include <wingpackage/node.hpp>
#include <sngxml/dom/Element.hpp>

namespace wingstall { namespace wingpackage {

using namespace soulng::util;

class Directory;
class PathMatcher;

class Component : public Node
{
public:
    Component();
    Component(const std::string& name_);
    Component(PathMatcher& pathMatcher, sngxml::dom::Element* element);
    const std::vector<std::unique_ptr<Directory>>& Directories() const { return directories; }
    void AddDirectory(Directory* directory);
    void WriteIndex(BinaryStreamWriter& writer) override;
    void ReadIndex(BinaryStreamReader& reader) override;
    void WriteData(BinaryStreamWriter& writer) override;
    void ReadData(BinaryStreamReader& reader) override;
    void Uninstall();
    sngxml::dom::Element* ToXml() const override;
private:
    std::vector<std::unique_ptr<Directory>> directories;
};

} } // namespace wingstall::wingpackage

#endif // WINGSTALL_WINGPACKAGE_COMPONENT_INCLUDED

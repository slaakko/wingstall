// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_WINGPACKAGE_PREINSTALL_COMPONENT_INCLUDED
#define WINGSTALL_WINGPACKAGE_PREINSTALL_COMPONENT_INCLUDED
#include <wingpackage/directory.hpp>
#include <wingpackage/file.hpp>
#include <wingpackage/component.hpp>
#include <wingpackage/path_matcher.hpp>
#include <soulng/util/FileStream.hpp>

namespace wingstall { namespace wingpackage {

class PreinstallComponent : public Component
{
public:
    PreinstallComponent();
    PreinstallComponent(PathMatcher& pathMatcher, sngxml::dom::Element* element);
    void Write(Streams& streams) override;
    void Read(Streams& streams) override;
    void RunCommands() override;
    void AddFile(File* file);
    void WriteIndex(BinaryStreamWriter& writer) override;
    void ReadIndex(BinaryStreamReader& reader) override;
    void WriteData(BinaryStreamWriter& writer) override;
    void ReadData(BinaryStreamReader& reader) override;
private:
    std::vector<FileInfo> fileInfos;
    std::vector<std::unique_ptr<File>> files;
    std::vector<std::string> commands;
    void RemovePreinstallDir();
};

} } // namespace wingstall::wingpackage

#endif // WINGSTALL_WINGPACKAGE_PREINSTALL_COMPONENT_INCLUDED

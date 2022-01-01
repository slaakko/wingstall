// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_WINGPACKAGE_DIRECTORY_INCLUDED
#define WINGSTALL_WINGPACKAGE_DIRECTORY_INCLUDED
#include <wingpackage/node.hpp>
#include <sngxml/dom/Element.hpp>

namespace wingstall { namespace wingpackage {

using namespace soulng::util;

class File;
class PathMatcher;

enum class DirectoryFlags : uint8_t
{
    none = 0, exists = 1 << 0
};

inline DirectoryFlags operator|(DirectoryFlags left, DirectoryFlags right)
{
    return DirectoryFlags(uint8_t(left) | uint8_t(right));
}

inline DirectoryFlags operator&(DirectoryFlags left, DirectoryFlags right)
{
    return DirectoryFlags(uint8_t(left) & uint8_t(right));
}

inline DirectoryFlags operator~(DirectoryFlags flags)
{
    return DirectoryFlags(~uint8_t(flags));
}

class Directory : public Node
{
public:
    Directory();
    Directory(const std::string& name_);
    Directory(PathMatcher& pathMatcher, const std::string& name, std::time_t time_, sngxml::dom::Element* element);
    int Level() const;
    DirectoryFlags Flags() const { return flags; }
    void SetFlags(DirectoryFlags flags_) { flags = flags_; }
    void SetFlag(DirectoryFlags flag, bool value);
    bool GetFlag(DirectoryFlags flag) const { return (flags & flag) != DirectoryFlags::none; }
    const std::vector<std::unique_ptr<Directory>>& Directories() const { return directories; }
    void AddDirectory(Directory* directory);
    const std::vector<std::unique_ptr<File>>& Files() const { return files; }
    void AddFile(File* file);
    void WriteIndex(BinaryStreamWriter& writer) override;
    void ReadIndex(BinaryStreamReader& reader) override;
    void WriteData(BinaryStreamWriter& writer) override;
    void ReadData(BinaryStreamReader& reader) override;
    bool HasDirectoriesOrFiles();
    void Remove();
    void Uninstall() override;
    sngxml::dom::Element* ToXml() const override;
private:
    std::time_t time;
    DirectoryFlags flags;
    std::vector<std::unique_ptr<Directory>> directories;
    std::vector<std::unique_ptr<File>> files;
};

} } // namespace wingstall::wingpackage

#endif // WINGSTALL_WINGPACKAGE_DIRECTORY_INCLUDED

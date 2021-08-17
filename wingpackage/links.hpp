// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_WINGPACKAGE_LINKGS_INCLUDED
#define WINGSTALL_WINGPACKAGE_LINKGS_INCLUDED
#include <wingpackage/node.hpp>
 
namespace wingstall { namespace wingpackage {

class PathMatcher;

class Link : public Node
{
public:
    Link();
    Link(const std::string& linkFilePath_, const std::string& path_, const std::string& arguments_, const std::string& workingDirectory_, 
        const std::string& description_, const std::string& iconPath_, int iconIndex_);
    Link(PathMatcher& pathMatcher, sngxml::dom::Element* element);
    void SetOld(Link* old_);
    void WriteIndex(BinaryStreamWriter& writer) override;
    void ReadIndex(BinaryStreamReader& reader) override;
    sngxml::dom::Element* ToXml() const override;
    void Create(const std::string& expandedLinkFilePath, const std::string& expandedPath, const std::string& expandedWorkingDirectory, const std::string& expandedIconPath);
    void Install();
    void Uninstall() override;
    const std::string& LinkFilePath() const { return linkFilePath; }
    const std::string& Path() const { return path; }
    const std::string& Arguments() const { return arguments; }
    const std::string& WorkingDirectory() const { return workingDirectory; }
    const std::string& Description() const { return description; }
    const std::string& IconPath() const { return iconPath; }
    int IconIndex() const { return iconIndex; }
private:
    std::string linkFilePath;
    std::string expandedLinkFilePath;
    std::string path;
    std::string expandedPath;
    std::string arguments;
    std::string workingDirectory;
    std::string expandedWorkingDirectory;
    std::string description;
    std::string iconPath;
    std::string expandedIconPath;
    int iconIndex;
    std::unique_ptr<Link> old;
};

enum class LinkDirectoryFlags : uint8_t
{
    none = 0, exists = 1 << 0
};

inline LinkDirectoryFlags operator|(LinkDirectoryFlags left, LinkDirectoryFlags right)
{
    return LinkDirectoryFlags(uint8_t(left) | uint8_t(right));
}

inline LinkDirectoryFlags operator&(LinkDirectoryFlags left, LinkDirectoryFlags right)
{
    return LinkDirectoryFlags(uint8_t(left) & uint8_t(right));
}

inline LinkDirectoryFlags operator~(LinkDirectoryFlags flags)
{
    return LinkDirectoryFlags(~uint8_t(flags));
}

class LinkDirectory : public Node
{
public:
    LinkDirectory();
    LinkDirectory(const std::string& path_);
    LinkDirectory(PathMatcher& pathMatcher, sngxml::dom::Element* element);
    LinkDirectoryFlags Flags() const { return flags; }
    void SetFlags(LinkDirectoryFlags flags_) { flags = flags_; }
    void SetFlag(LinkDirectoryFlags flag, bool value);
    bool GetFlag(LinkDirectoryFlags flag) const { return (flags & flag) != LinkDirectoryFlags::none; }
    void WriteIndex(BinaryStreamWriter& writer) override;
    void ReadIndex(BinaryStreamReader& reader) override;
    sngxml::dom::Element* ToXml() const override;
    void Create();
    void Remove();
    void Install();
    void Uninstall() override;
    const std::string& Path() const { return path; }
    void SetPath(const std::string& path_) { path = path_; }
private:
    LinkDirectoryFlags flags;
    std::string path;
    std::string expandedPath;
};

class Links : public Node
{
public:
    Links();
    Links(PathMatcher& pathMatcher, sngxml::dom::Element* element);
    void AddLinkDirectory(LinkDirectory* linkDirectory);
    void AddLink(Link* link);
    sngxml::dom::Element* ToXml() const override;
    void WriteIndex(BinaryStreamWriter& writer);
    void ReadIndex(BinaryStreamReader& reader);
    void Install();
    void Uninstall() override;
private:
    std::vector<std::unique_ptr<LinkDirectory>> linkDirectories;
    std::vector<std::unique_ptr<Link>> links;
};

} } // namespace wingstall::wingpackage

#endif // WINGSTALL_WINGPACKAGE_LINKGS_INCLUDED

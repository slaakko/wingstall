// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_WINGPACKAGE_ENVIRONMENT_INCLUDED
#define WINGSTALL_WINGPACKAGE_ENVIRONMENT_INCLUDED
#include <wingpackage/node.hpp>

namespace wingstall { namespace wingpackage {

class PathMatcher;

enum class EnvironmentVariableFlags : uint8_t
{
    none = 0, exists = 1 << 0
};

inline EnvironmentVariableFlags operator|(EnvironmentVariableFlags left, EnvironmentVariableFlags right)
{
    return EnvironmentVariableFlags(uint8_t(left) | uint8_t(right));
}

inline EnvironmentVariableFlags operator&(EnvironmentVariableFlags left, EnvironmentVariableFlags right)
{
    return EnvironmentVariableFlags(uint8_t(left) & uint8_t(right));
}

inline EnvironmentVariableFlags operator~(EnvironmentVariableFlags flags)
{
    return EnvironmentVariableFlags(~uint8_t(flags));
}

class EnvironmentVariable : public Node
{
public:
    EnvironmentVariable();
    EnvironmentVariable(const std::string& name_, const std::string& value_);
    EnvironmentVariable(PathMatcher& pathMatcher, sngxml::dom::Element* element);
    EnvironmentVariableFlags Flags() const { return flags; }
    void SetFlags(EnvironmentVariableFlags flags_) { flags = flags_; }
    void SetFlag(EnvironmentVariableFlags flag, bool value);
    bool GetFlag(EnvironmentVariableFlags flag) const { return (flags & flag) != EnvironmentVariableFlags::none; }
    const std::string& Value() const { return value; }
    void SetValue(const std::string& value_) { value = value_; }
    sngxml::dom::Element* ToXml() const override;
    void WriteIndex(BinaryStreamWriter& writer) override;
    void ReadIndex(BinaryStreamReader& reader) override;
    void SetOldValue();
    void Remove();
    void Install();
    void Uninstall() override;
private:
    EnvironmentVariableFlags flags;
    std::string oldValue;
    std::string value;
};

enum class PathDirectoryFlags : uint8_t
{
    none = 0, exists = 1 << 0
};

inline PathDirectoryFlags operator|(PathDirectoryFlags left, PathDirectoryFlags right)
{
    return PathDirectoryFlags(uint8_t(left) | uint8_t(right));
}

inline PathDirectoryFlags operator&(PathDirectoryFlags left, PathDirectoryFlags right)
{
    return PathDirectoryFlags(uint8_t(left) & uint8_t(right));
}

inline PathDirectoryFlags operator~(PathDirectoryFlags flags)
{
    return PathDirectoryFlags(~uint8_t(flags));
}

class PathDirectory : public Node
{
public:
    PathDirectory();
    PathDirectory(const std::string& value_);
    PathDirectory(PathMatcher& pathMatcher, sngxml::dom::Element* element);
    const std::string& Value() const { return value; }
    void SetValue(const std::string& value_) { value = value_; }
    PathDirectoryFlags Flags() const { return flags; }
    void SetFlags(PathDirectoryFlags flags_) { flags = flags_; }
    void SetFlag(PathDirectoryFlags flag, bool value);
    bool GetFlag(PathDirectoryFlags flag) const { return (flags & flag) != PathDirectoryFlags::none; }
    sngxml::dom::Element* ToXml() const override;
    void WriteIndex(BinaryStreamWriter& writer) override;
    void ReadIndex(BinaryStreamReader& reader) override;
    void Install();
    void Remove();
    void Uninstall() override;
private:
    PathDirectoryFlags flags;
    std::string value;
    std::string expandedValue;
};

class Environment : public Node
{
public:
    Environment();
    Environment(PathMatcher& pathMatcher, sngxml::dom::Element* element);
    void AddVariable(EnvironmentVariable* variable);
    void AddPathDirectory(PathDirectory* pathDirectory);
    sngxml::dom::Element* ToXml() const override;
    void WriteIndex(BinaryStreamWriter& writer) override;
    void ReadIndex(BinaryStreamReader& reader) override;
    void Install();
    void Uninstall() override;
private:
    std::vector<std::unique_ptr<EnvironmentVariable>> variables;
    std::vector<std::unique_ptr<PathDirectory>> pathDirectories;
};

} } // namespace wingstall::wingpackage

#endif // WINGSTALL_WINGPACKAGE_ENVIRONMENT_INCLUDED

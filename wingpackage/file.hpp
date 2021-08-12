// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_WINGPACKAGE_FILE_INCLUDED
#define WINGSTALL_WINGPACKAGE_FILE_INCLUDED
#include <wingpackage/node.hpp>
#include <ctime>

namespace wingstall { namespace wingpackage {

using namespace soulng::util;

enum class FileFlags : uint8_t
{
    none = 0, exists = 1 << 0
};

inline FileFlags operator|(FileFlags left, FileFlags right)
{
    return FileFlags(uint8_t(left) | uint8_t(right));
}

inline FileFlags operator&(FileFlags left, FileFlags right)
{
    return FileFlags(uint8_t(left) & uint8_t(right));
}

inline FileFlags operator~(FileFlags flags)
{
    return FileFlags(~uint8_t(flags));
}

class File : public Node
{
public:
    File();
    File(const std::string& name_);
    uintmax_t Size() const { return size; }
    void SetSize(uintmax_t size_) { size = size_; }
    time_t Time() const { return time; }
    void SetTime(const time_t time_) { time = time_; }
    const std::string& Hash() const { return hash; }
    void SetHash(const std::string& hash_) { hash = hash_; }
    std::string ComputeHash() const;
    FileFlags Flags() const { return flags; }
    void SetFlags(FileFlags flags_) { flags = flags_; }
    void SetFlag(FileFlags flag, bool value);
    bool GetFlag(FileFlags flag) const { return (flags & flag) != FileFlags::none;  }
    bool Changed() const;
    void WriteIndex(BinaryStreamWriter& writer) override;
    void ReadIndex(BinaryStreamReader& reader) override;
    void WriteData(BinaryStreamWriter& writer) override;
    void ReadData(BinaryStreamReader& reader) override;
    void Remove();
    void Uninstall();
    sngxml::dom::Element* ToXml() const override;
private:
    uintmax_t size;
    std::time_t time;
    std::string hash;
    FileFlags flags;
};

} } // namespace wingstall::wingpackage

#endif // WINGSTALL_WINGPACKAGE_FILE_INCLUDED

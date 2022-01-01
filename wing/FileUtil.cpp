// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/FileUtil.hpp>
#include <soulng/util/Unicode.hpp>

namespace wing {

using namespace soulng::unicode;

void MoveFile(const std::string& from, const std::string& to)
{
    MoveFile(from, to, true, true);
}

void MoveFile(const std::string& from, const std::string& to, bool allowCopy, bool replaceExisting)
{
    DWORD flags = 0;
    if (allowCopy)
    {
        flags = flags | MOVEFILE_COPY_ALLOWED;
    }
    if (replaceExisting)
    {
        flags = flags | MOVEFILE_REPLACE_EXISTING;
    }
    std::u16string fromP = ToUtf16(from);
    std::u16string toP = ToUtf16(to);
    bool succeeded = MoveFileExW((LPCWSTR)fromP.c_str(), (LPCWSTR)toP.c_str(), flags);
    if (!succeeded)
    {
        throw WindowsException(GetLastError());
    }
}

void RemoveOnReboot(const std::string& path)
{
    std::u16string pth = ToUtf16(path);
    DWORD flags = MOVEFILE_DELAY_UNTIL_REBOOT;
    bool succeeded = MoveFileExW((LPCWSTR)pth.c_str(), nullptr, flags);
    if (!succeeded)
    {
        throw WindowsException(GetLastError());
    }
}

} // wing

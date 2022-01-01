// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_FILE_UTIL_INCLUDED
#define SOULNG_UTIL_FILE_UTIL_INCLUDED
#include <soulng/util/UtilApi.hpp>
#include <string>

#undef CopyFile

namespace soulng { namespace util {

class UTIL_API CopyFileObserver
{
public:
    virtual void WriteLine(const std::string& line) {}
    virtual void FileCopyProgress(int64_t pos, int64_t size) {}
};

class UTIL_API CoutCopyFileObserver : public CopyFileObserver
{
public:
    void WriteLine(const std::string& line) override;
};

UTIL_API void CopyFile(const std::string& source, const std::string& dest, bool force, bool makeDir, bool verbose);
UTIL_API void CopyFile(const std::string& source, const std::string& dest, bool force, bool makeDir, bool verbose, CopyFileObserver* observer);

} } // namespace soulng::util

#endif // SOULNG_UTIL_FILE_UTIL_INCLUDED

// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WING_FILE_UTIL_INCLUDED
#define WING_FILE_UTIL_INCLUDED
#include <wing/Wing.hpp>

#undef MoveFile

namespace wing {

void WING_API MoveFile(const std::string& from, const std::string& to);
void WING_API MoveFile(const std::string& from, const std::string& to, bool allowCopy, bool replaceExisting);
void WING_API RemoveOnReboot(const std::string& path);

} // wing

#endif // WING_FILE_UTIL_INCLUDED

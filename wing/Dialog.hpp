// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WING_DIALOG_INCLUDED
#define WING_DIALOG_INCLUDED
#include <wing/WingApi.hpp>
#include <string>
#include <vector>
#include <Windows.h>

namespace wing {

// OFN_PATHMUSTEXIST, OFN_FILEMUSTEXIST

WING_API bool OpenFileName(HWND windowHandle, const std::vector<std::pair<std::string, std::string>>& descriptionFilterPairs, const std::string& initialDirectory,
    const std::string& defaultFilePath, const std::string& defaultExtension,
    uint32_t flags, std::string& firstPath, std::string& currentDirectory, std::vector<std::string>& fileNames);

} // wing

#endif // WING_DIALOG_INCLUDED

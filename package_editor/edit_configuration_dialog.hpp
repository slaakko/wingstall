// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_EDIT_CONFIGURATION_DIALOG_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_EDIT_CONFIGURATION_DIALOG_INCLUDED
#include <wing/Button.hpp>
#include <wing/Window.hpp>
#include <wing/TextBox.hpp>

namespace wingstall { namespace package_editor {

using namespace wing;

class EditConfigurationDialog : public Window
{
public:
    EditConfigurationDialog(const std::string& boostIncludeDir, const std::string& boostLibDir, const std::string& vcVarsPath, const std::string& vcPlatformToolset);
    void GetData(std::string& boostIncludeDir, std::string& boostLibDir, std::string& vcVarsPath, std::string& vcPlatformToolset);
private:
    void CheckInput();
    void SelectBoostIncludeDir();
    void SelectBoostLibDir();
    void SelectVCVarsPath();
    Button* okButton;
    Button* cancelButton;
    TextBox* boostIncludeDirTextBox;
    TextBox* boostLibDirTextBox;
    TextBox* vcVarsPathTextBox;
    TextBox* vcPlatformToolsetTextBox;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_EDIT_CONFIGURATION_DIALOG_INCLUDED

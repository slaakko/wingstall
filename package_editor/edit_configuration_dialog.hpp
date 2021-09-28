// =================================
// Copyright (c) 2021 Seppo Laakko
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
    EditConfigurationDialog(const std::string& boostIncludeDir, const std::string& boostLibDir, const std::string& vcVarsPath);
    void GetData(std::string& boostIncludeDir, std::string& boostLibDir, std::string& vcVarsPath);
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
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_EDIT_CONFIGURATION_DIALOG_INCLUDED

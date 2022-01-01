// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_ENVIRONMENT_VARIABLE_DIALOG_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_ENVIRONMENT_VARIABLE_DIALOG_INCLUDED
#include <package_editor/environment.hpp>
#include <wing/Window.hpp>
#include <wing/Button.hpp>
#include <wing/TextBox.hpp>

namespace wingstall { namespace package_editor {

using namespace wing;

class EnvironmentVariableDialog : public Window
{
public:
    EnvironmentVariableDialog(const std::string& caption);
    const std::string& EnvironmentVariableName() const { return nameTextBox->Text(); }
    void SetData(EnvironmentVariable* environmentVariable);
    void GetData(EnvironmentVariable* environmentVariable);
private:
    void CheckValid();
    Button* okButton;
    Button* cancelButton;
    TextBox* nameTextBox;
    TextBox* valueTextBox;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_ENVIRONMENT_VARIABLE_DIALOG_INCLUDED

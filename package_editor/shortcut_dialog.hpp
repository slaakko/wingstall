// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_SHORTCUT_DIALOG_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_SHORTCUT_DIALOG_INCLUDED
#include <package_editor/links.hpp>
#include <wing/Window.hpp>
#include <wing/Button.hpp>
#include <wing/TextBox.hpp>

namespace wingstall { namespace package_editor {

using namespace wing;

class ShortcutDialog : public Window
{
public:
    ShortcutDialog(const std::string& caption);
    const std::string& LinkFilePath() const { return linkFilePathTextBox->Text(); }
    void GetData(Shortcut* shortcut);
    void SetData(Shortcut* shortcut);
private:
    void CheckValid();
    Button* okButton;
    Button* cancelButton;
    TextBox* linkFilePathTextBox;
    TextBox* pathTextBox;
    TextBox* argumentsTextBox;
    TextBox* workingDirectoryTextBox;
    TextBox* descriptionTextBox;
    TextBox* iconFilePathTextBox;
    TextBox* iconIndexTextBox;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_SHORTCUT_DIALOG_INCLUDED

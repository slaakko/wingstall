// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_NEW_PACKAGE_DIALOG_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_NEW_PACKAGE_DIALOG_INCLUDED
#include <wing/Window.hpp>
#include <wing/TextBox.hpp>
#include <wing/Button.hpp>

namespace wingstall { namespace package_editor {

using namespace wing;

class NewPackageDialog : public Window
{
public:
    NewPackageDialog();
    std::string GetPackageFilePath() const;
    std::string GetPackageName() const;
private:
    void CheckInput();
    void SelectPackageLocation();
    Button* okButton;
    TextBox* locationTextBox;
    TextBox* packageNameTextBox;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_NEW_PACKAGE_DIALOG_INCLUDED

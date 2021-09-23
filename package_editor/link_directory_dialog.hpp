// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_LINK_DIRECTORY_DIALOG_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_LINK_DIRECTORY_DIALOG_INCLUDED
#include <package_editor/links.hpp>
#include <wing/Window.hpp>
#include <wing/Button.hpp>
#include <wing/TextBox.hpp>

namespace wingstall { namespace package_editor {

using namespace wing;

class LinkDirectoryDialog : public Window
{
public:
    LinkDirectoryDialog(const std::string& caption);
    const std::string& PathDirectoryValue() const { return pathTextBox->Text(); }
    void SetData(LinkDirectory* linkDirectory);
    void GetData(LinkDirectory* linkDirectory);
private:
    void CheckValid();
    Button* okButton;
    Button* cancelButton;
    TextBox* pathTextBox;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_LINK_DIRECTORY_DIALOG_INCLUDED

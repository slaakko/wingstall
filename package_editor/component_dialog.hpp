// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_COMPONENT_DIALOG_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_COMPONENT_DIALOG_INCLUDED
#include <wing/Window.hpp>
#include <wing/TextBox.hpp>

namespace wingstall { namespace package_editor {

using namespace wing;

class ComponentDialog : public Window
{
public:
    ComponentDialog(const std::string& caption_);
    std::string ComponentName() const { return textBox->Text(); }
    void SetComponentName(const std::string& componentName);
private:
    void TextBoxTextChanged();
    Button* okButton;
    Button* cancelButton;
    TextBox* textBox;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_COMPONENT_DIALOG_INCLUDED

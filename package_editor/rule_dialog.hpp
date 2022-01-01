// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_RULE_DIALOG_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_RULE_DIALOG_INCLUDED
#include <package_editor/rule.hpp>
#include <wing/Window.hpp>
#include <wing/ComboBox.hpp>
#include <wing/CheckBox.hpp>
#include <wing/TextBox.hpp>
#include <wing/Button.hpp>

namespace wingstall { namespace package_editor {

using namespace wing;

class RuleDialog : public Window
{
public:
    RuleDialog(const std::string& caption);
    void SetData(Rule* rule);
    void GetData(Rule* rule);
private:
    void CheckValid();
    Button* okButton;
    Button* cancelButton;
    ComboBox* ruleKindComboBox;
    ComboBox* pathKindComboBox;
    TextBox* textBox;
    CheckBox* cascadeCheckBox;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_RULE_DIALOG_INCLUDED

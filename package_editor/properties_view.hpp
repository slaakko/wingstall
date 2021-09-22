// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_PROPERTIES_VIEW_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_PROPERTIES_VIEW_INCLUDED
#include <wing/ContainerControl.hpp>
#include <wing/Button.hpp>
#include <wing/CheckBox.hpp>
#include <wing/ComboBox.hpp>
#include <wing/TextBox.hpp>

namespace wingstall { namespace package_editor {

using namespace wing;

class Package;
class MainWindow;

class PropertiesView : public ContainerControl
{
public:
    PropertiesView(Package* package_);
    ~PropertiesView();
protected:
    void OnCreated() override;
    void OnSizeChanged() override;
private:
    void Exit(CancelArgs& args);
    void EnableApply();
    bool CheckApplyChanges();
    void ApplyButtonClick();
    void SelectSourceRootDir();
    void CreateProductId();
    void SourceRootDirRelativeCheckBoxCheckedChanged();
    bool initializing;
    Package* package;
    MainWindow* mainWindow;
    int exitHandlerId;
    Size defaultButtonSize;
    Size defaultControlSpacing;
    ComboBox* compressionComboBox;
    CheckBox* includeUninstallerCheckBox;
    CheckBox* sourceRootDirRelativeCheckBox;
    Button* applyButton;
    TextBox* sourceRootDirTextBox;
    TextBox* targetRootDirTextBox;
    TextBox* appNameTextBox;
    TextBox* publisherTextBox;
    TextBox* versionTextBox;
    TextBox* productIdTextBox;
    TextBox* iconFilePathTextBox;
    bool dirty;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_PROPERTIES_VIEW_INCLUDED

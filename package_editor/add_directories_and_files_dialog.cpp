// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/add_directories_and_files_dialog.hpp>
#include <wing/Metrics.hpp>
#include <wing/Panel.hpp>
#include <wing/Button.hpp>

namespace wingstall { namespace package_editor {

AddDirectoriesAndFilesDialog::AddDirectoriesAndFilesDialog(wingstall::package_editor::Component* component_) : 
    Window(WindowCreateParams().Text("Add Directories and Files").SetSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(160), ScreenMetrics::Get().MMToVerticalPixels(100)))),
    component(component_)
{
    Icon& icon = Application::GetResourceManager().GetIcon("package.icon");
    SetIcon(icon);
    SetSmallIcon(icon);

    Size s = GetSize();
    Size defaultControlSpacing = ScreenMetrics::Get().DefaultControlSpacing();
    Size defaultButtonSize = ScreenMetrics::Get().DefaultButtonSize();
    Size defaultTextBoxSize = ScreenMetrics::Get().DefaultTextBoxSize();
    Size defaultComboBoxSize = ScreenMetrics::Get().DefaultComboBoxSize();
    Size defaultLabelSize = ScreenMetrics::Get().DefaultLabelSize();

    std::unique_ptr<Panel> panelPtr(new Panel(ControlCreateParams().SetSize(Size(0, defaultButtonSize.Height + 2 * defaultControlSpacing.Height)).SetDock(Dock::bottom)));
    Panel* panel = panelPtr.get();
    AddChild(panelPtr.release());

    Size panelSize = panel->GetSize();
    int x = panelSize.Width - defaultButtonSize.Width - defaultControlSpacing.Width;
    int y = panelSize.Height - defaultButtonSize.Height - defaultControlSpacing.Height;

    Point cancelButtonLoc(x, y);
    std::unique_ptr<Button> cancelButtonPtr(new Button(ControlCreateParams().Location(cancelButtonLoc).SetSize(defaultButtonSize).Text("Cancel").SetAnchors(static_cast<Anchors>(Anchors::bottom | Anchors::right))));
    Button* cancelButton = cancelButtonPtr.get();
    cancelButton->SetDialogResult(DialogResult::cancel);
    panel->AddChild(cancelButtonPtr.release());

    x = x - defaultButtonSize.Width - defaultControlSpacing.Width;

    Point okButtonLoc(x, y);
    std::unique_ptr<Button> okButtonPtr(new Button(ControlCreateParams().Location(okButtonLoc).SetSize(defaultButtonSize).Text("OK").SetAnchors(static_cast<Anchors>(Anchors::bottom | Anchors::right))));
    Button* okButton = okButtonPtr.get();
    okButton->SetDialogResult(DialogResult::ok);
    okButton->SetDefault();
    SetDefaultButton(okButton);
    SetCancelButton(cancelButton);
    panel->AddChild(okButtonPtr.release());
}

} } // wingstall::package_editor

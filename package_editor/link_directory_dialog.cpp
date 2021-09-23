// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/link_directory_dialog.hpp>
#include <package_editor/properties_view.hpp>
#include <wing/Label.hpp>
#include <wing/Metrics.hpp>
#include <wing/PaddedControl.hpp>
#include <wing/BorderedControl.hpp>

namespace wingstall { namespace package_editor {

LinkDirectoryDialog::LinkDirectoryDialog(const std::string& caption) : 
    Window(WindowCreateParams().WindowClassName("wingstall.package_editor.link_directory_dialog").Text(caption).
    WindowStyle(DialogWindowStyle()).WindowClassBackgroundColor(DefaultControlWindowClassBackgroundColor()).BackgroundColor(DefaultControlBackgroundColor()).
    Location(DefaultLocation()).SetSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(120), ScreenMetrics::Get().MMToVerticalPixels(80)))),
    okButton(nullptr),
    cancelButton(nullptr),
    pathTextBox(nullptr)
{
    Size s = GetSize();
    Size defaultControlSpacing = ScreenMetrics::Get().DefaultControlSpacing();
    Size defaultButtonSize = ScreenMetrics::Get().DefaultButtonSize();
    Size defaultTextBoxSize = ScreenMetrics::Get().DefaultTextBoxSize();
    Size defaultLabelSize = ScreenMetrics::Get().DefaultLabelSize();

    Point pathLabelLoc(16, 16);
    std::unique_ptr<Label> pathLabelPtr(new Label(LabelCreateParams().Text("Path:").Location(pathLabelLoc).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(pathLabelPtr.release());

    Point pathTextBoxLoc(16, 16 + 24);
    Size textBoxSize(ScreenMetrics::Get().MMToHorizontalPixels(60), defaultTextBoxSize.Height + 4);
    std::unique_ptr<TextBox> pathTextBoxPtr(new TextBox(TextBoxCreateParams().Location(pathTextBoxLoc).SetSize(textBoxSize).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    pathTextBox = pathTextBoxPtr.get();
    pathTextBox->TextChanged().AddHandler(this, &LinkDirectoryDialog::CheckValid);
    std::unique_ptr<PaddedControl> paddedTextBoxPtr(new PaddedControl(PaddedControlCreateParams(pathTextBoxPtr.release()).Location(pathTextBoxLoc).SetSize(PaddedSize(textBoxSize, DefaultPadding())).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    std::unique_ptr<BorderedControl> borderedTextBoxPtr(new BorderedControl(BorderedControlCreateParams(paddedTextBoxPtr.release()).Location(pathTextBoxLoc).
        SetSize(BorderedSize(PaddedSize(textBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(borderedTextBoxPtr.release());

    int x = s.Width - defaultButtonSize.Width - defaultControlSpacing.Width;
    int y = s.Height - defaultButtonSize.Height - defaultControlSpacing.Height;
    std::unique_ptr<Button> cancelButtonPtr(new Button(ControlCreateParams().Location(Point(x, y)).SetSize(defaultButtonSize).Text("Cancel").SetAnchors(Anchors::right | Anchors::bottom)));
    cancelButton = cancelButtonPtr.get();
    cancelButton->SetDialogResult(DialogResult::cancel);
    x = x - defaultButtonSize.Width - defaultControlSpacing.Width;
    std::unique_ptr<Button> okButtonPtr(new Button(ControlCreateParams().Location(Point(x, y)).SetSize(defaultButtonSize).Text("OK").SetAnchors(Anchors::right | Anchors::bottom)));
    okButton = okButtonPtr.get();
    okButton->Disable();
    okButton->SetDialogResult(DialogResult::ok);
    okButton->SetDefault();
    AddChild(okButtonPtr.release());
    AddChild(cancelButtonPtr.release());
    SetDefaultButton(okButton);
    SetCancelButton(cancelButton);

    pathTextBox->SetFocus();
}

void LinkDirectoryDialog::SetData(LinkDirectory* linkDirectory)
{
    pathTextBox->SetText(linkDirectory->Path());
}

void LinkDirectoryDialog::GetData(LinkDirectory* linkDirectory)
{
    linkDirectory->SetName("Link Directory");
    linkDirectory->SetPath(pathTextBox->Text());
}

void LinkDirectoryDialog::CheckValid()
{
    if (IsValidPath(pathTextBox->Text()))
    {
        okButton->Enable();
    }
    else
    {
        okButton->Disable();
    }
}

} } // wingstall::package_editor

// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/environment_variable_dialog.hpp>
#include <wing/Label.hpp>
#include <wing/Metrics.hpp>
#include <wing/PaddedControl.hpp>
#include <wing/BorderedControl.hpp>
#include <soulng/util/Unicode.hpp>

namespace wingstall { namespace package_editor {

using namespace soulng::unicode;

bool IsValidEnvironmentVariableName(const std::string& name)
{
    std::u32string str(ToUtf32(name));
    if (str.empty()) return false;
    char32_t first = str.front();
    if (!IsIdStart(first)) return false;
    int n = str.length();
    for (int i = 1; i < n; ++i)
    {
        if (!IsIdCont(str[i])) return false;
    }
    return true;
}

EnvironmentVariableDialog::EnvironmentVariableDialog(const std::string& caption) : 
    Window(WindowCreateParams().WindowClassName("wingstall.package_editor.environment_variable_dialog").Text(caption).
    WindowStyle(DialogWindowStyle()).WindowClassBackgroundColor(DefaultControlWindowClassBackgroundColor()).BackgroundColor(DefaultControlBackgroundColor()).
    Location(DefaultLocation()).SetSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(120), ScreenMetrics::Get().MMToVerticalPixels(80)))),
    okButton(nullptr),
    cancelButton(nullptr),
    nameTextBox(nullptr),
    valueTextBox(nullptr)
{
    Size s = GetSize();
    Size defaultControlSpacing = ScreenMetrics::Get().DefaultControlSpacing();
    Size defaultButtonSize = ScreenMetrics::Get().DefaultButtonSize();
    Size defaultTextBoxSize = ScreenMetrics::Get().DefaultTextBoxSize();
    Size defaultLabelSize = ScreenMetrics::Get().DefaultLabelSize();

    Point nameLabelLoc(16, 16);
    std::unique_ptr<Label> nameLabelPtr(new Label(LabelCreateParams().Text("Name:").Location(nameLabelLoc).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(nameLabelPtr.release());

    Point nameTextBoxLoc(16, 16 + 24);
    Size textBoxSize(ScreenMetrics::Get().MMToHorizontalPixels(60), defaultTextBoxSize.Height + 4);
    std::unique_ptr<TextBox> nameTextBoxPtr(new TextBox(TextBoxCreateParams().Location(nameTextBoxLoc).SetSize(textBoxSize).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    nameTextBox = nameTextBoxPtr.get();
    nameTextBox->TextChanged().AddHandler(this, &EnvironmentVariableDialog::CheckValid);
    std::unique_ptr<PaddedControl> paddedTextBoxPtr(new PaddedControl(PaddedControlCreateParams(nameTextBoxPtr.release()).Location(nameTextBoxLoc).SetSize(PaddedSize(textBoxSize, DefaultPadding())).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    std::unique_ptr<BorderedControl> borderedTextBoxPtr(new BorderedControl(BorderedControlCreateParams(paddedTextBoxPtr.release()).Location(nameTextBoxLoc).
        SetSize(BorderedSize(PaddedSize(textBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(borderedTextBoxPtr.release());

    Point valueLabelLoc(16, 16 + 16 + 24 + 24);
    std::unique_ptr<Label> valueLabelPtr(new Label(LabelCreateParams().Text("Value:").Location(valueLabelLoc).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(valueLabelPtr.release());

    Point valueTextBoxLoc(16, 16 + 16 + 24 + 24 + 24);
    std::unique_ptr<TextBox> valueTextBoxPtr(new TextBox(TextBoxCreateParams().Location(valueTextBoxLoc).SetSize(textBoxSize).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    valueTextBox = valueTextBoxPtr.get();
    std::unique_ptr<PaddedControl> paddedValueTextBoxPtr(new PaddedControl(PaddedControlCreateParams(valueTextBoxPtr.release()).Location(valueTextBoxLoc).SetSize(PaddedSize(textBoxSize, DefaultPadding())).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    std::unique_ptr<BorderedControl> borderedValueTextBoxPtr(new BorderedControl(BorderedControlCreateParams(paddedValueTextBoxPtr.release()).Location(valueTextBoxLoc).
        SetSize(BorderedSize(PaddedSize(textBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(borderedValueTextBoxPtr.release());

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

    nameTextBox->SetFocus();
}

void EnvironmentVariableDialog::CheckValid()
{
    if (IsValidEnvironmentVariableName(nameTextBox->Text()))
    {
        okButton->Enable();
    }
    else
    {
        okButton->Disable();
    }
}

void EnvironmentVariableDialog::SetData(EnvironmentVariable* environmentVariable)
{
    nameTextBox->SetText(environmentVariable->Name());
    valueTextBox->SetText(environmentVariable->Value());
}

void EnvironmentVariableDialog::GetData(EnvironmentVariable* environmentVariable)
{
    environmentVariable->SetName(nameTextBox->Text());
    environmentVariable->SetValue(valueTextBox->Text());
}

} } // wingstall::package_editor

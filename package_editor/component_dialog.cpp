// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/component_dialog.hpp>
#include <wing/Metrics.hpp>
#include <wing/Button.hpp>
#include <wing/Label.hpp>
#include <wing/BorderedControl.hpp>
#include <wing/PaddedControl.hpp>
#include <soulng/util/Unicode.hpp>

namespace wingstall { namespace package_editor {

using namespace soulng::unicode;

bool IsValidComponentName(const std::string& name)
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

ComponentDialog::ComponentDialog(const std::string& caption_) : 
    Window(WindowCreateParams().Text(caption_).WindowClassName("wingstall.package_editor.component_dialog").
    WindowStyle(DialogWindowStyle()).WindowClassBackgroundColor(DefaultControlWindowClassBackgroundColor()).BackgroundColor(DefaultControlBackgroundColor()).
    Location(DefaultLocation()).SetSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(100), ScreenMetrics::Get().MMToVerticalPixels(60)))),
    okButton(nullptr),
    cancelButton(nullptr)
{
    Size s = GetSize();
    Size defaultControlSpacing = ScreenMetrics::Get().DefaultControlSpacing();
    Size defaultButtonSize = ScreenMetrics::Get().DefaultButtonSize();
    Size defaultTextBoxSize = ScreenMetrics::Get().DefaultTextBoxSize();
    Size defaultLabelSize = ScreenMetrics::Get().DefaultLabelSize();

    Point nameLabelLoc(16, 16);
    std::unique_ptr<Label> nameLabelPtr(new Label(LabelCreateParams().Text("Name:").Location(nameLabelLoc).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(nameLabelPtr.release());

    Point textBoxLoc(16, 16 + 24);
    Size textBoxSize(ScreenMetrics::Get().MMToHorizontalPixels(60), defaultTextBoxSize.Height + 4);
    std::unique_ptr<TextBox> textBoxPtr(new TextBox(TextBoxCreateParams().Location(textBoxLoc).SetSize(textBoxSize).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    textBox = textBoxPtr.get();
    std::unique_ptr<PaddedControl> paddedTextBoxPtr(new PaddedControl(PaddedControlCreateParams(textBoxPtr.release()).Location(textBoxLoc).SetSize(PaddedSize(textBoxSize, DefaultPadding())).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    std::unique_ptr<BorderedControl> borderedTextBoxPtr(new BorderedControl(BorderedControlCreateParams(paddedTextBoxPtr.release()).Location(textBoxLoc).
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

    textBox->SetFocus();
    textBox->TextChanged().AddHandler(this, &ComponentDialog::TextBoxTextChanged);
}

void ComponentDialog::SetComponentName(const std::string& componentName)
{
    textBox->SetText(componentName); 
    textBox->SelectAll();
}

void ComponentDialog::TextBoxTextChanged()
{
    std::string text = textBox->Text();
    if (IsValidComponentName(text))
    {
        okButton->Enable();
    }
    else
    {
        okButton->Disable();
    }
}

} } // wingstall::package_editor

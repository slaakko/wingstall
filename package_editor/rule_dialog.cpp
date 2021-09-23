// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/rule_dialog.hpp>
#include <wing/Metrics.hpp>
#include <wing/Label.hpp>
#include <wing/PaddedControl.hpp>
#include <wing/BorderedControl.hpp>

namespace wingstall { namespace package_editor {

RuleDialog::RuleDialog(const std::string& caption) : Window(WindowCreateParams().Text(caption).WindowClassName("wingstall.package_editor.rule_dialog").
    WindowStyle(DialogWindowStyle()).WindowClassBackgroundColor(DefaultControlWindowClassBackgroundColor()).BackgroundColor(DefaultControlBackgroundColor()).
    Location(DefaultLocation()).SetSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(120), ScreenMetrics::Get().MMToVerticalPixels(80)))),
    okButton(nullptr),
    cancelButton(nullptr), 
    ruleKindComboBox(nullptr),
    pathKindComboBox(nullptr),
    cascadeCheckBox(nullptr)
{
    Size s = GetSize();
    Size defaultControlSpacing = ScreenMetrics::Get().DefaultControlSpacing();
    Size defaultButtonSize = ScreenMetrics::Get().DefaultButtonSize();
    Size defaultTextBoxSize = ScreenMetrics::Get().DefaultTextBoxSize();
    Size defaultLabelSize = ScreenMetrics::Get().DefaultLabelSize();
    Size defaultComboBoxSize = ScreenMetrics::Get().DefaultComboBoxSize();

    Point ruleKindComboBoxLoc(16, 16);
    Size ruleKindComboBoxSize(defaultComboBoxSize.Width, defaultComboBoxSize.Height + 48);
    std::unique_ptr<ComboBox> ruleKindComboBoxPtr(new ComboBox(ComboBoxCreateParams().Location(ruleKindComboBoxLoc).SetSize(ruleKindComboBoxSize).SetAnchors(
        static_cast<Anchors>(Anchors::top | Anchors::left))));
    ruleKindComboBox = ruleKindComboBoxPtr.get();
    AddChild(ruleKindComboBoxPtr.release());
    ruleKindComboBox->AddItem("Exclude");
    ruleKindComboBox->AddItem("Include");
    ruleKindComboBox->SetSelectedIndex(0);
    ruleKindComboBox->SelectedIndexChanged().AddHandler(this, &RuleDialog::CheckValid);

    Point pathKindComboBoxLoc(16 + defaultComboBoxSize.Width + defaultControlSpacing.Width, 16);
    Size pathKindComboBoxSize(defaultComboBoxSize.Width, defaultComboBoxSize.Height + 48);
    std::unique_ptr<ComboBox> pathKindComboBoxPtr(new ComboBox(ComboBoxCreateParams().Location(pathKindComboBoxLoc).SetSize(pathKindComboBoxSize).SetAnchors(
        static_cast<Anchors>(Anchors::top | Anchors::left))));
    pathKindComboBox = pathKindComboBoxPtr.get();
    AddChild(pathKindComboBoxPtr.release());
    pathKindComboBox->AddItem("Directory");
    pathKindComboBox->AddItem("File");
    pathKindComboBox->SetSelectedIndex(0);
    pathKindComboBox->SelectedIndexChanged().AddHandler(this, &RuleDialog::CheckValid);

    Point nameLabelLoc(16, 24 + 24);
    std::unique_ptr<Label> nameLabelPtr(new Label(LabelCreateParams().Text("Name/Pattern:").Location(nameLabelLoc).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(nameLabelPtr.release());

    Point textBoxLoc(16, 24 + 24 + 24);
    Size textBoxSize(ScreenMetrics::Get().MMToHorizontalPixels(60), defaultTextBoxSize.Height + 4);
    std::unique_ptr<TextBox> textBoxPtr(new TextBox(TextBoxCreateParams().Location(textBoxLoc).SetSize(textBoxSize).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    textBox = textBoxPtr.get();
    textBox->TextChanged().AddHandler(this, &RuleDialog::CheckValid);
    std::unique_ptr<PaddedControl> paddedTextBoxPtr(new PaddedControl(PaddedControlCreateParams(textBoxPtr.release()).Location(textBoxLoc).SetSize(PaddedSize(textBoxSize, DefaultPadding())).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    std::unique_ptr<BorderedControl> borderedTextBoxPtr(new BorderedControl(BorderedControlCreateParams(paddedTextBoxPtr.release()).Location(textBoxLoc).
        SetSize(BorderedSize(PaddedSize(textBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(borderedTextBoxPtr.release());

    Point cascadeCheckBoxLoc(16, 16 + 24 + 24 + 24 + 24);
    std::unique_ptr<CheckBox> cascadeCheckBoxkPtr(new CheckBox(CheckBoxCreateParams().Text("Cascade").Location(cascadeCheckBoxLoc).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    cascadeCheckBox = cascadeCheckBoxkPtr.get();
    AddChild(cascadeCheckBoxkPtr.release());

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
}

void RuleDialog::CheckValid()
{
    RuleKind ruleKind = static_cast<RuleKind>(ruleKindComboBox->GetSelectedIndex());
    if (ruleKind == RuleKind::exclude)
    {
        cascadeCheckBox->Enable();
    }
    else
    {
        cascadeCheckBox->SetChecked(false);
        cascadeCheckBox->Disable();
    }
    PathKind pathKind = static_cast<PathKind>(pathKindComboBox->GetSelectedIndex());
    if (!textBox->Text().empty())
    {
        okButton->Enable();
    }
    else
    {
        okButton->Disable();
    }
}

void RuleDialog::SetData(Rule* rule)
{
    ruleKindComboBox->SetSelectedIndex(static_cast<int>(rule->GetRuleKind()));
    pathKindComboBox->SetSelectedIndex(static_cast<int>(rule->GetPathKind()));
    textBox->SetText(rule->Value());
    cascadeCheckBox->SetChecked(rule->Cascade());
}

void RuleDialog::GetData(Rule* rule)
{
    RuleKind ruleKind = static_cast<RuleKind>(ruleKindComboBox->GetSelectedIndex());
    rule->SetRuleKind(ruleKind);
    PathKind pathKind = static_cast<PathKind>(pathKindComboBox->GetSelectedIndex());
    rule->SetPathKind(pathKind);
    std::string name;
    if (ruleKind == RuleKind::exclude)
    {
        name.append("Exclude");
    }
    else if (ruleKind == RuleKind::include)
    {
        name.append("Include");
    }
    if (pathKind == PathKind::dir)
    {
        name.append(" Directory");
    }
    else if (pathKind == PathKind::file)
    {
        name.append(" File");
    }
    rule->SetName(name);
    rule->SetValue(textBox->Text());
    rule->SetCascade(cascadeCheckBox->Checked());
    if (ruleKind != RuleKind::include || pathKind != PathKind::dir)
    {
        rule->RemoveRules();
    }
}

} } // wingstall::package_editor

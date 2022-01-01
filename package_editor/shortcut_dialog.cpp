// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/shortcut_dialog.hpp>
#include <package_editor/properties_view.hpp>
#include <wing/Label.hpp>
#include <wing/Metrics.hpp>
#include <wing/PaddedControl.hpp>
#include <wing/BorderedControl.hpp>
#include <boost/lexical_cast.hpp>

namespace wingstall { namespace package_editor {

bool IsValidIconIndex(const std::string& text)
{
    if (text.empty()) return false;
    try
    {
        int iconIndex = boost::lexical_cast<int>(text);
        if (iconIndex < 0)
        {
            return false;
        }
    }
    catch (...)
    {
        return false;
    }
    return true;
}

ShortcutDialog::ShortcutDialog(const std::string& caption) : 
    Window(WindowCreateParams().WindowClassName("wingstalll.package_editor.shortcut_dialog").Text(caption).
    WindowStyle(DialogWindowStyle()).WindowClassBackgroundColor(DefaultControlWindowClassBackgroundColor()).BackgroundColor(DefaultControlBackgroundColor()).
    Location(DefaultLocation()).SetSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(200), ScreenMetrics::Get().MMToVerticalPixels(150)))),
    okButton(nullptr),
    cancelButton(nullptr)
{
    Size s = GetSize();
    Size defaultControlSpacing = ScreenMetrics::Get().DefaultControlSpacing();
    Size defaultButtonSize = ScreenMetrics::Get().DefaultButtonSize();
    Size defaultTextBoxSize = ScreenMetrics::Get().DefaultTextBoxSize();
    Size defaultLabelSize = ScreenMetrics::Get().DefaultLabelSize();

    Point linkFilePathLabelLoc(16, 16);
    std::unique_ptr<Label> linkFilePathLabelPtr(new Label(LabelCreateParams().Text("Link File Path:").Location(linkFilePathLabelLoc).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(linkFilePathLabelPtr.release());

    Point linkFilePathTextBoxLoc(16, 16 + 24);
    Size textBoxSize(ScreenMetrics::Get().MMToHorizontalPixels(100), defaultTextBoxSize.Height + 4);
    std::unique_ptr<TextBox> linkFilePathTextBoxPtr(new TextBox(TextBoxCreateParams().Location(linkFilePathTextBoxLoc).SetSize(textBoxSize).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    linkFilePathTextBox = linkFilePathTextBoxPtr.get();
    linkFilePathTextBox->TextChanged().AddHandler(this, &ShortcutDialog::CheckValid);
    std::unique_ptr<PaddedControl> paddedLinkFilePathTextBoxPtr(new PaddedControl(PaddedControlCreateParams(linkFilePathTextBoxPtr.release()).Location(linkFilePathTextBoxLoc).SetSize(PaddedSize(textBoxSize, DefaultPadding())).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    std::unique_ptr<BorderedControl> borderedLinkFilePathTextBoxPtr(new BorderedControl(BorderedControlCreateParams(paddedLinkFilePathTextBoxPtr.release()).Location(linkFilePathTextBoxLoc).
        SetSize(BorderedSize(PaddedSize(textBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(borderedLinkFilePathTextBoxPtr.release());

    Point pathLabelLoc(16, 16 + 16 + 24 + 24);
    std::unique_ptr<Label> pathLabelPtr(new Label(LabelCreateParams().Text("Path:").Location(pathLabelLoc).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(pathLabelPtr.release());

    Point pathTextBoxLoc(16, 16 + 16 + 24 + 24 + 24);
    std::unique_ptr<TextBox> pathTextBoxPtr(new TextBox(TextBoxCreateParams().Location(pathTextBoxLoc).SetSize(textBoxSize).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    pathTextBox = pathTextBoxPtr.get();
    pathTextBox->TextChanged().AddHandler(this, &ShortcutDialog::CheckValid);
    std::unique_ptr<PaddedControl> paddedPathTextBoxPtr(new PaddedControl(PaddedControlCreateParams(pathTextBoxPtr.release()).Location(pathTextBoxLoc).SetSize(PaddedSize(textBoxSize, DefaultPadding())).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    std::unique_ptr<BorderedControl> borderedPathTextBoxPtr(new BorderedControl(BorderedControlCreateParams(paddedPathTextBoxPtr.release()).Location(pathTextBoxLoc).
        SetSize(BorderedSize(PaddedSize(textBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(borderedPathTextBoxPtr.release());

    Point argumentsLabelLoc(16, 16 + 16 + 16 + 24 + 24 + 24 + 24);
    std::unique_ptr<Label> argumentsLabelPtr(new Label(LabelCreateParams().Text("Arguments:").Location(argumentsLabelLoc).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(argumentsLabelPtr.release());

    Point argumentsTextBoxLoc(16, 16 + 16 + 16 + 16 + 24 + 24 + 24 + 24);
    std::unique_ptr<TextBox> argumentsTextBoxPtr(new TextBox(TextBoxCreateParams().Location(argumentsTextBoxLoc).SetSize(textBoxSize).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    argumentsTextBox = argumentsTextBoxPtr.get();
    argumentsTextBox->TextChanged().AddHandler(this, &ShortcutDialog::CheckValid);
    std::unique_ptr<PaddedControl> paddedArgumentsTextBoxPtr(new PaddedControl(PaddedControlCreateParams(argumentsTextBoxPtr.release()).Location(argumentsTextBoxLoc).SetSize(PaddedSize(textBoxSize, DefaultPadding())).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    std::unique_ptr<BorderedControl> borderedArgumentsTextBoxPtr(new BorderedControl(BorderedControlCreateParams(paddedArgumentsTextBoxPtr.release()).Location(argumentsTextBoxLoc).
        SetSize(BorderedSize(PaddedSize(textBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(borderedArgumentsTextBoxPtr.release());

    Point workingDirectoryLabelLoc(16, 16 + 16 + 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<Label> workingDirectoryLabelPtr(new Label(LabelCreateParams().Text("Working Directory:").Location(workingDirectoryLabelLoc).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(workingDirectoryLabelPtr.release());

    Point workingDirectoryTextBoxLoc(16, 16 + 16 + 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<TextBox> workingDirectoryTextBoxPtr(new TextBox(TextBoxCreateParams().Location(workingDirectoryTextBoxLoc).SetSize(textBoxSize).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    workingDirectoryTextBox = workingDirectoryTextBoxPtr.get();
    workingDirectoryTextBox->TextChanged().AddHandler(this, &ShortcutDialog::CheckValid);
    std::unique_ptr<PaddedControl> paddedWorkingDirectoryTextBoxPtr(new PaddedControl(PaddedControlCreateParams(workingDirectoryTextBoxPtr.release()).Location(workingDirectoryTextBoxLoc).SetSize(PaddedSize(textBoxSize, DefaultPadding())).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    std::unique_ptr<BorderedControl> borderedWorkingDirectoryTextBoxPtr(new BorderedControl(BorderedControlCreateParams(paddedWorkingDirectoryTextBoxPtr.release()).Location(workingDirectoryTextBoxLoc).
        SetSize(BorderedSize(PaddedSize(textBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(borderedWorkingDirectoryTextBoxPtr.release());

    Point descriptionLabelLoc(16, 16 + 16 + 16 + 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<Label> descriptionLabelPtr(new Label(LabelCreateParams().Text("Description:").Location(descriptionLabelLoc).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(descriptionLabelPtr.release());

    Point descriptionTextBoxLoc(16, 16 + 16 + 16 + 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<TextBox> descriptionTextBoxPtr(new TextBox(TextBoxCreateParams().Location(descriptionTextBoxLoc).SetSize(textBoxSize).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    descriptionTextBox = descriptionTextBoxPtr.get();
    descriptionTextBox->TextChanged().AddHandler(this, &ShortcutDialog::CheckValid);
    std::unique_ptr<PaddedControl> paddedDescriptionTextBoxPtr(new PaddedControl(PaddedControlCreateParams(descriptionTextBoxPtr.release()).Location(descriptionTextBoxLoc).SetSize(PaddedSize(textBoxSize, DefaultPadding())).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    std::unique_ptr<BorderedControl> borderedDescriptionTextBoxPtr(new BorderedControl(BorderedControlCreateParams(paddedDescriptionTextBoxPtr.release()).Location(descriptionTextBoxLoc).
        SetSize(BorderedSize(PaddedSize(textBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(borderedDescriptionTextBoxPtr.release());

    Point iconFilePathLabelLoc(16, 16 + 16 + 16 + 16 + 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<Label> iconFilePathLabelPtr(new Label(LabelCreateParams().Text("Icon File Path:").Location(iconFilePathLabelLoc).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(iconFilePathLabelPtr.release());

    Point iconFilePathTextBoxLoc(16, 16 + 16 + 16 + 16 + 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<TextBox> iconFilePathTextBoxPtr(new TextBox(TextBoxCreateParams().Location(iconFilePathTextBoxLoc).SetSize(textBoxSize).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    iconFilePathTextBox = iconFilePathTextBoxPtr.get();
    iconFilePathTextBox->TextChanged().AddHandler(this, &ShortcutDialog::CheckValid);
    std::unique_ptr<PaddedControl> paddedIconFilePathTextBoxPtr(new PaddedControl(PaddedControlCreateParams(iconFilePathTextBoxPtr.release()).Location(iconFilePathTextBoxLoc).SetSize(PaddedSize(textBoxSize, DefaultPadding())).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    std::unique_ptr<BorderedControl> borderedIconFilePathTextBoxPtr(new BorderedControl(BorderedControlCreateParams(paddedIconFilePathTextBoxPtr.release()).Location(iconFilePathTextBoxLoc).
        SetSize(BorderedSize(PaddedSize(textBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(borderedIconFilePathTextBoxPtr.release());

    Point iconIndexLabelLoc(16, 16 + 16 + 16 + 16 + 16 + 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<Label> iconIndexLabelPtr(new Label(LabelCreateParams().Text("Icon Index:").Location(iconIndexLabelLoc).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(iconIndexLabelPtr.release());

    Point iconIndexTextBoxLoc(16, 16 + 16 + 16 + 16 + 16 + 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<TextBox> iconIndexTextBoxPtr(new TextBox(TextBoxCreateParams().Text("0").Location(iconIndexTextBoxLoc).SetSize(textBoxSize).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    iconIndexTextBox = iconIndexTextBoxPtr.get();
    iconIndexTextBox->TextChanged().AddHandler(this, &ShortcutDialog::CheckValid);
    std::unique_ptr<PaddedControl> paddedIconIndexTextBoxPtr(new PaddedControl(PaddedControlCreateParams(iconIndexTextBoxPtr.release()).Location(iconIndexTextBoxLoc).SetSize(PaddedSize(textBoxSize, DefaultPadding())).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    std::unique_ptr<BorderedControl> borderedIconIndexTextBoxPtr(new BorderedControl(BorderedControlCreateParams(paddedIconIndexTextBoxPtr.release()).Location(iconIndexTextBoxLoc).
        SetSize(BorderedSize(PaddedSize(textBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(borderedIconIndexTextBoxPtr.release());

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

    linkFilePathTextBox->SetFocus();
}

void ShortcutDialog::CheckValid()
{
    if (IsValidPath(linkFilePathTextBox->Text()) && IsValidPath(pathTextBox->Text()) && IsValidIconIndex(iconIndexTextBox->Text()))
    {
        okButton->Enable();
    }
    else
    {
        okButton->Disable();
    }
}

void ShortcutDialog::GetData(Shortcut* shortcut)
{
    shortcut->SetName("Shortcut");
    shortcut->SetLinkFilePath(linkFilePathTextBox->Text());
    shortcut->SetPath(pathTextBox->Text());
    shortcut->SetArguments(argumentsTextBox->Text());
    shortcut->SetWorkingDirectory(workingDirectoryTextBox->Text());
    shortcut->SetDescription(descriptionTextBox->Text());
    shortcut->SetIconFilePath(iconFilePathTextBox->Text());
    shortcut->SetIconIndex(boost::lexical_cast<int>(iconIndexTextBox->Text()));
}

void ShortcutDialog::SetData(Shortcut* shortcut)
{
    linkFilePathTextBox->SetText(shortcut->LinkFilePath());
    pathTextBox->SetText(shortcut->Path());
    argumentsTextBox->SetText(shortcut->Arguments());
    workingDirectoryTextBox->SetText(shortcut->WorkingDirectory());
    descriptionTextBox->SetText(shortcut->Description());
    iconFilePathTextBox->SetText(shortcut->IconFilePath());
    iconIndexTextBox->SetText(std::to_string(shortcut->IconIndex()));
}

} } // wingstall::package_editor

// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/edit_configuration_dialog.hpp>
#include <wing/Dialog.hpp>
#include <wing/Metrics.hpp>
#include <wing/Label.hpp>
#include <wing/PaddedControl.hpp>
#include <wing/BorderedControl.hpp>
#include <soulng/util/Path.hpp>

namespace wingstall { namespace package_editor {

using namespace soulng::util;

EditConfigurationDialog::EditConfigurationDialog(const std::string& boostIncludeDir, const std::string& boostLibDir, const std::string& vcVarsPath, 
    const std::string& vcPlatformToolset) :
    Window(WindowCreateParams().Text("Edit Configuration").WindowClassName("wingstall.package_editor.edit_configuration_dialog").
    WindowStyle(DialogWindowStyle()).WindowClassBackgroundColor(DefaultControlWindowClassBackgroundColor()).BackgroundColor(DefaultControlBackgroundColor()).
    Location(DefaultLocation()).SetSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(180), ScreenMetrics::Get().MMToVerticalPixels(100)))),
    okButton(nullptr),
    cancelButton(nullptr)
{
    Size s = GetSize();
    Size defaultControlSpacing = ScreenMetrics::Get().DefaultControlSpacing();
    Size defaultButtonSize = ScreenMetrics::Get().DefaultButtonSize();
    Size defaultTextBoxSize = ScreenMetrics::Get().DefaultTextBoxSize();
    Size defaultLabelSize = ScreenMetrics::Get().DefaultLabelSize();

    Point boostIncludedDirLabelLoc(16, 16);
    std::unique_ptr<Label> boostIncludedDirLabelPtr(new Label(LabelCreateParams().Text("Boost Include Directory:").Location(boostIncludedDirLabelLoc).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(boostIncludedDirLabelPtr.release());

    Point boostIncludedDirTextBoxLoc(16, 16 + 24);
    Size textBoxSize(ScreenMetrics::Get().MMToHorizontalPixels(140), defaultTextBoxSize.Height + 4);
    std::unique_ptr<TextBox> boostIncludedDirTextBoxPtr(new TextBox(TextBoxCreateParams().Location(boostIncludedDirTextBoxLoc).Text(boostIncludeDir).SetSize(textBoxSize).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    boostIncludeDirTextBox = boostIncludedDirTextBoxPtr.get();
    boostIncludeDirTextBox->TextChanged().AddHandler(this, &EditConfigurationDialog::CheckInput);
    std::unique_ptr<PaddedControl> paddedBoostIncludedDirTextBoxPtr(new PaddedControl(PaddedControlCreateParams(boostIncludedDirTextBoxPtr.release()).Location(boostIncludedDirTextBoxLoc).SetSize(PaddedSize(textBoxSize, DefaultPadding())).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    std::unique_ptr<BorderedControl> borderedBoostIncludedDirTextBoxPtr(new BorderedControl(BorderedControlCreateParams(paddedBoostIncludedDirTextBoxPtr.release()).Location(boostIncludedDirTextBoxLoc).
        SetSize(BorderedSize(PaddedSize(textBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(borderedBoostIncludedDirTextBoxPtr.release());

    Point selectBoostIncludeDirButtonLoc(16 + 16 + ScreenMetrics::Get().MMToHorizontalPixels(140), 16 + 24);
    std::unique_ptr<Button> selectBoostIncludeDirButton(new Button(ControlCreateParams().Text("...").Location(selectBoostIncludeDirButtonLoc).SetSize(Size(24, 24)).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    selectBoostIncludeDirButton->Click().AddHandler(this, &EditConfigurationDialog::SelectBoostIncludeDir);
    AddChild(selectBoostIncludeDirButton.release());

    Point boostLibDirLabelLoc(16, 16 + 16 + 24 + 24);
    std::unique_ptr<Label> boostLibDirLabelPtr(new Label(LabelCreateParams().Text("Boost Library Directory:").Location(boostLibDirLabelLoc).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(boostLibDirLabelPtr.release());

    Point boostLibDirTextBoxLoc(16, 16 + 16 + 24 + 24 + 24);
    std::unique_ptr<TextBox> boostLibDirTextBoxPtr(new TextBox(TextBoxCreateParams().Location(boostLibDirTextBoxLoc).Text(boostLibDir).SetSize(textBoxSize).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    boostLibDirTextBox = boostLibDirTextBoxPtr.get();
    boostLibDirTextBox->TextChanged().AddHandler(this, &EditConfigurationDialog::CheckInput);
    std::unique_ptr<PaddedControl> paddedBoostLibDirTextBoxPtr(new PaddedControl(PaddedControlCreateParams(boostLibDirTextBoxPtr.release()).Location(boostLibDirTextBoxLoc).SetSize(PaddedSize(textBoxSize, DefaultPadding())).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    std::unique_ptr<BorderedControl> borderedBoostLibDirTextBoxPtr(new BorderedControl(BorderedControlCreateParams(paddedBoostLibDirTextBoxPtr.release()).Location(boostLibDirTextBoxLoc).
        SetSize(BorderedSize(PaddedSize(textBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(borderedBoostLibDirTextBoxPtr.release());

    Point selectBoostLibDirButtonLoc(16 + 16 + ScreenMetrics::Get().MMToHorizontalPixels(140), 16 + 16 + 24 + 24 + 24);
    std::unique_ptr<Button> selectBoostLibDirButton(new Button(ControlCreateParams().Text("...").Location(selectBoostLibDirButtonLoc).SetSize(Size(24, 24)).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    selectBoostLibDirButton->Click().AddHandler(this, &EditConfigurationDialog::SelectBoostLibDir);
    AddChild(selectBoostLibDirButton.release());

    Point vcVarsPathLabelLoc(16, 16 + 16 + 16 + 24 + 24 + 24 + 24);
    std::unique_ptr<Label> vcVarsPathLabelPtr(new Label(LabelCreateParams().Text("Visual C++ vcvars64.bat Path:").Location(vcVarsPathLabelLoc).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(vcVarsPathLabelPtr.release());

    Point vcVarsPathTextBoxLoc(16, 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<TextBox> vcVarsPathTextBoxPtr(new TextBox(TextBoxCreateParams().Text(vcVarsPath).Location(vcVarsPathTextBoxLoc).SetSize(textBoxSize).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    vcVarsPathTextBox = vcVarsPathTextBoxPtr.get();
    vcVarsPathTextBox->TextChanged().AddHandler(this, &EditConfigurationDialog::CheckInput);
    std::unique_ptr<PaddedControl> paddedVCVarsPathTextBoxPtr(new PaddedControl(PaddedControlCreateParams(vcVarsPathTextBoxPtr.release()).Location(vcVarsPathTextBoxLoc).SetSize(PaddedSize(textBoxSize, DefaultPadding())).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    std::unique_ptr<BorderedControl> borderedVCVarsPathTextBoxPtr(new BorderedControl(BorderedControlCreateParams(paddedVCVarsPathTextBoxPtr.release()).Location(vcVarsPathTextBoxLoc).
        SetSize(BorderedSize(PaddedSize(textBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(borderedVCVarsPathTextBoxPtr.release());

    Point selectVCVarsPathButtonLoc(16 + 16 + ScreenMetrics::Get().MMToHorizontalPixels(140), 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<Button> selectVCVarsPathButton(new Button(ControlCreateParams().Text("...").Location(selectVCVarsPathButtonLoc).SetSize(Size(24, 24)).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    selectVCVarsPathButton->Click().AddHandler(this, &EditConfigurationDialog::SelectVCVarsPath);
    AddChild(selectVCVarsPathButton.release());

    Point vcPlatformToolsetLabelLoc(16, 16 + 16 + 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<Label> vcPlatformToolsetLabelPtr(new Label(LabelCreateParams().Text("Visual C++ Platform Toolset:").Location(vcPlatformToolsetLabelLoc).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(vcPlatformToolsetLabelPtr.release());

    Point vcPlatformToolsetTextBoxLoc(16, 16 + 16 + 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<TextBox> vcPlatformToolsetTextBoxPtr(new TextBox(TextBoxCreateParams().Text(vcPlatformToolset).Location(vcPlatformToolsetTextBoxLoc).SetSize(textBoxSize).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    vcPlatformToolsetTextBox = vcPlatformToolsetTextBoxPtr.get();
    vcPlatformToolsetTextBox->TextChanged().AddHandler(this, &EditConfigurationDialog::CheckInput);
    std::unique_ptr<PaddedControl> paddedVCPlatformToolsetTextBoxPtr(new PaddedControl(PaddedControlCreateParams(vcPlatformToolsetTextBoxPtr.release()).Location(vcPlatformToolsetTextBoxLoc).SetSize(PaddedSize(textBoxSize, DefaultPadding())).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    std::unique_ptr<BorderedControl> borderedVCPlatformToolsetTextBoxPtr(new BorderedControl(BorderedControlCreateParams(paddedVCPlatformToolsetTextBoxPtr.release()).Location(vcPlatformToolsetTextBoxLoc).
        SetSize(BorderedSize(PaddedSize(textBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(borderedVCPlatformToolsetTextBoxPtr.release());

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

    boostIncludeDirTextBox->SetFocus();

    CheckInput();
}

void EditConfigurationDialog::GetData(std::string& boostIncludeDir, std::string& boostLibDir, std::string& vcVarsPath, std::string& vcPlatformToolset)
{
    boostIncludeDir = MakeNativePath(boostIncludeDirTextBox->Text());
    boostLibDir = MakeNativePath(boostLibDirTextBox->Text());
    vcVarsPath = MakeNativePath(vcVarsPathTextBox->Text());
    vcPlatformToolset = vcPlatformToolsetTextBox->Text();
}

void EditConfigurationDialog::CheckInput()
{
    if (!boostIncludeDirTextBox->Text().empty() && !boostLibDirTextBox->Text().empty() && !vcVarsPathTextBox->Text().empty() && !vcPlatformToolsetTextBox->Text().empty())
    {
        okButton->Enable();
    }
    else
    {
        okButton->Disable();
    }
}

void EditConfigurationDialog::SelectBoostIncludeDir()
{
    std::string boostIncludeDir = Path::MakeCanonical(SelectDirectory(nullptr, boostIncludeDirTextBox->Text()));
    if (!boostIncludeDir.empty())
    {
        boostIncludeDirTextBox->SetText(Path::MakeCanonical(boostIncludeDir));
    }
}

void EditConfigurationDialog::SelectBoostLibDir()
{
    std::string boostLibDir = Path::MakeCanonical(SelectDirectory(nullptr, boostLibDirTextBox->Text()));
    if (!boostLibDir.empty())
    {
        boostLibDirTextBox->SetText(Path::MakeCanonical(boostLibDir));
    }
}

void EditConfigurationDialog::SelectVCVarsPath()
{
    std::vector<std::pair<std::string, std::string>> descriptionFilterPairs;
    descriptionFilterPairs.push_back(std::make_pair("Batch Files (*.bat)", " *.bat"));
    std::string initialDirectory = Path::GetDirectoryName(Path::MakeCanonical(vcVarsPathTextBox->Text()));
    std::string filePath;
    std::string currentDirectory;
    std::vector<std::string> fileNames;
    bool selected = OpenFileName(Handle(), descriptionFilterPairs, initialDirectory, std::string(), "bat", OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, filePath, currentDirectory, fileNames);
    if (selected)
    {
        std::string vcVarsPath = GetFullPath(filePath);
        vcVarsPathTextBox->SetText(vcVarsPath);
    }
}

} } // wingstall::package_editor

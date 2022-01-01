// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/new_package_dialog.hpp>
#include <package_editor/main_window.hpp>
#include <package_editor/properties_view.hpp>
#include <wing/Metrics.hpp>
#include <wing/Button.hpp>
#include <wing/Label.hpp>
#include <wing/PaddedControl.hpp>
#include <wing/BorderedControl.hpp>
#include <soulng/util/Path.hpp>

namespace wingstall { namespace package_editor {

using namespace soulng::util;

NewPackageDialog::NewPackageDialog() : 
    Window(WindowCreateParams().Text("New Package").WindowClassName("wingstall.package_editor.new_package_dialog").
    WindowClassBackgroundColor(DefaultControlWindowClassBackgroundColor()).BackgroundColor(DefaultControlBackgroundColor()).
    SetSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(160), ScreenMetrics::Get().MMToVerticalPixels(100))).WindowStyle(DialogWindowStyle())),
    locationTextBox(nullptr), packageNameTextBox(nullptr)
{
    Size s = GetSize();
    Size defaultControlSpacing = ScreenMetrics::Get().DefaultControlSpacing();
    Size defaultButtonSize = ScreenMetrics::Get().DefaultButtonSize();
    Size defaultTextBoxSize = ScreenMetrics::Get().DefaultTextBoxSize();
    Size defaultComboBoxSize = ScreenMetrics::Get().DefaultComboBoxSize();
    Size defaultLabelSize = ScreenMetrics::Get().DefaultLabelSize();

    Point packageLocationLabelLoc(16, 16);
    std::unique_ptr<Label> packageLocationLabelPtr(new Label(LabelCreateParams().Text("Package Location:").Location(packageLocationLabelLoc).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(packageLocationLabelPtr.release());

    Point packageLocationTextBoxLoc(16, 16 + 24);
    Size textBoxSize(ScreenMetrics::Get().MMToHorizontalPixels(80), defaultTextBoxSize.Height + 4);
    std::unique_ptr<TextBox> packageLocationTextBoxPtr(new TextBox(TextBoxCreateParams().Text(WingstallPackagesDir()).Location(packageLocationTextBoxLoc).SetSize(textBoxSize).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    locationTextBox = packageLocationTextBoxPtr.get();
    locationTextBox->TextChanged().AddHandler(this, &NewPackageDialog::CheckInput);
    std::unique_ptr<PaddedControl> paddedPackageLocationTextBoxPtr(new PaddedControl(PaddedControlCreateParams(packageLocationTextBoxPtr.release()).Location(packageLocationTextBoxLoc).
        SetSize(PaddedSize(textBoxSize, DefaultPadding())).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    std::unique_ptr<BorderedControl> borderedPackageLocationTextBoxPtr(new BorderedControl(BorderedControlCreateParams(paddedPackageLocationTextBoxPtr.release()).Location(packageLocationTextBoxLoc).
        SetSize(BorderedSize(PaddedSize(textBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(borderedPackageLocationTextBoxPtr.release());

    Point selectPackageLocationButtonLoc(16 + 16 + ScreenMetrics::Get().MMToHorizontalPixels(80), 16 + 24);
    std::unique_ptr<Button> selectPackageLocationButton(new Button(ControlCreateParams().Text("...").Location(selectPackageLocationButtonLoc).SetSize(Size(24, 24)).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    selectPackageLocationButton->Click().AddHandler(this, &NewPackageDialog::SelectPackageLocation);
    AddChild(selectPackageLocationButton.release());

    Point packageNameLabelLoc(16, 16 + 16 + 24 + 24);
    std::unique_ptr<Label> packageNameLabelPtr(new Label(LabelCreateParams().Text("Package Name:").Location(packageNameLabelLoc).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(packageNameLabelPtr.release());

    Point packageNameTextBoxLoc(16, 16 + 16 + 24 + 24 + 24);
    std::unique_ptr<TextBox> packageNameTextBoxPtr(new TextBox(TextBoxCreateParams().Location(packageNameTextBoxLoc).SetSize(textBoxSize).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    packageNameTextBox = packageNameTextBoxPtr.get();
    packageNameTextBox->TextChanged().AddHandler(this, &NewPackageDialog::CheckInput);
    std::unique_ptr<PaddedControl> paddedPackageNameTextBoxPtr(new PaddedControl(PaddedControlCreateParams(packageNameTextBoxPtr.release()).Location(packageNameTextBoxLoc).
        SetSize(PaddedSize(textBoxSize, DefaultPadding())).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    std::unique_ptr<BorderedControl> borderedPackageNameTextBoxPtr(new BorderedControl(BorderedControlCreateParams(paddedPackageNameTextBoxPtr.release()).Location(packageNameTextBoxLoc).
        SetSize(BorderedSize(PaddedSize(textBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(borderedPackageNameTextBoxPtr.release());

    int x = s.Width - defaultButtonSize.Width - defaultControlSpacing.Width;
    int y = s.Height - defaultButtonSize.Height - defaultControlSpacing.Height;

    Point cancelButtonLoc(x, y);
    std::unique_ptr<Button> cancelButtonPtr(new Button(ControlCreateParams().Location(cancelButtonLoc).SetSize(defaultButtonSize).Text("Cancel").SetAnchors(static_cast<Anchors>(Anchors::bottom | Anchors::right))));
    Button* cancelButton = cancelButtonPtr.get();
    cancelButton->SetDialogResult(DialogResult::cancel);
    AddChild(cancelButtonPtr.release());

    x = x - defaultButtonSize.Width - defaultControlSpacing.Width;

    Point okButtonLoc(x, y);
    std::unique_ptr<Button> okButtonPtr(new Button(ControlCreateParams().Location(okButtonLoc).SetSize(defaultButtonSize).Text("OK").SetAnchors(static_cast<Anchors>(Anchors::bottom | Anchors::right))));
    okButton = okButtonPtr.get();
    okButton->Disable();
    okButton->SetDialogResult(DialogResult::ok);
    okButton->SetDefault();
    SetDefaultButton(okButton);
    SetCancelButton(cancelButton);
    AddChild(okButtonPtr.release());

    locationTextBox->SetFocus();
}

void NewPackageDialog::CheckInput()
{
    if (IsValidPath(GetPackageFilePath()))
    {
        okButton->Enable();
    }
    else
    {
        okButton->Disable();
    }
}

void NewPackageDialog::SelectPackageLocation()
{
    std::string packageLocation = Path::MakeCanonical(SelectDirectory(nullptr, WingstallPackagesDir()));
    if (!packageLocation.empty())
    {
        locationTextBox->SetText(Path::MakeCanonical(packageLocation));
    }
}

std::string NewPackageDialog::GetPackageFilePath() const
{
    if (locationTextBox->Text().empty()) return std::string();
    if (packageNameTextBox->Text().empty()) return std::string();
    return GetFullPath(Path::Combine(Path::Combine(locationTextBox->Text(), packageNameTextBox->Text()), packageNameTextBox->Text() + ".package.xml"));
}

std::string NewPackageDialog::GetPackageName() const
{
    return packageNameTextBox->Text();
}

} } // wingstall::package_editor

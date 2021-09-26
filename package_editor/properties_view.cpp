// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/properties_view.hpp>
#include <package_editor/package.hpp>
#include <package_editor/main_window.hpp>
#include <wing/Panel.hpp>
#include <wing/Label.hpp>
#include <wing/MessageBox.hpp>
#include <wing/BorderedControl.hpp>
#include <wing/PaddedControl.hpp>
#include <wing/Metrics.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

namespace wingstall { namespace package_editor {

using namespace soulng::unicode;

bool IsValidPath(const std::string& text)
{
    if (text.empty()) return false;
    std::u32string str = ToUtf32(text);
    for (char32_t c : str)
    {
        switch (c)
        {
            case '<': case '>': case '"': case '|': case '?': case '*': return false;
        }
    }
    return true;
}

bool IsValidProductId(const std::string& text)
{
    if (text.empty()) return false;
    try
    {
        boost::uuids::uuid productId = boost::lexical_cast<boost::uuids::uuid>(text);
    }
    catch (const std::exception&)
    {
        return false;
    }
    return true;
}

PropertiesView::PropertiesView(Package* package_) :
    ContainerControl(ControlCreateParams().BackgroundColor(DefaultControlBackgroundColor()).WindowClassName("wingstall.package_editor.properties_view").SetDock(Dock::fill)),
    package(package_), mainWindow(package->GetMainWindow()), exitHandlerId(-1), compressionComboBox(nullptr), dirty(false), initializing(true)
{
    bool enableApplyButton = false;
    ResetCaretDisabled();
    SetDoubleBuffered();
    Size s = GetSize();
    defaultControlSpacing = ScreenMetrics::Get().DefaultControlSpacing();
    defaultButtonSize = ScreenMetrics::Get().DefaultButtonSize();
    Size defaultTextBoxSize = ScreenMetrics::Get().DefaultTextBoxSize();
    Size defaultComboBoxSize = ScreenMetrics::Get().DefaultComboBoxSize();
    Size defaultLabelSize = ScreenMetrics::Get().DefaultLabelSize();

    Point sourceRootDirLabelLoc(16, 16);
    std::unique_ptr<Label> sourceRootDirLabelPtr(new Label(LabelCreateParams().Text("Source Root Directory:").Location(sourceRootDirLabelLoc).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(sourceRootDirLabelPtr.release());

    Point sourceRootDirTextBoxLoc(16, 16 + 24);

    Size textBoxSize(ScreenMetrics::Get().MMToHorizontalPixels(80), defaultTextBoxSize.Height + 4);
    Size sourceRootDirTextBoxSize(textBoxSize);
    std::unique_ptr<TextBox> sourceRootDirTextBoxPtr(new TextBox(TextBoxCreateParams().Location(sourceRootDirTextBoxLoc).Text(package->GetProperties()->SourceRootDir()).SetSize(sourceRootDirTextBoxSize).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    if (!boost::filesystem::exists(package->GetProperties()->SourceRootDir()))
    {
        enableApplyButton = true;
    }
    sourceRootDirTextBox = sourceRootDirTextBoxPtr.get();
    sourceRootDirTextBox->TextChanged().AddHandler(this, &PropertiesView::EnableApply);
    std::unique_ptr<PaddedControl> paddedSourceRootDirTextBoxPtr(new PaddedControl(PaddedControlCreateParams(sourceRootDirTextBoxPtr.release()).SetSize(PaddedSize(sourceRootDirTextBoxSize, DefaultPadding())).
        Location(sourceRootDirTextBoxLoc).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    std::unique_ptr<BorderedControl> borderedSourceRootDirTextBoxPtr(new BorderedControl(BorderedControlCreateParams(paddedSourceRootDirTextBoxPtr.release()).Text(package->GetProperties()->SourceRootDir()).Location(sourceRootDirTextBoxLoc).SetSize(
        BorderedSize(PaddedSize(sourceRootDirTextBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(borderedSourceRootDirTextBoxPtr.release());

    Point selectSourceRootDirButtonLoc(16 + 16 + ScreenMetrics::Get().MMToHorizontalPixels(80), 16 + 24);
    std::unique_ptr<Button> selectSourceRootDirButton(new Button(ControlCreateParams().Text("...").Location(selectSourceRootDirButtonLoc).SetSize(Size(24, 24)).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    selectSourceRootDirButton->Click().AddHandler(this, &PropertiesView::SelectSourceRootDir);
    AddChild(selectSourceRootDirButton.release());

    Point sourceRootDirRelativeCheckBoxLoc(16 + 16 + 16 + ScreenMetrics::Get().MMToHorizontalPixels(80) + 24, 16 + 24 + 4);
    std::unique_ptr<CheckBox> sourceRootDirRelativeCheckBoxPtr(new CheckBox(CheckBoxCreateParams().SetAnchors(static_cast<Anchors>(Anchors::left | Anchors::top)).Text("Relative Path").Location(sourceRootDirRelativeCheckBoxLoc)));
    sourceRootDirRelativeCheckBox = sourceRootDirRelativeCheckBoxPtr.get();
    sourceRootDirRelativeCheckBox->CheckedChanged().AddHandler(this, &PropertiesView::SourceRootDirRelativeCheckBoxCheckedChanged);
    if (Path::IsRelative(package->GetProperties()->SourceRootDir()))
    {
        sourceRootDirRelativeCheckBox->SetChecked(true);
    }
    AddChild(sourceRootDirRelativeCheckBoxPtr.release());

    Point targetRootDirLabelLoc(16, 16 + 16 + 24 + 24);
    std::unique_ptr<Label> targetRootDirLabelPtr(new Label(LabelCreateParams().Text("Target Root Directory:").Location(targetRootDirLabelLoc).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(targetRootDirLabelPtr.release());

    Point targetRootDirTextBoxLoc(16, 16 + 16 + 24 + 24 + 24);
    Size targetRootDirTextBoxSize(textBoxSize);
    if (package->GetProperties()->TargetRootDir().empty())
    {
        enableApplyButton = true;
    }
    std::unique_ptr<TextBox> targetRootDirTextBoxPtr(new TextBox(TextBoxCreateParams().Location(targetRootDirTextBoxLoc).Text(package->GetProperties()->TargetRootDir()).SetSize(targetRootDirTextBoxSize).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    targetRootDirTextBox = targetRootDirTextBoxPtr.get();
    targetRootDirTextBox->TextChanged().AddHandler(this, &PropertiesView::EnableApply);
    std::unique_ptr<PaddedControl> paddedTargetRootDirTextBoxPtr(new PaddedControl(PaddedControlCreateParams(targetRootDirTextBoxPtr.release()).
        SetSize(PaddedSize(targetRootDirTextBoxSize, DefaultPadding())).
        Location(targetRootDirTextBoxLoc).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    std::unique_ptr<BorderedControl> borderedTargetRootDirTextBoxPtr(new BorderedControl(BorderedControlCreateParams(paddedTargetRootDirTextBoxPtr.release()).Location(targetRootDirTextBoxLoc).SetSize(
        BorderedSize(PaddedSize(targetRootDirTextBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(borderedTargetRootDirTextBoxPtr.release());

    Point appNameLabelLoc(16, 16 + 16 + 16 + 24 + 24 + 24 + 24);
    std::unique_ptr<Label> appNameLabelPtr(new Label(LabelCreateParams().Text("Application Name:").Location(appNameLabelLoc).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(appNameLabelPtr.release());

    Point appNameTextBoxLoc(16, 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24);
    Size appNameTextBoxSize(textBoxSize);
    std::unique_ptr<TextBox> appNameTextBoxPtr(new TextBox(TextBoxCreateParams().Location(appNameTextBoxLoc).Text(package->GetProperties()->AppName()).SetSize(appNameTextBoxSize).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    appNameTextBox = appNameTextBoxPtr.get();
    appNameTextBox->TextChanged().AddHandler(this, &PropertiesView::EnableApply);
    std::unique_ptr<PaddedControl> paddedAppNameTextBoxPtr(new PaddedControl(PaddedControlCreateParams(appNameTextBoxPtr.release()).
        SetSize(PaddedSize(appNameTextBoxSize, DefaultPadding())).
        Location(appNameTextBoxLoc).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    std::unique_ptr<BorderedControl> borderedAppNameTextBoxPtr(new BorderedControl(BorderedControlCreateParams(paddedAppNameTextBoxPtr.release()).Location(appNameTextBoxLoc).SetSize(
        BorderedSize(PaddedSize(appNameTextBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(borderedAppNameTextBoxPtr.release());

    Point publisherLabelLoc(16, 16 + 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<Label> publisherLabelPtr(new Label(LabelCreateParams().Text("Publisher:").Location(publisherLabelLoc).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(publisherLabelPtr.release());

    Point publisherTextBoxLoc(16, 16 + 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24 + 24 + 24);
    Size publisherTextBoxSize(textBoxSize);
    std::unique_ptr<TextBox> publisherTextBoxPtr(new TextBox(TextBoxCreateParams().Location(publisherTextBoxLoc).Text(package->GetProperties()->Publisher()).SetSize(publisherTextBoxSize).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    publisherTextBox = publisherTextBoxPtr.get();
    publisherTextBox->TextChanged().AddHandler(this, &PropertiesView::EnableApply);
    std::unique_ptr<PaddedControl> paddedPublisherTextBoxPtr(new PaddedControl(PaddedControlCreateParams(publisherTextBoxPtr.release()).
        SetSize(PaddedSize(publisherTextBoxSize, DefaultPadding())).
        Location(publisherTextBoxLoc).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    std::unique_ptr<BorderedControl> borderedPublisherTextBoxPtr(new BorderedControl(BorderedControlCreateParams(paddedPublisherTextBoxPtr.release()).Location(publisherTextBoxLoc).SetSize(
        BorderedSize(PaddedSize(publisherTextBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(borderedPublisherTextBoxPtr.release());

    Point versionLabelLoc(16, 16 + 16 + 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<Label> versionLabelPtr(new Label(LabelCreateParams().Text("Version:").Location(versionLabelLoc).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(versionLabelPtr.release());

    Point versionTextBoxLoc(16, 16 + 16 + 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24);
    Size versionTextBoxSize(textBoxSize);
    std::unique_ptr<TextBox> versionTextBoxPtr(new TextBox(TextBoxCreateParams().Location(versionTextBoxLoc).Text(package->GetProperties()->Version()).SetSize(versionTextBoxSize).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    versionTextBox = versionTextBoxPtr.get();
    versionTextBox->TextChanged().AddHandler(this, &PropertiesView::EnableApply);
    std::unique_ptr<PaddedControl> paddedVersionTextBoxPtr(new PaddedControl(PaddedControlCreateParams(versionTextBoxPtr.release()).
        SetSize(PaddedSize(versionTextBoxSize, DefaultPadding())).
        Location(versionTextBoxLoc).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    std::unique_ptr<BorderedControl> borderedVersionTextBoxPtr(new BorderedControl(BorderedControlCreateParams(paddedVersionTextBoxPtr.release()).Location(versionTextBoxLoc).SetSize(
        BorderedSize(PaddedSize(versionTextBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(borderedVersionTextBoxPtr.release());

    Point productIdLabelLoc(16, 16 + 16 + 16 + 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<Label> productIdLabelPtr(new Label(LabelCreateParams().Text("Product ID:").Location(productIdLabelLoc).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(productIdLabelPtr.release());

    Point productIdTextBoxLoc(16, 16 + 16 + 16 + 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24);
    Size productIdTextBoxSize(textBoxSize);
    std::unique_ptr<TextBox> productIdTextBoxPtr(new TextBox(TextBoxCreateParams().Location(productIdTextBoxLoc).Text(boost::lexical_cast<std::string>(package->GetProperties()->Id())).SetSize(productIdTextBoxSize).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    productIdTextBox = productIdTextBoxPtr.get();
    productIdTextBox->TextChanged().AddHandler(this, &PropertiesView::EnableApply);
    std::unique_ptr<PaddedControl> paddedProductIdTextBoxPtr(new PaddedControl(PaddedControlCreateParams(productIdTextBoxPtr.release()).
        SetSize(PaddedSize(productIdTextBoxSize, DefaultPadding())).
        Location(productIdTextBoxLoc).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    std::unique_ptr<BorderedControl> borderedProductIdTextBoxPtr(new BorderedControl(BorderedControlCreateParams(paddedProductIdTextBoxPtr.release()).Location(productIdTextBoxLoc).SetSize(
        BorderedSize(PaddedSize(productIdTextBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(borderedProductIdTextBoxPtr.release());

    Point createProduceIdButtonLoc(16 + 16 + ScreenMetrics::Get().MMToHorizontalPixels(80), 16 + 16 + 16 + 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<Button> createProduceIdButton(new Button(ControlCreateParams().Text("Random ID").Location(createProduceIdButtonLoc).SetSize(defaultButtonSize).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    createProduceIdButton->Click().AddHandler(this, &PropertiesView::CreateProductId);
    AddChild(createProduceIdButton.release());

    Point compressionLabelLoc(16, 16 + 16 + 16 + 16 + 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<Label> compressionLabelPtr(new Label(LabelCreateParams().Text("Compression:").Location(compressionLabelLoc).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(compressionLabelPtr.release());

    Point compressionComboBoxLoc(16, 16 + 16 + 16 + 16 + 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24);
    Size compressionComboBoxSize(defaultComboBoxSize.Width, defaultComboBoxSize.Height * 6);
    std::unique_ptr<ComboBox> compressionComboBoxPtr(new ComboBox(ComboBoxCreateParams().Location(compressionComboBoxLoc).SetSize(compressionComboBoxSize).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    compressionComboBox = compressionComboBoxPtr.get();
    compressionComboBox->SelectedIndexChanged().AddHandler(this, &PropertiesView::EnableApply);
    AddChild(compressionComboBoxPtr.release());

    Point iconFilePathLabelLoc(16, 16 + 16 + 16 + 16 + 16 + 16 + 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<Label> iconFilePathLabelPtr(new Label(LabelCreateParams().Text("Icon File Path:").Location(iconFilePathLabelLoc).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(iconFilePathLabelPtr.release());

    Point iconFilePathTextBoxLoc(16, 16 + 16 + 16 + 16 + 16 + 16 + 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24);
    Size iconFilePathTextBoxSize(textBoxSize);
    std::unique_ptr<TextBox> iconFilePathTextBoxPtr(new TextBox(TextBoxCreateParams().Location(iconFilePathTextBoxLoc).Text(package->GetProperties()->IconFilePath()).SetSize(iconFilePathTextBoxSize).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    iconFilePathTextBox = iconFilePathTextBoxPtr.get();
    iconFilePathTextBox->TextChanged().AddHandler(this, &PropertiesView::EnableApply);
    std::unique_ptr<PaddedControl> paddedIconFilePathTextBoxPtr(new PaddedControl(PaddedControlCreateParams(iconFilePathTextBoxPtr.release()).
        SetSize(PaddedSize(iconFilePathTextBoxSize, DefaultPadding())).
        Location(iconFilePathTextBoxLoc).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    std::unique_ptr<BorderedControl> borderedIconFilePathTextBoxPtr(new BorderedControl(BorderedControlCreateParams(paddedIconFilePathTextBoxPtr.release()).Location(iconFilePathTextBoxLoc).SetSize(
        BorderedSize(PaddedSize(iconFilePathTextBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    AddChild(borderedIconFilePathTextBoxPtr.release());

    Point includeUninstallerCheckBoxLoc(16, 16 + 16 + 16 + 16 + 16 + 16 + 16 + 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<CheckBox> includeUninstallerCheckBoxPtr(new CheckBox(CheckBoxCreateParams().Text("Include uninstaller").Location(includeUninstallerCheckBoxLoc).
        SetAnchors(static_cast<Anchors>(Anchors::top | Anchors::left))));
    includeUninstallerCheckBox = includeUninstallerCheckBoxPtr.get();
    includeUninstallerCheckBox->SetChecked(package->GetProperties()->IncludeUninstaller());
    includeUninstallerCheckBox->CheckedChanged().AddHandler(this, &PropertiesView::EnableApply);
    AddChild(includeUninstallerCheckBoxPtr.release());

    Point applyButtonLocation(ScreenMetrics::Get().MMToHorizontalPixels(100), 16 + 16 + 16 + 16 + 16 + 16 + 16 + 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24);

    std::unique_ptr<Button> applyButtonPtr(new Button(ControlCreateParams().Location(applyButtonLocation).Text("Apply").SetSize(defaultButtonSize).
        SetAnchors(static_cast<Anchors>(Anchors::left | Anchors::top))));
    applyButton = applyButtonPtr.get();
    applyButton->Click().AddHandler(this, &PropertiesView::ApplyButtonClick);
    AddChild(applyButtonPtr.release());

    SetScrollUnits(ScrollUnits(17, 17));
    SetContentSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(100), applyButtonLocation.Y + defaultButtonSize.Height));

    MainWindow* mainWindow = package->GetMainWindow();
    if (mainWindow)
    {
        exitHandlerId = mainWindow->ExitView().AddHandler(this, &PropertiesView::Exit);
    }

    dirty = false;
    initializing = false;
    applyButton->Disable();
    if (enableApplyButton)
    {
        dirty = true;
        applyButton->Enable();
    }

    if (mainWindow)
    {
        KeyPreviewMethod keypreview;
        keypreview.SetHandlerFunction(this, &PropertiesView::KeyPreview);
        mainWindow->SetKeyPreviewMethod(keypreview);
        Application::SetActiveWindow(mainWindow);
    }
}

PropertiesView::~PropertiesView()
{
    Application::SetActiveWindow(nullptr);
    if (mainWindow)
    {
        if (exitHandlerId != -1)
        {
            mainWindow->ExitView().RemoveHandler(exitHandlerId);
            exitHandlerId = -1;
        }
    }
}

void PropertiesView::KeyPreview(Keys keys, bool& handled)
{
    if (keys == Keys::tab)
    {
        mainWindow->FocusNext();
        handled = true;
    }
    else if (keys == (Keys::shiftModifier | Keys::tab))
    {
        mainWindow->FocusPrev();
        handled = true;
    }
}

void PropertiesView::OnCreated()
{
    ContainerControl::OnCreated();
    compressionComboBox->AddItem("none");
    compressionComboBox->AddItem("deflate");
    compressionComboBox->AddItem("bzip2");
    int compression = static_cast<int>(package->GetProperties()->GetCompression());
    compressionComboBox->SetSelectedIndex(compression);
}

void PropertiesView::OnSizeChanged()
{
    Size sz = GetSize();
    if (sz.Width > 0)
    {
        applyButton->SetLocation(Point(sz.Width - defaultButtonSize.Width - 4 * defaultControlSpacing.Width, applyButton->Location().Y));
    }
}

void PropertiesView::EnableApply()
{
    dirty = true;
    applyButton->Enable();
}

bool PropertiesView::CheckApplyChanges()
{
    if (!IsValidPath(sourceRootDirTextBox->Text()))
    {
        sourceRootDirTextBox->SelectAll();
        ShowErrorMessageBox(Handle(), "Source root directory path not valid");
        return false;
    }
    if (!IsValidPath(targetRootDirTextBox->Text()))
    {
        targetRootDirTextBox->SelectAll();
        ShowErrorMessageBox(Handle(), "Target root directory path not valid");
        return false;
    }
    if (appNameTextBox->Text().empty())
    {
        appNameTextBox->SelectAll();
        ShowErrorMessageBox(Handle(), "Application name cannot be empty");
        return false;
    }
    if (!IsValidProductId(productIdTextBox->Text()))
    {
        productIdTextBox->SelectAll();
        ShowErrorMessageBox(Handle(), "Invalid product ID");
        return false;
    }
    return true;
}

void PropertiesView::ApplyButtonClick()
{
    if (CheckApplyChanges())
    {
        if (!sourceRootDirTextBox->Text().empty())
        {
            if (Path::IsAbsolute(sourceRootDirTextBox->Text()))
            {
                boost::filesystem::create_directories(MakeNativeBoostPath(sourceRootDirTextBox->Text()));
            }
        }
        package->GetProperties()->SetSourceRootDir(sourceRootDirTextBox->Text());
        package->GetProperties()->SetTargetRootDir(targetRootDirTextBox->Text());
        package->GetProperties()->SetAppName(appNameTextBox->Text());
        package->GetProperties()->SetPublisher(publisherTextBox->Text());
        package->GetProperties()->SetVersion(versionTextBox->Text());
        package->GetProperties()->SetCompression(static_cast<wingstall::wingpackage::Compression>(compressionComboBox->GetSelectedIndex()));
        package->GetProperties()->SetIconFilePath(iconFilePathTextBox->Text());
        package->GetProperties()->SetIncludeUninstaller(includeUninstallerCheckBox->Checked());
        package->GetProperties()->SetId(boost::lexical_cast<boost::uuids::uuid>(productIdTextBox->Text()));
        package->GetComponents()->RemoveUnexistingDirectoriesAndFiles();
        package->GetEngineVariables()->Fetch();
        applyButton->Disable();
        dirty = false;
    }
    else
    {
        applyButton->Enable();
    }
}

void PropertiesView::SelectSourceRootDir()
{
    MainWindow* mainWindow = package->GetMainWindow();
    if (mainWindow)
    {
        std::string sourceRootDir = Path::MakeCanonical(SelectDirectory(nullptr, "C:/"));
        if (!sourceRootDir.empty())
        {
            initializing = true;
            if (Path::IsRelative(sourceRootDir))
            {
                sourceRootDirRelativeCheckBox->SetChecked(true);
            }
            else
            {
                sourceRootDirRelativeCheckBox->SetChecked(false);
            }
            sourceRootDirTextBox->SetText(Path::MakeCanonical(sourceRootDir));
            initializing = false;
        }
    }
}

void PropertiesView::CreateProductId()
{
    boost::uuids::uuid newProductId = boost::uuids::random_generator()();
    productIdTextBox->SetText(boost::lexical_cast<std::string>(newProductId));
}

void PropertiesView::SourceRootDirRelativeCheckBoxCheckedChanged()
{
    if (initializing) return;
    std::string referenceDirPath = Path::GetDirectoryName(package->FilePath());
    if (sourceRootDirRelativeCheckBox->Checked())
    {
        std::string absoluteDirPath = sourceRootDirTextBox->Text();
        if (Path::IsAbsolute(absoluteDirPath))
        {
            std::string relativeDirPath = MakeRelativeDirPath(absoluteDirPath, referenceDirPath);
            sourceRootDirTextBox->SetText(relativeDirPath);
        }
    }
    else
    {
        std::string relativeDirPath = sourceRootDirTextBox->Text();
        if (Path::IsRelative(relativeDirPath))
        {
            std::string absoluteDirPath = GetFullPath(Path::Combine(referenceDirPath, relativeDirPath));
            sourceRootDirTextBox->SetText(absoluteDirPath);
        }
    }
}

void PropertiesView::Exit(CancelArgs& args)
{
    Application::SetActiveWindow(nullptr);
    mainWindow->SetFocusedControl(nullptr);
    if (dirty)
    {
        MessageBoxResult result = MessageBox::Show("Apply changes?", "Package | Properties", this, MB_YESNOCANCEL);
        if (result == MessageBoxResult::cancel)
        {
            args.cancel = true;
        }
        else
        {
            if (result == MessageBoxResult::yes)
            {
                if (!CheckApplyChanges())
                {
                    args.cancel = true;
                    return;
                }
                applyButton->DoClick();
            }
            else if (result == MessageBoxResult::no)
            {
                dirty = false;
                applyButton->Disable();
            }
            MainWindow* mainWindow = package->GetMainWindow();
            if (mainWindow)
            {
                if (exitHandlerId != -1)
                {
                    mainWindow->ExitView().RemoveHandler(exitHandlerId);
                    exitHandlerId = -1;
                }
            }
        }
    }
}

} } // wingstall::package_editor

// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <winggui/install_window.hpp>
#include <winggui/action_dialog.hpp>
#include <winggui/message_dialog.hpp>
#include <wingpackage/file.hpp>
#include <wing/BorderedControl.hpp>
#include <wing/Metrics.hpp>
#include <wing/PaddedControl.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Time.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem/path.hpp>
#include <codecvt>

namespace wingstall { namespace winggui {

using namespace wingstall::wingpackage;
using namespace soulng::util;
using namespace soulng::unicode;

std::string GetAppName()
{
    InfoItem* appNameItem = GetInfoItem(InfoItemKind::appName);
    if (appNameItem && appNameItem->Type() == InfoItemType::string)
    {
        return static_cast<StringItem*>(appNameItem)->Value();
    }
    return std::string();
}

std::string GetAppVersion()
{
    InfoItem* appVersionItem = GetInfoItem(InfoItemKind::appVersion);
    if (appVersionItem && appVersionItem->Type() == InfoItemType::string)
    {
        return static_cast<StringItem*>(appVersionItem)->Value();
    }
    return std::string();
}

std::string GetInstallDirName()
{
    InfoItem* installDirNameItem = GetInfoItem(InfoItemKind::installDirName);
    if (installDirNameItem && installDirNameItem->Type() == InfoItemType::string)
    {
        return static_cast<StringItem*>(installDirNameItem)->Value();
    }
    return std::string();
}

std::string GetDefaultContainingDirPath()
{
    InfoItem* defaultContainingDirPathItem = GetInfoItem(InfoItemKind::defaultContainingDirPath);
    if (defaultContainingDirPathItem && defaultContainingDirPathItem->Type() == InfoItemType::string)
    {
        return static_cast<StringItem*>(defaultContainingDirPathItem)->Value();
    }
    return std::string();
}

std::string GetDefaultInstallationDirPath()
{
    return Path::Combine(GetDefaultContainingDirPath(), GetInstallDirName());
}

std::string GetInstallWindowCaption()
{
    std::string installWindowCaption;
    installWindowCaption.append(GetAppName()).append(" ").append(GetAppVersion()).append(" installation");
    return installWindowCaption;
}

int64_t GetUncompressedPackageSize()
{
    InfoItem* uncompressedPackageSizeItem = GetInfoItem(InfoItemKind::uncompressedPackageSize);
    if (uncompressedPackageSizeItem && uncompressedPackageSizeItem->Type() == InfoItemType::integer)
    {
        return static_cast<IntegerItem*>(uncompressedPackageSizeItem)->Value();
    }
    return 0;
}

int64_t GetCompressedPackageSize()
{
    InfoItem* compressedPackageSizeItem = GetInfoItem(InfoItemKind::compressedPackageSize);
    if (compressedPackageSizeItem && compressedPackageSizeItem->Type() == InfoItemType::integer)
    {
        return static_cast<IntegerItem*>(compressedPackageSizeItem)->Value();
    }
    return 0;
}

std::string GetPackageFilePath()
{
    InfoItem* packageFilePathItem = GetInfoItem(InfoItemKind::packageFilePath);
    if (packageFilePathItem && packageFilePathItem->Type() == InfoItemType::string)
    {
        return static_cast<StringItem*>(packageFilePathItem)->Value();
    }
    return std::string();
}

int64_t GetPackageAddress()
{
    InfoItem* packageAddressItem = GetInfoItem(InfoItemKind::packageDataAddress);
    if (packageAddressItem && packageAddressItem->Type() == InfoItemType::integer)
    {
        return static_cast<IntegerItem*>(packageAddressItem)->Value();
    }
    return 0;
}

Compression GetCompression()
{
    InfoItem* compressionItem = GetInfoItem(InfoItemKind::compression);
    if (compressionItem && compressionItem->Type() == InfoItemType::integer)
    {
        return static_cast<Compression>(static_cast<IntegerItem*>(compressionItem)->Value());
    }
    return Compression::none;
}

DataSource GetDataSource()
{
    InfoItem* dataSourceItem = GetInfoItem(InfoItemKind::dataSource);
    if (dataSourceItem && dataSourceItem->Type() == InfoItemType::integer)
    {
        return static_cast<DataSource>(static_cast<IntegerItem*>(dataSourceItem)->Value());
    }
    return DataSource::file;
}

void RunInstallation(InstallWindow* installWindow, const std::string& installationDir)
{
    try
    {
        installWindow->RunInstallation(installationDir);
    }
    catch (...)
    {
    }
}

InstallWindowPackageObserver::InstallWindowPackageObserver(InstallWindow* installWindow_) : installWindow(installWindow_)
{
}

void InstallWindowPackageObserver::StatusChanged(Package* package)
{
    installWindow->PutStatusMessage(new StatusChangedMessage(package->GetStatus(), package->GetStatusStr(), package->GetErrorMessage()));
}

void InstallWindowPackageObserver::ComponentChanged(Package* package)
{
    Node* component = package->GetComponent();
    if (component)
    {
        installWindow->PutStatusMessage(new ComponentChangedMessage(component->FullName()));
    }
    else
    {
        installWindow->PutStatusMessage(new ComponentChangedMessage(std::string()));
    }
}

void InstallWindowPackageObserver::FileChanged(Package* package)
{
    File* file = package->GetFile();
    if (file)
    {
        installWindow->PutStatusMessage(new FileChangedMessage(file->Name()));
    }
    else
    {
        installWindow->PutStatusMessage(new FileChangedMessage(std::string()));
    }
}

void InstallWindowPackageObserver::StreamPositionChanged(Package* package)
{
    Stream* stream = package->GetStream();
    int64_t position = stream->Position();
    int64_t size = GetUncompressedPackageSize();
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    if (position == size || now - installWindow->UpdateTime() >= std::chrono::milliseconds{ updateIntegervalMs })
    {
        installWindow->SetUpdateTime(now);
        installWindow->PutStatusMessage(new StreamPositionChangedMessage(position));
    }
}

void InstallWindowPackageObserver::LogError(Package* packge, const std::string& error)
{
    installWindow->PutStatusMessage(new LogErrorMessage(error));
}

InstallWindow::InstallWindow() : Window(WindowCreateParams().WindowStyle(DialogWindowStyle()).Text(GetInstallWindowCaption()).WindowClassName("winggui.InstallWindow").
    WindowClassBackgroundColor(DefaultControlWindowClassBackgroundColor()).BackgroundColor(DefaultControlBackgroundColor()).    
    Location(DefaultLocation()).SetSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(160), ScreenMetrics::Get().MMToVerticalPixels(120)))),
    editMode(false), closeButton(nullptr), installButton(nullptr), containingDirLabel(nullptr), containingDirTextBox(nullptr), 
    installationDirLabel(nullptr), installationDirTextBox(nullptr), statusLabel(nullptr), statusText(nullptr), containingDirButton(nullptr),
    componentLabel(nullptr), componentText(nullptr), fileLabel(nullptr), fileText(nullptr), progressLabel(nullptr), progressPercentText(nullptr), progressBar(nullptr),
    observer(this), package(nullptr), updateTime(), installationRunning(false)
{
    std::u16string ffName(ToUtf16("Segoe UI"));
    const WCHAR* fontFamilyName = (const WCHAR*)ffName.c_str();
    FontFamily family(fontFamilyName);
    boldFont = Font(family, DefaultWindowFontSize(), FontStyle::FontStyleBold, Unit::UnitPoint);
    Size s = GetSize();
    Size defaultControlSpacing = ScreenMetrics::Get().DefaultControlSpacing();
    Size defaultButtonSize = ScreenMetrics::Get().DefaultButtonSize();
    Size defaultTextBoxSize = ScreenMetrics::Get().DefaultTextBoxSize();
    Size defaultLabelSize = ScreenMetrics::Get().DefaultLabelSize();
    Size defaultCheckBoxSize = ScreenMetrics::Get().DefaultCheckBoxSize();

    Point containingDirLabelLocation(16, 16);
    std::unique_ptr<Label> containingDirLabelPtr(new Label(LabelCreateParams().Text("Containing directory:").Location(containingDirLabelLocation).SetSize(defaultLabelSize).
        SetAnchors(Anchors::left | Anchors::top)));
    containingDirLabel = containingDirLabelPtr.get();
    containingDirLabel->SetFont(boldFont);
    AddChild(containingDirLabelPtr.release());

    Point containingDirTextBoxLocation(16, 16 + defaultControlSpacing.Height + 16);
    Size containingDirTextBoxSize(ScreenMetrics::Get().MMToHorizontalPixels(120), defaultTextBoxSize.Height);
    std::unique_ptr<TextBox> containingDirTextBoxPtr(new TextBox(TextBoxCreateParams().Defaults()));
    containingDirTextBox = containingDirTextBoxPtr.get();
    containingDirTextBox->TextChanged().AddHandler(this, &InstallWindow::ContainingDirTextBoxTextChanged);
    std::unique_ptr<Control> paddedContainingDirTextBox(new PaddedControl(PaddedControlCreateParams(containingDirTextBoxPtr.release()).
        SetSize(PaddedSize(containingDirTextBoxSize, DefaultPadding()))));
    std::unique_ptr<Control> borderedContainingDirTextBox(new BorderedControl(BorderedControlCreateParams(paddedContainingDirTextBox.release()).Location(containingDirTextBoxLocation).
        SetSize(BorderedSize(PaddedSize(containingDirTextBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(Anchors::left | Anchors::top)));
    AddChild(borderedContainingDirTextBox.release());
    containingDirTextBox->SetText(MakeNativePath(GetDefaultContainingDirPath()));

    Point containingDirButtonLocation(ScreenMetrics::Get().MMToHorizontalPixels(120) + 32, 16 + defaultControlSpacing.Height + 16);
    Size containingDirButtonSize(24, 24);
    std::unique_ptr<Button> containingDirButtonPtr(new Button(ControlCreateParams().Location(containingDirButtonLocation).SetSize(containingDirButtonSize).Text("...").
        SetAnchors(Anchors::left | Anchors::top)));
    containingDirButton = containingDirButtonPtr.get();
    containingDirButton->Click().AddHandler(this, &InstallWindow::SelectContainingDirButtonClick);
    AddChild(containingDirButtonPtr.release());

    Point installationDirLabelLocation(16, 16 + 16 + 24 + 24);
    std::unique_ptr<Label> installationDirLabelPtr(new Label(LabelCreateParams().Text("Installation directory:").Location(installationDirLabelLocation).SetSize(defaultLabelSize).
        SetAnchors(Anchors::left | Anchors::top)));
    installationDirLabel = installationDirLabelPtr.get();
    installationDirLabel->SetFont(boldFont);
    AddChild(installationDirLabelPtr.release());

    Point installationDirTextBoxLocation(16, 16 + defaultControlSpacing.Height + 16 + 16 + 24 + 24);
    Size installationDirTextBoxSize(ScreenMetrics::Get().MMToHorizontalPixels(120), defaultTextBoxSize.Height);
    std::unique_ptr<TextBox> installationDirTextBoxPtr(new TextBox(TextBoxCreateParams().Defaults()));
    installationDirTextBox = installationDirTextBoxPtr.get();
    std::unique_ptr<Control> paddedInstallationDirTextBox(new PaddedControl(PaddedControlCreateParams(installationDirTextBoxPtr.release()).
        SetSize(PaddedSize(installationDirTextBoxSize, DefaultPadding()))));
    std::unique_ptr<Control> borderedInstallationDirTextBox(new BorderedControl(BorderedControlCreateParams(paddedInstallationDirTextBox.release()).Location(installationDirTextBoxLocation).
        SetSize(BorderedSize(PaddedSize(installationDirTextBoxSize, DefaultPadding()), BorderStyle::single)).SetAnchors(Anchors::left | Anchors::top)));
    AddChild(borderedInstallationDirTextBox.release());
    installationDirTextBox->SetText(MakeNativePath(GetDefaultInstallationDirPath()));

    Point statusLabelLocation(16, 16 + 16 + 16 + 24 + 24 + 24 + 24);
    std::unique_ptr<Label> statusLabelPtr(new Label(LabelCreateParams().Text("Status:").Location(statusLabelLocation).SetSize(defaultLabelSize).
        SetAnchors(Anchors::left | Anchors::top)));
    statusLabel = statusLabelPtr.get();
    statusLabel->SetFont(boldFont);
    AddChild(statusLabelPtr.release());

    Size statusTextSize = defaultLabelSize;
    statusTextSize.Width = s.Width - 24 - 24;
    Point statusTextLocation(16, 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<Label> statusTextPtr(new Label(LabelCreateParams().Location(statusTextLocation).AutoSize(false).SetSize(statusTextSize).SetAnchors(Anchors::left | Anchors::top)));
    statusText = statusTextPtr.get();
    AddChild(statusTextPtr.release());

    Point componentLabelLocation(16, 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<Label> componentLabelPtr(new Label(LabelCreateParams().Text("Component:").Location(componentLabelLocation).SetSize(defaultLabelSize).
        SetAnchors(Anchors::left | Anchors::top)));
    componentLabel = componentLabelPtr.get();
    componentLabel->SetFont(boldFont);
    AddChild(componentLabelPtr.release());

    Size componentTextSize = defaultLabelSize;
    componentTextSize.Width = s.Width - 24 - 24;
    Point componentTextLocation(16, 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<Label> componentTextPtr(new Label(LabelCreateParams().Location(componentTextLocation).AutoSize(false).SetSize(componentTextSize).SetAnchors(Anchors::left | Anchors::top)));
    componentText = componentTextPtr.get();
    AddChild(componentTextPtr.release());

    Point fileLabelLocation(16, 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<Label> fileLabelPtr(new Label(LabelCreateParams().Text("File:").Location(fileLabelLocation).SetSize(defaultLabelSize).SetAnchors(Anchors::left | Anchors::top)));
    fileLabel = fileLabelPtr.get();
    fileLabel->SetFont(boldFont);
    AddChild(fileLabelPtr.release());

    Size fileTextSize = defaultLabelSize;
    fileTextSize.Width = s.Width - 24 - 24;
    Point fileTextLocation(16, 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<Label> fileTextPtr(new Label(LabelCreateParams().Location(fileTextLocation).AutoSize(false).SetSize(fileTextSize).SetAnchors(Anchors::left | Anchors::top)));
    fileText = fileTextPtr.get();
    AddChild(fileTextPtr.release());

    Point progressLabelLocation(16, 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<Label> progressLabelPtr(new Label(LabelCreateParams().Text("Progress:").Location(progressLabelLocation).SetSize(defaultLabelSize).
        SetAnchors(Anchors::left | Anchors::top)));
    progressLabel = progressLabelPtr.get();
    progressLabel->SetFont(boldFont);
    AddChild(progressLabelPtr.release());

    Point progressBarLocation(16, 16 + 16 + 16 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24);
    Size progressBarSize(ScreenMetrics::Get().MMToHorizontalPixels(120), defaultTextBoxSize.Height);
    std::unique_ptr<ProgressBar> progressBarPtr(new ProgressBar(ProgressBarCreateParams().Defaults().Location(progressBarLocation).SetSize(
        PaddedSize(progressBarSize, DefaultPadding()))));
    progressBar = progressBarPtr.get();
    AddChild(progressBarPtr.release());

    Point progressPercentTextLocation(16 + 16 + ScreenMetrics::Get().MMToHorizontalPixels(120) + defaultControlPadding, 16 + 16 + 16 + 3 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<Label> progressPercentTextPtr(new Label(LabelCreateParams().Text("0%").Location(progressPercentTextLocation).SetSize(defaultLabelSize).
        SetAnchors(Anchors::left | Anchors::top)));
    progressPercentText = progressPercentTextPtr.get();
    AddChild(progressPercentTextPtr.release());

    int x = s.Width - defaultButtonSize.Width - defaultControlSpacing.Width;
    int y = s.Height - defaultButtonSize.Height - defaultControlSpacing.Height;
    std::unique_ptr<Button> closeButtonPtr(new Button(ControlCreateParams().Location(Point(x, y)).SetSize(defaultButtonSize).Text("Close").SetAnchors(Anchors::right | Anchors::bottom)));
    closeButton = closeButtonPtr.get();
    closeButton->SetDialogResult(DialogResult::cancel);
    closeButton->Click().AddHandler(this, &InstallWindow::CloseButtonClick);
    x = x - defaultButtonSize.Width - defaultControlSpacing.Width;
    std::unique_ptr<Button> installButtonPtr(new Button(ControlCreateParams().Location(Point(x, y)).SetSize(defaultButtonSize).Text("Install").SetAnchors(Anchors::right | Anchors::bottom)));
    installButton = installButtonPtr.get();
    installButton->Click().AddHandler(this, &InstallWindow::InstallButtonClick);
    installButton->SetDialogResult(DialogResult::ok);
    installButton->SetDefault();
    AddChild(installButtonPtr.release());
    AddChild(closeButtonPtr.release());
    SetDefaultButton(installButton);
    SetCancelButton(closeButton);
    containingDirTextBox->SetFocus();
    editMode = true;
}

void InstallWindow::PutStatusMessage(StatusMessage* statusMessage)
{
    statusMessageQueue.Put(statusMessage);
    SendMessage(Handle(), STATUS_MESSAGE_AVAILABLE, 0, 0);
}

bool InstallWindow::ProcessMessage(Message& msg)
{
    switch (msg.message)
    {
        case STATUS_MESSAGE_AVAILABLE:
        {
            HandleStatusMessage();
            msg.result = 0;
            return true;
        }
        default:
        {
            return Window::ProcessMessage(msg);
        }
    }
}

void InstallWindow::HandleStatusMessage()
{
    std::unique_ptr<StatusMessage> message = statusMessageQueue.Get();
    switch (message->Kind())
    {
        case StatusMessageKind::statusChanged:
        {
            StatusChangedMessage* statusChanged = static_cast<StatusChangedMessage*>(message.get());
            SetStatus(statusChanged->GetStatus(), statusChanged->StatusStr(), statusChanged->ErrorMessage());
            break;
        }
        case StatusMessageKind::componentChanged:
        {
            ComponentChangedMessage* componentChanged = static_cast<ComponentChangedMessage*>(message.get());
            SetComponent(componentChanged->Component());
            break;
        }
        case StatusMessageKind::fileChanged:
        {
            FileChangedMessage* fileChanged = static_cast<FileChangedMessage*>(message.get());
            SetFile(fileChanged->File());
            break;
        }
        case StatusMessageKind::streamPositionChanged:
        {
            StreamPositionChangedMessage* streamPositionChanged = static_cast<StreamPositionChangedMessage*>(message.get());
            int64_t position = streamPositionChanged->Position();
            SetStreamPosition(position);
            break;
        }
        case StatusMessageKind::logError:
        {
            LogErrorMessage* errorMessage = static_cast<LogErrorMessage*>(message.get());
            MessageDialog dialog(MessageKind::error, "error: " + errorMessage->Error());
            dialog.ShowDialog(*this);
            break;
        }
    }
}

void InstallWindow::SetPackage(Package* package_)
{
    package = package_;
    package->AddObserver(&observer);
}

InstallWindow::~InstallWindow()
{
    try
    {
        if (package)
        {
            package->RemoveObserver(&observer);
        }
        if (thread.joinable())
        {
            thread.join();
        }
    }
    catch (...)
    {
    }
}

void InstallWindow::SetProgressPercent(float percent)
{
    progressBar->SetProgressPercent(percent);
    progressPercentText->SetText(std::to_string(static_cast<int>(percent)) + "%");
}

void InstallWindow::SetStreamPosition(int64_t position)
{
    int64_t packageSize = GetUncompressedPackageSize();
    if (packageSize != 0)
    {
        float percent = (100.0f * position) / packageSize;
        SetProgressPercent(percent);
    }
}

std::string InstallWindow::GetInstallationDir() const
{
    std::string installationDir = installationDirTextBox->Text();
    return installationDir;
}

void InstallWindow::SetStatus(Status status, const std::string& statusStr, const std::string& errorMessage)
{
    statusText->SetText(statusStr);
    if (status == Status::failed)
    {
        MessageDialog dialog(MessageKind::error, statusStr + ": " + errorMessage);
        dialog.ShowDialog(*this);
    }
    else if (status == Status::aborted)
    {
        MessageDialog dialog(MessageKind::info, statusStr);
        dialog.ShowDialog(*this);
    }
}

void InstallWindow::SetComponent(const std::string& component)
{
    componentText->SetText(component);
}

void InstallWindow::SetFile(const std::string& file)
{
    fileText->SetText(MakeNativePath(file));
}

void InstallWindow::RunInstallation(const std::string& installationDir)
{
    try
    {
        if (package)
        {
            if (!installationDir.empty())
            {
                package->SetTargetRootDir(GetFullPath(installationDir));
                DataSource dataSource = GetDataSource();
                if (dataSource == DataSource::file)
                {
                    std::string packageFilePath = GetPackageFilePath();
                    if (!packageFilePath.empty())
                    {
                        package->Install(DataSource::file, packageFilePath, nullptr, 0, Content::all);
                    }
                }
                else if (dataSource == DataSource::memory)
                {
                    int64_t address = GetPackageAddress();
                    if (address != 0)
                    {
                        uint8_t* data = static_cast<uint8_t*>(reinterpret_cast<void*>(address));
                        int64_t size = GetCompressedPackageSize();
                        package->Install(DataSource::memory, std::string(), data, size, Content::all);
                    }
                }
                installationRunning = false;
            }
            else
            {
                installationRunning = false;
            }
        }
        else
        {
            installationRunning = false;
        }
    }
    catch (const std::exception& ex)
    {
        installationRunning = false;
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void InstallWindow::InstallButtonClick()
{
    try
    {
        installationRunning = true;
        installButton->Disable();
        containingDirTextBox->Disable();
        containingDirButton->Disable();
        installationDirTextBox->Disable();
        thread = std::thread{ wingstall::winggui::RunInstallation, this, GetInstallationDir() };
    }
    catch (const std::exception& ex)
    {
        installationRunning = false;
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void InstallWindow::CloseButtonClick()
{
    try
    {
        if (installationRunning)
        {
            if (package)
            {
                package->Interrupt();
                ActionDialog actionDialog(ActionDialogKind::installationAction);
                DialogResult result = actionDialog.ShowDialog(*this);
                if (result == DialogResult::abort)
                {
                    package->SetAction(Action::abortAction);
                }
                else if (result == DialogResult::ok)
                {
                    package->SetAction(Action::continueAction);
                }
            }
            else
            {
                Close();
            }
        }
        else
        {
            Close();
        }
    }
    catch (const std::exception& ex)
    {
        installationRunning = false;
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void InstallWindow::ContainingDirTextBoxTextChanged()
{
    if (editMode)
    {
        installationDirTextBox->SetText(MakeNativePath(Path::Combine(Path::MakeCanonical(containingDirTextBox->Text()), GetInstallDirName())));
    }
}

void InstallWindow::InstallationDirTextBoxTextChanged()
{
    if (editMode)
    {
        if (!installationDirTextBox->Text().empty())
        {
            installButton->Enable();
        }
        else
        {
            installButton->Disable();
        }
    }
}

void InstallWindow::SelectContainingDirButtonClick()
{
    std::string location = SelectDirectory(nullptr, Path::MakeCanonical(containingDirTextBox->Text()));
    if (!location.empty())
    {
        containingDirTextBox->SetText(location);
    }
}

} } // namespace wingstall::wingpackage


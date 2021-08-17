// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <winggui/uninstall_window.hpp>
#include <wingpackage/file.hpp>
#include <wing/Metrics.hpp>
#include <wing/PaddedControl.hpp>
#include <wing/BorderedControl.hpp>
#include <wing/ScrollableControl.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>

namespace wingstall { namespace winggui {

using namespace wingstall::wingpackage;
using namespace soulng::util;
using namespace soulng::unicode;

UninstallWindowPackageObserver::UninstallWindowPackageObserver(UninstallWindow* uninstallWindow_) : uninstallWindow(uninstallWindow_)
{
}

void UninstallWindowPackageObserver::StatusChanged(Package* package)
{
    uninstallWindow->PutStatusMessage(new StatusChangedMessage(package->GetStatus(), package->GetStatusStr(), package->GetErrorMessage()));
}

void UninstallWindowPackageObserver::ComponentChanged(Package* package)
{
    Node* component = package->GetComponent();
    if (component)
    {
        uninstallWindow->PutStatusMessage(new ComponentChangedMessage(component->FullName()));
    }
    else
    {
        uninstallWindow->PutStatusMessage(new ComponentChangedMessage(std::string()));
    }
}

void UninstallWindowPackageObserver::FileChanged(Package* package)
{
    File* file = package->GetFile();
    if (file)
    {
        uninstallWindow->PutStatusMessage(new FileChangedMessage(file->Name()));
    }
    else
    {
        uninstallWindow->PutStatusMessage(new FileChangedMessage(std::string()));
    }
}

void UninstallWindowPackageObserver::FileIndexChanged(Package* package)
{
    uninstallWindow->PutStatusMessage(new FileIndexChangedMessage(package->FileIndex()));
}

void UninstallWindowPackageObserver::LogError(Package* package, const std::string& error) 
{
    uninstallWindow->PutStatusMessage(new LogErrorMessage(error));
}

void RunUninstallation(UninstallWindow* uninstallWindow)
{
    try
    {
        uninstallWindow->RunUninstallation();
    }
    catch (...)
    {
    }
}

std::string GetUninstallWindowCaption()
{
    std::string uninstallWindowCaption;
    uninstallWindowCaption.append(GetAppName()).append(" ").append(GetAppVersion()).append(" uninstallation");
    return uninstallWindowCaption;
}

UninstallWindow::UninstallWindow() : Window(WindowCreateParams().WindowStyle(DialogWindowStyle()).Text(GetUninstallWindowCaption()).WindowClassName("winggui.UninstallWindow").
    WindowClassBackgroundColor(DefaultControlWindowClassBackgroundColor()).BackgroundColor(DefaultControlBackgroundColor()).
    Location(DefaultLocation()).SetSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(160), ScreenMetrics::Get().MMToVerticalPixels(120)))),
    package(nullptr), boldFont(), closeButton(), uninstallButton(), progressBar(), thread(), uninstallationRunning(false), observer(this),
    statusLabel(nullptr), statusText(nullptr), componentLabel(nullptr), componentText(nullptr), fileLabel(nullptr), fileText(nullptr), 
    progressLabel(nullptr), progressPercentText(nullptr)
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

    Point statusLabelLocation(16, 16);
    std::unique_ptr<Label> statusLabelPtr(new Label(LabelCreateParams().Text("Status:").Location(statusLabelLocation).SetSize(defaultLabelSize).
        SetAnchors(Anchors::left | Anchors::top)));
    statusLabel = statusLabelPtr.get();
    statusLabel->SetFont(boldFont);
    AddChild(statusLabelPtr.release());

    Size statusTextSize = defaultLabelSize;
    statusTextSize.Width = s.Width - 24 - 24;
    Point statusTextLocation(16, 16 + 24);
    std::unique_ptr<Label> statusTextPtr(new Label(LabelCreateParams().Location(statusTextLocation).AutoSize(false).SetSize(statusTextSize).SetAnchors(Anchors::left | Anchors::top)));
    statusText = statusTextPtr.get();
    AddChild(statusTextPtr.release());

    Point componentLabelLocation(16, 16 + 24 + 24);
    std::unique_ptr<Label> componentLabelPtr(new Label(LabelCreateParams().Text("Component:").Location(componentLabelLocation).SetSize(defaultLabelSize).
        SetAnchors(Anchors::left | Anchors::top)));
    componentLabel = componentLabelPtr.get();
    componentLabel->SetFont(boldFont);
    AddChild(componentLabelPtr.release());

    Size componentTextSize = defaultLabelSize;
    componentTextSize.Width = s.Width - 24 - 24;
    Point componentTextLocation(16, 16 + 24 + 24 + 24);
    std::unique_ptr<Label> componentTextPtr(new Label(LabelCreateParams().Location(componentTextLocation).AutoSize(false).SetSize(componentTextSize).SetAnchors(Anchors::left | Anchors::top)));
    componentText = componentTextPtr.get();
    AddChild(componentTextPtr.release());

    Point fileLabelLocation(16, 16 + 24 + 24 + 24 + 24);
    std::unique_ptr<Label> fileLabelPtr(new Label(LabelCreateParams().Text("File:").Location(fileLabelLocation).SetSize(defaultLabelSize).SetAnchors(Anchors::left | Anchors::top)));
    fileLabel = fileLabelPtr.get();
    fileLabel->SetFont(boldFont);
    AddChild(fileLabelPtr.release());

    Size fileTextSize = defaultLabelSize;
    fileTextSize.Width = s.Width - 24 - 24;
    Point fileTextLocation(16, 16 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<Label> fileTextPtr(new Label(LabelCreateParams().Location(fileTextLocation).AutoSize(false).SetSize(fileTextSize).SetAnchors(Anchors::left | Anchors::top)));
    fileText = fileTextPtr.get();
    AddChild(fileTextPtr.release());

    Point progressLabelLocation(16, 16 + 24 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<Label> progressLabelPtr(new Label(LabelCreateParams().Text("Progress:").Location(progressLabelLocation).SetSize(defaultLabelSize).
        SetAnchors(Anchors::left | Anchors::top)));
    progressLabel = progressLabelPtr.get();
    progressLabel->SetFont(boldFont);
    AddChild(progressLabelPtr.release());

    Point progressBarLocation(16, 16 + 24 + 24 + 24 + 24 + 24 + 24 + 24);
    Size progressBarSize(ScreenMetrics::Get().MMToHorizontalPixels(120), defaultTextBoxSize.Height);
    std::unique_ptr<ProgressBar> progressBarPtr(new ProgressBar(ProgressBarCreateParams().Defaults().Location(progressBarLocation).SetSize(
        PaddedSize(progressBarSize, DefaultPadding()))));
    progressBar = progressBarPtr.get();
    AddChild(progressBarPtr.release());

    Point progressPercentTextLocation(16 + 16 + ScreenMetrics::Get().MMToHorizontalPixels(120) + defaultControlPadding, 16 + 24 + 24 + 3 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<Label> progressPercentTextPtr(new Label(LabelCreateParams().Text("0%").Location(progressPercentTextLocation).SetSize(defaultLabelSize).
        SetAnchors(Anchors::left | Anchors::top)));
    progressPercentText = progressPercentTextPtr.get();
    AddChild(progressPercentTextPtr.release());

    Point logLabelLocation(16, 16 + 24 + 24 + 8 + 24 + 24 + 24 + 24 + 24 + 24);
    std::unique_ptr<Label> logLabelPtr(new Label(LabelCreateParams().Text("Log:").Location(logLabelLocation).SetSize(defaultLabelSize).
        SetAnchors(Anchors::left | Anchors::top)));
    logLabel = logLabelPtr.get();
    logLabel->SetFont(boldFont);
    AddChild(logLabelPtr.release());

    Point logLocation(16, 16 + 24 + 24 + 8 + 24 + 24 + 24 + 24 + 24 + 24 + 24);
    Size logSize(ScreenMetrics::Get().MMToHorizontalPixels(120), ScreenMetrics::Get().MMToHorizontalPixels(30));
    std::unique_ptr<LogView> logPtr(new LogView(TextViewCreateParams().Location(logLocation).SetSize(logSize)));
    log = logPtr.get();
    std::unique_ptr<Control> scrollableLog(new ScrollableControl(ScrollableControlCreateParams(logPtr.release()).Defaults()));
    std::unique_ptr<Control> paddedLog(new PaddedControl(PaddedControlCreateParams(scrollableLog.release()).Location(logLocation).SetSize(PaddedSize(logSize, DefaultPadding()))));
    std::unique_ptr<Control> borderedLog(new BorderedControl(BorderedControlCreateParams(paddedLog.release()).SetBorderStyle(BorderStyle::single).Location(logLocation).
        SetSize(PaddedSize(BorderedSize(logSize, BorderStyle::single), DefaultPadding()))));
    AddChild(borderedLog.release());

    int x = s.Width - defaultButtonSize.Width - defaultControlSpacing.Width;
    int y = s.Height - defaultButtonSize.Height - defaultControlSpacing.Height;
    std::unique_ptr<Button> closeButtonPtr(new Button(ControlCreateParams().Location(Point(x, y)).SetSize(defaultButtonSize).Text("Close").SetAnchors(Anchors::right | Anchors::bottom)));
    closeButton = closeButtonPtr.get();
    closeButton->SetDialogResult(DialogResult::cancel);
    closeButton->Click().AddHandler(this, &UninstallWindow::CloseButtonClick);
    x = x - defaultButtonSize.Width - defaultControlSpacing.Width;
    std::unique_ptr<Button> uninstallButtonPtr(new Button(ControlCreateParams().Location(Point(x, y)).SetSize(defaultButtonSize).Text("Uninstall").SetAnchors(Anchors::right | Anchors::bottom)));
    uninstallButton = uninstallButtonPtr.get();
    uninstallButton->Click().AddHandler(this, &UninstallWindow::UninstallButtonClick);
    uninstallButton->SetDialogResult(DialogResult::ok);
    uninstallButton->SetDefault();
    AddChild(uninstallButtonPtr.release());
    AddChild(closeButtonPtr.release());
    SetDefaultButton(uninstallButton);
    SetCancelButton(closeButton);
}

UninstallWindow::~UninstallWindow()
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

void UninstallWindow::SetPackage(Package* package_)
{
    package = package_;
    package->AddObserver(&observer);
}

bool UninstallWindow::ProcessMessage(Message& msg)
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

void UninstallWindow::HandleStatusMessage()
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
        case StatusMessageKind::fileIndexChanged:
        {
            FileIndexChangedMessage* fileIndexChanged = static_cast<FileIndexChangedMessage*>(message.get());
            SetFileIndex(fileIndexChanged->FileIndex());
            break;
        }
        case StatusMessageKind::logError:
        {
            LogErrorMessage* errorMessage = static_cast<LogErrorMessage*>(message.get());
            log->WriteLine("error: " + errorMessage->Error());
            break;
        }
    }
}

void UninstallWindow::SetStatus(Status status, const std::string& statusStr, const std::string& errorMessage)
{
    statusText->SetText(statusStr);
    if (status == Status::succeeded)
    {
        log->WriteLine(statusStr);
    }
    else if (status == Status::running)
    {
        log->WriteLine(statusStr);
    }
    else if (status == Status::failed)
    {
        log->WriteLine(statusStr + ": " + errorMessage);
    }
    else if (status == Status::aborted)
    {
        log->WriteLine(statusStr);
    }
    else if (status == Status::rollbacked)
    {
        log->WriteLine(statusStr);
    }
}

void UninstallWindow::SetComponent(const std::string& component)
{
    componentText->SetText(component);
}

void UninstallWindow::SetFile(const std::string& file)
{
    fileText->SetText(MakeNativePath(file));
}

void UninstallWindow::SetFileIndex(int fileIndex)
{
    int fileCount = package->FileCount();
    if (fileCount != 0)
    {
        float percent = (100.0f * fileIndex) / fileCount;
        SetProgressPercent(percent);
    }
}

void UninstallWindow::SetProgressPercent(float percent)
{
    progressBar->SetProgressPercent(percent);
    progressPercentText->SetText(std::to_string(static_cast<int>(percent)) + "%");
}

void UninstallWindow::UninstallButtonClick()
{
    try
    {
        uninstallationRunning = true;
        uninstallButton->Disable();
        thread = std::thread{ wingstall::winggui::RunUninstallation, this };
    }
    catch (const std::exception& ex)
    {
        uninstallationRunning = false;
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void UninstallWindow::CloseButtonClick()
{
    try
    {
        if (uninstallationRunning)
        {
            if (package)
            {
                package->Interrupt();
                //UninstallationActionDialog actionDialog;
                /*
                DialogResult result = actionDialog.ShowDialog(*this);
                if (result == DialogResult::abort)
                {
                    package->SetAction(Action::abortAction);
                }
                else if (result == DialogResult::cancel)
                {
                    package->SetAction(Action::rollbackAction);
                }
                else if (result == DialogResult::ok)
                {
                    package->SetAction(Action::continueAction);
                }
                */
                package->SetAction(Action::continueAction);
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
        uninstallationRunning = false;
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void UninstallWindow::RunUninstallation()
{
    try
    {
        if (package)
        {
            package->Uninstall();
            uninstallationRunning = false;
        }
        else
        {
            uninstallationRunning = false;
        }
    }
    catch (const std::exception& ex)
    {
        uninstallationRunning = false;
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void UninstallWindow::PutStatusMessage(StatusMessage* statusMessage)
{
    statusMessageQueue.Put(statusMessage);
    SendMessage(Handle(), STATUS_MESSAGE_AVAILABLE, 0, 0);
}

} } // namespace wingstall::wingui

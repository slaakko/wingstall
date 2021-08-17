// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_WINGGUI_UNINSTALL_WINDOW_INCLUDED
#define WINGSTALL_WINGGUI_UNINSTALL_WINDOW_INCLUDED
#include <winggui/status_message.hpp>
#include <winggui/install_window.hpp>
#include <wingpackage/info.hpp>
#include <wing/Button.hpp>
#include <wing/Label.hpp>
#include <wing/ProgressBar.hpp>
#include <wing/TextBox.hpp>
#include <wing/Window.hpp>
#include <wing/LogView.hpp>
#include <chrono>
#include <thread>

namespace wingstall { namespace winggui {

using namespace wingstall::wingpackage;
using namespace wing;

class UninstallWindow;

class UninstallWindowPackageObserver : public PackageObserver
{
public:
    UninstallWindowPackageObserver(UninstallWindow* uninstallWindow_);
    void StatusChanged(Package* package) override;
    void ComponentChanged(Package* package) override;
    void FileChanged(Package* package) override;
    void FileIndexChanged(Package* package) override;
    void LogError(Package* package, const std::string& error) override;
private:
    UninstallWindow* uninstallWindow;
};

class UninstallWindow : public Window
{
public:
    UninstallWindow();
    ~UninstallWindow();
    void SetPackage(Package* package_);
    void RunUninstallation();
    void PutStatusMessage(StatusMessage* statusMessage);
protected:
    bool ProcessMessage(Message& msg) override;
private:
    void HandleStatusMessage();
    void SetStatus(Status status, const std::string& statusStr, const std::string& errorMessage);
    void SetComponent(const std::string& component);
    void SetFile(const std::string& file);
    void SetFileIndex(int fileIndex);
    void SetProgressPercent(float percent);
    void UninstallButtonClick();
    void CloseButtonClick();
    Package* package;
    Font boldFont;
    Label* statusLabel;
    Label* statusText;
    Label* componentLabel;
    Label* componentText;
    Label* fileLabel;
    Label* fileText;
    Label* progressLabel;
    Label* progressPercentText;
    Label* logLabel;
    LogView* log;
    Button* closeButton;
    Button* uninstallButton;
    ProgressBar* progressBar;
    std::thread thread;
    bool uninstallationRunning;
    UninstallWindowPackageObserver observer;
    StatusMessageQueue statusMessageQueue;
};

} } // namespace wingstall::wingui

#endif // WINGSTALL_WINGGUI_UNINSTALL_WINDOW_INCLUDED

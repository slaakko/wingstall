// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_WINGGUI_INSTALL_WINDOW_INCLUDED
#define WINGSTALL_WINGGUI_INSTALL_WINDOW_INCLUDED
#include <winggui/status_message.hpp>
#include <wingpackage/info.hpp>
#include <wingpackage/package.hpp>
#include <wingpackage/component.hpp>
#include <wingpackage/environment.hpp>
#include <wingpackage/links.hpp>
#include <wing/Button.hpp>
#include <wing/Label.hpp>
#include <wing/ProgressBar.hpp>
#include <wing/TextBox.hpp>
#include <wing/Window.hpp>
#include <chrono>
#include <thread>

namespace wingstall { namespace winggui {

const int64_t updateIntegervalMs = 250;

class InstallWindow;

void RunInstallation(InstallWindow* installWindow, const std::string& installationDir);
std::string GetAppName();
std::string GetAppVersion();


using namespace wingstall::wingpackage;
using namespace wing;

class InstallWindowPackageObserver : public PackageObserver
{
public:
    InstallWindowPackageObserver(InstallWindow* installWindow_);
    void StatusChanged(Package* package) override;
    void ComponentChanged(Package* package) override;
    void FileChanged(Package* package) override;
    void StreamPositionChanged(Package* package) override;
    void LogError(Package* package, const std::string& error) override;
private:
    InstallWindow* installWindow;
};

class InstallWindow : public Window
{
public:
    InstallWindow();
    ~InstallWindow();
    void SetPackage(Package* package_);
    void SetProgressPercent(int64_t pos, int64_t size);
    void SetStatus(Status status, const std::string& statusStr, const std::string& errorMessage);
    void SetComponent(const std::string& component);
    void SetFile(const std::string& filePath);
    void SetStreamPosition(int64_t position);
    std::string GetInstallationDir() const;
    void PutStatusMessage(StatusMessage* statusMessage);
    void HandleStatusMessage();
    void RunInstallation(const std::string& installationDir);
    std::chrono::steady_clock::time_point UpdateTime() const { return updateTime; }
    void SetUpdateTime(std::chrono::steady_clock::time_point updateTime_) { updateTime = updateTime_; }
protected:
    bool ProcessMessage(Message& msg) override;
private:
    void InstallButtonClick();
    void CloseButtonClick();
    void ContainingDirTextBoxTextChanged();
    void InstallationDirTextBoxTextChanged();
    void SelectContainingDirButtonClick();
    Package* package;
    bool editMode;
    Font boldFont;
    Button* closeButton;
    Button* installButton;
    Button* containingDirButton;
    Label* containingDirLabel;
    TextBox* containingDirTextBox;
    Label* installationDirLabel;
    TextBox* installationDirTextBox;
    Label* statusLabel;
    Label* statusText;
    Label* componentLabel;
    Label* componentText;
    Label* fileLabel;
    Label* fileText;
    Label* progressLabel;
    Label* progressPercentText;
    ProgressBar* progressBar;
    InstallWindowPackageObserver observer;
    StatusMessageQueue statusMessageQueue;
    std::thread thread;
    std::chrono::steady_clock::time_point updateTime;
    bool installationRunning;
};

} } // namespace wingstall::wingui

#endif // WINGSTALL_WINGGUI_INSTALL_WINDOW_INCLUDED

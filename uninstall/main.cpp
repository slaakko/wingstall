// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <winggui/uninstall_window.hpp>
#include <wingpackage/package.hpp>
#include <wingpackage/links.hpp>
#include <wingpackage/environment.hpp>
#include <wingpackage/component.hpp>
#include <winggui/message_dialog.hpp>
#include <wing/InitDone.hpp>
#include <wing/Wing.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/System.hpp>
#include <stdexcept>

using namespace wing;

struct InitDone
{
    InitDone(HINSTANCE instance)
    {
        soulng::util::Init();
        sngxml::xpath::Init();
        wing::Init(instance);
    }
    ~InitDone()
    {
        wing::Done();
        sngxml::xpath::Done();
        soulng::util::Done();
    }
};

using namespace soulng::util;
using namespace wingstall::wingpackage;
using namespace wingstall::winggui;

int WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow)
{
    InitDone initDone(instance);
    try
    {
        std::string currentExecutablePath = GetFullPath(GetPathToExecutable());
        std::string uninstallPackageFilePath = Path::Combine(Path::GetDirectoryName(currentExecutablePath), "uninstall.bin");
        std::string commandLine(cmdLine);
        if (!commandLine.empty())
        {
            uninstallPackageFilePath = GetFullPath(commandLine);
        }
        Package package;
        package.ReadIndex(uninstallPackageFilePath);
        SetInfoItem(InfoItemKind::appName, new StringItem(package.AppName()));
        SetInfoItem(InfoItemKind::appVersion, new StringItem(package.Version()));
        Icon& setupIcon = Application::GetResourceManager().GetIcon("setup_icon");
        UninstallWindow uninstallWindow;
        uninstallWindow.SetPackage(&package);
        uninstallWindow.SetIcon(setupIcon);
        uninstallWindow.SetSmallIcon(setupIcon);
        Application::Run(uninstallWindow);
    }
    catch (const std::exception& ex)
    {
        ShowMessageBox(nullptr, "Uninstall error", ex.what());
        return 1;
    }
    return 0;
}

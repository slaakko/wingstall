// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/main_window.hpp>
#include <package_editor/configuration.hpp>
#include <wing/Metrics.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Process.hpp>
#include <soulng/util/Unicode.hpp>
#include <wing/InitDone.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <soulng/util/InitDone.hpp>
#include <boost/filesystem.hpp>

using namespace wing;

void InitApplication(HINSTANCE instance)
{
    soulng::util::Init();
    sngxml::xpath::Init();
    wing::Init(instance);
    wingstall::package_editor::InitConfiguration();
}

using namespace soulng::util;

int WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow)
{
    try
    {
        InitApplication(instance);
        wingstall::package_editor::MainWindow mainWindow;
        Icon& icon = Application::GetResourceManager().GetIcon("package.icon");
        mainWindow.SetIcon(icon);
        mainWindow.SetSmallIcon(icon);
        Application::Run(mainWindow);
    }
    catch (const std::exception& ex)
    {
        ShowMessageBox(nullptr, "error", ex.what());
        return 1;
    }
    return 0;
}
// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_MAIN_WINDOW_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_MAIN_WINDOW_INCLUDED
#include <package_editor/package.hpp>
#include <package_editor/package_explorer.hpp>
#include <wing/Menu.hpp>
#include <wing/Window.hpp>

namespace wingstall { namespace package_editor {

using namespace wing;

class MainWindow : public Window
{
public:
    MainWindow();
private:
    MenuItem* newPackageMenuItem;
    MenuItem* openPackageMenuItem;
    MenuItem* closePackageMenuItem;
    MenuItem* exitMenuItem;
    std::unique_ptr<Package> package;
    PackageExplorer* packageExplorer;
    void NewPackageClick();
    void OpenPackageClick();
    void ClosePackageClick();
    void ExitClick();
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_MAIN_WINDOW_INCLUDED
